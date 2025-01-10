#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>

#include "macros.h"
#include "ls.h"
#include "put.h"

int
main(int argc, char **argv)
{
	struct stat st = {0};
	Mode mode = {0};
	Format fallback = {1, 1, 1, 1, 0};
	struct dirent pin = {0};
	char cwd[4096] = {0};
	char c = 0;
	if(!getcwd(cwd, 4096)) return 1;
	mode.block_size = 512;
	mode.by_name = 1;
	while((c = getopt(argc, argv, "1AaCFHLRlkfpni")) != -1) {
		switch(c) {
		case '1':
			mode.newline_split = 1;
			break;
		case 'a':
			mode.all_dots = 1;
			mode.write_dots = 1;
			break;
		case 'A':
			mode.write_dots = 1;
			break;
		case 'C':
			mode.newline_split = 1;
			mode.long_out = 0;
			break;
		case 'F':
			mode.mark_dirs = 1;
			mode.mark_others = 1;
			break;
		case 'H':
		case 'L':
			break;
		case 'R':
			mode.recurse = 1;
			break;
		case 'l':
			mode.long_out = 1;
			mode.newline_split = 1;
			break;
		case 'k':
			mode.block_size = 1024;
			break;
		case 'f':
			mode.by_order = 1;
			mode.by_name = 0;
			mode.all_dots = 1;
			mode.write_dots = 1;
			break;
		case 'p':
			mode.mark_dirs = 1;
			break;
		case 'n':
			mode.write_numeric_ids = 1;
			mode.long_out = 1;
			mode.newline_split = 1;
			break;
		case 'i':
			mode.write_serial = 1;
			break;
		default:
			break;
		}
	}
	argv += optind;
	if(!*argv) {
		ls(cwd, &mode);
		return 0;
	}
	for(; *argv; ++argv) {
		if(lstat(*argv, &st) == -1) {
			perror("ls: could not stat");
			continue;
		}
		if(!S_ISDIR(st.st_mode)) {
			strncpy(pin.d_name, *argv, sizeof_struct_field(struct dirent, d_name));
			put(&pin, &fallback, &mode);
			if(!mode.newline_split) fputc('\n', stdout);
		} else {
			chdir(*argv);
			ls(*argv, &mode);
			chdir(cwd);
		}
	}
	return 0;
}
