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
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define DEBUG 1

#define BUFFER_SIZE 1024


int get_files_from_dir(const char* dir_name, const char* extension, char*** file_list, size_t* cnt);
char* execute_line_comment(const char *str);
char* execute_multiline_comment(const char *str);

#endif //HW1_WORD_FINDER_H
