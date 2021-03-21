cmake_minimum_required(VERSION 3.16)
project(hw1)
set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-g -coverage -O0 -Wall -Werror -Wpedantic -fprofile-arcs -ftest-coverage -fPIC")
set(CMAKE_CXX_FLAGS "-g -coverage -O0 -Wall -Werror -Wpedantic -fprofile-arcs -ftest-coverage -fPIC")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

include_directories(project/include/)
add_executable(program program.c word_finder.c)

#enable_testing()
#find_package(GTest REQUIRED)

#include_directories(${GTEST_INCLUDE_DIRS})
#message(${GTEST_LIBRARIES})
#message(${GTEST_INCLUDE_DIRS})

#add_executable(tests testing/tests.cpp)
#target_link_libraries(tests ${GTEST_LIBRARIES} -lpthread)

#add_test(program_and_functions_test ./tests)
#add_definitions(-fprofile-arcs -ftest-coverage)
#target_link_libraries(tests -fprofile-arcs)
