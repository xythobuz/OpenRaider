/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : MTK
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  :
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: Memory testing tool kit
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History ------------------------------------------------
 *
 * 2002.03.27:
 * Mongoose - Created
 ================================================================*/

#include <cstddef>

#ifndef __MTK_MONGOOSE_MEMORY_TEST_H_
#define __MTK_MONGOOSE_MEMORY_TEST_H_

#if defined(DEBUG_MEMORY) && !defined(UNIT_TEST_MEMORY)

#define DEBUG_NEW new(__FILE__, __LINE__)
void *operator new(size_t size, const char *file, int line);
void *operator new [](size_t size, const char *file, int line);

#define DEBUG_DELETE delete_check(__FILE__, __LINE__, 0);delete
void operator delete(void *p);
void operator delete [](void *p);

#else

#define DEBUG_NEW new
#define DEBUG_DELETE delete

#endif

#define new DEBUG_NEW
#define delete DEBUG_DELETE


void delete_check(const char *file, int line, int print);

void display_memory_usage();

long memory_used();
/*------------------------------------------------------
 * Pre  :
 * Post : Returns amount of total memory used
 *
 *-- History ------------------------------------------
 *
 * 2002.03.27:
 * Mongoose - Created
 ------------------------------------------------------*/

void dump_memory_report();
/*------------------------------------------------------
 * Pre  :
 * Post : Dumps raw Tree holding memory accounting
 *
 *-- History ------------------------------------------
 *
 * 2002.03.27:
 * Mongoose - Created
 ------------------------------------------------------*/

#ifdef DEBUG_MEMORY

#define DWORD unsigned long
#define ZERO_ALLOC_SLOTS 3

typedef enum { RB_BLACK = 0, RB_RED = 1 } rbtree_color_t;

typedef struct rbtree_s {
    void *data;
    DWORD key;
    rbtree_color_t color;
    struct rbtree_s *left;
    struct rbtree_s *right;
    struct rbtree_s *parent;
} rbtree_t;

typedef struct meminfo_filename_s {
    char *filename;
    char filename_len;
    DWORD size;
    unsigned int alloc_zero;
    unsigned short int alloc_zero_at_line[ZERO_ALLOC_SLOTS];
    struct meminfo_filename_s *next;
} meminfo_filename_t;

#endif

#endif
