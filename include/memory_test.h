/*!
 * \file include/memory_test.h
 * Memory testing Toolkit
 *
 * \author Mongoose
 */

#ifndef _MEMORY_TEST_H_
#define _MEMORY_TEST_H_

#include <cstddef>

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

typedef enum {
    MEMORY_USED_BY_PROGRAM      = 1,
    MEMORY_USED_BY_OVERHEAD     = 2,
    MEMORY_USED_TOTAL           = 3,
    MAX_MEMORY_USED_BY_PROGRAM  = 4,
    MAX_MEMORY_USED_BY_OVERHEAD = 5
} memory_query_t;

void delete_check(const char *file, int line, int print);

void display_memory_usage();

/*!
 * \brief Get total memory usage
 * \param query what to return
 * \returns amount of requested memory used
 */
long memory_used(memory_query_t query);

/*!
 * \brief Dumps raw Tree holding memory accounting
 */
void dump_memory_report();

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
    unsigned int alloc_zero_at_line[ZERO_ALLOC_SLOTS];
    struct meminfo_filename_s *next;
} meminfo_filename_t;

#endif

#endif
