#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>

#include "macros.h"
#include "ls.h"
#include "put.h"
#include "strmode.h"

int
numlen(int num)
{
	char buf[32] = {0};
	snprintf(buf, 32, "%d", num);
	return strlen(buf);
}

int
spclen(int num)
{
	char buf[32] = {0};
	snprintf(buf, 32, "%#x", num);
	return strlen(buf);
}

void
set_format(struct dirent **entry, int entries, Format *format, Mode *mode)
{
	int own = 0, grp = 0, e = 0;
	off_t siz = 0;
	nlink_t nli = 0;
	dev_t spc = 0;
	struct stat st = {0};
	for(e = 0; e < entries; ++e) {
		lstat(entry[e]->d_name, &st);
		setown(mode, &st, &own);
		setgrp(mode, &st, &grp);
		if(st.st_size > siz) siz = st.st_size;
		if(st.st_rdev > spc) spc = st.st_rdev;
		if(st.st_nlink > nli) nli = st.st_nlink;
		if(strchr(entry[e]->d_name, ' ')) format->extra_space = 1;
	}
	spc = spclen(spc);
	siz = numlen(siz);
	nli = numlen(nli);
	if((long int)spc > siz) siz = spc;
	format->nlink = nli;
	format->group = grp;
	format->owner = own;
	format->size  = siz;
}

void
setown(Mode *mode, struct stat *st, int *own)
{
	struct passwd p = {0};
	struct passwd *unneeded;
	char *buf;
	long bufsiz;
	int a = 0;
	bufsiz = sysconf(_SC_GETPW_R_SIZE_MAX);
	if(bufsiz == -1)
		bufsiz = 8192;
	buf = malloc(bufsiz);
	if(!buf) return;
	if(getpwuid_r(st->st_uid, &p, buf, bufsiz, &unneeded) || mode->write_numeric_ids) {
		a = numlen(st->st_uid);
		if(a > *own) *own = a;
		free(buf);
		return;
	}
	a = strlen(p.pw_name);
	if(a > *own) *own = a;
	free(buf);
}

void
setgrp(Mode *mode, struct stat *st, int *grp)
{
	struct group g = {0};
	struct group *unneeded;
	char *buf;
	long bufsiz;
	int a = 0;
	bufsiz = sysconf(_SC_GETGR_R_SIZE_MAX);
	if(bufsiz == -1)
		bufsiz = 8192;
	buf = malloc(bufsiz);
	if(!buf) return;
	if(getgrgid_r(st->st_gid, &g, buf, bufsiz, &unneeded) || mode->write_numeric_ids) {
		a = numlen(st->st_gid);
		if(a > *grp) *grp = a;
		free(buf);
		return;
	}
	a = strlen(g.gr_name);
	if(a > *grp) *grp = a;
	free(buf);
}

void
setmark(mode_t mode, char *mark, int i)
{
	if(ISDIR(mode))				mark[i] = '/';
	if(S_ISLNK(mode) && !S_ISDIR(mode))	mark[i] = '@';
	if(S_ISREG(mode) && mode & 0111)	mark[i] = '*';
	if(S_ISFIFO(mode))			mark[i] = '|';
	if(S_ISSOCK(mode))			mark[i] = '=';
}

int
_should(char c, Mode *mode)
{
	switch(c) {
	case '/':
		if(mode->mark_dirs) return 1;
		return 0;
	default:
		if(mode->mark_others) {
			if(c == '@' && mode->long_out) return 0;
			return 1;
		}
		return 0;
	}
}

void
setname(char **names, struct stat *st, Mode *mode)
{
	if(getpwuid(st->st_uid) && !mode->write_numeric_ids)
		names[0] = getpwuid(st->st_uid)->pw_name;
	names[2] = names[0];
	if(getgrgid(st->st_gid) && !mode->write_numeric_ids)
		names[1] = getgrgid(st->st_gid)->gr_name;
	names[3] = names[1];
	if(!names[0]) {
		names[0] = malloc(numlen(st->st_uid) + 2);
		snprintf(names[0], numlen(st->st_uid) + 1, "%u", st->st_uid);
	}
	if(!names[1]) {
		names[1] = malloc(numlen(st->st_gid) + 2);
		snprintf(names[1], numlen(st->st_gid) + 1, "%u", st->st_gid);
	}
} 

int
printname(char *name, Format *format)
{
	if(strchr(name, ' ')) {
		return printf("'%s'", name);
	} else {
		if(format) if(format->extra_space) fputc(' ', stdout);
		return printf("%s", name);
	}
}

int
put_long(struct dirent *e, Format *format, Mode *mode, struct stat *st, char *mark)
{
	time_t diff = 0;
	char fmode[11] = {0};
	char *names[4] = {0};
	char *symlink_path;
	char date[32] = {0};
	char buf[32] = {0};
	struct tm tt = {0};
	setname(names, st, mode);
	strmode(st->st_mode, fmode);
	localtime_r(&st->st_mtim.tv_sec, &tt);
	diff = difftime(time(0), st->st_mtim.tv_sec);
	if(diff > SIXMONTHS || diff < FUTURE)
		strftime(date, sizeof(date), "%b %e  %Y", &tt);
	else
		strftime(date, sizeof(date), "%b %e %H:%M", &tt);
	printf("%s ", fmode);
	snprintf(buf, sizeof(buf), "%%%du ", format->nlink);
	printf(buf, st->st_nlink);
	snprintf(buf, sizeof(buf), "%%-%ds ", format->owner);
	printf(buf, names[0]);
	snprintf(buf, sizeof(buf), "%%-%ds ", format->group);
	printf(buf, names[1]);
	if(ISSPECIAL(st->st_mode)) {
		snprintf(buf, sizeof(buf), "%%#%dx ", format->size);
		printf(buf, st->st_rdev);
	} else {
		snprintf(buf, sizeof(buf), "%%%du ", format->size);
		printf(buf, st->st_size);
	}
	printf("%s ", date);
	printname(e->d_name, format);
	printmark(mark, 0, mode);
	if(S_ISLNK(st->st_mode)) {
		symlink_path = malloc(st->st_size + 1);
		symlink_path[ readlink(e->d_name, symlink_path, st->st_size + 1) ] = 0;
		lstat(symlink_path, st);
		setmark(st->st_mode, mark, 1);
		printf(" -> ");
		printname(symlink_path, format);
		printmark(mark, 1, mode);
		free(symlink_path);
	}
	fputc('\n', stdout);
	if(!names[2]) free(names[0]);
	if(!names[3]) free(names[1]);
	return 0;
}

int
put(struct dirent *e, Format *format, Mode *mode)
{
	char mark[2];
	struct stat st = {0};
	lstat(e->d_name,  &st);
	setmark(st.st_mode, mark, 0);
	if(mode->write_serial) printf("%lu ", st.st_ino);
	if(mode->long_out) return put_long(e, format, mode, &st, mark);
	printname(e->d_name, format);
	printmark(mark, 0, mode);
	fputc(' ', stdout);
	return 0;
}