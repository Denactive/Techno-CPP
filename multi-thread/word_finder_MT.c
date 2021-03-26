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
    // переместил эти определения сюда, чтобы компилятор gtest не ругался на goto
    // инициализацию оставил в том месте, где эти переменные используются
    string_sizex2* res = NULL;
    string_size_pair* sorted_by_size = NULL;
    char*** file_list_for_thread = NULL;
    size_t mod = 0;
    // распределим файлы в списке по убыванию их размера
    size_t* res_sizes = (size_t*)calloc(num_tr, sizeof(size_t));
    if (!res_sizes)
        return NULL;
    size_t* file_sizes = (size_t *)calloc(files_amount, sizeof(size_t));
    if (!file_sizes) {
        free(res_sizes);
        return NULL;
    }
    for (size_t i = 0; i < files_amount; ++i) {
        file_sizes[i] = gain_file_size(file_list[i]);
    }

    // чтобы не писать еще одну сортировку
    sorted_by_size = create_word_search_result(file_list, files_amount);
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
    mod = files_amount % num_tr;
    while (mod) {
        res_sizes[files_amount % num_tr - mod]++;
        mod--;
    }

    // распределяем файлы по потокам
    file_list_for_thread = (char***)calloc(num_tr, sizeof(char **));
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

void* thread_searcher(void* args) {
    if (!args)
        pthread_exit((void*)(-1));
    // int word_search(const char* pattern, string_size_pair** word_search_result, size_t files_amount);
    const char* pattern = ((thread_searcher_data*)args)->pattern;
    string_size_pair** res = ((thread_searcher_data*)args)->word_search_result;
    size_t files_amount = ((thread_searcher_data*)args)->files_amount;
    size_t tr_no = ((thread_searcher_data*)args)->tr_no;
    if (MT_DEBUG) {
            printf("\nThread %zu got %zu files:\n", tr_no, files_amount);
            for (size_t j = 0; j < files_amount; ++j) {
                printf("\t[%zu] %s | %zu\n", tr_no, (*res)[j].name, (*res)[j].matches_amount);
            }
    }
    if (word_search(pattern, res, files_amount))
        pthread_exit((void *)(2));

    if (merge_sort_desc(*res, files_amount)) {
        printf("Error | sort failed in [%zu] thread \n", tr_no);
        pthread_exit((void *)(-1));
    }
    pthread_exit(0);
}

string_size_pair* word_search_mt(const char* pattern, char** file_list, size_t files_amount, size_t num_tr) {
    if (!pattern || !files_amount || !num_tr)
        return NULL;

    // создаем промежуточную структуру, которая назначит каждому треду свой список файлов для обработки
    string_sizex2* res = create_word_search_result_mt(num_tr, file_list, files_amount);
    if (!res)
        return NULL;

    if (MT_DEBUG) {
        for (size_t i = 0; i < num_tr; ++i) {
            printf("Thread %zu gets %zu files:\n", i, res[i].size);
            for (size_t j = 0; j < res[i].size; ++j) {
                printf("\t%s | %zu\n", res[i].res[j].name, res[i].res[j].matches_amount);
            }
        }
    }

    // раздаем потокам задачи - выделенный в предыдущей структуре список файлов
    //pthread_t ptid[num_tr];
    pthread_t* ptid = (pthread_t*)calloc(num_tr, sizeof(pthread_t));
    if (!ptid) {
        free(res);
        return NULL;
    }
    thread_searcher_data* data = (thread_searcher_data*)calloc(num_tr, sizeof(thread_searcher_data));
    if (!data) {
        free(res);
        free(ptid);
        return NULL;
    }
    void* exit_status = 0;
    for (size_t i = 0; i < num_tr; ++i) {
        if (MT_DEBUG)
            printf("\tthread creation [%zu]\n", i);
        data[i].tr_no = i;
        data[i].files_amount = res[i].size;
        data[i].word_search_result = &(res[i].res);
        data[i].pattern = pattern;
        if (pthread_create(&ptid[i], NULL, thread_searcher, &(data[i]))) {
            if (MT_DEBUG)
                printf("\tthread creation failed\n");
            // не удалось выделить i-ый поток - сворачиваем остальные. Их ошибки не важны
            for (size_t j = 0; j < i; ++j)
                pthread_join(ptid[i], NULL);
            clear_word_search_result_mt(&res, num_tr);
            free(res);
            free(ptid);
            free(data);
            return NULL;
        }
    }
    free(data);

    // ожидание потоков, сбор ошибок
    int join_status = 0;
    for (size_t i = 0; i < num_tr; ++i) {
        pthread_join(ptid[i], (void**)&exit_status);
        if (exit_status == (void *)(-1) && exit_status == 0)
            join_status = -1;  // fatal
        if (exit_status == (void *)(2) && exit_status == 0)
            join_status = 2;  // NULL pattern or word_search_result addr | non fatal | impossible to get here
    }
    if (join_status == -1) {
        clear_word_search_result_mt(&res, num_tr);
        free(res);
        free(ptid);
        return NULL;
    }
    free(ptid);

    if (MT_DEBUG) {
        printf("\nafter sort\n");
        for (size_t i = 0; i < num_tr; ++i) {
            printf("Thread %zu gets %zu files:\n", i, res[i].size);
            for (size_t j = 0; j < res[i].size; ++j) {
                printf("\t%s | %zu\n", res[i].res[j].name, res[i].res[j].matches_amount);
            }
        }
    }

    // merge sorted results
    string_size_pair* sorted_res = (string_size_pair*)calloc(files_amount, sizeof(string_size_pair));
    if (!sorted_res) {
        clear_word_search_result_mt(&res, num_tr);
        free(res);
        free(ptid);
        return NULL;
    }
    // счетчики-итераторы для каждого из потоков
    size_t* pos = (size_t*)calloc(num_tr, sizeof(size_t));
//    for (size_t i = 0; i < num_tr; ++i)
//        pos[i] = 0;
    if (!pos) {
        clear_word_search_result_mt(&res, num_tr);
        free(res);
        free(ptid);
        return NULL;
    }
    size_t pos_sum = 0;  // сумма итераторов во всех массивых = итератору в итоговом массиве sorted_res
    size_t index_max = 0;  // храним не сам максимум, а его индекс
    while (pos_sum != files_amount) {
        // за начальное значение берем наименший по номеру тред, так, чтобы чтение по индексу i
        // было валидно, т.е. pos[index_max] < числа файлов в списке треда с номером index_max
        index_max = 0;
        while (res[index_max].size == pos[index_max])
            index_max++;
        for (size_t i = 1; i < num_tr; ++i) {
            // если элементы в массиве данного треда закончиись - пропустить
            if (res[i].size == pos[i])
                continue;
            if (res[i].res[pos[i]].matches_amount > res[index_max].res[pos[index_max]].matches_amount)
                index_max = i;
        }
        sorted_res[pos_sum].name = res[index_max].res[pos[index_max]].name;
        sorted_res[pos_sum].matches_amount = res[index_max].res[pos[index_max]].matches_amount;
        pos[index_max]++;
        pos_sum++;
    }
    // слили результаты успешно
    clear_word_search_result_mt(&res, num_tr);
    free(res);
    free(pos);
    return sorted_res;
}
