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

int parse_comments(char* input);
int parse_comments_from_file(const char *filename);
int parse_comments_from_console();

char* skip_to_single_quote(char *str, size_t *line_cnt);
char* skip_to_double_quote(char *str, size_t *line_cnt);
char* execute_line_comment(char *str, size_t *line_cnt, comment **res, size_t *comments_amount);
char* execute_multiline_comment(char *str, size_t *line_cnt, comment **res, size_t *comments_amount);

int res_exp(comment **res, size_t *comments_amount);
int free_res(comment **res);

char* console_input();
char* file_input(const char *filename, struct stat* stat_buf);
void print_res(const comment *res, size_t *comments_amount);
void print_comment(const comment* c);

#endif //PROJECT_HW_1_H