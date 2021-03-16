#include "gtest/gtest.h"
#include <string>
#include <fstream>
#include <iostream>
#include <unistd.h>

#define LOGIC_TESTS_COUNT 17
#define TESTS_OPEN_FILE_COUNT 4

extern "C" {
    #include "hw-1.h"
    #include "../project/src/hw-1.c"
};

// TODO: no console print in funcs, but print in main

TEST(LogicTest, Main) {
    std::cout << "Logic test" << std::endl;
    for (int i = 1; i <= LOGIC_TESTS_COUNT; i++) {
        std::string test_filename = "../testing/logic/cases/" + std::to_string(i) + ".txt";
        std::string answer_filename = "../testing/logic/answers/" + std::to_string(i) + ".txt";
        std::cout << "\tTesting " << test_filename << std::endl;
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

            execl("program", "program", test_filename.c_str(), NULL);

            close(fd[1]);
            exit(0);
        }

        // Родительский процесс
        close(fd[1]);

        ssize_t sz;
        char* buf = (char*)calloc(20 * BUFFER_SIZE, sizeof(char));
        int err = 0;
        if (!buf)
            err = -1;
        ASSERT_EQ(err, 0);

        sz = read(fd[0], buf, 20 * BUFFER_SIZE);
        close(fd[0]);
        /*if (sz > 0) {
            write(STDOUT_FILENO, buf, sz);
        }*/
        waitpid(pid_fork, &status, 0);

        EXPECT_STREQ(buf, answer_str.c_str());
        free(buf);
    }
}

TEST(OpenFile, EasyTest) {
    std::cout << "File opening test" << std::endl;
    for (int i = 1; i <= TESTS_OPEN_FILE_COUNT; i++) {
        std::string test_filename = "../testing/openfile/cases/" + std::to_string(i) + ".txt";
        std::string answer_filename = "../testing/openfile/answers/" + std::to_string(i) + ".txt";
        std::cout << "\tTesting " << test_filename << std::endl;
        std::fstream is(answer_filename);
        ASSERT_TRUE(is.is_open());
        std::string answer_str;
        char sym = '\0';
        while (is.get(sym))
            answer_str += sym;
        is.close();

        struct stat stat_buf;
        std::string res = file_input(test_filename.c_str(), &stat_buf);
        EXPECT_STREQ(res.c_str(), answer_str.c_str());
    }
}

TEST(ErrorTest, BadFile) {
    const char invalid_file [] = "not_existing_file.wtf";
    std::cout << "File opening test" << std::endl;
    std::cout << "\tTesting " << invalid_file << std::endl;

    struct stat stat_buf;
    char* res = file_input(invalid_file, &stat_buf);
    EXPECT_FALSE(res);
}

TEST(ErrorTest, SingleQuotesSkipping) {
    const char invalid_line [] = "' here we can see an opening quote, but no closing one";
    std::cout << "Error test | no closing single quote in a file" << std::endl;

    size_t cnt = 1;
    char* res = skip_to_single_quote(const_cast<char *>(&invalid_line[1]), &cnt);
    EXPECT_FALSE(res);
    EXPECT_EQ(cnt, 1);
}

TEST(ErrorTest, SingleQuotesSkippingMultiline) {
    const char invalid_line [] = "\" here we can see\nan opening quote,\nbut no closing one";
    std::cout << "Error test | no closing single quote in a multiline file" << std::endl;

    size_t cnt = 1;
    char* res = skip_to_single_quote(const_cast<char *>(&invalid_line[1]), &cnt);
    EXPECT_FALSE(res);
    EXPECT_EQ(cnt, 3);
}

TEST(ErrorTest, DoubleQuotesSkipping) {
    const char invalid_line [] = "' here we can see an opening quote, but no closing one";
    std::cout << "Error test | no closing double quote in a file" << std::endl;

    size_t cnt = 1;
    char* res = skip_to_single_quote(const_cast<char *>(&invalid_line[1]), &cnt);
    EXPECT_FALSE(res);
    EXPECT_EQ(cnt, 1);
}

TEST(ErrorTest, DoubleQuotesSkippingMultiline) {
    const char invalid_line [] = "\" here we can see\nan opening quote,\nbut no closing one";
    std::cout << "Error test | no closing double quote in a multiline file" << std::endl;

    size_t cnt = 1;
    char* res = skip_to_single_quote(const_cast<char *>(&invalid_line[1]), &cnt);
    EXPECT_FALSE(res);
    EXPECT_EQ(cnt, 3);
}

TEST(EdgeTest, SlashesAfterOpeningMultilineComment) {
    const char line [] = "/this is fucked up multiline\n/*/";
    const char answer [] = "/this is fucked up multiline\n/";
    std::cout << "EdgeTest | /*/ comment /*/ situation" << std::endl;

    size_t line_cnt = 1;
    size_t comments_cnt = 0;
    comment* res = NULL;
    execute_multiline_comment(const_cast<char *>(line), &line_cnt, &res, &comments_cnt);

    ASSERT_NE(res, nullptr);
    EXPECT_EQ(line_cnt, 2);
    EXPECT_EQ(comments_cnt, 1);
    EXPECT_EQ(res[0].type, multiline);
    EXPECT_EQ(res[0].line, 1);
    std::string res_line;
    for (char* i = res[0].beg; i <= res[0].end; i++)
        res_line += *i;
    EXPECT_STREQ(res_line.c_str(), answer);
    free_res(&res);
}

TEST(EdgeTest, ShieldingSingleQuotes) {
    const char line_const [] = "this is a \\'line'";
    char* line = (char*)malloc(strlen(line_const) + 1);
    ASSERT_NE(line, nullptr);
    strcpy(line, line_const);
    line[strlen(line_const)] = '\0';

    const char answer [] = "this is a \\'line";
    std::cout << "EdgeTest | \\' situation" << std::endl;

    size_t line_cnt = 1;
    char* closing_quote = skip_to_single_quote(line, &line_cnt);

    ASSERT_NE(closing_quote, nullptr);
    EXPECT_EQ(line_cnt, 1);
    std::string res_line;
    for (char* i = line; i < closing_quote; i++)
        res_line += *i;
    EXPECT_STREQ(res_line.c_str(), answer);
    free(line);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}