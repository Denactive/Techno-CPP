#include "gtest/gtest.h"
#include <string>
#include <iostream>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 4096

extern "C" {
#include "word_finder_MT.h"
}

void print_res(const string_size_pair* word_search_result, size_t files_amount) {
    for (size_t i = 0; i < files_amount; i++) {
        printf("%s: %zu\n", word_search_result[i].name, word_search_result[i].matches_amount);
    }
}

TEST(SpeedTEST, MultiTread) {
    std::cout << "Speed test | Multi-thread" << std::endl;

    char** file_list = NULL;
    size_t files_amount = 0;
    int err_dir = get_files_from_dir("../testing/manyfiles/", ".c", &file_list, &files_amount);
    ASSERT_EQ(err_dir, 0);
    ASSERT_EQ(files_amount, 500);

    for (int i = 4; i <= 16; ++i) {
        std::cout << "\t" << i << " Threads:" << std::endl;

        string_size_pair* res = NULL;
        clock_t start = clock();
        res = word_search_mt("test", file_list, files_amount, i);
        clock_t end = clock();
        ASSERT_NE(res, nullptr);

        free(res);
        printf("\texecution time: %lf\n", (double)(end - start) / CLOCKS_PER_SEC/ i);
    }

    for (size_t i = 0; i < files_amount; i++)
        free(file_list[i]);
    free(file_list);
}

TEST(SpeedTEST, SingleTread) {
    std::cout << "Speed test | Single Thread" << std::endl;

    char** file_list = NULL;
    size_t files_amount = 0;
    int err_dir = get_files_from_dir("../testing/manyfiles/", ".c", &file_list, &files_amount);
    ASSERT_EQ(err_dir, 0);
    ASSERT_EQ(files_amount, 500);

    clock_t start = clock();
    string_size_pair* res = create_word_search_result(file_list, files_amount);
    ASSERT_NE(res, nullptr);
    word_search("test", &res, files_amount);
    int merge_res = merge_sort_desc(res, files_amount);
    ASSERT_EQ(merge_res, 0);
    clock_t end = clock();

    free(res);
    printf("\texecution time: %lf\n", (double)(end - start) / CLOCKS_PER_SEC);

    for (size_t i = 0; i < files_amount; i++)
        free(file_list[i]);
    free(file_list);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}