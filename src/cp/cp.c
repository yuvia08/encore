#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    FILE *src_fp;
    FILE *dest_fp;
    char c;
    if (argc < 3) {
        fputs("cp: missing operand\n", stderr);
        return EXIT_FAILURE;
    }

    src_fp = fopen(argv[1], "rb");
    if (!src_fp) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    dest_fp = fopen(argv[2], "wb");
    if (!dest_fp) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    for (; (c = fgetc(src_fp)); fputc(c, dest_fp))
        if (c == EOF)
            break;

    return EXIT_SUCCESS;
}
