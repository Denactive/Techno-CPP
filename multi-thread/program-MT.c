//
// Created by denactive on 24.03.2021.
//

#include "word_finder_MT.h"

void print_res(const string_size_pair* word_search_result, size_t files_amount);
void clear_file_list(char*** file_list, size_t files_amount);

int main(int argc, const char** argv) {
    if (argc < 3) {
        printf("Please pass a source path and a word to search\n");
        return -1;
    }
    size_t num_proc = get_nprocs();
    if (DEBUG)
        printf("detected %zu available processors detected\n", num_proc);
    size_t num_tr = num_proc / 2;

    char** file_list = NULL;
    size_t files_amount = 0;
    if (get_files_from_dir(argv[1], ".c", &file_list, &files_amount))
        return -1;

    string_sizex2* res = create_word_search_result_mt(num_tr, file_list, files_amount);
    if (!res)
        return -1;

    if (MT_DEBUG) {
        for (size_t i = 0; i < num_tr; ++i) {
            printf("Thread %zu gets %zu files:\n", i, res[i].size);
            for (size_t j = 0; j < res[i].size; ++j) {
                printf("\t%s | %zu\n", res[i].res[j].name, res[i].res[j].matches_amount);
            }
        }
    }
    clear_word_search_result_mt(&res, num_tr);
    clear_file_list(&file_list, files_amount);
    return 0;
}

void clear_file_list(char*** file_list, size_t files_amount) {
    for (size_t i = 0; i < files_amount; i++) {
        if (DEBUG)
            printf("\twonna clear [%zu] %s\n", i, (*file_list)[i]);
        free((*file_list)[i]);
    }
    free(*file_list);
}

void print_res(const string_size_pair* word_search_result, size_t files_amount) {
    for (size_t i = 0; i < files_amount; i++) {
        printf("%s: %zu\n", word_search_result[i].name, word_search_result[i].matches_amount);
    }
}
