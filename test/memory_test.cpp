/*!
 * \file test/memory_test.cpp
 * Memory testing Toolkit Unit test
 *
 * \author Mongoose
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

#define PRINT_TEST_BREAK_LINE   printf("\n############################################################\n\n");

int memory_test_unit_test() {
    int *i, *j, *k, *l, *m, *n, *o;

    PRINT_TEST_BREAK_LINE
        printf("Memory test\n");
    PRINT_TEST_BREAK_LINE

    i = new int;
    printf(">\ti = new int; (%p)  %s:%i\n", i, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);

    delete i;
    printf(">\tdelete i; (%p)  %s:%i\n", i, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);

    PRINT_TEST_BREAK_LINE

    i = new int[3];
    printf(">\ti = new int[3]; (%p)  %s:%i\n", i, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);

    delete [] i;
    printf(">\tdelete [] i; (%p)  %s:%i\n", i, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);

    PRINT_TEST_BREAK_LINE

    i = new int[3];
    printf(">\ti = new int[3]; (%p)  %s:%i\n", i, __FILE__, __LINE__);
    j = new int;
    printf(">\tj = new int; (%p)  %s:%i\n", j, __FILE__, __LINE__);
    k = new int[3];
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

    PRINT_TEST_BREAK_LINE

    i = new int[3];
    printf(">\ti = new int[3]; (%p)  %s:%i\n", i, __FILE__, __LINE__);
    j = new int;
    printf(">\tj = new int; (%p)  %s:%i\n", j, __FILE__, __LINE__);
    k = new int[3];
    printf(">\tk = new int[3]; (%p)  %s:%i\n", k, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);
    delete j;
    printf(">\tdelete j; (%p)  %s:%i\n", j, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);
    j = new int[3];
    printf(">\tj = new int[3]; (%p)  %s:%i\n", j, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);
    l = new int[3];
    printf(">\tl = new int[3]; (%p)  %s:%i\n", l, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);
    m = new int[3];
    printf(">\tm = new int[3]; (%p)  %s:%i\n", m, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);

    n = new int[3];
    printf(">\tn = new int[3]; (%p)  %s:%i\n", n, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);

    o = new int[3];
    printf(">\to = new int[3]; (%p)  %s:%i\n", o, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);

    delete [] k;
    printf(">\tdelete [] k; (%p)  %s:%i\n", k, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);

    delete [] j;
    printf(">\tdelete [] j; (%p)  %s:%i\n", j, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);

    delete [] l;
    printf(">\tdelete [] l; (%p)  %s:%i\n", l, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);

    delete [] m;
    printf(">\tdelete [] m; (%p)  %s:%i\n", m, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);

    delete [] n;
    printf(">\tdelete [] n; (%p)  %s:%i\n", n, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);

    delete [] o;
    printf(">\tdelete [] o; (%p)  %s:%i\n", o, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);

    delete [] i;
    printf(">\tdelete [] i; (%p)  %s:%i\n", i, __FILE__, __LINE__);
    dump_memory_report();
    tree_valid_report(MEMORY_INFO);

    PRINT_TEST_BREAK_LINE

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 1) {
        printf("Usage:\n\t%s\n", argv[0]);
        return 1;
    } else {
        printf("[memory_test test]\n");
        return memory_test_unit_test();
    }
}

