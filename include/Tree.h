/*!
 * \file include/Tree.h
 * \brief Template Red-Black Tree
 *
 * Red-Black trees are a type of binary search trees
 * with the properities:
 *
 * * Every node is red or black.
 * * The root node must be black.
 * * Every leaf node is black. (null pointers)
 * * If a node is red, then both its children are black.
 * * Every simple path from a node to a descendant leaf contains the same number of black nodes.
 * * Any path from the root to a leaf must not have adjacent red nodes.
 *
 * Define `USE_IOSTREAM` to get additional print methods
 *
 * \author Mongoose
 */

#ifndef _TREE_H_
#define _TREE_H_

#include <stdlib.h>
#include <stdio.h>

#ifdef USE_IOSTREAM
#include <iostream.h>
#endif

#ifdef DEBUG_MEMORY
#include <memory_test.h>
#endif

/*!
 * \brief Color a Tree node can have
 */
typedef enum {
    _tree_h_black, //!< Black node
    _tree_h_red    //!< Red node
}  _tree_h_color_t;

/*!
 * \brief Template class for a single Tree node
 * \tparam Key key type
 * \tparam Data data type
 */
template <class Key, class Data> class TreeNode {
public:

    /*!
     * \brief Construct a TreeNode
     * \param key Key for this node
     * \param data Data for this node
     */
    TreeNode(Key key, Data data) {
        SetColor(_tree_h_red);
        SetData(data);
        SetKey(key);
        SetParent(NULL);
        SetLeft(NULL);
        SetRight(NULL);
    }

    /*!
     * \brief Deconstruct a TreeNode. Also deletes childs.
     */
    ~TreeNode() {
        TreeNode<Key, Data> *left;
        TreeNode<Key, Data> *right;
        left = GetLeft();
        right = GetRight();
        SetParent(NULL);
        SetLeft(NULL);
        SetRight(NULL);
        if (left) {
            left->SetParent(NULL);
            delete left;
        }
        if (right) {
            right->SetParent(NULL);
            delete right;
        }
    }

    /*!
     * \brief Set the color
     * \param color new color
     */
    void SetColor(_tree_h_color_t color) {
        _color = color;
    }

    /*!
     * \brief Get the color
     * \returns current color
     */
    _tree_h_color_t GetColor() {
        return _color;
    }

    /*!
     * \brief Get the child
     * \returns the left child
     */
    TreeNode<Key, Data> *GetChild() {
        return GetLeft();
    }

    /*!
     * \brief Set the child
     * \param tree new left child
     */
    void SetChild(TreeNode<Key, Data> *tree) {
        Left(tree);
    }

    /*!
     * \brief Get the child
     * \returns the left child
     */
    TreeNode<Key, Data> *GetLeft() {
        return _left;
    }

     /*!
     * \brief Set the child
     * \param tree new left child
     */
    void SetLeft(TreeNode<Key, Data> *tree) {
        if (tree == this)
            return;
        _left = tree;
        if (tree)
            tree->SetParent(this);
    }

    /*!
     * \brief Get the sibling
     * \returns the right child
     */
    TreeNode<Key, Data> *GetSibling() {
        return GetRight();
    }

    /*!
     * \brief Set the sibling
     * \param tree new right child
     */
    void SetSibling(TreeNode<Key, Data> *tree) {
        SetRight(tree);
    }

    /*!
     * \brief Get the sibling
     * \returns the right child
     */
    TreeNode<Key, Data> *GetRight() {
        return _right;
    }

    /*!
     * \brief Set the sibling
     * \param tree new right child
     */
    void SetRight(TreeNode<Key, Data> *tree) {
        if (tree == this)
            return;
        _right = tree;
        if (tree)
            tree->SetParent(this);
    }

    /*!
     * \brief Get the data
     * \returns current data
     */
    Data GetData() {
        return _data;
    }

    /*!
     * \brief Set the data
     * \param data new data
     */
    void SetData(Data data) {
        _data = data;
    }

    /*!
     * \brief Get the key
     * \returns current key
     */
    Key GetKey() {
        return _key;
    }

    /*!
     * \brief Set the key
     * \param key new key
     */
    void SetKey(Key key) {
        _key = key;
    }

    /*!
     * \brief Get the parent
     * \returns current parent
     */
    TreeNode<Key, Data> *GetParent() {
        return _parent;
    }

    /*!
     * \brief Set the parent
     * \param parent new parent
     */
    void SetParent(TreeNode<Key, Data> *parent) {
        _parent = parent;
    }

#ifdef USE_IOSTREAM
    /*!
     * \brief Print this node
     */
    void PrintNode() {
        cout << "(" << _key << ", " << _data << ", "
              << ((GetColor() == _tree_h_red) ? "Red" : "Black")
              << ")";
    }

    /*!
     * \brief Print this node and its children in the correct order
     */
    void PrintInorder() {
        if (_left) {
            _left->PrintInorder();
            cout << endl;
        }
        PrintNode();
        if (_right) {
            cout << endl;
            _right->PrintInorder();
        }
    }
#endif

    /*!
     * \brief Print this node with custom methods to print key and data
     * \param print_func_k key printing function
     * \param print_func_d data printing function
     */
    void PrintNodeSpecial(void (*print_func_k)(Key), void (*print_func_d)(Data)) {
        printf("(");
        if (print_func_k)
            (*print_func_k)(_key);
        printf(", ");
        if (print_func_d)
            (*print_func_d)(_data);
        printf(", %s)", ((GetColor() == _tree_h_red) ? "Red" : "Black"));
    }

    /*!
     * \brief Print this node and its children in the correct order, with custom methods to print key and data
     * \param print_func_k key printing function
     * \param print_func_d data printing function
     */
    void PrintInorderSpecial(void (*print_func_k)(Key), void (*print_func_d)(Data)) {
        if (_left) {
            _left->PrintInorderSpecial(print_func_k, print_func_d);
            printf(",\n");
        }
        PrintNodeSpecial(print_func_k, print_func_d);
        if (_right) {
            printf(",\n");
            _right->PrintInorderSpecial(print_func_k, print_func_d);
        }
    }

    /*!
     * \brief Search this node and its children for specific data
     * \param data data to search for
     * \param error will be true if nothing was found
     * \returns the TreeNode containing the data, or NULL
     */
    TreeNode<Key, Data> *SearchByData(Data data, bool *error) {
        TreeNode<Key, Data> *tree = NULL;
        *error = true;
        if (_data == data) {
            *error = false;
            return this;
        }
        if (_left)
            tree = _left->SearchByData(data, error);
        if (_right && !tree)
            tree = _right->SearchByData(data, error);
        return tree;
    }

    /*!
     * \brief Search this node and its children for a specific key
     * \param key key to search for
     * \param error will be true if nothing was found
     * \returns the TreeNode containing the key, or NULL
     */
    TreeNode<Key, Data> *SearchByKey(Key key, bool *error) {
        *error = false;
        if (_key == key) {
            return this;
        } else if (_left && key < _key) {
            return _left->SearchByKey(key, error);
        } else if (_right) {
            return _right->SearchByKey(key, error);
        } else {
            *error = true;
            return 0; //NULL;
        }
    }

    /*!
     * \brief Insert a TreeNode after this one.
     * If the key of the TreeNode to be inserted is smaller than the key
     * in this TreeNode, it will be added as left child, else as right child.
     * \param tree TreeNode to insert
     */
    void Insert(TreeNode<Key, Data> *tree) {
        if (!tree || tree == this) {
            return;
        }
        if (tree->GetKey() < _key) {
            if (!_left) {
                SetLeft(tree);
            } else {
                _left->Insert(tree);
            }
        } else {
            if (!_right) {
                SetRight(tree);
            } else {
                _right->Insert(tree);
            }
        }
    }

private:
    _tree_h_color_t _color;       //!< Color of tree node
    Key _key;                     //!< Unique identifer?
    Data _data;                   //!< Data for this tree
    TreeNode<Key, Data> *_left;   //!< Left or child node
    TreeNode<Key, Data> *_right;  //!< Right or sibling node
    TreeNode<Key, Data> *_parent; //!< Parent of the tree node
};

