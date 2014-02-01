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
extern meminfo_filename_t *MEMORY_FILENAME;
extern long MEMORY_USED;
extern long MEMORYA_USED;
extern long MEMORYC_USED;
extern long MAX_MEMORY_USED;
extern long MAX_MEMORYA_USED;
extern long MAX_MEMORYC_USED;
void tree_valid_report(rbtree_t *root);

TEST singleInteger() {
    int *i = new int;
    printf("\n>\ti = new int; (%p)  %s:%i\n", i, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);
    delete i;
    printf(">\tdelete i; (%p)  %s:%i\n", i, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);
    PASS();
}

TEST arrayInteger() {
    int *i = new int[3];
    printf("\n>\ti = new int[3]; (%p)  %s:%i\n", i, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);
    delete [] i;
    printf(">\tdelete [] i; (%p)  %s:%i\n", i, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);
    PASS();
}

TEST arrayCombinedInteger() {
    int *i = new int[3];
    printf("\n>\ti = new int[3]; (%p)  %s:%i\n", i, __FILE__, __LINE__);
    int *j = new int;
    printf(">\tj = new int; (%p)  %s:%i\n", j, __FILE__, __LINE__);
    int *k = new int[3];
    printf(">\tk = new int[3]; (%p)  %s:%i\n", k, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);
    delete [] i;
    printf(">\tdelete [] i; (%p)  %s:%i\n", i, __FILE__, __LINE__);
    delete j;
    printf(">\tdelete j; (%p)  %s:%i\n", j, __FILE__, __LINE__);
    delete [] k;
    printf(">\tdelete [] k; (%p)  %s:%i\n", k, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);
    PASS();
}

SUITE(integerSuite) {
    RUN_TEST(singleInteger);
    RUN_TEST(arrayInteger);
    RUN_TEST(arrayCombinedInteger);
}

GREATEST_MAIN_DEFS();

int main(int argc, char *argv[]) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(integerSuite);
    GREATEST_MAIN_END();
}

