/*!
 * \file include/List.h
 * \brief Template list
 *
 * UINT_MAX is an error condition, used in place of -1
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _LIST_H_
#define _LIST_H_

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

#ifdef DEBUG_MEMORY
#include <memory_test.h>
#endif

/*!
 * \brief Template class encapsulating a single list node
 * \tparam T encapsulated data type
 */
template <class T> class ListNode {
public:

    /*!
     * \brief Create a new ListNode
     * \param data Data to be stored in node
     * \param id node id
     */
    ListNode(T data, unsigned int id) {
        _data  = data;
        _id = id;
        _next = NULL;
    }

    /*!
     * \brief Destroy a ListNode
     */
    ~ListNode() {
    }

    /*!
     * \brief Set the id of this ListNode
     * \param id new id
     */
    void Id(unsigned int id) {
        _id = id;
    }

    /*!
     * \brief Get the id of this ListNode
     * \returns current id
     */
    unsigned int Id() {
        return _id;
    }

    /*!
     * \brief Set the data of this ListNode
     * \param data new data
     */
    void Data(T data) {
        _data = data;
    }

    /*!
     * \brief Get the data of this ListNode
     * \returns current data
     */
    T Data() {
        return _data;
    }

    /*!
     * \brief Get the next ListNode in the List
     * \returns next pointer or NULL
     */
    ListNode<T> *Next() {
        return _next;
    }

    /*!
     * \brief Set the next ListNode in the List
     * \param next new next pointer
     */
    void Next(ListNode<T> *next) {
        _next = next;
    }

    /*!
     * \brief Print this ListNode
     */
    void Print() {
        printf("(%u, %p)",  _id, _data);
    }

private:

    ListNode<T> *_next; //!< Next Pointer
    unsigned int _id; //!< ListNode ID
    T _data; //!< Encapsulated data
};

/*!
 * \brief Template class representing a linked list
 * \tparam T encapsulated data type
 */
