//
// Created by denactive on 20.03.2021.
//

#include "word_finder.h"

void print_res(const string_size_pair* word_search_result, size_t files_amount);
void clear_mem(char*** filelist, string_size_pair** res, size_t num);

int main(int argc, const char** argv) {
    if (argc < 3) {
        printf("Please pass a source path and a word to search\n");
        return -1;
    }
    size_t num_proc = get_nprocs();
    if (DEBUG)
        printf("detected %zu available processors detected\n", num_proc);

    char** file_list = NULL;
    size_t files_amount = 0;
    if (get_files_from_dir(argv[1], ".c", &file_list, &files_amount))
        return -1;

    string_size_pair *word_search_result = create_word_search_result(file_list, files_amount);
    if (!word_search_result && !files_amount) {
        printf("no .c files found\n");
        clear_mem(&file_list, &word_search_result, files_amount);
        return 0;
    }
    if (!word_search_result) {
        printf("Error | failed to create string_size_pair structure for current file list");
        clear_mem(&file_list, &word_search_result, files_amount);
        return -1;
    }
    if (word_search(argv[2], &word_search_result, files_amount)) {
        printf("Error | word reading failed\n");
        clear_mem(&file_list, &word_search_result, files_amount);
        return -1;
    }

    if (DEBUG)
        printf("\tbefore sort\n");
    if (DEBUG)
        print_res(word_search_result, files_amount);

    if (merge_sort_desc(word_search_result, files_amount)) {
        printf("Error | sort failed\n");
        clear_mem(&file_list, &word_search_result, files_amount);
        return -1;
    }

    if (DEBUG)
        printf("\tafter sort\n");
    print_res(word_search_result, files_amount);
    clear_mem(&file_list, &word_search_result, files_amount);
    return 0;
}

void clear_mem(char*** file_list, string_size_pair** res, size_t num) {
    for (size_t i = 0; i < num; i++) {
        if (DEBUG)
            printf("\twonna clear [%zu] %s\n", i, (*file_list)[i]);
        free((*file_list)[i]);
    }
    free(*file_list);
    free(*res);
}

void print_res(const string_size_pair* word_search_result, size_t files_amount) {
    for (size_t i = 0; i < files_amount; i++) {
        printf("%s: %zu\n", word_search_result[i].name, word_search_result[i].matches_amount);
    }
}
