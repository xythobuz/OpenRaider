/*!
 * \file include/Map.h
 * \brief Template Map
 *
 * Using RBTree and list overlay for faster access
 *
 *     Access: O(1)/O(n)/O(logn) ?
 *     Insert: O(nlogn)          ?
 *     Remove: O(nlogn)          ?
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _MAP_H_
#define _MAP_H_

#include <stdlib.h>
#include <stdio.h>

#include <Tree.h>

#ifdef DEBUG_MEMORY
#include <memory_test.h>
#endif

/*!
 * \brief Template class encapsulating a single map node
 * \tparam K key type
 * \tparam D data type
 */
template <class K, class D> class MapNode {
public:

    /*!
     * \brief Construct a MapNode
     * \param key key of this Node
     * \param data data of this Node
     */
    MapNode(K key, D data) {
        _data  = data;
        _key = key;
        _next = NULL;
    }

    /*!
     * \brief Deconstruct a MapNode
     */
    ~MapNode() {
    }

    /*!
     * \brief Set the key of this MapNode
     * \param key new key
     */
    void Key(K key) {
        _key = key;
    }

    /*!
     * \brief Get the key of this MapNode
     * \returns key
     */
    K Key() {
        return _key;
    }

    /*!
     * \brief Set the data of this MapNode
     * \param data new data
     */
    void Data(D data) {
        _data = data;
    }

    /*!
     * \brief Get the data of this MapNode
     * \returns data
     */
    D Data() {
        return _data;
    }

    /*!
     * \brief Get the next MapNode in the Map
     * \returns next pointer
     */
    MapNode<K, D> *Next() {
        return _next;
    }

    /*!
     * \brief Set the next MapNode in the Map
     * \param next new next pointer
     */
    void Next(MapNode<K, D> *next) {
        _next = next;
    }

private:

    MapNode<K, D> *_next; //!< Next pointer
    K _key; //!< Key of the MapNode
    D _data; //!< Data of the MapNode
};

/*!
 * \brief Template class representing a Map, mapping data to a key.
 * \tparam K key type
 * \tparam D data type
 */
