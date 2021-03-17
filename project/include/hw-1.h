//
// Created by denactive on 12.03.2021.
//
#ifndef PROJECT_HW_1_H
#define PROJECT_HW_1_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define SYNTAX_ERROR 1

enum {
    single,
    multiline
};

typedef struct {
    char* beg;
    char* end;
    size_t line;
    unsigned int type;
} comment;

int parse_comments(const char* str, size_t *line_cnt, comment **res, size_t *comments_amount);

char* execute_line_comment(const char *str, size_t *line_cnt, comment **res, size_t *comments_amount);
char* execute_multiline_comment(const char *str, size_t *line_cnt, comment **res, size_t *comments_amount);

int res_exp(comment **res, size_t *comments_amount);
int free_res(comment **res);
void print_res(const comment *res, size_t *comments_amount);
void print_comment(const comment* c);

char* console_input();
char* file_input(const char *filename, struct stat* stat_buf);

#endif //PROJECT_HW_1_H