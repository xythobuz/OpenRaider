/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 * 
 * Project : Freyja
 * Author  : Mongoose
 * Website : http://gooseegg.sourceforge.net/
 * Email   : mongoose@users.sourceforge.net
 * Object  : Tree
 * License : GPL See file COPYING, also (C) 2000 Mongoose
 * Comments: Template tree class, which is also the iterator
 *
 *
 *           Red-Black trees are a type of binary search trees
 *           with the properities:
 *
 *           1. Every node is red or black.
 *           2. The root node must be black.
 *           3. Every leaf node is black. (null pointers)
 *           4. If a node is red, then both its children are black.
 *           5. Every simple path from a node to a descendant 
 *              leaf contains the same number of black nodes.
 *           6. Any path from the root to a leaf must not have
 *              adjacent red nodes.
 *
 *
 *           This file was generated using Mongoose's C++ 
 *           template generator script.  <stu7440@westga.edu>
 * 
 *-- History ------------------------------------------------ 
 *
 * 2002.02.17:
 * Mongoose - Rewritten as a Red-Black tree
 *
 * 2002.02.16:
 * Mongoose - Dug out of cobwebs and fixed up 
 *            Yes, I believe in code reuse!  =)
 *
 * 2000.10.26:
 * Mongoose - Created
 ================================================================*/


#ifndef __FREYJA_MONGOOSE_TREE_H_
#define __FREYJA_MONGOOSE_TREE_H_

#include <stdlib.h>
#include <stdio.h>

#ifdef USE_IOSTREAM
#   include <iostream.h>
#endif

#ifdef DEBUG_MEMEORY
#   include "memeory_test.h"
#endif


typedef enum
{
	_tree_h_black,
	_tree_h_red
	
}  _tree_h_color_t;


