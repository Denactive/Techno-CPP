//
// Created by denactive on 24.03.2021.
//

#include "word_finder_MT.h"

size_t gain_file_size(const char* filename);

// This function allocates memory!
// create_word_search_result_mt separates the file list for threads so each has
// approximately equal bytes in total and initializes complicated structure string_sizex2 array with
// amount of files in a thread's file list and initializes a 'res' field to save a searching statistics
string_sizex2* create_word_search_result_mt(size_t num_tr, char** file_list, size_t files_amount) {
    if (!file_list || !files_amount || !num_tr)
        return NULL;
    string_sizex2* res = NULL;
    // распределим файлы в списке по убыванию их размера
    size_t res_sizes[num_tr];
    size_t* file_sizes = (size_t *)calloc(files_amount, sizeof(size_t));
    if (!file_sizes)
        return NULL;
    for (size_t i = 0; i < files_amount; ++i) {
        file_sizes[i] = gain_file_size(file_list[i]);
    }

    // чтобы не писать еще одну сортировку
    string_size_pair* sorted_by_size = create_word_search_result(file_list, files_amount);
    if (!sorted_by_size)
        goto free_file_sizes;
    for (size_t i = 0; i < files_amount; ++i)
        sorted_by_size[i].matches_amount = file_sizes[i];
    if (num_tr > 1) {
        if (merge_sort_desc(sorted_by_size, files_amount))
            goto free_file_sizes;
    }
    // разделим равномерно по потокам в порядке убывания размера файлов (Round Robin по потокам)

    for (size_t i = 0; i < num_tr; ++i)
        res_sizes[i] = files_amount / num_tr;
    // остаток файлов тоже распределяем "равномерно"
    size_t mod = files_amount % num_tr;
    while (mod) {
        res_sizes[files_amount % num_tr - mod]++;
        mod--;
    }

    // распределяем файлы по потокам
    char*** file_list_for_thread = (char***)calloc(num_tr, sizeof(char **));
    if (!file_list_for_thread)
        goto free_file_sizes;
    for (size_t i = 0; i < num_tr; ++i) {
        file_list_for_thread[i] = (char **) calloc(res_sizes[i], sizeof(char *));
        if (!file_list_for_thread[i]) {
            for (size_t j = 0; j < i; ++j) {
                free(file_list_for_thread[j]);
            }
            goto free_sorted_by_size;
        }
    }
    for (size_t i = 0; i < files_amount; ++i) {
        size_t thread_no = i % num_tr;
        file_list_for_thread[thread_no][i / num_tr] = sorted_by_size[i].name;
    }

    // создание итоговой структуры с пустыми значениями числа вхождений (matches_amount)
    res = (string_sizex2 *)calloc(num_tr, sizeof(string_sizex2));
    if (!res)
        goto free_file_list_for_threads;
    for (size_t i = 0; i < num_tr; ++i) {
        res[i].res = create_word_search_result(file_list_for_thread[i], res_sizes[i]);
        res[i].size = res_sizes[i];
    }

    // cleaning up
    free_file_list_for_threads:
    for (size_t i = 0; i < num_tr; ++i)
        free(file_list_for_thread[i]);
    free(file_list_for_thread);
    free_sorted_by_size:
        free(sorted_by_size);
    free_file_sizes:
        free(file_sizes);
    return res;
}

void clear_word_search_result_mt(string_sizex2** word_search_result, size_t num_tr) {
    for (size_t i = 0; i < num_tr; ++i)
        free((*word_search_result)[i].res);
    free(*word_search_result);
    *word_search_result = NULL;
}

size_t gain_file_size(const char* filename) {
    int fdin = 0;
    if ((fdin = open(filename, O_RDONLY)) < 0) {
        if (DEBUG)
            printf("Error | Impossible to gain file size | Usage %s invalid\n", filename);
        return -1;
    }

    // gain file size
    struct stat stat_buf;
    if (fstat(fdin, &stat_buf) < 0) {
        if (DEBUG)
            printf("Error | Impossible to gain %s file size| fstat error\n", filename);
        return -1;
    }
    close(fdin);
    return stat_buf.st_size;
}

int word_search_mt(const char* pattern, string_sizex2** word_search_result) {
    return 0;
}