template <class K, class D> class Map {
public:

    /*!
     * \brief Construct a Map
     */
    Map() {
        UnSetError();
        _num_items = 0;
        _head = NULL;
        _current = NULL;
        _cache = NULL;
    }

    /*!
     * \brief Deconstruct a Map
     * \sa Map::Clear()
     */
    ~Map() {
        Clear();
    }

    /*!
     * \brief Deletes every item in the Map. Clears error flag.
     */
    void Clear() {
        UnSetError();
        _num_items = 0;
        _cache = NULL;
        while (_head) {
            _current = _head;
            _head = _head->Next();
            delete _current;
        }
        _tree.Clear();
    }

    /*!
     * \brief Set the error flag
     */
    void SetError() {
        _error = true;
    }

    /*!
     * \brief Unset the error flag
     */
    void UnSetError() {
        _error = false;
    }

    /*!
     * \brief Get the error flag
     * \returns error flag
     */
    bool GetError() {
        return _error;
    }

    /*!
     * \brief Search for data with a key. Sets error flag if nothing is found.
     * \param key key to search for
     * \returns data matching key or 0
     */
    D FindDataByKey(K key) {
        MapNode<K, D> *current = NULL;
        MapNode<K, D> *next = NULL;


        if (_head) {
            UnSetError();

            if (_cache) {
                next = _cache->Next();
            }

            // Mongoose 2002.02.19, Optimize for sequential searches
            if (next && key == next->Key()) {
                current = next;
            } else { // Mongoose 2002.02.19, Use search algorithm otherwise
                current = _tree.SearchByKey(key, &_error);

                if (_error)
                    return false;
            }

            if (current) {
                _cache = _current = current;
                return current->Data();
            }
        }

        SetError();
        return 0;
    }

    /*!
     * \brief Search for data with a key
     * \param key key to search for
     * \returns data matching key or 0
     * \sa Map::FindDataByKey()
     */
    D operator [] (K key) {
        return FindDataByKey(key);
    }

    /*!
     * \brief Search for a key with specific data. Sets error flag if nothing is found.
     * \param data data to search for
     * \returns key matching data or 0
     */
    K FindKeyByData(D data) {
        MapNode<K, D> *current = _head;
        MapNode<K, D> *last = NULL;


        UnSetError();

        while (current) {
            // Found
            if (current->Data() == data) {
                _cache = current;
                return current->Key();
            }

            last = current;
            current = current->Next();
        }

        SetError();

        return 0;
    }

    /*!
     * \brief Returns data of the cached MapNode
     * \returns cached data
     */
    D getCache() {
        if (_cache == 0x0) {
            printf("Map::getCache> Bad request - should segfault\n");
        }

        return _cache->Data();
    }

    /*!
     * \brief Check if a key is in this map. Sets error flag if nothing is found.
     * \returns true if the key is in the map, false otherwise
     * \sa Tree:SearchByKey()
     */
    bool findKey(K key) {
        MapNode<K, D> *current = NULL;
        MapNode<K, D> *next = NULL;

        if (_head) {
            UnSetError();

            if (_cache) {
                if (_cache->Key() == key) {
                    return true;
                }

                next = _cache->Next();
            }

            // Mongoose 2002.02.19, Optimize for sequential searches
            if (next && key == next->Key()) {
                current = next;
            } else { // Mongoose 2002.02.19, Use search algorithm otherwise
                current = _tree.SearchByKey(key, &_error);
            }

            if (current) {
                _cache = _current = current;
                //curData = current->Data();
                return true;
            }
        }

        SetError();
        return false;
    }

    /*!
     * \brief Add a Key-Data pair to the Map, creating a new MapNode. Clears error flag.
     * \param key key to add
     * \param data to add
     * \returns true on success, false if there is not enough memory
     */
    bool Add(K key, D data) {
        MapNode<K, D> *node;

        UnSetError();
        node = new MapNode<K, D>(key, data);
        _num_items++;
        return Add(node);
    }

    /*!
     * \brief Add a MapNode to the Map. Clears error flag on success.
     * \param node node to add
     * \returns true on success, false if node is NULL
     * \sa Tree:Insert()
     */
    bool Add(MapNode<K, D> *node) {
        MapNode<K, D> *current = _head;
        MapNode<K, D> *last = NULL;

        if (!node)
            return false;

        UnSetError();

        if (_head) {
            current = _head;
            last = NULL;

            while (current) {
                // Prepend
                if (current->Key() > node->Key()) {
                    node->Next(current);

                    if (current == _head) {
                        _head = node;
                    } else if (last) {
                        last->Next(node);
                    }

                    _tree.Insert(node->Key(), node);
                    return true;
                }

                last = current;
                current = current->Next();
            }

            // Append
            last->Next(node);
        } else {
            _head = node;
        }

        _tree.Insert(node->Key(), node);
        return true;
    }

    /*!
     * \brief Remove item with the specified key. Sets error flag if nothing is removed.
     * \param key key to remove
     */
    void RemoveByKey(K key) {
        MapNode<K, D> *current = _head;
        MapNode<K, D> *last = NULL;

        UnSetError();

        _cache = NULL;

        while (current) {
            // Remove
            if (current->Key() == key) {
                if (current == _head) {
                    _head = current->Next();
                } else {
                    last->Next(current->Next());
                }

                if (_current == current) {
                    _current = NULL;
                }

                _tree.RemoveByKey(current->Key());
                delete current;
                _num_items--;

                return;
            }

            last = current;
            current = current->Next();
        }

        SetError();
    }

    /*!
     * \brief Remove item with the specified data. Sets error flag if nothing is removed.
     * \param data data to remove
     */
    void RemoveByData(D data) {
        MapNode<K, D> *current = _head;
        MapNode<K, D> *last = NULL;

        UnSetError();

        _cache = NULL;

        while (current)
        {
            // Remove
            if (current->Data() == data) {
                if (current == _head) {
                    _head = current->Next();
                } else {
                    last->Next(current->Next());
                }

                if (_current == current) {
                    _current = NULL;
                }

                _tree.RemoveByKey(current->Key());
                delete current;
                _num_items--;

                return;
            }

            last = current;
            current = current->Next();
        }

        SetError();
    }

    /*!
     * \brief Check if the Map is empty
     * \returns true if Map is empty, false otherwise
     */
    bool Empty() {
        return (_head == NULL);
    }

    /*!
     * \brief Returns number of items in the Map
     * \returns number of items
     */
    unsigned int NumItems() {
        return _num_items;
    }

    /*!
     * \brief Print the Map. Sets error flag if one or both functions aren't provided.
     * \param print_key_func function that prints a key
     * \param print_data_func function that prints the data
     */
    void Print(void (*print_key_func)(K), void (*print_data_func)(D)) {
        MapNode<K, D> *current = _head;

        UnSetError();

        if (!print_key_func || !print_data_func) {
            SetError();
            return;
        }

        printf(" [%u] {\n", _num_items);

        while (current) {
            printf("(");
            (*print_key_func)(current->Key());
            printf(", ");
            (*print_data_func)(current->Data());
            printf("), ");

            current = current->Next();
            fflush(stdout);
        }

        printf(" }\n");
    }

    /*!
     * \brief Reset the Map iterator
     */
    void Reset() {
        _current = _head;
        _cache = _head;
    }

    /*!
     * \brief Iterate over the Map
     * \returns true if there is a new current element
     * \sa Map::Next()
     */
    bool operator ++ (int) {
        return Next();
    }

    /*!
     * \brief Iterate over the Map
     * \returns true if there is a new current element
     * \sa Map::Next()
     */
    bool operator ++ () {
        return Next();
    }

    /*!
     * \brief Iterate over the Map
     * \returns true if there is a new current element
     * \sa Map::Next()
     */
    bool Next() {
        if (_current) {
            _current = _current->Next();
        }

        return (_current != NULL);
    }

    /*!
     * \brief Check if there is a current item in the iterator
     * \returns true if the current item exists
     */
    bool CurrentExists() {
        return (_current != 0);
    }

    /*!
     * \brief Returns the key of the current item. Sets error flag if there is no current item.
     * \returns key or 0 if there is no current item
     */
    K CurrentKey() {
        UnSetError();

        if (!_current) {
            SetError();
            return 0;
        }

        return _current->Key();
    }

    /*!
     * \brief Returns the data of the current item. Sets error flag if there is no current item.
     * \returns data or 0 if there is no current item
     */
    D Current() {
        UnSetError();

        if (!_current) {
            SetError();
            return 0;
        }

        return _current->Data();
    }

private:

    unsigned int _num_items; //!< Number of items in the Map
    bool _error; //!< Error flag
    Tree<K, MapNode<K, D> *> _tree; //!< Tree containing the MapNodes
    MapNode<K, D> *_head; //!< First item of the Map
    MapNode<K, D> *_current; //!< Current item for the iterator
    MapNode<K, D> *_cache; //!< Cached item used by search & remove methods
};
#endif
