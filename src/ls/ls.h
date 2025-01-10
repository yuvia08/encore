#ifndef ___LS_H
#define ___LS_H

typedef struct {
	char by_name;
	char by_order;
	char recurse;
	char long_out;
	char newline_split;
	char write_serial;
	char write_numeric_ids;
	char write_dots;
	char all_dots;
	char mark_dirs;
	char mark_others;
	size_t block_size;
} Mode;

typedef struct {
	int nlink;
	int group;
	int owner;
	int size;
	char extra_space;
} Format;

typedef struct {
	struct dirent ***entries;
	int count;
} EntryList;

int ls(char *, Mode *);

#endif
