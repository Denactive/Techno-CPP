//
// Created by denactive on 12.03.2021.
//

#include "hw-1.h"

char* skip_to_single_quote(const char *str, size_t *line_cnt);
char* skip_to_double_quote(const char *str, size_t *line_cnt);

// starts from a sym after ' and returns a sym after closing '
char* skip_to_single_quote(const char* str, size_t *line_cnt) {
    if (!line_cnt || !str)
        return NULL;
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
    return (char*)str + 1;
}

// starts from a sym after " and returns a sym after closing "
char* skip_to_double_quote(const char* str, size_t *line_cnt) {
    if (!line_cnt || !str)
        return NULL;
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
    return (char*)str + 1;
}

char* execute_line_comment(const char *str, size_t *line_cnt, comment **res, size_t *comments_amount) {
    if (!line_cnt || !res || !comments_amount || !str)
        return NULL;
    comment executed = {(char*)str, NULL, *line_cnt, single};

    while (*str != '\n') {
        if (*str == NULL || *str == EOF || *str == '\0')
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
        if (*str == NULL || *str == EOF || *str == '\0')
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

int res_exp(comment **res, size_t *comments_amount) {
    if (!comments_amount)
        return -1;

    size_t size = 0;
    if (*comments_amount == 0) {
        size = sizeof(comment);
        if (*res) {
            free(*res);
            *res = NULL;
        }
        *res = (comment*)malloc(size);
        if (*res) {
            (*comments_amount)++;
            return 0;
        } else {
            return -1;
        }
    } else {
        size = sizeof(comment) * ((*comments_amount) + 1);
        comment *tmp = (comment*)realloc(*res, size);
        if (tmp) {
            (*comments_amount)++;
            *res = tmp;
            return 0;
        } else {
            return -1;
        }
    }
}

int free_res(comment **res) {
    if (!res)
        return -1;
    free(*res);
    *res = NULL;
    return 0;
}

void print_res(const comment *res, size_t *comments_amount) {
    size_t comments_amount_local = 0;
    if (comments_amount)
        comments_amount_local = *comments_amount;

    printf("=================\n");
    if (!res || comments_amount_local == 0)
        printf("no comments found");

    for (size_t i = 0; i < comments_amount_local; i++) {
        printf("[%zu]: ", i);
        print_comment(&res[i]);
        if (i < comments_amount_local - 1)
            printf("\n-----------------\n");
    }
    printf("\n=================\n");
}

// this function allocates memory!
char *console_input() {
    char *input = (char*)malloc(BUFFER_SIZE * sizeof(char));
    if (!input)
        return NULL;

    printf("Enter C code | Press Ctrl+D to end the input | >> ");
    char sym = '\0';
    for (int i = 0; sym != EOF && i < BUFFER_SIZE; i++) {
        sym = getc(stdin);
        if (i == BUFFER_SIZE - 1) {
            printf("\nWarning: Buffer overflow - only %d symbols were saved", BUFFER_SIZE - 1);
            break;
        }
        input[i] = sym;
    }
    printf("\n");

    //safety
    input[BUFFER_SIZE - 1] = '\0';
    return input;
}

void print_comment(const comment* c) {
    if (!c) {
        printf("An empty comment\n");
        return;
    }
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

int parse_comments(const char* str, size_t *line_cnt, comment **res, size_t *comments_amount) {
    if (!line_cnt || !res || !comments_amount || !str)
        return -1;

    if (*res && *comments_amount == 0) {
        free(*res);
        *res = NULL;
    }
    if (*res == NULL)
        *comments_amount = 0;

    while (*str != '\0' && *str != NULL && *str != EOF) {
        switch (*str) {
            case '\"':
                str = skip_to_double_quote(str + 1, line_cnt);
                if (!str) {
                    printf("Error | syntax error\n");
                    return SYNTAX_ERROR;
                }
                break;
            case '\'':
                str = skip_to_single_quote(str + 1, line_cnt);
                if (!str) {
                    printf("Error | syntax error\n");
                    return SYNTAX_ERROR;
                }
                break;
            case '\n':
                (*line_cnt)++;
                str++;
                break;
            case '/':
                if (*(str + 1) == '/') {
                    str = execute_line_comment(str + 2, line_cnt, res, comments_amount);
                    if (!str) {
                        free_res(res);
                        return -1;
                    }
                }
                if (*(str + 1) == '*') {
                    str = execute_multiline_comment(str + 2, line_cnt, res, comments_amount);
                    if (!str) {
                        free_res(res);
                        return -1;
                    }
                }
                break;
            default:
                str++;
                break;
        }
    }
    return 0;
}