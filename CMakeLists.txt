cmake_minimum_required(VERSION 3.16)
project(iz-2)
set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-g -coverage -O0 -Wall -Werror -Wpedantic -fprofile-arcs -ftest-coverage -fPIC")
set(CMAKE_CXX_FLAGS "-g -coverage -O0 -Wall -Werror -Wpedantic -fprofile-arcs -ftest-coverage -fPIC")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)


include_directories(single-process/)
# static and shared versions
add_library(word_finder_static STATIC single-process/word_finder.c)
add_library(word_finder_shared SHARED single-process/word_finder.c)
add_executable(program single-process/program.c)
#target_link_libraries(program word_finder)
target_link_libraries(program word_finder_shared)


set(MULTITHREAD_PROJ_DIR multi-thread/)
find_library(PTHREAD_LIBRARY pthread)
include_directories(${MULTITHREAD_PROJ_DIR})
# shared only
add_library(word_finder-mt SHARED ${MULTITHREAD_PROJ_DIR}/word_finder_MT.c)
target_link_libraries(word_finder-mt word_finder_shared)
add_executable(program-mt ${MULTITHREAD_PROJ_DIR}/program-MT.c)
target_link_libraries(program-mt word_finder-mt ${PTHREAD_LIBRARY} -lpthread)


enable_testing()
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
message(${GTEST_LIBRARIES})
message(${GTEST_INCLUDE_DIRS})

add_executable(tests testing/tests.cpp)
target_link_libraries(tests word_finder_shared ${GTEST_LIBRARIES} -lpthread)

add_test(test ./tests)
add_definitions(-fprofile-arcs -ftest-coverage)
target_link_libraries(tests -fprofile-arcs)


add_executable(tests-mt testing/tests_mt.cpp)
target_link_libraries(tests-mt word_finder-mt ${GTEST_LIBRARIES} -lpthread)

add_test(test-mt ./tests-mt)
add_definitions(-fprofile-arcs -ftest-coverage)
target_link_libraries(tests-mt -fprofile-arcs)