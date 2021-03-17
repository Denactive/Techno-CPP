#include "hw-1.h"

int main(int argc, const char** argv) {
    char* input = NULL;
    struct stat stat_buf = {};

    if (argc > 1) {
        input = file_input(argv[1], &stat_buf);
    } else {
        input = console_input();
    }

    if (!input) {
        printf("Error | Empty input\n");
        return -1;
    }

    comment *res = NULL;
    size_t comments_amount = 0;
    size_t line_cnt = 1;
    int exec_code = parse_comments(input, &line_cnt, &res, &comments_amount);

    if (exec_code == -1)
        return -1;

    if (exec_code == 0)
        print_res(res, &comments_amount);

    if (argc > 1) {
        // file mapping closing
        if (munmap((void*)input, stat_buf.st_size))
            printf("Error | failed to unmap %s\n", argv[1]);
    } else {
        free(input);
    }

    free_res(&res);
    return 0;
}
