#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

#define iscontbyte(a) (a > 0x7f && c < 0xc0)

struct _mode {
	_Bool do_line;
	_Bool do_word;
	_Bool do_char;
	_Bool do_utf8;
	_Bool do_path;
};

struct _count {
	unsigned long int lines;
	unsigned long int words;
	unsigned long int chars;
};

struct _state {
	_Bool inword;
	char  utf8_contbyte_count;
	char  utf8_contbyte_counter;
	char  utf8_char_len;
	_Bool in_utf8_character;
};

int wc(struct _mode *, struct _count *, const char *);
int iserror(int, struct _state *);
int put(const char *, struct _mode *, struct _count *);

int
main(int argc, char **argv)
{
	struct _mode mode = {0, 0, 0, 0, 1};
	struct _count total = {0};
	int c = 0;
	while((c = getopt(argc, argv, "mclw")) != -1) {
		switch(c) {
		case 'm':
			mode.do_utf8 = 1;
			mode.do_char = 0;
			break;
		case 'c':
			mode.do_char = 1;
			mode.do_utf8 = 0;
			break;
		case 'l':
			mode.do_line = 1;
			break;
		case 'w':
			mode.do_word = 1;
			break;
		default:
			fprintf(stderr, "usage: %s [-mclw] [file...]\n", argv[0]);
			break;
		}
	}
	if(!mode.do_line && !mode.do_word && !mode.do_char && !mode.do_utf8) {
		mode.do_line = 1;
		mode.do_word = 1;
		mode.do_char = 1;
		mode.do_path = 1;
	}
	argv += optind;
	if(!*argv) {
		mode.do_path = 0;
		return wc(&mode, &total, "-");
	}
	for(; *argv; ++argv)
		if(wc(&mode, &total, *argv))
			return 1;
}

int
wc(struct _mode *mode, struct _count *total, const char *path)
{
	struct _count count = {0};
	struct _state state = {0};
	struct stat st = {0};
	FILE *fp = 0;
	int c = 0;
	if(path[0] == '-'  && path[1] == '\0')
		fp = stdin;
	else
		fp = fopen(path, "rb");
	if(!fp) {
		perror("wc");
		return 1;
	}
	if(!mode->do_utf8) {
		stat(path, &st);
		count.chars = st.st_size;
	}
	for(; c != EOF; c = fgetc(fp)) {
		if(c == '\n') ++count.lines;
		if(state.inword && isspace(c)) state.inword = 0, ++count.words;
		if(!state.inword && c > 0x20 && c != 0x7f) state.inword = 1;
		if(mode->do_utf8) {
			if(c < 0x80) ++count.chars;
			if(iscontbyte(c)) {
				++state.utf8_contbyte_counter;
				if(state.utf8_contbyte_counter == state.utf8_contbyte_count) {
					++count.chars;
					state.utf8_char_len = 0;
					state.utf8_contbyte_count = 0;
					state.utf8_contbyte_counter = 0;
					state.in_utf8_character = 0;
				}
			}
			if(iserror(c, &state)) continue;
			if(c > 0xc1 && c < 0xe0) {
				state.utf8_char_len = 2;
				state.utf8_contbyte_count = 1;
				state.utf8_contbyte_counter = 0;
				state.in_utf8_character = 1;
			}
			if(c > 0xdf && c < 0xf0) {
				state.utf8_char_len = 3;
				state.utf8_contbyte_count = 2;
				state.utf8_contbyte_counter = 0;
				state.in_utf8_character = 1;
			}
			if(c > 0xef && c < 0xf5) {
				state.utf8_char_len = 4;
				state.utf8_contbyte_count = 3;
				state.utf8_contbyte_counter = 0;
				state.in_utf8_character = 1;
			}
		}
	}
	if(count.chars && mode->do_utf8) --count.chars;
	put(path, mode, &count);
	total->lines += count.lines;
	total->words += count.words;
	total->chars += count.chars;
	fclose(fp);
	return 0;
}

int
iserror(int c, struct _state *state)
{
	if(
	 (iscontbyte(c) && !state->in_utf8_character) ||
	 (!iscontbyte(c) && state->in_utf8_character) ||
	 (c == 0xc0 || c == 0xc1 || c > 0xf4) ||
	 (state->utf8_contbyte_counter > state->utf8_contbyte_count)
	) {
		return 1;
	}
	return 0;
}

int
put(const char *path, struct _mode *mode, struct _count *count)
{
	if(mode->do_line) printf("%lu ", count->lines);
	if(mode->do_word) printf("%lu ", count->words);
	if(mode->do_char || mode->do_utf8) printf("%lu ", count->chars);
	if(mode->do_path) printf("%s", path);
	fputc('\n', stdout);
	return 0;
}
