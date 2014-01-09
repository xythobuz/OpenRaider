/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : MTK
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : memory_test
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: Memory testing tool kit
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History -------------------------------------------------
 *
 * 2002.03.27:
 * Mongoose - Created
 =================================================================*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <memory_test.h>

#define PRINT_TEST_BREAK_LINE	printf("\n############################################################\n\n");

int memory_test_unit_test(int argc, char *argv[])
{
	int *i, *j, *k, *l, *m, *n, *o;


	PRINT_TEST_BREAK_LINE
	printf("Memory test\n");
	PRINT_TEST_BREAK_LINE

	printf(">\ti = new int;  %s:%i\n", __FILE__, __LINE__);
	i = new int;
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);

	printf(">\tdelete i; (%p)  %s:%i\n", i, __FILE__, __LINE__);
	delete i;
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);

	PRINT_TEST_BREAK_LINE

	printf(">\ti = new int[3]; (%p)  %s:%i\n", i, __FILE__, __LINE__);
	i = new int[3];
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);

	printf(">\tdelete [] i; (%p)  %s:%i\n", i, __FILE__, __LINE__);
	delete [] i;
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);

	PRINT_TEST_BREAK_LINE

	printf(">\ti = new int[3]; (%p)  %s:%i\n", i, __FILE__, __LINE__);
	i = new int[3];
	printf(">\tj = new int; (%p)  %s:%i\n", j, __FILE__, __LINE__);
	j = new int;
	printf(">\tk = new int[3]; (%p)  %s:%i\n", k, __FILE__, __LINE__);
	k = new int[3];
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);
	printf(">\tdelete [] i; (%p)  %s:%i\n", i, __FILE__, __LINE__);
	delete [] i;
	printf(">\tdelete j; (%p)  %s:%i\n", j, __FILE__, __LINE__);
	delete j;
	printf(">\tdelete [] k; (%p)  %s:%i\n", k, __FILE__, __LINE__);
	delete [] k;
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);

	PRINT_TEST_BREAK_LINE

	printf(">\ti = new int[3]; (%p)  %s:%i\n", i, __FILE__, __LINE__);
	i = new int[3];
	printf(">\tj = new int; (%p)  %s:%i\n", j, __FILE__, __LINE__);
	j = new int;
	printf(">\tk = new int[3]; (%p)  %s:%i\n", k, __FILE__, __LINE__);
	k = new int[3];
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);
	printf(">\tdelete j; (%p)  %s:%i\n", j, __FILE__, __LINE__);
	delete j;
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);
	printf(">\tj = new int[3]; (%p)  %s:%i\n", j, __FILE__, __LINE__);
	j = new int[3];
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);
	printf(">\tl = new int[3]; (%p)  %s:%i\n", l, __FILE__, __LINE__);
	l = new int[3];
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);
	printf(">\tm = new int[3]; (%p)  %s:%i\n", m, __FILE__, __LINE__);
	m = new int[3];
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);

	printf(">\tn = new int[3]; (%p)  %s:%i\n", n, __FILE__, __LINE__);
	n = new int[3];
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);

	printf(">\to = new int[3]; (%p)  %s:%i\n", o, __FILE__, __LINE__);
	o = new int[3];
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);

	printf(">\tdelete [] k; (%p)  %s:%i\n", k, __FILE__, __LINE__);
	delete [] k;
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);

	printf(">\tdelete [] j; (%p)  %s:%i\n", j, __FILE__, __LINE__);
	delete [] j;
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);

	printf(">\tdelete [] l; (%p)  %s:%i\n", l, __FILE__, __LINE__);
	delete [] l;
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);

	printf(">\tdelete [] m; (%p)  %s:%i\n", m, __FILE__, __LINE__);
	delete [] m;
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);

	printf(">\tdelete [] n; (%p)  %s:%i\n", n, __FILE__, __LINE__);
	delete [] n;
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);

	printf(">\tdelete [] o; (%p)  %s:%i\n", o, __FILE__, __LINE__);
	delete [] o;
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);

	printf(">\tdelete [] i; (%p)  %s:%i\n", i, __FILE__, __LINE__);
	delete [] i;
	dump_memory_report();
	tree_valid_report(MEMORY_INFO);

	PRINT_TEST_BREAK_LINE

	return 0;
}


int main(int argc, char *argv[])
{
	printf("[memory_test test]\n");

	return memory_test_unit_test(argc, argv);
}

