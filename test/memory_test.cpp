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

int *i, *j, *k;
float *l, *m, *n;

void integerTeardown(void *) {
    free(i);
    free(j);
    free(k);
}

void floatTeardown(void *) {
    free(l);
    free(m);
    free(n);
}

TEST singleInteger() {
    i = new int;
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), sizeof(int));

    delete i;
    i = NULL;
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 0);
    PASS();
}

TEST arrayInteger() {
    i = new int[3];
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 3 * sizeof(int));

    delete [] i;
    i = NULL;
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 0);
    PASS();
}

TEST arrayCombinedInteger() {
    i = new int[3];
    j = new int;
    k = new int[3];
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 7 * sizeof(int));

    delete [] i;
    delete j;
    delete [] k;
    i = NULL;
    j = NULL;
    k = NULL;
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 0);
    PASS();
}

SUITE(integerSuite) {
    SET_TEARDOWN(integerTeardown, NULL);
    RUN_TEST(singleInteger);
    RUN_TEST(arrayInteger);
    RUN_TEST(arrayCombinedInteger);
}

TEST singleFloat() {
    l = new float;
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), sizeof(float));

    delete l;
    l = NULL;
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 0);
    PASS();
}

TEST arrayFloat() {
    l = new float[3];
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 3 * sizeof(float));

    delete [] l;
    l = NULL;
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 0);
    PASS();
}

TEST arrayCombinedFloat() {
    l = new float[3];
    m = new float;
    n = new float[3];
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 7 * sizeof(float));

    delete [] l;
    delete m;
    delete [] n;
    l = NULL;
    m = NULL;
    n = NULL;
    ASSERTm("No valid red-black tree!", check_red_black_tree(MEMORY_INFO, true, true));
    ASSERT_EQm("Memory-Tracking faulty!", memory_used(MEMORY_USED_BY_PROGRAM), 0);
    PASS();
}

SUITE(floatSuite) {
    SET_TEARDOWN(floatTeardown, NULL);
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