template <class Key, class Data> class TreeNode
{
public:

	TreeNode(Key key, Data data)
	{
		SetColor(_tree_h_red);
		SetData(data);
		SetKey(key);

		SetParent(NULL);
		SetLeft(NULL);
		SetRight(NULL);
	}


	~TreeNode()
	{		
		TreeNode<Key, Data> *left;
		TreeNode<Key, Data> *right;


		left = GetLeft();
		right = GetRight();

		SetParent(NULL);
		SetLeft(NULL);
		SetRight(NULL);

		if (left)
		{
			left->SetParent(NULL);
			delete left;
		}
			
		if (right)
		{
			right->SetParent(NULL);
			delete right;
		}
	}

	/// Color /////////////////////////////////////

	void SetColor(_tree_h_color_t color)
	{
		_color = color;
	}


	_tree_h_color_t GetColor()
	{
		return _color;
	}


	/// Left, child ///////////////////////////////

	TreeNode<Key, Data> *GetChild() 
	{
		return Left();
	}

	
	void SetChild(TreeNode<Key, Data> *tree) 
	{
		Left(tree);
	}


	TreeNode<Key, Data> *GetLeft() 
	{
		return _left;
	}


	void SetLeft(TreeNode<Key, Data> *tree)
	{	
		if (tree == this)
		{
			return;
		}

		_left = tree;

		if (tree)
		{
			tree->SetParent(this);
		}
	} 

	/// Right, sibling ///////////////////////////

	TreeNode<Key, Data> *GetSibling() 
	{
		return Right();
	}


	void SetSibling(TreeNode<Key, Data> *tree) 
	{
		Right(tree);
	}


	TreeNode<Key, Data> *GetRight() 
	{
		return _right;
	}


	void SetRight(TreeNode<Key, Data> *tree)
	{		
		if (tree == this)
		{
			return;
		}

		_right = tree;

		if (tree)
		{
			tree->SetParent(this);
		}
	}

	/// Data //////////////////////////////////////

	Data GetData()
	{
		return _data;
	}


	void SetData(Data data)
	{
		_data = data;
	}

	/// Key ////////////////////////////////////////

	Key GetKey()
	{
		return _key;
	}


	void SetKey(Key key)
	{
		_key = key;
	}

	/// Parent /////////////////////////////////////

	TreeNode<Key, Data> *GetParent()
	{
		return _parent;
	}


	void SetParent(TreeNode<Key, Data> *parent)
	{
		_parent = parent;
	}


	/// Misc ///////////////////////////////////////

#ifdef USE_IOSTREAM
	void PrintNode()
	{
		cout << "(" << _key << ", " << _data << ", "
			  << ((GetColor() == _tree_h_red) ? "Red" : "Black")
			  << ")";
	}


	void PrintInorder() 
	{
		if (_left)
		{
			_left->PrintInorder();
			//cout << ", ";
			cout << endl;
		}

		PrintNode();
    
		if (_right)
		{
			//cout << ", ";
			cout << endl;
			_right->PrintInorder();
		}
	}
#endif


	void PrintNodeSpecial(void (*print_func_k)(Key), void (*print_func_d)(Data))
	{
		printf("(");

		if (print_func_k)
			(*print_func_k)(_key);

		printf(", ");

		if (print_func_d)
			(*print_func_d)(_data);

		printf(", %s)", ((GetColor() == _tree_h_red) ? "Red" : "Black"));
	}


	void PrintInorderSpecial(void (*print_func_k)(Key), void (*print_func_d)(Data))
	{
		if (_left)
		{
			_left->PrintInorderSpecial(print_func_k, print_func_d);
			printf(",\n");
			//			printf(", ");
		}

		PrintNodeSpecial(print_func_k, print_func_d);

		if (_right)
		{
			printf(",\n");
			//			printf(", ");
			_right->PrintInorderSpecial(print_func_k, print_func_d);
		}
	}


	TreeNode<Key, Data> *SearchByData(Data data, bool *error)
	{
		TreeNode<Key, Data> *tree = NULL;
		*error = true;


		if (_data == data)
		{
			*error = false;
			return this;
		}

		if (_left)
		{
			tree = _left->SearchByData(data, error);
		}

		if (_right && !tree)
		{
			tree = _right->SearchByData(data, error);
		}

		return tree;
	}


	TreeNode<Key, Data> *SearchByKey(Key key, bool *error)
	{
		*error = false;

		if (_key == key)
		{
			return this;
		}
		else if (_left && key < _key)
		{
			return _left->SearchByKey(key, error);
		}
		else if (_right)
		{
			return _right->SearchByKey(key, error);
		}
		else
		{
			*error = true;
			return 0; //NULL;
		}
	}


	void Insert(TreeNode<Key, Data> *tree)
	{
		if (!tree || tree == this)
		{
			return;
		}

		if (tree->GetKey() < _key)
		{
			if (!_left)
			{
				SetLeft(tree);
			}
			else
			{
				_left->Insert(tree);
			}
		}
		else
		{
			if (!_right)
			{
				SetRight(tree);
			}
			else
			{
				_right->Insert(tree);     
			}
		}
	}


private:

	_tree_h_color_t _color;               /* Color of tree node */

	Key _key;                             /* Unique identifer? */

	Data _data;                           /* Data for this tree */

	TreeNode<Key, Data> *_left;           /* Left or child node */

	TreeNode<Key, Data> *_right;          /* Right or sibling node */

	TreeNode<Key, Data> *_parent;         /* Parent of the tree node */
};


////////////////////////////////////////////////////////////////
// Iterator
////////////////////////////////////////////////////////////////


