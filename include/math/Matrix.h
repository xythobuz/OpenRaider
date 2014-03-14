/*!
 * \file include/math/Matrix.h
 * \brief 3D Matrix
 *
 * \author Mongoose
 */

#ifndef _MATH_MATRIX_H_
#define _MATH_MATRIX_H_

#include "math/math.h"
#include "math/Quaternion.h"
#include "math/Vector3d.h"


/*!
 * \brief 3D Matrix
 *
 * Multidim map for row order encoding
 *
 *     ///////////////////////////////////////////////
 *     // 0,0 - 0;   0,1 - 1;   0,2 - 2;   0,3 - 3  //
 *     // 1,0 - 4;   1,1 - 5;   1,2 - 6;   1,3 - 7  //
 *     // 2,0 - 8;   2,1 - 9;   2,2 - 10;  2,3 - 11 //
 *     // 3,0 - 12;  3,1 - 13;  3,2 - 14;  3,3 - 15 //
 *     ///////////////////////////////////////////////
 *
 * Multidim map for column order encoding
 *
 *     ///////////////////////////////////////////////
 *     // 0,0 - 0;   0,1 - 4;   0,2 - 8;   0,3 - 12 //
 *     // 1,0 - 1;   1,1 - 5;   1,2 - 9;   1,3 - 13 //
 *     // 2,0 - 2;   2,1 - 6;   2,2 - 10;  2,3 - 14 //
 *     // 3,0 - 3;   3,1 - 7;   3,2 - 11;  3,3 - 15 //
 *     ///////////////////////////////////////////////
 */
class Matrix {
public:

    /*!
     * \brief Constructs an object of Matrix
     */
    Matrix();

    /*!
     * \brief Constructs an object of Matrix
     * \param mat Matrix as data source
     */
    Matrix(matrix_t mat);

    /*!
     * \brief Constructs an object of Matrix
     * \param q Converts and assigns the Quaternion to the Matrix
     */
    Matrix(Quaternion &q);

    /*!
     * \brief Returns this matrix copy
     * \param mat target
     */
    void getMatrix(matrix_t mat);

    /*!
     * \brief Returns this matrix transposed
     * \param mat target
     */
    void getTransposeMatrix(matrix_t mat);

    /*!
     * \brief Returns this matrix inverted
     * \param mat target
     */
    bool getInvert(matrix_t mat);

    /*!
     * \brief Multiplies two matrices
     * \param a first matrix
     * \param b second matrix
     * \returns resultant matrix
     */
    static Matrix multiply(const Matrix &a, const Matrix &b);

    /*!
     * \brief Multiplies v vector and this matrix
     * \param v vector
     * \param result where the result will be stored, may be same as v
     */
    void multiply4v(vec4_t v, vec4_t result);

    /*!
     * \brief Multiplies v vector and this matrix
     * \param v vector
     * \param result where the result will be stored, may be same as v
     */
    void multiply3v(vec3_t v, vec3_t result);

    /*!
     * \brief Prints matrix values to stdout
     */
    void print();

    /*!
     * \brief Is this matrix the identity matrix?
     * \returns true if it is identity, false otherwise
     */
    bool isIdentity();

    /*!
     * \brief Multiplies a and this matrix
     * \param a matrix to multiply with
     * \returns resultant matrix
     */
    Matrix operator *(const Matrix &a);

    /*!
     * \brief Multiply vector by this matrix
     * \param v Vector to multiply with
     * \returns resultant vector (mult)
     */
    Vector3d operator *(Vector3d v);

    /*!
     * \brief Sets to identity matrix
     */
    void setIdentity();

    /*!
     * \brief S et the matrix
     * \fixme dangerous, scary, boo!
     * \param mat new matrix
     */
    void setMatrix(matrix_t mat);

    /*!
     * \brief Rotate object in 3D space
     * \param x x rotation in radians
     * \param y y rotation in radians
     * \param z z rotation in radians
     */
    void rotate(vec_t x, vec_t y, vec_t z);

    /*!
     * \brief Rotate object in 3D space
     * \param xyz rotation in radians
     */
    void rotate(const vec_t *xyz);

    /*!
     * \brief Scale object in 3D space
     * \param x x scaling
     * \param y y scaling
     * \param z z scaling
     */
    void scale(vec_t x, vec_t y, vec_t z);

    /*!
     * \brief Scale object in 3D space
     * \param xyz scaling factors
     */
    void scale(const vec_t *xyz);

    /*!
     * \brief Translate (move) object in 3D space
     * \param x x translation
     * \param y y translation
     * \param z z translation
     */
    void translate(vec_t x, vec_t y, vec_t z);

    /*!
     * \brief Translate (move) object in 3D space
     * \param xyz translations
     */
    void translate(const vec_t *xyz);

    /*!
     * \brief Transpose this matrix
     */
    void transpose();

    matrix_t mMatrix; //!< Data model, moved public for faster external renderer feedback use

private:

    /*!
     * \brief Copys value from source to dest
     * \param source source
     * \param dest destination
     */
    static void copy(matrix_t source, matrix_t dest);

    /*!
     * \brief Multiplies matrices a and b. Neither a or b is also the result.
     * \param a first matrix
     * \param b second matrix
     * \param result wil be set to resultant matrix value
     */
    static void multiply(const matrix_t a, const matrix_t b, matrix_t result);
};

#endif

