#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "macros.h"
#include "ls.h"
#include "put.h"
#include "make_entry.h"

int
ls(char *cwd, Mode *mode)
{
	struct dirent **entry = 0;
	struct dirent **rdirs = 0;
	struct stat st = {0};
	Format format = {0};
	int entries = 0, dirs = 0, e = 0, r = 0;
	entries = make_entries(&entry, mode, cwd, 0);
	dirs = make_entries(&rdirs, mode, cwd, 1);
	set_format(entry, entries, &format, mode);
	if(mode->recurse) printf("%s:\n", cwd);
	if(mode->long_out) put_total(entry, entries);
	for(e = 0, r = 0; e < entries; ++e) {
		lstat(entry[e]->d_name, &st);
		if(!mode->all_dots && are_dot_or_dotdot(entry[e]->d_name)) goto bad;
		if(!mode->write_dots && entry[e]->d_name[0] == '.') goto bad;
		put(entry[e], &format, mode);
		if(ISDIR(st.st_mode) && mode->recurse) {
			if(!are_dot_or_dotdot(entry[e]->d_name)) {
				if(entry[e]->d_name[0] == 0) goto bad;
				rdirs[r] = malloc(sizeof( struct dirent ));
				memcpy(rdirs[r], entry[e], sizeof( struct dirent ));
				++r;
				++dirs;
			}
		}
bad:		free(entry[e]);
	}
	if(mode->recurse && dirs) {
		for(r = 0; r < dirs; ++r) {
			if(chdir(rdirs[r]->d_name) == -1)
				continue;
			fputc('\n', stdout);
			ls(rdirs[r]->d_name, mode);
			free(rdirs[r]);
			chdir("..");
		}
	}
	if(!mode->newline_split && !mode->recurse) fputc('\n', stdout);
	free(rdirs);
	free(entry);
	return 0;
}
