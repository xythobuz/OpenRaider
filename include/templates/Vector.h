/*!
 * \file include/templates/Vector.h
 * \brief Template Vector
 *
 * \author Mongoose
 */

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdlib.h>
#include <stdio.h>

/*!
 * \brief Template class for a (pretty strange) Vector
 * \tparam Object datatype the Vector can store
 */
template <class Object> class Vector {
public:

    /*!
     * \brief Construct an empty Vector
     */
    Vector() {
        mData = 0x0;
        mError = 0x0;
        mReserve = 0;
        mStart = 0;
        mEnd = 0;
        mIndex = 0;
    }

    /*!
     * \brief Construct a Vector from another one
     * \param vector Vector to copy from
     */
    Vector(Vector &vector) {
        mData = 0x0;
        mError = 0x0;
        mReserve = 0;
        mStart = 0;
        mEnd = 0;
        mIndex = 0;
        copy(vector);
    }

    /*!
     * \brief Construct a Vector with a specific size
     * \param size initial size
     */
    Vector(unsigned int size) {
        mData = 0x0;
        mError = 0x0;
        mReserve = 0;
        mStart = 0;
        mEnd = 0;
        mIndex = 0;
        resize(size);
    }

    /*!
     * \brief Deconstruct a Vector
     */
    ~Vector() {
        if (!empty() && mData)
            delete [] mData;
        clear();
        mReserve = 0;
    }

    /*!
     * \brief Clears the Vector, but deletes nothing
     * \sa Vector::erase()
     */
    void clear() {
        mStart = 0;
        mEnd = 0;
        mIndex = 0;
    }

    /*!
     * \brief Clears the vector and deletes everything contained
     * \sa Vector::clear()
     */
    void erase() {
        for (start(); forward(); next()) {
            if (current())
                delete current();
        }
        clear();
    }

    /*!
     * \brief Reserve more memory
     * \param count new maximum size
     */
    void reserve(unsigned int count) {
        Object *swap = 0x0;
        if (count > mReserve) {
            swap = mData;
            mReserve = count;
            mData = new Object[count];
        }
        if (swap) {
            for (unsigned int i = begin(); i < end(); ++i)
                mData[i] = swap[i];
            delete [] swap;
        }
    }

    /*!
     * \brief Resize the Vector
     * \param count new size
     */
    void resize(unsigned int count) {
        resize(count, 0x0);
    }

    /*!
     * \brief Resize the Vector
     * \param count new size
     * \param object what to put into blank spaces
     */
    void resize(unsigned int count, Object object) {
        reserve(count);
        for (unsigned int i = 0; i < count; ++i) {
            if (i < begin() || i >= end())
                mData[i] = object;
        }
        mEnd = count;
    }

    /*!
     * \brief Increase size by 1
     */
    void pushBack() {
        pushBack(0x0);
    }

    /*!
     * \brief Increase size by 1
     * \param object what to put into new space
     */
    void pushBack(Object object) {
        resize(size() + 1);
        mData[size()-1] = object;
    }

    /*!
     * \brief Check if empty
     * \returns true if begin() equals end()
     */
    bool empty() {
        return (begin() == end());
    }

    /*!
     * \brief Check maximum capacity
     * \returns reserved memory
     */
    unsigned int capacity() {
        return mReserve;
    }

    /*!
     * \brief Get start index
     * \returns start index
     */
    unsigned int begin() {
        return mStart;
    }

    /*!
     * \brief Get end index
     * \returns end index
     */
    unsigned int end() {
        return mEnd;
    }

    /*!
     * \brief Get size
     * \returns end index
     */
    unsigned int size() {
        return mEnd;
    }

    /*!
     * \brief Copy a Vector into this one.
     * May increase size.
     * \param vector Vector to copy from
     */
    void copy(Vector<Object> &vector) {
        unsigned int i;
        if (vector.capacity() > capacity()) {
            resize(vector.capacity());
        }
        mStart = vector.begin();
        mEnd = vector.end();
        mError = vector.Error();
        mIndex = vector.getCurrentIndex();
        for (i = vector.begin(); i < vector.end(); ++i) {
            mData[i] = vector[i];
        }
    }

    /*!
     * \brief Sort the Vector
     * \param compareFunc comparison function for qsort
     */
    void qSort(int (*compareFunc)(const void *, const void *)) {
        qsort(mData, end(), sizeof(Object), compareFunc);
    }

    /*!
     * \brief Swap two items
     * \param index first index
     * \param index2 second index
     */
    void swap(unsigned int index, unsigned int index2) {
        if (index < begin() || index > end())
            return;

        if (index2 < begin() || index2 > end())
            return;

        Object swap = mData[index];
        mData[index] = mData[index2];
        mData[index2] = swap;
    }

    /*!
     * \brief Set an index to a value
     * \param index index to set
     * \param object object to set it to
     */
    void assign(unsigned int index, Object object) {
        mData[index] = object;
    }

    /*!
     * \brief Get value at index
     * \param index index to look at
     * \returns data for index, or error object
     */
    Object operator [] (unsigned int index) {
        if (mData == 0x0 || index < begin() || index > end() || index >= size() || empty())
            return mError;
        return mData[index];
    }

    /*!
     * \brief Print the Vector
     * \param print_func function that can print Objects
     */
    void print(void (*print_func)(Object)) {
        for (unsigned int i = begin(); i < end(); ++i) {
            if (print_func)
                (*print_func)(mData[i]);
            fflush(stdout);
        }
        printf("\n");
    }


    /*!
     * \brief Start Iterator at specific index
     * \param index where to start
     */
    void start(unsigned int index) {
        if (mData == 0x0 || index < begin() || index > end() ||
             index >= size() || empty())
            return;

        mIndex = index;
    }

    /*!
     * \brief Set Iterator to the first element
     */
    void start() {
        mIndex = begin();
    }

    /*!
     * \brief Set Iterator to the last element
     */
    void finish() {
        mIndex = end() - 1;
    }

    /*!
     * \brief Check if the Iterator can go forward
     * \returns true if Iterator is still in range
     */
    bool forward() {
        return (mIndex < end());
    }

    /*!
     * \brief Check if the Iterator can go backwards
     * \returns true if Iterator is already in range
     */
    bool backward() {
        return (mIndex + 1 > begin());
    }

    /*!
     * \brief Increment the Iterator
     */
    void next() {
        if (mIndex < end())
            ++mIndex;
    }

    /*!
     * \brief Decrement the Iterator
     */
    void prev() {
        --mIndex;
    }

    /*!
     * \brief Set the error object
     * \param object new error object
     */
    void setError(Object object) {
        mError = object;
    }

    /*!
     * \brief Get Iterator index
     * \returns current Iterator index
     */
    unsigned int getCurrentIndex() {
        return mIndex;
    }

    /*!
     * \brief Set Iterator index
     * \param index new Iterator index
     */
    void setCurrentIndex(unsigned int index) {
        if (index < end())
            mIndex = index;
    }

    /*!
     * \brief Get current element
     * \returns element at Iterator index
     */
    Object current() {
        return mData[mIndex];
    }

    /*!
     * \brief Check if an object is in the Vector.
     * Requires objects to support `==`.
     * \param object object to find
     * \returns true if found, false if not.
     */
    bool find(Object object) {
        for (start(); forward(); next()) {
            if (object == current())
                return true;
        }
        return false;
    }

    //
    /*!
     * \brief Add an object.
     * Instead of appending objects this attempts replacing 'removed' objects.
     * \param object object to add
     * \returns index of added object
     */
    unsigned int add(Object object) {
        if (begin() > 0) {
            mData[begin() - 1] = object;
            --mStart;

            return begin();
        }

        pushBack(object);
        return size();
    }


    /*!
     * \brief Remove an object. This will change the index of another element!
     * \param index index to "remove".
     */
    void remove(unsigned int index) {
        mData[index] = mData[begin()];
        ++mStart;
    }

private:
    Object *mData;         //!< Data array
    Object mError;         //!< Error object
    unsigned int mReserve; //!< Index for reserved space
    unsigned int mStart;   //!< Start index
    unsigned int mEnd;     //!< End index
    unsigned int mIndex;   //!< Iterator index
};

#endif
