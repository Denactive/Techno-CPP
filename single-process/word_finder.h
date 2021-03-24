//
// Created by denactive on 20.03.2021.
//

#ifndef HW1_WORD_FINDER_H
#define HW1_WORD_FINDER_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/dir.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define DEBUG 1
#define QUOTES_CHECK 0
#define MEMORY 0
#define MEMORY_EXP_PATH 0

#define BUFFER_SIZE 1024

// 16 байт
// оптимизация в виде представления файла номером не сработает, т.к.
// file_amount ~ size_t, т.е. тоже 8 байт как и адрес
typedef struct {
    char* name;
    size_t matches_amount;
} string_size_pair;


int get_files_from_dir(const char* dir_name, const char* extension, char*** file_list, size_t* cnt);
string_size_pair* create_word_search_result(char** file_list, size_t files_amount);
int word_search(const char* pattern, string_size_pair** word_search_result, size_t files_amount);
int merge_sort_desc(string_size_pair* arr, size_t len);
int merge_desc(string_size_pair* l, size_t lsize, string_size_pair* r, size_t rsize, string_size_pair* res);
#endif //HW1_WORD_FINDER_H
