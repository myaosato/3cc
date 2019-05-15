#include "3cc.h"

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    if (strncmp(argv[1], "-test", 5) == 0) {
        test_vec();
        test_map();
        return 0;
    }

    parse(argv[1]);
    gen_main();
    return 0;
}
