/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : Freyja
 * Author  : Mongoose
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Vector
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: mtk template vector
 *
 *           UINT_MAX is an error condition, used in place of -1
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History ------------------------------------------------
 *
 * 2002.08.31:
 * Mongoose - Created
 ================================================================*/


#ifndef __FREYJA_MONGOOSE_VECTOR_H_
#define __FREYJA_MONGOOSE_VECTOR_H_

#include <stdlib.h>
#include <stdio.h>

#ifdef DEBUG_MEMEORY
#   include "memeory_test.h"
#endif


template <class Object> class Vector
{
public:

	Vector()
	{
		mData = 0x0;
		mError = 0x0;

		mReserve = 0;
		mStart = 0;
		mEnd = 0;

		mIndex = 0;
	}


	Vector(Vector &vector)
	{
		mData = 0x0;
		mError = 0x0;

		mReserve = 0;
		mStart = 0;
		mEnd = 0;

		copy(vector);
	}


	Vector(unsigned int size)
	{
		mData = 0x0;
		mError = 0x0;

		mReserve = 0;
		mStart = 0;
		mEnd = 0;

		mIndex = 0;

		resize(size);
	}


	~Vector()
	{
		if (!empty() && mData)
		{
			delete [] mData;
		}

		clear();
		mReserve = 0;
	}


	void clear()
	{
		mStart = 0;
		mEnd = 0;

		mIndex = 0;
	}


	void erase()
	{
		for (start(); forward(); next())
		{
			if (current())
			{
				delete current();
			}
		}

		clear();
	}


	void reserve(unsigned int count)
	{
		unsigned int i;
		Object *swap = 0x0;


		if (count > mReserve)
		{
			swap = mData;
			mReserve = count;
			mData = new Object[count];
		}

		if (swap)
		{
			for (i = begin(); i < end(); ++i)
			{
				mData[i] = swap[i];
			}

			delete [] swap;
		}
	}


	void resize(unsigned int count)
	{
		resize(count, 0x0);
	}


	void resize(unsigned int count, Object object)
	{
		unsigned int i;


		reserve(count);

		for (i = 0; i < count; ++i)
		{
			if (i < begin() || i >= end())
			{
				mData[i] = object;
			}
		}

		mEnd = count;
	}


	void pushBack()
	{
		pushBack(0x0);
	}


	void pushBack(Object object)
	{
		resize(size() + 1);
		mData[size()-1] = object;
	}


	bool empty()
	{
		return (begin() == end());
	}


	unsigned int capacity()
	{
		return mReserve;
	}


	unsigned int begin()
	{
		return mStart;
	}


	unsigned int end()
	{
		return mEnd;
	}


	unsigned int size()
	{
		return mEnd;
	}


	void copy(Vector<Object> &vector)
	{
		unsigned int i;


		if (vector.capacity() > capacity())
		{
			resize(vector.capacity());
		}

		mStart = vector.begin();
		mEnd = vector.end();

		for (i = vector.begin(); i < vector.end(); ++i)
		{
			mData[i] = vector[i];
			//Add(list->Current()); // WTFBBQ?
		}
	}


	void qSort(int (*compareFunc)(const void *, const void *))
	{
		qsort(mData, end(), sizeof(Object), compareFunc);
	}


	void swap(unsigned int index, unsigned int index2)
	{
		if (index < begin() || index > end())
			return;

		if (index2 < begin() || index2 > end())
			return;

		Object swap = mData[index];
		mData[index] = mData[index2];
		mData[index2] = swap;
	}


	void assign(unsigned int index, Object object)
	{
		mData[index] = object;
	}


	Object operator [] (unsigned int index)
	{
		if (mData == 0x0 || index < begin() || index > end() ||
			 index >= size() || empty())
			return mError;

		return mData[index];
	}


	void print(void (*print_func)(Object))
	{
		unsigned int i;


		for (i = begin(); i < end(); ++i)
		{
			if (print_func)
			{
				(*print_func)(mData[i]);
			}

			fflush(stdout);
		}

		printf("\n");
	}


	// Built-in iterator methods ////////////////////////////////

	void start(unsigned int index)
	{
		if (mData == 0x0 || index < begin() || index > end() ||
			 index >= size() || empty())
			return;

		mIndex = index;
	}

	void start()
	{
		mIndex = begin();
	}


	void finish()
	{
		mIndex = end() - 1;
	}


	bool forward()
	{
		return (mIndex < end());
	}


	bool backward()
	{
		return (mIndex + 1 > begin());
	}


	void next()
	{
		if (mIndex < end())
			++mIndex;
	}

	void prev()
	{
		--mIndex;
	}


	void setError(Object object)
	{
		mError = object;
	}


	unsigned int getCurrentIndex()
	{
		return mIndex;
	}


	unsigned int setCurrentIndex(unsigned int index)
	{
		if (index < end())
		{
			mIndex = index;
		}
	}


	Object current()
	{
		return mData[mIndex];
	}


	/* Requires objects to support '=='  */
	bool find(Object object)
	{
		for (start(); forward(); next())
		{
			if (object == current())
			{
				return true;
			}
		}

		return false;
	}

	// Instead of appending objects this apptempts replacing 'removed' objects
	unsigned int add(Object object)
	{
		if (begin() > 0)
		{
			mData[begin() - 1] = object;
			--mStart;

			return begin();
		}

		pushBack(object);
		return size();
	}


	// Hhhmm... dangerous and fun - this gets your data out of order
	void remove(unsigned int index)
	{
		mData[index] = mData[begin()];
		++mStart;
	}

private:

	Object *mData;

	Object mError;

	unsigned int mReserve;
	unsigned int mStart;
	unsigned int mEnd;

	unsigned int mIndex;
};

#endif
