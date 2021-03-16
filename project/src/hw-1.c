//
// Created by denactive on 12.03.2021.
//

#include "hw-1.h"

// starts from a sym after ' and returns a sym after closing '
char* skip_to_single_quote(char* str, size_t *line_cnt) {
    while (*str != '\'') {
        if (*str == '\n')
            (*line_cnt)++;
        if (*str == NULL || *str == EOF || *str == '\0')
            return NULL;
        // \' avoidance
        if (*str == '\\') {
            str++;
            if (*str == NULL || *str == EOF || *str == '\0')
                str--;
        }
        str++;
    }
    return str++;
}

// starts from a sym after " and returns a sym after closing "
char* skip_to_double_quote(char* str, size_t *line_cnt) {
    while (*str != '\"') {
        if (*str == '\n')
            (*line_cnt)++;
        if (*str == NULL || *str == EOF || *str == '\0')
            return NULL;
        // \" avoidance
        if (*str == '\\') {
            str++;
            if (*str == NULL || *str == EOF || *str == '\0')
                str--;
        }
        str++;
    }
    return str++;
}

char* execute_line_comment(char *str, size_t *line_cnt, comment **res, size_t *comments_amount) {
    comment executed = {str, NULL, *line_cnt, single};

    while (*str != '\n') {
        if (*str == NULL || *str == EOF || *str == '\0')
            return NULL;
        str++;
    }
    executed.end = str - 1;

    // saving
    if (res_exp(res, comments_amount))
        return NULL;

    (*res)[(*comments_amount) - 1] = executed;
    (*line_cnt)++;
    return str++;
}

char* execute_multiline_comment(char *str, size_t *line_cnt, comment **res, size_t *comments_amount) {
    // we are here after /* symbols.
    comment executed = {str, NULL, *line_cnt, single};

    // an exception for /*/ smtg */ situation
    if (*str == '/')
        str++;

    // totally safe :)
    while (*str != '/' || *(str - 1) != '*') {
        if (*str == '\n') {
            executed.type = multiline;
            (*line_cnt)++;
        }
        if (*str == NULL || *str == EOF || *str == '\0')
            return NULL;
        str++;
    }
    executed.end = str - 2;

    // saving
    if (res_exp(res, comments_amount))
        return NULL;

    (*res)[(*comments_amount) - 1] = executed;
    return str++;
}

int res_exp(comment **res, size_t *comments_amount) {
    size_t size = 0;
    if (*comments_amount == 0) {
        size = sizeof(comment);
        *res = (comment*)malloc(size);
        if (*res) {
            (*comments_amount)++;
            return 0;
        } else {
            return -1;
        }
    } else {
        size = sizeof(comment) * ((*comments_amount) + 1);
        *res = (comment*)realloc(*res, size);
        if (*res) {
            (*comments_amount)++;
            return 0;
        } else {
            return -1;
        }
    }
}

int free_res(comment **res) {
    free(*res);
    *res = NULL;
    return 0;
}

void print_res(const comment *res, size_t *comments_amount) {
    printf("=================\n");
    if (!res)
        printf("no comments found");

    for (size_t i = 0; i < *comments_amount; i++) {
        printf("[%zu]: ", i);
        print_comment(&res[i]);
        if (i < *comments_amount - 1)
            printf("\n-----------------\n");
    }
    printf("\n=================\n");
}

char *console_input() {
    char *input = (char*)malloc(BUFFER_SIZE * sizeof(char));
    if (!input)
        return NULL;

    printf("Enter C code | Press Ctrl+D to end the input | >> ");
    char sym = '\0';
    for (int i = 0; sym != EOF && i < BUFFER_SIZE; i++) {
        sym = getc(stdin);
        if (i == BUFFER_SIZE) {
            printf("Warning: Buffer overflow - only %d symbols were saved\n", BUFFER_SIZE - 1);
            break;
        }
        input[i] = sym;
    }

    //safety
    input[BUFFER_SIZE - 1] = '\0';
    return input;
}

void print_comment(const comment* c) {
    printf("line: %zu; type: ", c->line);
    if (c->type == single)
        printf("single-line;\n");
    if (c->type == multiline)
        printf("multiline;\n");

    for (char* addr = c->beg; addr <= c->end; addr++)
        printf("%c", *addr);
}

char* file_input(const char* filename, struct stat* stat_buf) {
    int fdin = 0;
    if ((fdin = open(filename, O_RDONLY)) < 0) {
        printf("Error | Usage %s invalid\n", filename);
        NULL;
    }

    // gain file size
    if (fstat(fdin, stat_buf) < 0) {
        printf("Error | Impossible to get the %s size\n", filename);
        return NULL;
    }

    // using mmap
    void* content = NULL;
    if ((content = mmap(0, stat_buf->st_size, PROT_READ, MAP_SHARED, fdin, 0)) == MAP_FAILED) {
        printf("Error | mmap error\n");
        return NULL;
    }

    return (char*)content;
}

// gtest ругается на const
int parse_comments(char* input) {
    char *str = input;

    size_t line_cnt = 1;
    comment *res = NULL;
    size_t comments_amount = 0;

    while (*str != '\0' && *str != NULL && *str != EOF) {
        if (*str == '\"')
            str = skip_to_double_quote(str + 1, &line_cnt);
        if (!str) {
            printf("Error | syntax error");
            return 0;
        }
        if (*str == '\'')
            str = skip_to_single_quote(str + 1, &line_cnt);
        if (!str) {
            printf("Error | syntax error");
            return 0;
        }
        if (*str == '\n')
            line_cnt++;

        if (*str == '/') {
            if (*(str + 1) == '/') {
                str = execute_line_comment(str + 2, &line_cnt, &res, &comments_amount);
                if (!str) {
                    free_res(&res);
                    return -1;
                }
            }
            if (*(str + 1) == '*') {
                str = execute_multiline_comment(str + 2, &line_cnt, &res, &comments_amount);
                if (!str) {
                    free_res(&res);
                    return -1;
                }
            }
        }
        str++;
    }
    print_res(res, &comments_amount);
    free_res(&res);
}

int parse_comments_from_file(const char *filename) {
    struct stat stat_buf = {};
    char* input = file_input(filename, &stat_buf);
    if (!input) {
        printf("Error | Empty input\n");
        return -1;
    }

    if(parse_comments(input))
        return -1;

    // file mapping closing
    if (munmap((void*)input, stat_buf.st_size))
        printf("Error | failed to unmap %s", filename);
    return 0;
}

int parse_comments_from_console() {
    char* input = console_input();
    if (!input) {
        printf("Error | Empty input\n");
        return -1;
    }

    if(parse_comments(input))
        return -1;

    free(input);
    return 0;
}