template <class T> class List {
public:

    /*!
     * \brief Construct a new linked list
     */
    List() {
        _num_items = 0;
        _head = NULL;
        _current = NULL;
        _last = NULL;
        _cache = NULL;
    }

    /*!
     * \brief Deconstruct a linked list
     * \sa List::Clear()
     */
    ~List() {
        Clear();
    }

    /*!
     * \brief Add all data from another list to this one
     * NOTE: this only copies data, the containers aren't the same ids
     * \param list source list where data does come from
     */
    void Copy(List<T> *list) {
        if (!list)
            return;
        for (list->Reset(); list->CurrentExists(); list->Next())
            Add(list->Current());
    }

    /*!
     * \brief Delete every item in this list
     */
    void Clear() {
        _num_items = 0;
        _last = _cache = NULL;
        while (_head) {
            _current = _head;
            _head = _head->Next();
            delete _current;
        }
    }

    /*!
     * \brief Searches the list for an id
     * \param id id to be searched
     * \returns data with id, or 0
     */
    T SearchId(unsigned int id) {
        ListNode<T> *current = _head;
        ListNode<T> *last = NULL;
        if (_cache) {
            if (id >= _cache->Id())
                current = _cache;
        }
        while (current) {
            // Found
            if (current->Id() == id) {
                _cache = current;
                return current->Data();
            }
            last = current;
            current = current->Next();
        }
        return 0;
    }

    /*!
     * \brief Searches the list for specific data
     * \param data data to be searched for
     * \returns id of data, or UINT_MAX
     */
    unsigned int SearchKey(T data) {
        ListNode<T> *current = _head;
        ListNode<T> *last = NULL;
        if (_cache) {
            // Mongoose: 2001-01-31, hhmmm... fixed?
            if (data == _cache->Data())
                return _cache->Id();
        }
        while (current) {
            // Found
            if (current->Data() == data) {
                _cache = current;
                return current->Id();
            }
            last = current;
            current = current->Next();
        }
        return UINT_MAX;
    }

    /*!
     * \brief Search for an id
     * \param i id to be searched
     * \returns id or 0
     * \sa List::SearchId()
     */
    T operator [] (unsigned int i) {
        if (_head) {
            return SearchId(i);
        } else {
#ifdef DEBUG_INDEX_EMPTY_LIST
            printf("List[%u] = NULL\n", i);
#endif
        }
        return 0;
    }

    /*!
     * \brief Search for an id and remove the associated item from the list
     * \param id id to be deleted
     */
    void RemoveId(unsigned int id) {
        ListNode<T> *current = _head;
        ListNode<T> *last = NULL;
        _last = _cache = NULL;
        while (current) {
            // Remove
            if (current->Id() == id) {
                if (current == _head)
                    _head = current->Next();
                else
                    last->Next(current->Next());
                if (_current == current)
                    _current = NULL;
                delete current;
                _num_items--;
                return;
            }
            last = current;
            current = current->Next();
        }
    }

    /*!
     * \brief Search for data and remove the associated item from the list
     * \param data data to be deleted
     */
    void Remove(T data) {
        ListNode<T> *current = _head;
        ListNode<T> *last = NULL;
        _last = _cache = NULL;
        while (current) {
            // Remove
            if (current->Data() == data) {
                if (current == _head)
                    _head = current->Next();
                else
                    last->Next(current->Next());
                if (_current == current)
                    _current = NULL;
                delete current;
                _num_items--;
                return;
            }
            last = current;
            current = current->Next();
        }
    }

    /*!
     * \brief Is the list empty?
     * \returns true if the list is empty
     */
    bool Empty() {
        return (_head == NULL);
    }

    /*!
     * \brief Length of the list
     * \returns number of items in the list
     */
    unsigned int NumItems() {
        return _num_items;
    }

    /*!
     * \brief Print the list
     * \param print_func function that will be called for each item in the list, after its id was printed. Or NULL.
     */
    void Print(void (*print_func)(T)) {
        ListNode<T> *current = _head;
        printf(" [%u] {\n", _num_items);
        while (current) {
            printf("#%i, ", current->Id());
            if (print_func)
                (*print_func)(current->Data());
            current = current->Next();
            fflush(stdout);
        }
        printf(" }\n");
    }

    /*!
     * \brief Print all IDs in this list
     */
    void Print() {
        ListNode<T> *current = _head;
        printf("List %u {\n", _num_items);
        while (current) {
            //current->Print();
            printf("%i", current->Id());
            current = current->Next();
            if (current)
                printf(", ");
            fflush(stdout);
        }
        printf(" }\n");
    }

    /*!
     * \brief Reset the iterator
     */
    void Reset() {
        _current = _head;
        _cache = _head;
    }

    /*!
     * \brief Traverses the list
     * \returns true if there is a new current item, false if the list is at the end
     * \sa List::Next()
     */
    bool operator ++ (int) {
        return Next();
    }

    /*!
     * \brief Traverses the list
     * \returns true if there is a new current item, false if the list is at the end
     * \sa List::Next()
     */
    bool operator ++ () {
        return Next();
    }

    /*!
     * \brief Traverses the list
     * \returns true if there is a new current item, false if the list is at the end
     */
    bool Next() {
        if (_current)
            _current = _current->Next();
        return (_current != NULL);
    }

    /*!
     * \brief Get the id of the current item
     * \returns id of current item or UINT_MAX if there is no current item
     */
    unsigned int CurrentId() {
        if (!_current)
            return UINT_MAX;

        return _current->Id();
    }

    /*!
     * \brief Is there a current item?
     * \returns true if there is a current item, false otherwise
     */
    bool CurrentExists() {
        return (_current != 0);
    }

    /*!
     * \brief Get the current items data
     * \returns data of the current item or 0 if there is no current item
     */
    T Current() {
        if (_current)
            return _current->Data();
        else
            return 0;
    }

    /*!
     * \brief Add data to the list. Constructs a new ListNode encapsulating the data.
     * \param data data to be stored
     * \returns id of the new ListNode
     */
    unsigned int Add(T data) {
        ListNode<T> *node;

        node = new ListNode<T>(data, _num_items++);
        return Add(node);
    }

    /*!
     * \brief Add a ListNode to the end of this List.
     * \param node new node
     * \returns id of the node
     */
    unsigned int Add(ListNode<T> *node) {
        ListNode<T> *current;
        ListNode<T> *last;
        unsigned int i;

        if (_head) {
            current = _head;
            last = NULL;
            i = 0;

            //EXP
            if (_last) {
                i = _last->Id();
                current = _last;
            }

            while (current) {
                // Prepend
                if (current->Id() > i) {
                    node->Id(i);
                    node->Next(current);

                    if (current == _head)
                        _head = node;
                    else if (last)
                        last->Next(node);

                    return node->Id();
                }

                i++;
                last = current;
                current = current->Next();
            }

            // Append
            last->Next(node);
        } else
            _head = node;

        _last = node; //EXP

        return node->Id();
    }

private:

    unsigned int _num_items; //!< Number of items in the list
    ListNode<T> *_head; //!< First item in the list
    ListNode<T> *_current; //!< Current item for the list iterator
    ListNode<T> *_last; //!< Last item in the list
    ListNode<T> *_cache; //!< cache used by the search methods
};

#endif
