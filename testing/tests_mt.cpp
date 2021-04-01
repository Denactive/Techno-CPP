#include "gtest/gtest.h"
#include <string>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 4096

extern "C" {
#include "word_finder_MT.h"
#include "word_finder.h"
}

void print_res(const string_size_pair* word_search_result, size_t files_amount) {
    for (size_t i = 0; i < files_amount; i++) {
        printf("%s: %zu\n", word_search_result[i].name, word_search_result[i].matches_amount);
    }
}

const char binary_file_name [] = "program-mt";

TEST(LogicTest, Main) {
    std::cout << "Logic test - Basic" << std::endl;
    std::string answer_filename = "../testing/logic/answer.txt";
    std::fstream is(answer_filename);
    ASSERT_TRUE(is.is_open());
    std::string answer_str;
    char sym = '\0';
    while (is.get(sym))
        answer_str += sym;
    is.close();

    int fd[2];
    pipe(fd);
    pid_t pid_fork = fork();
    int status = 0;

    if (!pid_fork) {
        // Дочерний процесс
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);

        execl(binary_file_name, binary_file_name, "../testing/logic/", "test", NULL);

        close(fd[1]);
        exit(0);
    }

    // Родительский процесс
    close(fd[1]);

    ssize_t sz;
    char* buf = (char*)calloc(BUFFER_SIZE, sizeof(char));
    int err = 0;
    if (!buf)
        err = -1;
    ASSERT_EQ(err, 0);

    sz = read(fd[0], buf, BUFFER_SIZE);
    close(fd[0]);
    ASSERT_GT(sz, 0);
    waitpid(pid_fork, &status, 0);

    EXPECT_STREQ(buf, answer_str.c_str());
    free(buf);
}

TEST(LogicTest, ManyFiles) {
    std::cout << "Logic test - Many Files" << std::endl;
    std::string answer_filename = "../testing/manyfiles/answer.txt";
    std::fstream is(answer_filename);
    ASSERT_TRUE(is.is_open());
    std::string answer_str;
    char sym = '\0';
    while (is.get(sym))
        answer_str += sym;
    is.close();

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

        std::string res_str;
        for (size_t j = 0; j < files_amount; ++j)
            res_str = res_str + std::string(res[j].name) + ": " + std::to_string(res[j].matches_amount) + '\n';

        ASSERT_STREQ(answer_str.c_str(), res_str.c_str());
        free(res);
        printf("\texecution time: %lf\n", (double)(end - start) / CLOCKS_PER_SEC/ i);
    }

    for (size_t i = 0; i < files_amount; i++)
        free(file_list[i]);
    free(file_list);
}

TEST(ErrorTest, NoArgs) {
    std::cout << "Error test - Empty folder" << std::endl;
    std::string answer_str = "no .c files found\n";

    int fd[2];
    pipe(fd);
    pid_t pid_fork = fork();
    int status = 0;

    if (!pid_fork) {
        // Дочерний процесс
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);

        execl(binary_file_name, binary_file_name, "../testing/empty_folder/", "test", NULL);

        close(fd[1]);
        exit(0);
    }

    // Родительский процесс
    close(fd[1]);

    ssize_t sz;
    char* buf = (char*)calloc(BUFFER_SIZE, sizeof(char));
    int err = 0;
    if (!buf)
        err = -1;
    ASSERT_EQ(err, 0);

    sz = read(fd[0], buf, BUFFER_SIZE);
    close(fd[0]);
    ASSERT_GT(sz, 0);
    waitpid(pid_fork, &status, 0);

    EXPECT_STREQ(buf, answer_str.c_str());
    free(buf);
}

TEST(ErrorTest, NoCFiles) {
    std::cout << "Error test - No .c files" << std::endl;
    std::string answer_str = "no .c files found\n";

    int fd[2];
    pipe(fd);
    pid_t pid_fork = fork();
    int status = 0;

    if (!pid_fork) {
        // Дочерний процесс
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);

        execl(binary_file_name, binary_file_name, "../testing/some .cpp project", "test", NULL);

        close(fd[1]);
        exit(0);
    }

    // Родительский процесс
    close(fd[1]);

    ssize_t sz;
    char* buf = (char*)calloc(BUFFER_SIZE, sizeof(char));
    int err = 0;
    if (!buf)
        err = -1;
    ASSERT_EQ(err, 0);

    sz = read(fd[0], buf, BUFFER_SIZE);
    close(fd[0]);
    ASSERT_GT(sz, 0);
    waitpid(pid_fork, &status, 0);

    EXPECT_STREQ(buf, answer_str.c_str());
    free(buf);
}

TEST(ErrorTest, NoArgsMT) {
    std::cout << "Error test - No Args" << std::endl;
    std::string answer_str = "Please pass a source path and a word to search\n";

    int fd[2];
    pipe(fd);
    pid_t pid_fork = fork();
    int status = 0;

    if (!pid_fork) {
        // Дочерний процесс
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);

        execl(binary_file_name, binary_file_name, "../testing/empty_folder/", NULL);

        close(fd[1]);
        exit(0);
    }

    // Родительский процесс
    close(fd[1]);

    ssize_t sz;
    char* buf = (char*)calloc(BUFFER_SIZE, sizeof(char));
    int err = 0;
    if (!buf)
        err = -1;
    ASSERT_EQ(err, 0);

    sz = read(fd[0], buf, BUFFER_SIZE);
    close(fd[0]);
    ASSERT_GT(sz, 0);
    waitpid(pid_fork, &status, 0);

    EXPECT_STREQ(buf, answer_str.c_str());
    free(buf);
}

TEST(CriticalTest, StressTest) {
    std::cout << "Critical test | Stress-test" << std::endl;

    char** file_list = NULL;
    size_t files_amount = 0;
    int err_dir = get_files_from_dir("../testing/manyfiles/", ".c", &file_list, &files_amount);
    ASSERT_EQ(err_dir, 0);
    ASSERT_EQ(files_amount, 500);

    string_size_pair* res = create_word_search_result(file_list, files_amount);
    ASSERT_NE(res, nullptr);
    word_search("test", &res, files_amount);
    int merge_res = merge_sort_desc(res, files_amount);
    ASSERT_EQ(merge_res, 0);

    string_size_pair* res_mt = NULL;
    res_mt = word_search_mt("test", file_list, files_amount, 4);
    ASSERT_NE(res_mt, nullptr);

    for (size_t i = 0; i < files_amount; ++i) {
        EXPECT_EQ(res[i].matches_amount, res_mt[i].matches_amount);
        EXPECT_STREQ(res[i].name, res[i].name);;
    }

    free(res);
    free(res_mt);
    for (size_t i = 0; i < files_amount; i++)
        free(file_list[i]);
    free(file_list);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}