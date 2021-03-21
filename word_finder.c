//
// Created by denactive on 20.03.2021.
//

#include "word_finder.h"

char* skip_to_single_quote(const char *str, size_t *line_cnt);
char* skip_to_double_quote(const char *str, size_t *line_cnt);
char* execute_line_comment(const char *str, size_t *line_cnt, comment **res, size_t *comments_amount);
char* execute_multiline_comment(const char *str, size_t *line_cnt, comment **res, size_t *comments_amount);

// starts from a sym after ' and returns a sym after closing '
char* skip_to_single_quote(const char* str, size_t *line_cnt) {
    if (!line_cnt || !str)
        return NULL;
    while (*str != '\'') {
        if (*str == '\n')
            (*line_cnt)++;
        if (*str == 0 || *str == EOF || *str == '\0')
            return NULL;
        // \' avoidance
        if (*str == '\\') {
            str++;
            if (*str == 0 || *str == EOF || *str == '\0')
                str--;
        }
        str++;
    }
    return (char*)str + 1;
}

// starts from a sym after " and returns a sym after closing "
char* skip_to_double_quote(const char* str, size_t *line_cnt) {
    if (!line_cnt || !str)
        return NULL;
    while (*str != '\"') {
        if (*str == '\n')
            (*line_cnt)++;
        if (*str == 0 || *str == EOF || *str == '\0')
            return NULL;
        // \" avoidance
        if (*str == '\\') {
            str++;
            if (*str == 0 || *str == EOF || *str == '\0')
                str--;
        }
        str++;
    }
    return (char*)str + 1;
}

char* execute_line_comment(const char *str, size_t *line_cnt, comment **res, size_t *comments_amount) {
    if (!line_cnt || !res || !comments_amount || !str)
        return NULL;
    comment executed = {(char*)str, NULL, *line_cnt, single};

    while (*str != '\n') {
        if (*str == 0 || *str == EOF || *str == '\0')
            return NULL;
        str++;
    }
    executed.end = (char*)str - 1;

    // saving
    if (res_exp(res, comments_amount))
        return NULL;

    (*res)[(*comments_amount) - 1] = executed;
    (*line_cnt)++;
    return (char*)str + 1;
}

char* execute_multiline_comment(const char *str, size_t *line_cnt, comment **res, size_t *comments_amount) {
    if (!line_cnt || !res || !comments_amount || !str)
        return NULL;
    // we are here after /* symbols.
    comment executed = {(char*)str, NULL, *line_cnt, single};

    // an exception for /*/ smtg */ situation
    if (*str == '/')
        str++;

    // totally safe :)
    while (*str != '/' || *(str - 1) != '*') {
        if (*str == '\n') {
            executed.type = multiline;
            (*line_cnt)++;
        }
        if (*str == 0 || *str == EOF || *str == '\0')
            return NULL;
        str++;
    }
    executed.end = (char*)str - 2;

    // saving
    if (res_exp(res, comments_amount))
        return NULL;

    (*res)[(*comments_amount) - 1] = executed;
    return (char*)str + 1;
}

int get_files_from_dir(const char* dir_name) {
    FILE *file = fopen(dir_name, "O_RDONLY");
    DIR* dir = fdopendir(file->_fileno);
    if (!dir) {
        printf("Error | DIR opening failed\n");
        return -1;
    }

    //ERRNO = 0;
    struct dirent* dirs = NULL;
    while (readdir(dir)) {
        printf("%s | %s", dir->d_type, dirs->d_name);
    }
//    if (!readdir(dir)) {
//        printf("An empty directory chosen\n");
//        return 0;
//    }



    if (!closedir(dir)) {
        printf("Error | DIR closing failed\n");
        return -1;
    };
}