#include <sys/types.h>
#include <sys/stat.h>

void
strmode(mode_t mode, char *s)
{
	/*  type  */
	switch(mode & S_IFMT) {
	case S_IFDIR:
		*s++ = 'd';
		break;
	case S_IFCHR:
		*s++ = 'c';
		break;
	case S_IFBLK:
		*s++ = 'b';
		break;
	case S_IFREG:
		*s++ = '-';
		break;
	case S_IFLNK:
		*s++ = 'l';
		break;
	case S_IFSOCK:
		*s++ = 's';
		break;
#ifdef S_IFIFO
	/* some systems might not have it ? */
	case S_IFIFO:
		*s++ = 'p';
		break;
#endif
#ifdef S_IFWHT
	/*   whiteout    */
	case S_IFWHT:
		*s++ = 'w';
		break;
#endif
	default:
		*s++ = '?';
		break;
	}
	/*  owner  */
	if(mode & S_IRUSR)
		*s++ = 'r';
	else
		*s++ = '-';
	if(mode & S_IWUSR)
		*s++ = 'w';
	else
		*s++ = '-';
	switch(mode & (S_IXUSR | S_ISUID)) {
	case 0:
		*s++ = '-';
		break;
	case S_IXUSR:
		*s++ = 'x';
		break;
	case S_ISUID:
		*s++ = 'S';
		break;
	case S_IXUSR | S_ISUID:
		*s++ = 's';
		break;
	}
	/*  group  */
	if(mode & S_IRGRP)
		*s++ = 'r';
	else
		*s++ = '-';
	if(mode & S_IWGRP)
		*s++ = 'w';
	else
		*s++ = '-';
	switch(mode & (S_IXGRP | S_ISGID)) {
	case 0:
		*s++ = '-';
		break;
	case S_IXGRP:
		*s++ = 'x';
		break;
	case S_ISGID:
		*s++ = 'S';
		break;
	case S_IXGRP | S_ISGID:
		*s++ = 's';
		break;
	}
	/*  other  */
	if(mode & S_IROTH)
		*s++ = 'r';
	else
		*s++ = '-';
	if(mode & S_IWOTH)
		*s++ = 'w';
	else
		*s++ = '-';
	switch(mode & (S_IXOTH | S_ISVTX)) {
	case 0:
		*s++ = '-';
		break;
	case S_IXOTH:
		*s++ = 'x';
		break;
	case S_ISVTX:
		*s++ = 'T';
		break;
	case S_IXOTH | S_ISVTX:
		*s++ = 't';
		break;
	}
	*s = 0;
}
