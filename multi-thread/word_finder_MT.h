//
// Created by denactive on 24.03.2021.
//

#ifndef HW1_WORD_FINDER_MT_H
#define HW1_WORD_FINDER_MT_H

#include "word_finder.h"

#define MT_DEBUG 1

typedef struct {
    size_t size;
    string_size_pair* res;
} string_sizex2;

string_sizex2* create_word_search_result_mt(size_t num_tr, char** file_list, size_t files_amount);
int word_search_mt(const char* pattern, string_sizex2** word_search_result);
void clear_word_search_result_mt(string_sizex2** word_search_result, size_t num_tr);

#endif //HW1_WORD_FINDER_MT_H
