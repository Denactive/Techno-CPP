//
// Created by denactive on 20.03.2021.
//

#include "word_finder.h"

int main(int argc, const char** argv) {
    char** file_list = NULL;
    size_t files_amount = 0;
    get_files_from_dir(argv[1], ".c", &file_list, &files_amount);
    for (size_t i = 0; i < files_amount; i++) {
        printf("%s\n", file_list[i]);
    }
//    struct stat stat_buf = {0};
//
//    if (argc > 1) {
//        input = file_input(argv[1], &stat_buf);
//    } else {
//        input = console_input();
//    }
//
//    if (!input) {
//        printf("Error | Empty input\n");
//        return -1;
//    }
//
//    comment *res = NULL;
//    size_t comments_amount = 0;
//    size_t line_cnt = 1;
//    int exec_code = parse_comments(input, &line_cnt, &res, &comments_amount);
//
//    if (exec_code == -1)
//        return -1;
//
//    if (exec_code == 0)
//        print_res(res, &comments_amount);
//
//    if (argc > 1) {
//        // file mapping closing
//        if (munmap((void*)input, stat_buf.st_size))
//            printf("Error | failed to unmap %s\n", argv[1]);
//    } else {
//        free(input);
//    }
//
//    free_res(&res);
    for (size_t i = 0; i < files_amount; i++) {
        free(file_list[i]);
    }
    free(file_list);
    return 0;
}

