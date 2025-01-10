#ifndef ___MACROS_H
#define ___MACROS_H

#define ISSPECIAL(x) (S_ISBLK(x) || S_ISCHR(x))
#define ISDIR(x) (S_ISDIR(x) && !S_ISLNK(x))
#define are_dot_or_dotdot(x) (( x[0] == '.' && x[1] == '\0' ) || ( x[0] == '.' && x[1] == '.' && x[2] == '\0' ))
#define printmark(mark, i, mode) if(_should(mark[i], mode)) fputc(mark[i], stdout);

#define sizeof_struct_field(s, field) ( sizeof( ((s *)0)->field ) )

#define SIXMONTHS (60.0f * 60.0f * 24.0f * 180.0f)
#define FUTURE (0.0f)

#define WEAK __attribute__ ((weak))

#endif
