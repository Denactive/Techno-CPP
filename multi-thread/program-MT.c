//
// Created by denactive on 24.03.2021.
//

#include "word_finder_MT.h"
#include <dlfcn.h>

void print_res(const string_size_pair* word_search_result, size_t files_amount);
void clear_file_list(char*** file_list, size_t files_amount);

int main(int argc, const char** argv) {
    if (argc < 3) {
        printf("Please pass a source path and a word to search\n");
        return 1;
    }
    size_t num_proc = get_nprocs();
    if (DEBUG)
        printf("detected %zu available processors detected\n", num_proc);
    size_t num_tr = num_proc / 2;

    void *ext_lib = dlopen("./libword_finder-mt.so",RTLD_LAZY);
    if (!ext_lib){
        //если ошибка, то вывести ее на экран
        fprintf(stderr,"dlopen() error: %s\n", dlerror());
        return 1;
    }
    int (*load_files)(const char* dir_name, const char* extension, char*** file_list, size_t* cnt);
    string_size_pair* (*multithread_search)(const char* pattern, char** file_list, size_t files_amount, size_t num_tr);

    load_files = dlsym(ext_lib, "get_files_from_dir");
    if (!load_files)
        return 1;
    multithread_search = dlsym(ext_lib, "word_search_mt");
    if (!multithread_search)
        return 1;

    char** file_list = NULL;
    size_t files_amount = 0;
//    if (get_files_from_dir(argv[1], ".c", &file_list, &files_amount))
    if ((*load_files)(argv[1], ".c", &file_list, &files_amount))
        return 1;

    string_size_pair* res = (*multithread_search)(argv[2], file_list, files_amount, num_tr);
    if (!res && !files_amount) {
        printf("no .c files found\n");
        clear_file_list(&file_list, files_amount);
        return 0;
    }

    if (!res) {
        clear_file_list(&file_list, files_amount);
        return 1;
    }

    print_res(res, files_amount);
    for (size_t i = 0; i < files_amount; ++i)
        free(res[i].name);
    free(res);
    clear_file_list(&file_list, files_amount);

    dlclose(ext_lib);
    pthread_exit(NULL);
}

void clear_file_list(char*** file_list, size_t files_amount) {
    if (!file_list)
        return;
    for (size_t i = 0; i < files_amount; ++i) {
        if (DEBUG)
            printf("\twonna clear [%zu] %s\n", i, (*file_list)[i]);
        free((*file_list)[i]);
    }
    free(*file_list);
}

void print_res(const string_size_pair* word_search_result, size_t files_amount) {
    if (!word_search_result)
        return;
    for (size_t i = 0; i < files_amount; ++i) {
        printf("%s: %zu\n", word_search_result[i].name, word_search_result[i].matches_amount);
    }
}
