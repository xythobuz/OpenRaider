/*!
 * \file test/memory_test.cpp
 * Memory testing Toolkit Unit test
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "greatest.h"
#include <memory_test.h>

extern rbtree_t *MEMORY_INFO;
bool check_red_black_tree(rbtree_t *current, bool valid, bool strict);

TEST singleInteger() {
    int *i = new int;
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), sizeof(int));

    delete i;
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 0);
    PASS();
}

TEST arrayInteger() {
    int *i = new int[3];
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 3 * sizeof(int));

    delete [] i;
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 0);
    PASS();
}

TEST arrayCombinedInteger() {
    int *i = new int[3];
    int *j = new int;
    int *k = new int[3];
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 7 * sizeof(int));

    delete [] i;
    delete j;
    delete [] k;
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 0);
    PASS();
}

SUITE(integerSuite) {
    RUN_TEST(singleInteger);
    RUN_TEST(arrayInteger);
    RUN_TEST(arrayCombinedInteger);
}

TEST singleFloat() {
    float *i = new float;
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), sizeof(float));

    delete i;
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 0);
    PASS();
}

TEST arrayFloat() {
    float *i = new float[3];
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 3 * sizeof(float));

    delete [] i;
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 0);
    PASS();
}

TEST arrayCombinedFloat() {
    float *i = new float[3];
    float *j = new float;
    float *k = new float[3];
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 7 * sizeof(float));

    delete [] i;
    delete j;
    delete [] k;
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 0);
    PASS();
}

SUITE(floatSuite) {
    RUN_TEST(singleFloat);
    RUN_TEST(arrayFloat);
    RUN_TEST(arrayCombinedFloat);
}

GREATEST_MAIN_DEFS();

int main(int argc, char *argv[]) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(integerSuite);
    RUN_SUITE(floatSuite);
    GREATEST_MAIN_END();
}

