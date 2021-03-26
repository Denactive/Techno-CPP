#include "gtest/gtest.h"
#include <string>
#include <fstream>
#include <iostream>
#include <unistd.h>

#define BUFFER_SIZE 2048

extern "C" {
    #include "word_finder.h"
    #include "word_finder.c"
}

const char binary_file_name [] = "program";
const char binary_file_name_2 [] = "program-mt";

TEST(LogicTest, Main) {
    std::cout << "Logic test" << std::endl;
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

TEST(LogicTest, BigFiles) {
    std::cout << "Logic test - Big Files" << std::endl;
    std::string answer_filename = "../testing/bigfiles/answer.txt";
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

        execl(binary_file_name, binary_file_name, "../testing/bigfiles/", "test", NULL);

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

TEST(ErrorTest, EmptyFolder) {
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

TEST(ErrorTest, WrongDir) {
    std::cout << "Error test - No Directory" << std::endl;
    std::string answer_str = "Error | ../testing/no_dir opening failed\n";

    int fd[2];
    pipe(fd);
    pid_t pid_fork = fork();
    int status = 0;

    if (!pid_fork) {
        // Дочерний процесс
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);

        execl(binary_file_name, binary_file_name, "../testing/no_dir", "test", NULL);

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

TEST(ErrorTest, NoArgs) {
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

TEST(InnerFuncTest, lowercase) {
    std::cout << "Inner functions test - to_lower_case" << std::endl;
    char test_syms [] = "AbCd1234Z-_-+ =";
    char answer [] = "abcd1234z-_-+ =";
    for (int i = 0; test_syms[i] != '\0'; ++i)
        EXPECT_EQ (to_lower_case(test_syms[i]), answer[i]);
}

TEST(InnerFuncTest, get_files_from_dir_c) {
    std::cout << "Inner functions test - get_files_from_dir (.c)" << std::endl;
    char** file_list = NULL;
    size_t files_amount = 0;
    int err = get_files_from_dir("../testing/dir_reading/", ".c", &file_list, &files_amount);
    ASSERT_EQ(err, 0);
    std::string answer [] = {
            "../testing/dir_reading/file.c",
            "../testing/dir_reading/inner/inner-x2/inner_file_x2.c",
            "../testing/dir_reading/inner/inner_file.c"
    };

    for (size_t i = 0; i < files_amount; ++i)
        EXPECT_EQ (strcmp(file_list[i], answer[i].c_str()), 0);

    for (size_t i = 0; i < files_amount; i++)
        free(file_list[i]);
    free(file_list);
}

TEST(InnerFuncTest, get_files_from_dir_cpp) {
    std::cout << "Inner functions test - get_files_from_dir (.cpp)" << std::endl;
    char** file_list = NULL;
    size_t files_amount = 0;
    int err = get_files_from_dir("../testing/dir_reading/", ".cpp", &file_list, &files_amount);
    ASSERT_EQ(err, 0);
    std::string answer [] = {
            "../testing/dir_reading/file.cpp",
            "../testing/dir_reading/trash/trash_but_still_cpp.cpp",
            "../testing/dir_reading/inner_cpp/inner.cpp",
            "../testing/dir_reading/inner_cpp/inner_x2_cpp/inner_x2.cpp"
    };

    for (size_t i = 0; i < files_amount; ++i)
        EXPECT_EQ (strcmp(file_list[i], answer[i].c_str()), 0);

    for (size_t i = 0; i < files_amount; i++)
        free(file_list[i]);
    free(file_list);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}