template <class Key, class Data> class Tree
{
public:
	
	Tree()
	{
		_error = false;
		_num_elements = 0;
		_root = 0;
	}


	~Tree()
	{
		Clear();
	}
  

	unsigned int NumElements()
	{
		return _num_elements;
	}


	Data SearchByKey(Key key, bool *error)
	{
		TreeNode<Key, Data> *seeking;


		*error = true;

		// Mongoose 2002.02.16, Nothing to search
		if (!_root)
		{
			return 0;
		}

		seeking = _root->SearchByKey(key, error);
		
		if (seeking)
		{
			return seeking->GetData();
		}

		return 0;
	}


	Key SearchByData(Data data, bool *error)
	{
		TreeNode<Key, Data> *seeking;


		*error = true;

		// Mongoose 2002.02.16, Nothing to search
		if (!_root)
		{
			return 0;
		}

		seeking = _root->SearchByData(data, error);
		
		if (seeking)
		{
			return seeking->GetKey();
		}

		return 0;
	}


	void Insert(Key key, Data data)
	{
		TreeNode<Key, Data> *tree;


		tree = new TreeNode<Key, Data>(key, data);
		++_num_elements;

		if (_root)
		{
			_root->Insert(tree);
			RestoreRedBlackAfterInsert(tree);
		}
		else
		{
			_root = tree;
			_root->SetColor(_tree_h_black);
		}
	}


	bool RemoveByData(Data data)
	{
		bool error;


		if (_root)
		{
			Remove(_root->SearchByData(data, &error));
		}

		return error;
	}


	bool RemoveByKey(Key key)
	{
		bool error;


		if (_root)
		{
#ifdef OBSOLETE
			// Mongoose 2002.02.18, To remove duplicates 
			erorr = false;

			while (!error)
			{
#endif
				Remove(_root->SearchByKey(key, &error));
#ifdef OBSOLETE
			}
#endif
		}

		return error;
	}  


	void Erase()
	{
		Clear();
	}


	void Clear()
	{
		if (_root)
		{
			delete _root;
		}

		_num_elements = 0;
		_error = false;
		_root = 0;
	}

	/// Misc //////////////////////////////////////

	Data operator [] (Key key)
	{
		_error = false;

		if (_root)
		{
			return SearchByKey(key, &_error);
		}

		_error = true;

		return 0;
	}


#ifdef USE_IOSTREAM
	void PrintTree(TreeNode<Key, Data> *tree, unsigned int height,
						unsigned int seek, bool rightmost)
	{
		TreeNode<Key, Data> *left, *right, *parent;


		if (!tree)
		{
			return;
		}

		parent = tree->GetParent();

		if (height == seek)
		{
			if (!parent)
			{
				cout << endl << "[height " << height << "]   " << endl;

				if (tree->GetColor() == _tree_h_red)
				{
					cout << "*";
				}
			}
			else
			{
				if (parent->GetColor() == _tree_h_red &&
					 tree->GetColor() == _tree_h_red)
				{
					cout << "*";
				}
			}

			cout << "(" << tree->GetKey() << ", " 
				  << ((tree->GetColor() == _tree_h_red) ? "red" : "blk")
				  << ")";

			if (rightmost)
			{
				cout << endl << "[height " << (height+1) << "]   " << endl;

				PrintTree(_root, 0, ++seek, true);
			}
			else
			{
				cout << " ";
			}

			return;
		}
		else if (seek < height)
		{			
			return;
		}

		left = tree->GetLeft();
		right = tree->GetRight();
		++height;

		if (left)
		{
			PrintTree(left, height, seek, false);
		}
		else
		{
			cout << "(-, blk) ";
		}
		
		if (right)
		{
			PrintTree(right, height, seek, rightmost);
		}
		else
		{
			cout << "(-, blk) ";
		}

		if (parent)
		{
			if (parent->GetRight() != tree)
			{
				cout << " |  ";
			}
		}
	}


	void PrintAsTree()
	{
		PrintTree(_root, 0, 0, true);
		cout << endl << "Nodes marked with * are in error" << endl;
	}
  

	void Print()
	{
		cout << "Tree: " << _num_elements <<" elements {" << endl;    

		if (_root)
		{
			cout << "Root: ";
			_root->PrintNode();
			cout << endl;
			_root->PrintInorder();
		}

		cout << endl << "}" << endl;
	}
#endif


	void PrintSpecial(void (*print_func_k)(Key), void (*print_func_d)(Data))
	{
		printf("Tree: %i elements {\n", _num_elements);

		if (_root && print_func_k && print_func_d)
		{
			printf("Root: ");
			_root->PrintNodeSpecial(print_func_k, print_func_d);
			printf("\n");
			_root->PrintInorderSpecial(print_func_k, print_func_d);
		}

		printf("\n}\n");
	}


	Key Root()
	{
		if (_root)
		{
			return _root->GetKey();
		}

		return 0;
	}


	bool Error()
	{
		return _error;
	}

	bool IsValidRedBlackTree()
	{
		return IsValidRedBlackTreeCheck(_root, true);
	}

private:

	TreeNode<Key, Data> *GetSuccessor(TreeNode<Key, Data> *tree)
	{
		TreeNode<Key, Data> *successor;


		successor = tree->GetRight();

		if (successor)
		{
			while (successor->GetLeft())
			{
				successor = successor->GetLeft();
			}

			return successor;
		}
		else
		{
			successor = tree->GetParent();

			while (tree == successor->GetRight())
			{
				tree = successor;
				successor = successor->GetParent();
			}

			if (successor == _root)
			{
				return NULL;
			}

			return successor;
		}
	}


	TreeNode<Key, Data> *GetPredecessor(TreeNode<Key, Data> *tree)
	{
		TreeNode<Key, Data> *predecessor;


		predecessor = tree->GetLeft();

		if (predecessor)
		{
			while (predecessor->GetRight())
			{
				predecessor = predecessor->GetRight();
			}

			return predecessor;
		}
		else
		{
			predecessor = tree->GetParent();

			while (tree == predecessor->GetLeft())
			{
				if (predecessor == _root)
				{
					return NULL;
				}

				tree = predecessor;
				predecessor = predecessor->GetParent();
			}

			return predecessor;
		}		
	}


	bool IsValidRedBlackTreeCheck(TreeNode<Key, Data> *current, bool valid)
	{
		TreeNode<Key, Data> *right, *left;
		_tree_h_color_t color_red;

		if (!current)
		{
			return valid;
		}

		// Mongoose 2002.02.19, Check for a red root
		if (!current->GetParent() && current->GetColor() == _tree_h_red)
		{
			return false;
		}

		color_red = (current->GetColor() == _tree_h_red);
		left = current->GetLeft();
		right = current->GetRight();

		// Mongoose 2002.02.19, Check for adj red nodes
		if (left)
		{
			if (color_red && left->GetColor() == _tree_h_red)
			{
				return false;
			}

			if (!IsValidRedBlackTreeCheck(left, valid))
				return false;
		}

		if (right)
		{
			if (color_red && right->GetColor() == _tree_h_red)
			{
				return false;
			}

			if (!IsValidRedBlackTreeCheck(right, valid))
				return false;
		}

		return true;
	}


	void RotateLeft(TreeNode<Key, Data> *tree)
	{
		TreeNode<Key, Data> *right, *right_leftchild, *parent, *uncle;


		if (!tree || !_root)
		{
			return;
		}

		// Get tree's right node
		right = tree->GetRight();

		// Get right node's left child
		right_leftchild = NULL;

		if (right)
		{
			right_leftchild = right->GetLeft();
		}

		// Set tree's right node to right's left child
		tree->SetRight(right_leftchild);

		// Child now has a new parent
		if (right_leftchild)
		{
			right_leftchild->SetParent(tree);
		}

		// Right also has a new parent
		if (right)
		{
        right->SetParent(tree->GetParent());
		}

		// Get parent
		parent = tree->GetParent();

		if (parent)  // Not root
		{
			uncle = parent->GetLeft();

			// Mix up at hosptial, switch parent's children!
			if (tree == uncle)
			{
				parent->SetLeft(right);
			}
			else
			{
				parent->SetRight(right);				
			}
		}
		else // TreeNode 'tree' was root, so now right is root
		{
			_root = right;
		}

		if (right)
		{
			// TreeNode 'tree' is now right's left child
			right->SetLeft(tree);

			if (tree)
			{
				tree->SetParent(right);
			}
		}
	}


	void RotateRight(TreeNode<Key, Data> *tree)
	{
		TreeNode<Key, Data> *left, *left_rightchild, *parent, *uncle;


		if (!tree || !_root)
		{
			return;
		}

		left = tree->GetLeft();

		left_rightchild = NULL;

		if (left)
		{
			left_rightchild = left->GetRight();
		}

		tree->SetLeft(left_rightchild);

		if (left_rightchild)
		{
        left_rightchild->SetParent(tree);
		}

		if (left)
		{
        left->SetParent(tree->GetParent());  
		}

		parent = tree->GetParent();

		if (parent)    //if node is not the root
		{
			uncle = parent->GetRight();

			if (tree == uncle)
			{
				parent->SetRight(left);
			}
			else
			{
				parent->SetLeft(left);
			}
		}
		else
		{
        _root = left;
		}

		left->SetRight(tree);

		if (tree)
		{
        tree->SetParent(left);
		}
	}


	void TreeNodeShallowCopy(TreeNode<Key, Data> *src, 
									 TreeNode<Key, Data> *dest, bool no_links)
	{
		if (!src || !dest)
		{
			return;
		}

		dest->SetKey(src->GetKey());
		dest->SetData(src->GetData());
		dest->SetColor(src->GetColor());

		if (!no_links)
		{
			dest->SetRight(src->GetRight());
			dest->SetLeft(src->GetLeft());
			dest->SetParent(src->GetParent());
		}
	}


	void Remove(TreeNode<Key, Data> *tree)
	{
		TreeNode<Key, Data> *left, *right, *parent, *prev, *cur;


		// Mongoose 2002.02.16, Nothing to remove
		if (!tree || !_root)
		{
			return;
		}

		left = tree->GetLeft();
		right = tree->GetRight();
		parent = tree->GetParent();


		if (!left || !right)
		{
			prev = tree;
		}
		else
		{
			prev = GetSuccessor(tree);
		}

		if (prev->GetLeft())
		{
			cur = prev->GetLeft();
		}
		else
		{
			cur = prev->GetRight();  
		}

		if (cur)
		{
			cur->SetParent(prev->GetParent());
		}

		if (!prev->GetParent())
		{
			_root = cur;
		}
		else
		{
			parent = prev->GetParent();

			if (prev == parent->GetLeft())
			{
				parent->SetLeft(cur);
			}
			else
			{
				parent->SetRight(cur);
			}
		}

		if (prev != tree)
		{
			TreeNodeShallowCopy(prev, tree, true);
			
			if (prev->GetParent())
			{
				if (prev == (prev->GetParent())->GetLeft())
					(prev->GetParent())->SetLeft(tree);
				else if (prev == (prev->GetParent())->GetRight())
					(prev->GetParent())->SetRight(tree);
			}
		}

		--_num_elements;

		if (prev)
		{
			prev->SetRight(NULL);
			prev->SetParent(NULL);
			prev->SetLeft(NULL);

			delete prev;     
		}

		if (tree->GetColor() == _tree_h_black)
		{
			RestoreRedBlackAfterRemove(cur);
		}
	}


	void RestoreRedBlackAfterRemove(TreeNode<Key, Data> *tree)
	{
		TreeNode<Key, Data> *parent, *sibling, *sleft, *sright;


		if (!tree || !_root)
		{
			return;
		}

		parent = tree->GetParent();

		while ((tree != _root) && (parent->GetColor() == _tree_h_black)) 
		{
			if (tree == parent->GetLeft()) 
			{
				sibling = parent->GetRight();

				if (sibling && sibling->GetColor() == _tree_h_red) 
				{
					sibling->SetColor(_tree_h_black);
					parent->SetColor(_tree_h_red);
					RotateLeft(parent);
					sibling = parent->GetRight();
				}

				if (sibling)
				{
					sleft = sibling->GetLeft();
					sright = sibling->GetRight();
				}
				else
				{
					sleft = sright = NULL;
				}

				if (sright && sright->GetColor() == _tree_h_black &&
					 sleft && sleft->GetColor() ==_tree_h_black)
				{
					sibling->SetColor(_tree_h_red);
					tree = parent;
				}
				else
				{
					if (sright && sright->GetColor() == _tree_h_black)
					{
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
			}
			else
			{
				sibling = parent->GetLeft();

				if (sibling && sibling->GetColor() == _tree_h_red)
				{
					sibling->SetColor(_tree_h_black);
					parent->SetColor(_tree_h_red);
					RotateLeft(parent);
					sibling = parent->GetLeft();
				}

				if (sibling)
				{
					sleft = sibling->GetLeft();
					sright = sibling->GetRight();
				}
				else
				{
					sleft = sright = NULL;
				}

				if (sright && sright->GetColor() == _tree_h_black &&
					 sleft && sleft->GetColor() ==_tree_h_black)
				{
					sibling->SetColor(_tree_h_red);
					tree = parent;
				}
				else
				{
					if (sleft && sleft->GetColor() == _tree_h_black)
					{
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


	void RestoreRedBlackAfterInsert(TreeNode<Key, Data> *tree)
	{
		TreeNode<Key, Data> *parent, *grandparent, *uncle;


		if (!tree || !_root || tree == _root)
		{
			return;
		}

		tree->SetColor(_tree_h_red);
		
		parent = tree->GetParent();

		while ((tree != _root) && (parent->GetColor() == _tree_h_red)) 
		{
			grandparent = parent->GetParent();
			
			if (parent == grandparent->GetLeft()) 
			{
				uncle = grandparent->GetRight();

				if (uncle && uncle->GetColor() == _tree_h_red) 
				{
               // Case 1 - Change the colors
               parent->SetColor(_tree_h_black);
					uncle->SetColor(_tree_h_black);
					grandparent->SetColor(_tree_h_red);

               // Move up the tree
               tree = grandparent;
				}
				else // Uncle is a black node
				{
               if (tree == parent->GetRight())
					{
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
			}
			else // TreeNode 'tree' is in right subtree
			{
				uncle = grandparent->GetLeft();
								
				if (uncle && uncle->GetColor() == _tree_h_red) 
				{
               // Case 1 - Change the colors
               parent->SetColor(_tree_h_black);
					uncle->SetColor(_tree_h_black);
					grandparent->SetColor(_tree_h_red);

               // Move up the tree
               tree = grandparent;
				}
				else // Uncle is a black node
				{
               if (tree == parent->GetLeft())
					{
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


	bool _error;                      /* Error reporting for operator use */

	unsigned int _num_elements;       /* Number of nodes in this tree */

	TreeNode<Key, Data> *_root;       /* Root node */
};

#endif