/*!
 * \brief Template class for a Red-Black Tree
 * \tparam Key key datatype
 * \tparam Data data datatype
 */
template <class Key, class Data> class Tree {
public:

    /*!
     * \brief Construct an object of Tree
     */
    Tree() {
        _error = false;
        _num_elements = 0;
        _root = 0;
    }

    /*!
     * \brief Deconstruct an object of Tree
     */
    ~Tree() {
        Clear();
    }

    /*!
     * \brief Get the number of elements
     * \returns number of elements in this tree
     */
    unsigned int NumElements() {
        return _num_elements;
    }

    /*!
     * \brief Search for data with a key
     * \param key key to search for
     * \param error will be true if nothing is found
     * \returns Data matching Key or NULL
     * \sa TreeNode::SearchByKey()
     */
    Data SearchByKey(Key key, bool *error) {
        TreeNode<Key, Data> *seeking;
        *error = true;

        // Mongoose 2002.02.16, Nothing to search
        if (!_root)
            return 0;

        seeking = _root->SearchByKey(key, error);

        if (seeking)
            return seeking->GetData();

        return 0;
    }

    /*!
     * \brief Search for a key with data
     * \param data data to search for
     * \param error will be true if nothing is found
     * \returns Key matching Data or NULL
     * \sa TreeNode::SearchByData()
     */
    Key SearchByData(Data data, bool *error) {
        TreeNode<Key, Data> *seeking;
        *error = true;

        // Mongoose 2002.02.16, Nothing to search
        if (!_root)
            return 0;

        seeking = _root->SearchByData(data, error);

        if (seeking)
            return seeking->GetKey();

        return 0;
    }

    /*!
     * \brief Insert a key-data pair into the tree.
     * \param key key to insert
     * \param data corresponding to key to insert
     * \sa TreeNode::Insert()
     * \sa Tree::RestoreRedBlackAfterInsert()
     */
    void Insert(Key key, Data data) {
        TreeNode<Key, Data> *tree = new TreeNode<Key, Data>(key, data);
        ++_num_elements;

        if (_root) {
            _root->Insert(tree);
            RestoreRedBlackAfterInsert(tree);
        } else {
            _root = tree;
            _root->SetColor(_tree_h_black);
        }
    }

    /*!
     * \brief Search for data and remove, if found
     * \param data data to remove
     * \returns true if nothing was deleted
     * \sa TreeNode::SearchByData()
     * \sa Tree::Remove()
     */
    bool RemoveByData(Data data) {
        bool error = true;
        if (_root)
            Remove(_root->SearchByData(data, &error));
        return error;
    }

    /*!
     * \brief Search for a key and remove, if found
     * \param key key to remove
     * \returns true if nothing was deleted
     * \sa TreeNode::SearchByKey()
     * \sa Tree::Remove()
     */
    bool RemoveByKey(Key key) {
        bool error = true;
        if (_root) {
#ifdef OBSOLETE
            // Mongoose 2002.02.18, To remove duplicates
            error = false;
            while (!error) {
#endif
                Remove(_root->SearchByKey(key, &error));
#ifdef OBSOLETE
            }
#endif
        }
        return error;
    }

    /*!
     * \brief Clear the list, deleting all TreeNodes
     */
    void Erase() {
        Clear();
    }

    /*!
     * \brief Clear the list, deleting all TreeNodes
     */
    void Clear() {
        if (_root)
            delete _root;

        _num_elements = 0;
        _error = false;
        _root = 0;
    }

    /*!
     * \brief Search for data with a key
     * \param key key to search for
     * \returns data corresponding to key, or 0
     * \sa TreeNode::SearchByKey()
     */
    Data operator [] (Key key) {
        _error = false;

        if (_root)
            return SearchByKey(key, &_error);

        _error = true;
        return 0;
    }

#ifdef USE_IOSTREAM
    /*!
     * \brief Print a/this Tree?
     * \param tree TreeNode from which to start printing?
     * \param height ?
     * \param seek ?
     * \param rightmost ?
     * \fixme Fix documentation
     * \sa Tree::PrintAsTree()
     */
    void PrintTree(TreeNode<Key, Data> *tree, unsigned int height,
                        unsigned int seek, bool rightmost) {
        TreeNode<Key, Data> *left, *right, *parent;

        if (!tree)
            return;

        parent = tree->GetParent();

        if (height == seek) {
            if (!parent) {
                cout << endl << "[height " << height << "]   " << endl;

                if (tree->GetColor() == _tree_h_red)
                    cout << "*";
            } else {
                if ((parent->GetColor() == _tree_h_red) && (tree->GetColor() == _tree_h_red))
                    cout << "*";
            }

            cout << "(" << tree->GetKey() << ", "
                  << ((tree->GetColor() == _tree_h_red) ? "red" : "blk")
                  << ")";

            if (rightmost) {
                cout << endl << "[height " << (height+1) << "]   " << endl;
                PrintTree(_root, 0, ++seek, true);
            } else {
                cout << " ";
            }

            return;
        } else if (seek < height) {
            return;
        }

        left = tree->GetLeft();
        right = tree->GetRight();
        ++height;

        if (left) {
            PrintTree(left, height, seek, false);
        } else {
            cout << "(-, blk) ";
        }

        if (right) {
            PrintTree(right, height, seek, rightmost);
        } else {
            cout << "(-, blk) ";
        }

        if (parent) {
            if (parent->GetRight() != tree) {
                cout << " |  ";
            }
        }
    }

    /*!
     * \brief Print this tree as tree
     * \sa Tree::PrintTree()
     */
    void PrintAsTree() {
        PrintTree(_root, 0, 0, true);
        cout << endl << "Nodes marked with * are in error" << endl;
    }

    /*!
     * \brief Print this tree
     * \sa TreeNode::PrintNode()
     * \sa TreeNode::PrintInorder()
     */
    void Print() {
        cout << "Tree: " << _num_elements <<" elements {" << endl;
        if (_root) {
            cout << "Root: ";
            _root->PrintNode();
            cout << endl;
            _root->PrintInorder();
        }
        cout << endl << "}" << endl;
    }
#endif

    /*!
     * \brief Print this tree with it's keys and data
     * \param print_func_k key printing function
     * \param print_func_d data printing function
     * \sa TreeNode::PrintNodeSpecial()
     * \sa TreeNode::PrintInorderSpecial()
     */
    void PrintSpecial(void (*print_func_k)(Key), void (*print_func_d)(Data)) {
        printf("Tree: %u elements {\n", _num_elements);
        if (_root && print_func_k && print_func_d) {
            printf("Root: ");
            _root->PrintNodeSpecial(print_func_k, print_func_d);
            printf("\n");
            _root->PrintInorderSpecial(print_func_k, print_func_d);
        }
        printf("\n}\n");
    }

    /*!
     * \brief Get the key of the root node
     * \returns key of root node or 0
     */
    Key Root() {
        if (_root) {
            return _root->GetKey();
        }
        return 0;
    }

    /*!
     * \brief Get the error flag
     * \returns error flag
     */
    bool Error() {
        return _error;
    }

    bool IsValidRedBlackTree() {
        return IsValidRedBlackTreeCheck(_root, true);
    }

private:

    TreeNode<Key, Data> *GetSuccessor(TreeNode<Key, Data> *tree) {
        TreeNode<Key, Data> *successor;
        successor = tree->GetRight();

        if (successor) {
            while (successor->GetLeft())
                successor = successor->GetLeft();
        } else {
            successor = tree->GetParent();

            while (tree == successor->GetRight()) {
                tree = successor;
                successor = successor->GetParent();
            }

            if (successor == _root)
                return NULL;
        }

        return successor;
    }

    TreeNode<Key, Data> *GetPredecessor(TreeNode<Key, Data> *tree) {
        TreeNode<Key, Data> *predecessor;
        predecessor = tree->GetLeft();

        if (predecessor) {
            while (predecessor->GetRight())
                predecessor = predecessor->GetRight();
        } else {
            predecessor = tree->GetParent();

            while (tree == predecessor->GetLeft()) {
                if (predecessor == _root)
                    return NULL;

                tree = predecessor;
                predecessor = predecessor->GetParent();
            }
        }

        return predecessor;
    }

    bool IsValidRedBlackTreeCheck(TreeNode<Key, Data> *current, bool valid) {
        TreeNode<Key, Data> *right, *left;
        _tree_h_color_t color_red;

        if (!current)
            return valid;

        // Mongoose 2002.02.19, Check for a red root
        if (!current->GetParent() && current->GetColor() == _tree_h_red)
            return false;

        color_red = (current->GetColor() == _tree_h_red);
        left = current->GetLeft();
        right = current->GetRight();

        // Mongoose 2002.02.19, Check for adj red nodes
        if (left) {
            if (color_red && left->GetColor() == _tree_h_red)
                return false;

            if (!IsValidRedBlackTreeCheck(left, valid))
                return false;
        }

        if (right) {
            if (color_red && right->GetColor() == _tree_h_red)
                return false;

            if (!IsValidRedBlackTreeCheck(right, valid))
                return false;
        }

        return true;
    }

    void RotateLeft(TreeNode<Key, Data> *tree) {
        TreeNode<Key, Data> *right, *right_leftchild, *parent, *uncle;

        if (!tree || !_root)
            return;

        // Get tree's right node
        right = tree->GetRight();

        // Get right node's left child
        right_leftchild = NULL;

        if (right)
            right_leftchild = right->GetLeft();

        // Set tree's right node to right's left child
        tree->SetRight(right_leftchild);

        // Child now has a new parent
        if (right_leftchild)
            right_leftchild->SetParent(tree);

        // Right also has a new parent
        if (right)
            right->SetParent(tree->GetParent());

        // Get parent
        parent = tree->GetParent();

        if (parent) { // Not root
            uncle = parent->GetLeft();

            // Mix up at hosptial, switch parent's children!
            if (tree == uncle)
                parent->SetLeft(right);
            else
                parent->SetRight(right);
        } else { // TreeNode 'tree' was root, so now right is root
            _root = right;
        }

        if (right) {
            // TreeNode 'tree' is now right's left child
            right->SetLeft(tree);

            if (tree)
                tree->SetParent(right);
        }
    }

    void RotateRight(TreeNode<Key, Data> *tree) {
        TreeNode<Key, Data> *left, *left_rightchild, *parent, *uncle;

        if (!tree || !_root)
            return;

        left = tree->GetLeft();

        left_rightchild = NULL;

        if (left)
            left_rightchild = left->GetRight();

        tree->SetLeft(left_rightchild);

        if (left_rightchild)
            left_rightchild->SetParent(tree);

        if (left)
            left->SetParent(tree->GetParent());

        parent = tree->GetParent();

        if (parent) { //if node is not the root
            uncle = parent->GetRight();

            if (tree == uncle)
                parent->SetRight(left);
            else
                parent->SetLeft(left);
        } else {
        _root = left;
        }

        left->SetRight(tree);

        if (tree)
            tree->SetParent(left);
    }


    void TreeNodeShallowCopy(TreeNode<Key, Data> *src,
                                TreeNode<Key, Data> *dest, bool no_links) {
        if (!src || !dest)
            return;

        dest->SetKey(src->GetKey());
        dest->SetData(src->GetData());
        dest->SetColor(src->GetColor());

        if (!no_links) {
            dest->SetRight(src->GetRight());
            dest->SetLeft(src->GetLeft());
            dest->SetParent(src->GetParent());
        }
    }

    void Remove(TreeNode<Key, Data> *tree) {
        TreeNode<Key, Data> *left, *right, *parent, *prev, *cur;

        // Mongoose 2002.02.16, Nothing to remove
        if (!tree || !_root)
            return;

        left = tree->GetLeft();
        right = tree->GetRight();
        parent = tree->GetParent();

        if (!left || !right)
            prev = tree;
        else
            prev = GetSuccessor(tree);

        if (!prev)
            return; // Probably a good idea, as there's a check if(prev) later

        if (prev->GetLeft())
            cur = prev->GetLeft();
        else
            cur = prev->GetRight();

        if (cur)
            cur->SetParent(prev->GetParent());

        if (!prev->GetParent()) {
            _root = cur;
        } else {
            parent = prev->GetParent();

            if (prev == parent->GetLeft())
                parent->SetLeft(cur);
            else
                parent->SetRight(cur);
        }

        if (prev != tree) {
            TreeNodeShallowCopy(prev, tree, true);

            if (prev->GetParent()) {
                if (prev == (prev->GetParent())->GetLeft())
                    (prev->GetParent())->SetLeft(tree);
                else if (prev == (prev->GetParent())->GetRight())
                    (prev->GetParent())->SetRight(tree);
            }
        }

        --_num_elements;

        if (prev) {
            prev->SetRight(NULL);
            prev->SetParent(NULL);
            prev->SetLeft(NULL);

            delete prev;
        }

        if (tree->GetColor() == _tree_h_black)
            RestoreRedBlackAfterRemove(cur);
    }

    void RestoreRedBlackAfterRemove(TreeNode<Key, Data> *tree) {
        TreeNode<Key, Data> *parent, *sibling, *sleft, *sright;

        if (!tree || !_root)
            return;

        parent = tree->GetParent();

        while ((tree != _root) && (parent->GetColor() == _tree_h_black)) {
            if (tree == parent->GetLeft()) {
                sibling = parent->GetRight();

                if (sibling && sibling->GetColor() == _tree_h_red) {
                    sibling->SetColor(_tree_h_black);
                    parent->SetColor(_tree_h_red);
                    RotateLeft(parent);
                    sibling = parent->GetRight();
                }

                if (sibling) {
                    sleft = sibling->GetLeft();
                    sright = sibling->GetRight();
                } else {
                    sleft = sright = NULL;
                }

                if (sright && sright->GetColor() == _tree_h_black &&
                        sleft && sleft->GetColor() ==_tree_h_black) {
                    sibling->SetColor(_tree_h_red);
                    tree = parent;
                } else {
                    if (sright && sright->GetColor() == _tree_h_black) {
                        sibling->SetColor(_tree_h_red);
                        sleft->SetColor(_tree_h_black);
                        RotateRight(sibling);
                        sibling = parent->GetRight();
                    }

                    sibling->SetColor(parent->GetColor());
                    parent->SetColor(_tree_h_black);
                    sright->SetColor(_tree_h_black);
                    RotateLeft(parent);
                    tree = _root;
                }
            } else {
                sibling = parent->GetLeft();

                if (sibling && sibling->GetColor() == _tree_h_red) {
                    sibling->SetColor(_tree_h_black);
                    parent->SetColor(_tree_h_red);
                    RotateLeft(parent);
                    sibling = parent->GetLeft();
                }

                if (sibling) {
                    sleft = sibling->GetLeft();
                    sright = sibling->GetRight();
                } else {
                    sleft = sright = NULL;
                }

                if (sright && sright->GetColor() == _tree_h_black &&
                     sleft && sleft->GetColor() ==_tree_h_black) {
                    sibling->SetColor(_tree_h_red);
                    tree = parent;
                } else {
                    if (sleft && sleft->GetColor() == _tree_h_black) {
                        sibling->SetColor(_tree_h_red);
                        sright->SetColor(_tree_h_black);
                        RotateLeft(sibling);
                        sibling = parent->GetLeft();
                    }

                    sibling->SetColor(parent->GetColor());
                    parent->SetColor(_tree_h_black);
                    sleft->SetColor(_tree_h_black);
                    RotateRight(parent);
                    tree = _root;
                }
            }

            parent = tree->GetParent();
        }

        tree->SetColor(_tree_h_black);
    }

    void RestoreRedBlackAfterInsert(TreeNode<Key, Data> *tree) {
        TreeNode<Key, Data> *parent, *grandparent, *uncle;

        if (!tree || !_root || tree == _root)
            return;

        tree->SetColor(_tree_h_red);

        parent = tree->GetParent();

        while ((tree != _root) && (parent->GetColor() == _tree_h_red)) {
            grandparent = parent->GetParent();

            if (parent == grandparent->GetLeft()) {
                uncle = grandparent->GetRight();

                if (uncle && uncle->GetColor() == _tree_h_red) {
                    // Case 1 - Change the colors
                    parent->SetColor(_tree_h_black);
                    uncle->SetColor(_tree_h_black);
                    grandparent->SetColor(_tree_h_red);

                    // Move up the tree
                    tree = grandparent;
                } else { // Uncle is a black node
                    if (tree == parent->GetRight()) {
                        // Case 2 - Move up and rotate
                        tree = parent;
                        RotateLeft(tree);
                    }

                    // Case 3 - Make no changes to _root tree

                    // Change colors for Case 2 / Case 3
                    parent->SetColor(_tree_h_black);
                    grandparent->SetColor(_tree_h_red);
                    RotateRight(grandparent);
                }
            } else { // TreeNode 'tree' is in right subtree
                uncle = grandparent->GetLeft();

                if (uncle && uncle->GetColor() == _tree_h_red) {
                    // Case 1 - Change the colors
                    parent->SetColor(_tree_h_black);
                    uncle->SetColor(_tree_h_black);
                    grandparent->SetColor(_tree_h_red);

                    // Move up the tree
                    tree = grandparent;
                } else { // Uncle is a black node
                    if (tree == parent->GetLeft()) {
                        // Case 2 - Move up and rotate
                        tree = parent;
                        RotateRight(tree);
                    }

                    // Case 3 - Make no changes to _root tree

                    // Change colors for Case 2 / Case 3
                    parent->SetColor(_tree_h_black);
                    grandparent->SetColor(_tree_h_red);
                    RotateLeft(grandparent);
                }
            }

            // Have to adjust parent for new tree node
            parent = tree->GetParent();
        }

        // Mongoose 2002.02.17, Color root black ( heh )
        _root->SetColor(_tree_h_black);
    }

    bool _error;                //!< Error reporting for operator use
    unsigned int _num_elements; //!< Number of nodes in this tree
    TreeNode<Key, Data> *_root; //!< Root node
};

#endif
