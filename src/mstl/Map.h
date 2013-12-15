/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 * 
 * Project : Freyja
 * Author  : Mongoose
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Map
 * License : No use w/o permission (C) 2000-2002 Mongoose
 * Comments: mtk Template list
 * 
 *           This file was generated using Mongoose's C++ 
 *           template generator script.  <stu7440@westga.edu>
 * 
 *-- History ------------------------------------------------ 
 *
 * 2002.02.19:
 * Mongoose - Using RBTree and list overlay for faster access
 *
 *            Access: O(1)/O(n)/O(logn) ?
 *            Insert: O(nlogn)          ?
 *            Remove: O(nlogn)          ?
 *
 * 2002.02.16:
 * Mongoose - Binary tree overlay started... at 0300  /_\ Zzzz
 *
 * 2002.02.01:
 * Mongoose - Dug out of cobwebs and fixed up 
 *            Yes, I believe in code reuse!  =)
 *
 * 2000.10.26:
 * Mongoose - Created
 ================================================================*/


#ifndef __FREYJA_MONGOOSE_MAP_H_
#define __FREYJA_MONGOOSE_MAP_H_

#include <stdlib.h>
#include <stdio.h>
#include "Tree.h"

#ifdef DEBUG_MEMEORY
#   include "memeory_test.h"
#endif



template <class K, class D> class MapNode
{
public:

	MapNode(K key, D data)
	{
		_data  = data;
		_key = key;
		_next = NULL;
	}

	
	~MapNode()
	{
	}
	

	void Key(K key)
	{
		_key = key;
	}
	

	K Key()
	{
		return _key;
	}
	

	void Data(D data)
	{
    _data = data;
	}
	

	D Data()
	{
		return _data;
	}
	

	MapNode<K, D> *Next()
	{
		return _next;
	}
	

	void Next(MapNode<K, D> *next)
	{
		_next = next;
	}
	
private:

	MapNode<K, D> *_next;

	K _key;

	D _data;
};


template <class K, class D> class Map
{
public:
	
	Map()
	{
		UnSetError();
		_num_items = 0;
		_head = NULL;
		_current = NULL;
		_cache = NULL;
	}
	
	
	~Map()
	{
		Clear();
	}
	

	void Clear()
	{
		UnSetError();
		_num_items = 0;
		_cache = NULL;
		
		while (_head)
		{
			_current = _head;
			_head = _head->Next();
			delete _current;
		}

		_tree.Clear();
	}


	void SetError()
	{
		_error = true;
	}
	

	void UnSetError()
	{
		_error = false;
	}
	

	bool GetError()
	{
		return _error;
	}


	D FindDataByKey(K key)
	{
		MapNode<K, D> *current = NULL;
		MapNode<K, D> *next = NULL;
		

		if (_head)
		{
			UnSetError();

			if (_cache)
			{
				next = _cache->Next();
			}

			// Mongoose 2002.02.19, Optimize for sequential searches
			if (next && key == next->Key())
			{
				current = next;
			}
			else // Mongoose 2002.02.19, Use search algorithm otherwise
			{
				current = _tree.SearchByKey(key, &_error);

				if (_error)
					return false;
			}
			
			if (current)
			{
				_cache = _current = current;
				return current->Data();
			}
		}

		SetError();
		return 0;
	}


	D operator [] (K key)
	{
		return FindDataByKey(key);
	}
	

	K FindKeyByData(D data)
	{
		MapNode<K, D> *current = _head;
		MapNode<K, D> *last = NULL;
		

		UnSetError();
		
		while (current)
		{
			// Found
			if (current->Data() == data)
			{
				_cache = current;
				return current->Key();
			}
			
			last = current;
			current = current->Next();
		}

		SetError();

		return 0;
	}


	D getCache()
	{
		if (_cache == 0x0)
		{
			printf("Map::getCache> Bad request - should segfault\n");
		}

		return _cache->Data();
	}


	bool findKey(K key)
	{
		MapNode<K, D> *current = NULL;
		MapNode<K, D> *next = NULL;

		if (_head)
		{
			UnSetError();

			if (_cache)
			{
				if (_cache->Key() == key)
				{
					return true;
				}

				next = _cache->Next();
			}

			// Mongoose 2002.02.19, Optimize for sequential searches
			if (next && key == next->Key())
			{
				current = next;
			}
			else // Mongoose 2002.02.19, Use search algorithm otherwise
			{
				current = _tree.SearchByKey(key, &_error);
			}
			
			if (current)
			{
				_cache = _current = current;
				//curData = current->Data();
				return true;
			}
		}

		SetError();
		return false;
	}

	
	bool Add(K key, D data)
	{
		MapNode<K, D> *node;
		
		
		UnSetError();
		node = new MapNode<K, D>(key, data);
		_num_items++;
		return Add(node);
	}


	bool Add(MapNode<K, D> *node)
	{
		MapNode<K, D> *current = _head;
		MapNode<K, D> *last = NULL;


		if (!node)
			return false;

		UnSetError();

		if (_head)
		{
			current = _head;
			last = NULL;

			while (current)
			{
				// Prepend
				if (current->Key() > node->Key())
				{
					node->Next(current);
					
					if (current == _head)
					{
						_head = node;
					}
					else if (last)
					{
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
		}
		else
		{
			_head = node;
		}

		_tree.Insert(node->Key(), node);		
		return true;
	}


	void RemoveByKey(K key)
	{
		MapNode<K, D> *current = _head;
		MapNode<K, D> *last = NULL;
		

		UnSetError();

		_cache = NULL;

		while (current)
		{
			// Remove
			if (current->Key() == key)
			{
				if (current == _head)
				{
					_head = current->Next();
				}
				else
				{
					last->Next(current->Next());
				}
				
				if (_current == current)
				{
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


	void RemoveByData(D data)
	{
		MapNode<K, D> *current = _head;
		MapNode<K, D> *last = NULL;
		

		UnSetError();

		_cache = NULL;

		while (current)
		{
			// Remove
			if (current->Data() == data)
			{
				if (current == _head)
				{
					_head = current->Next();
				}
				else
				{
					last->Next(current->Next());
				}
				
				if (_current == current)
				{
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


	bool Empty()
	{
		return (_head == NULL);
	}


	unsigned int count()
	{
		return _num_items;
	}

	unsigned int NumItems()
	{
		return _num_items;
	}


	void Print(void (*print_key_func)(K), void (*print_data_func)(D))
	{
		MapNode<K, D> *current = _head;


		UnSetError();

		if (!print_key_func || !print_data_func)
		{
			SetError();
			return;
		}

		printf(" [%i] {\n", _num_items);
		
		while (current)
		{
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


	void Reset()
	{
		_current = _head;
		_cache = _head;
	}


	bool operator ++ (int dummy)
	{
		return Next();
	}


	bool Next()
	{
		if (_current)
		{
			_current = _current->Next();
		}
		
		return (_current != NULL);
	}


	bool CurrentExists()
	{
		return (_current != 0);
	}


	K CurrentKey()
	{
		UnSetError();
		
		if (!_current)
		{
			SetError();
			return 0;
		}
		
		return _current->Key();
	}


	D Current()
	{
		UnSetError();
		
		if (!_current)
		{
			SetError();
			return 0;
		}
		
		return _current->Data();
	}

private:
	
	unsigned int _num_items;
	
	bool _error;
	
	Tree<K, MapNode<K, D> *> _tree;

	MapNode<K, D> *_head;
	
	MapNode<K, D> *_current;

	MapNode<K, D> *_cache;
};
#endif
