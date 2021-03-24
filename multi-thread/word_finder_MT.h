//
// Created by denactive on 24.03.2021.
//

#ifndef HW1_WORD_FINDER_MT_H
#define HW1_WORD_FINDER_MT_H

#include <pthread.h>
#include <time.h>
#include "word_finder.h"

#define MT_DEBUG 0

typedef struct {
    size_t size;
    string_size_pair* res;
} string_sizex2;

typedef struct {
    size_t tr_no;
    const char* pattern;
    string_size_pair** word_search_result;
    size_t files_amount;
} thread_searcher_data;

string_sizex2* create_word_search_result_mt(size_t num_tr, char** file_list, size_t files_amount);
string_size_pair* word_search_mt(const char* pattern, char** file_list, size_t files_amount, size_t num_tr);
void clear_word_search_result_mt(string_sizex2** word_search_result, size_t num_tr);
void* thread_searcher(void* args);

#endif //HW1_WORD_FINDER_MT_H
