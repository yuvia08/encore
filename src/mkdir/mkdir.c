#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define check(x) if(x) { perror("mkdir"); if(final) free(final); _exit(1); }

struct stat st = {0};

char flag = 0;

int
_should(const char *p)
{
	if(lstat(p, &st) != 0) return 1;
	return 0;
}

void
make(char *path, int mask, int mode)
{
	char *t;
	char *final = 0;
	char cwd[4096];
	check(!getcwd(cwd, sizeof(cwd)));
	final = malloc(strlen(path) + 1);
	strncpy(final, path, strlen(path));
	if(!flag) {
		check(mkdir(path, mode & ~mask) != 0);
		if(flag & 2) {
			check(chmod(path, mode) != 0);
		} else {
			check(chmod(path, mode & ~mask) != 0);
		}
		return;
	}
	for(t = strtok(path, "/"); t != NULL; t = strtok(NULL, "/")) {
		if(_should(t)) {
			mkdir(t, 0);
			check(chmod(t, (S_IWUSR|S_IXUSR|~mask)&0777) != 0);
		}
		check(chdir(t) != 0);
	}
	chdir(cwd);
	if(flag & 2)
		chmod(final, mode);
	else
		chmod(final, mode & ~mask);
	free(final);
}

int
main(int argc, char **argv)
{
	char c = 0;
	char *endptr;
	unsigned int mode = 0777;
	unsigned int mask = umask(0);
	(void)umask(mask);
	while((c = getopt(argc, argv, ":pm:")) != -1) {
		switch(c) {
		case 'p':
			flag = flag | 1;
			break;
		case 'm':
			mode = strtol(optarg, &endptr, 8);
			if(*endptr) {
				fprintf(stderr, "mode '%s' is invalid.\n", optarg);
				goto bad;
			}
			flag = flag | 2;
			break;
		case ':':
		case '?':
bad:			(void)fprintf(stderr, "usage: mkdir [-p] [-m mode] dir...\n");
			return 1;
		}
	}
	argv += optind;
	if(*argv == NULL) goto bad;
	for(;*argv;argv++) make(*argv, mask, mode);
	return 0;
}
