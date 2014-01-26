/*!
 * \file src/memory_test.cpp
 * Memory testing Toolkit
 *
 * \author Mongoose
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define UNIT_TEST_MEMORY
#include <memory_test.h>

#define USE_ITERATIVE_TREE_INSERT


bool tree_check(rbtree_t *tree, const char *file, int line)
{
    bool ret = true;


    if (tree)
    {
        if (tree == tree->parent ||
                tree == tree->right ||
                tree == tree->left)
        {
            printf("\nERROR: Self pointing   %s:%i\n", file, line);
            ret = false;
        }

        if (tree->parent &&
                (tree->parent == tree->left || tree->parent == tree->right))
        {
            printf("\nERROR: Parent is also child   %s:%i\n", file, line);
            ret = false;
        }

        if (tree->left && tree->left == tree->right)
        {
            printf("\nERROR: Duplicate children   %s:%i\n", file, line);
            ret = false;
        }
    }

    if (!ret)
    {
        printf("ERROR: Left %p, Parent %p, Right %p, This %p\a\n",
                tree->left, tree->parent, tree->right, tree);
    }

    return ret;
}

#define CHECK_TREENODE(tree) if (!tree_check(tree, __FILE__, __LINE__)) {exit(0);}


bool check_red_black_tree(rbtree_t *current, bool valid, bool strict)
{
    rbtree_t *right, *left;
    int color_red;


    if (!current)
    {
        return valid;
    }

    if (strict)
    {
        CHECK_TREENODE(current);
    }

    // Mongoose 2002.02.19, Check for a red root
    if (!current->parent && current->color == RB_RED)
    {
        printf("Invalid root color\n");
        return false;
    }

    color_red = (current->color == RB_RED);
    left = current->left;
    right = current->right;

    // Mongoose 2002.02.19, Check for adj red nodes
    if (left)
    {
        if (color_red && left->color == RB_RED)
        {
            printf("Invalid adjacent red colors in a left branch, %p\n",
                    left);
            return false;
        }

        if (!check_red_black_tree(left, valid, strict))
        {
            return false;
        }
    }

    if (right)
    {
        if (color_red && right->color == RB_RED)
        {
            printf("Invalid adjacent red colors in a right branch, %p\n",
                    right);
            return false;
        }

        if (!check_red_black_tree(right, valid, strict))
        {
            return false;
        }
    }

    return true;
}


void tree_valid_report(rbtree_t *root)
{
    printf("Is valid red-black tree? %s\n",
            check_red_black_tree(root, true, true) ? "true" : "false \a");
}


rbtree_t *tree_find(rbtree_t *tree, DWORD key)
{
    if (!tree)
        return 0;

    if (key == tree->key)
    {
        return tree;
    }
    else if (tree->left && key < tree->key)
    {
        return tree_find(tree->left, key);
    }
    else if (tree->right)
    {
        return tree_find(tree->right, key);
    }

    return 0;
}


rbtree_t *tree_new(void *data, DWORD key)
{
    rbtree_t *tree;


    tree = (rbtree_t *)malloc(sizeof(rbtree_t));
    tree->parent = 0;
    tree->left = 0;
    tree->right = 0;
    tree->data = data;
    tree->color = RB_RED;
    tree->key = key;

    return tree;
}


void tree_rotate_left(rbtree_t **root, rbtree_t *tree)
{
    rbtree_t *right, *right_leftchild, *uncle;


    if (!tree || !(*root))
    {
        return;
    }

    // Get tree's right node
    right = tree->right;

    // Get right node's left child
    if (right)
    {
        right_leftchild = right->left;
    }
    else
    {
        right_leftchild = 0;
    }

    // Set tree's right node to right's left child
    tree->right = right_leftchild;

    // Child now has a new parent
    if (right_leftchild)
    {
        right_leftchild->parent = tree;
    }

    // Right also has a new parent
    if (right)
    {
        right->parent = tree->parent;
    }

    if (tree->parent)  // Not root
    {
        uncle = (tree->parent)->left;

        // Mix up at hosptial, switch parent's children!
        if (tree == uncle)
        {
            (tree->parent)->left = right;
        }
        else
        {
            (tree->parent)->right = right;
        }
    }
    else // TreeNode 'tree' was root, so now right is root
    {
        *root = right;
    }

    if (right)
        right->left = tree;

    tree->parent = right;
}


void tree_rotate_right(rbtree_t **root, rbtree_t *tree)
{
    rbtree_t *left, *left_rightchild, *parent, *uncle;


    if (!tree || !(*root))
    {
        return;
    }

    left = tree->left;

    left_rightchild = 0;

    if (left)
    {
        left_rightchild = left->right;
    }

    tree->left = left_rightchild;

    if (left_rightchild)
    {
        left_rightchild->parent = tree;
    }

    if (left)
    {
        left->parent = tree->parent;
    }

    parent = tree->parent;

    if (parent)    //if node is not the root
    {
        uncle = parent->right;

        if (tree == uncle)
        {
            parent->right = left;
        }
        else
        {
            parent->left = left;
        }
    }
    else
    {
        *root = left;
    }

    left->right = tree;
    tree->parent = left;
}


rbtree_t *tree_insert(rbtree_t **root, void *data, DWORD key)
{
    rbtree_t *tree, *prev, *uncle;


    // Allocate and store new node in tree ////////

    if (!(*root))
    {
        *root = tree_new(data, key);
        (*root)->color = RB_BLACK;
        return (*root);
    }

    tree = *root;
    prev = 0x0;

    do
    {
        prev = tree;

        if (key < prev->key)
        {
            tree = prev->left;
        }
        else if (key > prev->key)
        {
            tree = prev->right;
        }
        else // Duplicate
        {
            return 0x0;  // Don't allow duplicates
        }

    }   while (tree);


    if (key < prev->key)
    {
        prev->left = tree_new(data, key);
        tree = prev->left;
    }
    else
    {
        prev->right = tree_new(data, key);
        tree = prev->right;
    }

    tree->parent = prev;
    prev = tree;

    // Now do red-black fix for insertion ///////////////////

    while ((tree != (*root)) && tree->parent->color == RB_RED)
    {
        // Parent is a left child
        if (tree->parent == tree->parent->parent->left)
        {
            // Tree's right uncle
            uncle = tree->parent->parent->right;

            // Parent and Uncle are RED
            if ((uncle) && (uncle->color == RB_RED))
            {
                // Change Parent and Uncle to BLACK, make grandparent RED
                tree->parent->color = RB_BLACK;
                uncle->color = RB_BLACK;
                tree->parent->parent->color = RB_RED;

                // Continue from grandparent
                tree = tree->parent->parent;
            }
            else  // Parent is RED and Uncle is BLACK
            {
                // Tree is a right child
                if (tree == tree->parent->right)
                {
                    tree_rotate_left(root, tree->parent);
                    tree = tree->left;
                }

                tree->parent->color = RB_BLACK;
                tree->parent->parent->color = RB_RED;
                tree_rotate_right(root, tree->parent->parent);
            }
        }
        else // Parent is a right child
        {
            // Tree's left uncle
            uncle = tree->parent->parent->left;

            // Parent and Uncle are RED
            if ((uncle) && (uncle->color == RB_RED))
            {
                // Change Parent and Uncle to BLACK, make grandparent RED
                tree->parent->color = RB_BLACK;
                uncle->color = RB_BLACK;
                tree->parent->parent->color = RB_RED;

                // Continue from grandparent
                tree = tree->parent->parent;
            }
            else  // Parent is RED and Uncle is BLACK
            {
                // Tree is a left child
                if (tree == tree->parent->left)
                {
                    tree_rotate_right(root, tree->parent);
                    tree = tree->right;
                }

                tree->parent->color = RB_BLACK;
                tree->parent->parent->color = RB_RED;
                tree_rotate_left(root, tree->parent->parent);
            }
        }
    }

    (*root)->color = RB_BLACK;

    return prev;
}


int tree_print(rbtree_t *tree, void (*print_func)(void *))
{
    int i = 0;

    if (!tree)
        return 0;

    if (tree->left)
    {
        i += tree_print(tree->left, print_func);
        printf(",\n");
    }

    if (tree->data && print_func)
    {
        (*print_func)(tree->data);

#ifdef DEBUG_MEMORY_RBTREE
        printf(" :%s%s)",
                (!tree->parent) ? " (root, " : " ("),
            (tree->color == RB_BLACK) ? "black" : "red");
#endif
    }

    ++i;

    if (tree->right)
    {
        printf(",\n");
        i += tree_print(tree->right, print_func);
    }

    if (!tree->parent)
    {
        printf("\n%i nodes, %lu bytes each : %lu bytes\n",
                i, sizeof(rbtree_t), i * sizeof(rbtree_t));
    }

    return i;
}


rbtree_t *tree_get_successor(rbtree_t *root, rbtree_t *tree)
{
    rbtree_t    *successor;


    successor = tree->right;

    if (successor)
    {
        while (successor->left)
        {
            successor = successor->left;
        }

        return successor;
    }
    else
    {
        successor = tree->parent;

        while (tree == successor->right)
        {
            tree = successor;
            successor = successor->parent;
        }

        if (successor == root)
        {
            return 0;
        }

        return successor;
    }
}


rbtree_t *tree_get_predecessor(rbtree_t *root,rbtree_t *tree)
{
    rbtree_t *predecessor;


    predecessor = tree->left;

    if (predecessor)
    {
        while (predecessor->right)
        {
            predecessor = predecessor->right;
        }

        return predecessor;
    }
    else
    {
        predecessor = tree->parent;

        while (tree == predecessor->left)
        {
            if (predecessor == root)
            {
                return 0;
            }

            tree = predecessor;
            predecessor = predecessor->parent;
        }

        return predecessor;
    }
}


void tree_restore_after_remove(rbtree_t **root, rbtree_t *tree)
{
    rbtree_t    *parent, *sibling, *sleft, *sright;


    if (!tree || !(*root))
    {
        return;
    }

    parent = tree->parent;

    while ((tree != (*root)) && (tree->color == RB_BLACK)) // was parent->color
    {
        if (tree == parent->left)
        {
            sibling = parent->right;

            if (sibling && sibling->color == RB_RED)
            {
                sibling->color = RB_BLACK;
                parent->color = RB_RED;
                tree_rotate_left(root, tree->parent);
                parent = tree->parent;
                sibling = parent->right;
            }

            if (sibling)
            {
                sleft = sibling->left;
                sright = sibling->right;
            }
            else
            {
                sleft = sright = NULL;
            }

            if ((!sright || sright->color == RB_BLACK) &&
                    (!sleft || sleft->color == RB_BLACK))
            {
                if (sibling)
                    sibling->color = RB_RED;

                tree = parent;
            }
            else
            {
                if (!sright || sright->color == RB_BLACK)
                {
                    sibling->color = RB_RED;
                    sleft->color = RB_BLACK;
                    tree_rotate_right(root, sibling);
                    sibling = parent->right;
                    sright = sibling->right;
                    sleft = sibling->left;
                }

                sibling->color = parent->color;
                parent->color = RB_BLACK;
                sright->color = RB_BLACK;
                tree_rotate_left(root, tree->parent);
                parent = tree->parent;
                tree = (*root);
            }
        }
        else
        {
            sibling = parent->left;

            if (sibling && sibling->color == RB_RED)
            {
                sibling->color = RB_BLACK;
                parent->color = RB_RED;
                tree_rotate_right(root, tree->parent);
                parent = tree->parent;
                sibling = parent->left;
            }

            if (sibling)
            {
                sleft = sibling->left;
                sright = sibling->right;
            }
            else
            {
                sleft = sright = 0;
            }

            if ((!sright || sright->color == RB_BLACK) &&
                    (!sleft || sleft->color == RB_BLACK))
            {
                if (sibling)
                    sibling->color = RB_RED;

                tree = parent;
            }
            else
            {
                if (!sleft || sleft->color == RB_BLACK)
                {
                    sibling->color = RB_RED;
                    sright->color = RB_BLACK;
                    tree_rotate_left(root, sibling);
                    sibling = parent->left;
                }

                sibling->color = parent->color;
                parent->color = RB_BLACK;
                if (sleft) sleft->color = RB_BLACK;
                tree_rotate_right(root, tree->parent);
                parent = tree->parent;
                tree = (*root);
            }
        }

        parent = tree->parent;
    }

    tree->color = RB_BLACK;
}


void tree_remove(rbtree_t **root, rbtree_t *tree)
{
    rbtree_t *left, *right, *parent, *prev, *cur;


    // Mongoose 2002.02.16, Nothing to remove
    if (!tree || !(*root))
    {
        return;
    }

    left = tree->left;
    right = tree->right;
    parent = tree->parent;

    if (!left || !right)
    {
        prev = tree;
    }
    else
    {
        prev = tree_get_successor((*root), tree);
    }

    if (prev->left)
    {
        cur = prev->left;
    }
    else
    {
        cur = prev->right;
    }

    if (cur)
    {
        cur->parent = prev->parent;
    }

    if (!prev->parent)
    {
        *root = cur;
    }
    else
    {
        parent = prev->parent;

        if (prev == parent->left)
        {
            parent->left = cur;
        }
        else
        {
            parent->right = cur;
        }
    }

    if (prev != tree)
    {
        tree->data = prev->data;
        tree->key = prev->key;
    }

    if (prev->color == RB_BLACK)
    {
        tree_restore_after_remove(root, cur);
    }

    free(prev);
}


#ifdef DEBUG_MEMORY


typedef struct meminfo_s
{
    DWORD address;
    DWORD size;
    unsigned short int line;
    meminfo_filename_t *filename;

} meminfo_t;


void __print_dword(DWORD d)
{
    printf("0x%08lx", d);
}


void __print_meminfo(meminfo_t *meminfo)
{
    if (meminfo)
    {
        printf("<addr 0x%08lx, sz %lu, \"%s\":%u>",
                meminfo->address,
                meminfo->size,
                (!meminfo->filename) ? "?" : meminfo->filename->filename,
                meminfo->line);
    }
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

rbtree_t *MEMORY_INFO = 0;
meminfo_filename_t *MEMORY_FILENAME = 0;
long MEMORY_USED  = 0L;
long MEMORYA_USED = 0L;
long MEMORYC_USED = 0L;
long MAX_MEMORY_USED = 0L;
long MAX_MEMORYA_USED = 0L;
long MAX_MEMORYC_USED = 0L;


typedef enum
{
    MEMORY_USED_BY_PROGRAM      = 1,
    MEMORY_USED_BY_OVERHEAD     = 2,
    MEMORY_USED_TOTAL           = 3,
    MAX_MEMORY_USED_BY_PROGRAM  = 4,
    MAX_MEMORY_USED_BY_OVERHEAD = 5

} memory_query_t;

long memory_used(memory_query_t query)
{
    switch (query)
    {
        case MEMORY_USED_BY_PROGRAM:
            return MEMORY_USED;
            break;
        case MAX_MEMORY_USED_BY_PROGRAM:
            return MAX_MEMORY_USED;
            break;
        case MAX_MEMORY_USED_BY_OVERHEAD:
            return MAX_MEMORYA_USED + MAX_MEMORYC_USED;
            break;
        case MEMORY_USED_BY_OVERHEAD:
            return MEMORYA_USED + MEMORYC_USED;
            break;
        case MEMORY_USED_TOTAL:
            return MEMORY_USED + MEMORYA_USED + MEMORYC_USED;
            break;
        default:
            ;
    }

    return 0;
}


void display_memory_usage()
{
    unsigned int i;
    meminfo_filename_t *cur = MEMORY_FILENAME;


    printf("\n============================================================\n");

    printf("Memory usage per file:\n");

    while (cur)
    {
        printf(" %s : ( %lu bytes, %3.2f%% )\n",
                cur->filename, cur->size,
                100.0 * ((float)cur->size / (float)MEMORY_USED));

        cur = cur->next;
    }

    printf("------------------------------------------------------------\n");

    cur = MEMORY_FILENAME;

    printf("Memory special errors per file:\n");

    while (cur)
    {
        if (cur->alloc_zero)
        {
            printf(" %s : %u zero allocation errors\n",
                    cur->filename, cur->alloc_zero);

            printf("    lines:");

            for (i = 0; i < ZERO_ALLOC_SLOTS; ++i)
            {
                if (cur->alloc_zero_at_line[i])
                {
                    printf("   %u",
                            cur->alloc_zero_at_line[i]);
                    break;
                }
            }

            printf("\n");
        }

        cur = cur->next;
    }

    printf("------------------------------------------------------------\n");

    printf("Memory usage summary:\n");

    printf(" Tracked program memory    : %lu bytes \t(%.2f MB)\n",
            MEMORY_USED, (double)MEMORY_USED / 1024000.0);
    printf(" Untracked overhead memory : %lu bytes \t(%.2f MB)\n",
            MEMORYA_USED, (double)MEMORYA_USED / 1024000.0);
    printf(" Untracked m-string memory : %lu bytes\n",
            MEMORYC_USED);

    printf("\n Total accounted memory    : %lu bytes \t(%.2f MB)\n",
            MEMORY_USED + MEMORYA_USED + MEMORYC_USED,
            (double)(MEMORY_USED + MEMORYA_USED + MEMORYC_USED) / 1024000.0);

    printf("------------------------------------------------------------\n");

    printf("Memory max usage summary:\n");

    printf(" Tracked program memory    : %lu bytes \t(%.2f MB)\n",
            MAX_MEMORY_USED, (double)MAX_MEMORY_USED / 1024000.0);
    printf(" Untracked overhead memory : %lu bytes \t(%.2f MB)\n",
            MAX_MEMORYA_USED, (double)MAX_MEMORYA_USED / 1024000.0);
    printf(" Untracked m-string memory : %lu bytes\n",
            MAX_MEMORYC_USED);

    printf("\n Total accounted memory    : %lu bytes \t(%.2f MB)\n",
            MAX_MEMORY_USED + MAX_MEMORYA_USED + MAX_MEMORYC_USED,
            (double)(MAX_MEMORY_USED + MAX_MEMORYA_USED + MAX_MEMORYC_USED) / 1024000.0);

    printf("============================================================\n");
}


void dump_memory_report()
{
    int i;


    printf("\n============================================================\n");
    printf(" Memory status report\n");
    printf("============================================================\n");
    printf("Memory tracking table:\n");
    i = tree_print(MEMORY_INFO, (void (*)(void *))&__print_meminfo);
    printf("%i records, %lu bytes each : %lu bytes\n",
            i, sizeof(meminfo_t), i * sizeof(meminfo_t));
    display_memory_usage();
    printf("============================================================\n\n");
}

void delete_check(const char *file, int line, int print);


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

void add_track(DWORD addr, DWORD size, const char *filename, DWORD line_num)
{
    meminfo_filename_t *memfile, *memfile_prev;
    meminfo_t *meminfo;
    unsigned int len, i;


    MEMORY_USED += size;

    if (MEMORY_USED > MAX_MEMORY_USED)
    {
        MAX_MEMORY_USED = MEMORY_USED;
    }

    MEMORYA_USED += sizeof(meminfo_t);

    if (MEMORYA_USED > MAX_MEMORYA_USED)
    {
        MAX_MEMORYA_USED = MEMORYA_USED;
    }

    meminfo = (meminfo_t *)malloc(sizeof(meminfo_t));

    meminfo->address = addr;
    meminfo->line = line_num;
    meminfo->size = size;
    meminfo->filename = 0;

    if (size == 0)
    {
        printf("\nERROR: %s:%lu Allocated 0 bytes\n", filename, line_num);
    }

    if (filename || filename[0])
    {
        len = strlen(filename);

        memfile = memfile_prev = 0;

        if (MEMORY_FILENAME)
        {
            memfile = MEMORY_FILENAME;

            while (memfile)
            {
                if (strncmp(filename, memfile->filename, len) == 0)
                    break;

                memfile_prev = memfile;
                memfile = memfile->next;
            }
        }

        if (!memfile)
        {
            memfile = (meminfo_filename_t *)malloc(sizeof(meminfo_filename_t));
            memfile->filename_len = len;
            memfile->filename = (char *)malloc(len+1);
            strncpy(memfile->filename, filename, len);
            memfile->filename[len+1] = 0;
            memfile->next = 0;
            memfile->size = 0;
            memfile->alloc_zero = 0;
            memfile->alloc_zero_at_line[0] = 0;
            memfile->alloc_zero_at_line[1] = 0;
            memfile->alloc_zero_at_line[2] = 0;

            MEMORYC_USED += sizeof(meminfo_filename_t) + memfile->filename_len;

            if (MEMORYC_USED > MAX_MEMORYC_USED)
                MAX_MEMORYC_USED = MEMORYC_USED;

            if (memfile_prev)
            {
                memfile_prev->next = memfile;
            }

            if (!MEMORY_FILENAME)
            {
                MEMORY_FILENAME = memfile;
            }
        }

        meminfo->filename = memfile;

        if (memfile)
        {
            if (meminfo->size == 0)
            {
                memfile->alloc_zero += 1;

                for (i = 0; i < ZERO_ALLOC_SLOTS; ++i)
                {
                    if (memfile->alloc_zero_at_line[i] == line_num)
                        break;

                    if (!memfile->alloc_zero_at_line[i])
                    {
                        memfile->alloc_zero_at_line[i] = line_num;
                        break;
                    }
                }
            }

            memfile->size += meminfo->size;
        }
    }

    MEMORYA_USED += sizeof(rbtree_t);
    tree_insert(&MEMORY_INFO, meminfo, meminfo->address);

#ifdef DEBUG_MEMORY_VERBOSE
    printf("add_track> addr 0x%08lx, size %lu, filename %s, line %lu\n",
            addr, size, filename, line_num);
#endif
}


/////////////////////////////////////////////////////////////////


void remove_track(DWORD addr)
{
    meminfo_t *meminfo = 0;
    rbtree_t *tree = 0;


    tree = tree_find(MEMORY_INFO, addr);

    if (tree)
    {
        meminfo = (meminfo_t *)tree->data;

        if (meminfo)
        {
            tree_remove(&MEMORY_INFO, tree);

            if (meminfo->filename)
            {
                meminfo->filename->size -= meminfo->size;
            }

            if (meminfo->size == 0)
            {
                printf("\nERROR: %s:%u Deallocated 0 bytes\n",
                        (meminfo->filename) ? meminfo->filename->filename : "?",
                        meminfo->line);
            }

            MEMORY_USED -= meminfo->size;
            MEMORYA_USED -= sizeof(meminfo_t);
            MEMORYA_USED -= sizeof(rbtree_t);

            free(meminfo);
        }
    }
    else
    {
#ifndef DEBUG_MEMORY_ERROR_OFF
        printf("\nERROR: remove_track> Unknown addr 0x%08lx ", addr);
        delete_check(0, 0, 1);
        printf("?\n");
#endif
    }

    if (!MEMORY_INFO)
    {
        meminfo_filename_t *cur;
        meminfo_filename_t *del;


        cur = MEMORY_FILENAME;
        MEMORY_FILENAME = 0;

        while (cur)
        {
            del = cur;
            cur = cur->next;

            MEMORYC_USED -= sizeof(meminfo_filename_t);

            if (del->filename)
            {
                MEMORYC_USED -= del->filename_len;
                free(del->filename);
            }

            free(del);
        }
    }

#ifdef DEBUG_MEMORY_VERBOSE
    printf("remove_track> addr 0x%lx\n", addr);
#endif
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void *operator new(size_t size, const char *file, int line)
{
    void *ptr = (void *)malloc(size);

    add_track((DWORD)ptr, size, file, line);
    return ptr;
}


void *operator new [](size_t size, const char *file, int line)
{
    void *ptr = (void *)malloc(size);

    add_track((DWORD)ptr, size, file, line);
    return ptr;
}


// Mongoose 2002.03.29, Prob should reduce this with meminfo_filename use
void delete_check(const char *file, int line, int print)
{
    static char s[64];
    static int l;
    unsigned int len;


    if (!print)
    {
        l = line;

        if (!file || !file[0])
        {
            s[0] = 0;
            return;
        }

        len = strlen(file);

        if (len > 63)
            len = 63;

        strncpy(s, file, len);
        s[len] = 0;
    }
    else
    {
        printf("%s:%i", s, l);
    }
}


void operator delete(void *p)
{
    remove_track((DWORD)p);
    free(p);
}


void operator delete [](void *p)
{
    remove_track((DWORD)p);
    free(p);
}
#endif

