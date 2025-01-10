#include <dirent.h>

#include "make_entry.h"
#include "ls.h"

int
dont_sort(const struct dirent **__unused1, const struct dirent **__unused2)
{
	return 0 * (__unused1 - __unused2);
}

int
make_entries(struct dirent ***res, Mode *mode, const char *path)
{
	if(mode->by_name)  return scandir(path, res, 0, alphasort);
	if(mode->by_order) return scandir(path, res, 0, dont_sort);
	return 0;
}
