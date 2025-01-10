#include <sys/stat.h>
#include <dirent.h>

#include "make_entry.h"
#include "ls.h"
#include "macros.h"

int
dont_sort(const struct dirent **__unused1, const struct dirent **__unused2)
{
	return 0 * (__unused1 - __unused2);
}

int
select_dirs(const struct dirent *e)
{
	struct stat st = {0};
	lstat(e->d_name, &st);
	if(ISDIR(st.st_mode) && !are_dot_or_dotdot(e->d_name)) return 1;
	return 0;
}

int
exclude_dots(const struct dirent *e)
{
	if(are_dot_or_dotdot(e->d_name)) return 0;
	return 1;
}

int
exclude_all_dots(const struct dirent *e)
{
	if(e->d_name[0] == '.') return 0;
	return 1;
}

int
make_entries(struct dirent ***res, Mode *mode, const char *path, int flag)
{
	int(*sort)(const struct dirent **, const struct dirent **);
	if(mode->by_name)
		sort = alphasort;
	else
		sort = dont_sort;
	if(flag) return scandir(path, res, select_dirs, sort);
	if(!mode->write_dots && !mode->all_dots) return scandir(path, res, exclude_all_dots, sort);
	if(!mode->all_dots && mode->write_dots) return scandir(path, res, exclude_dots, sort);
	if(mode->all_dots && mode->write_dots) return scandir(path, res, 0, sort);
	return 0;
}
