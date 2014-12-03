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
#include "math/Vec3.h"


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
    Matrix(float mat[16]);

    /*!
     * \brief Constructs an object of Matrix
     * \param q Converts and assigns the Quaternion to the Matrix
     */
    Matrix(Quaternion& q);

    /*!
     * \brief Returns this matrix copy
     * \param mat target
     */
    void getMatrix(float mat[16]);

    /*!
     * \brief Returns this matrix transposed
     * \param mat target
     */
    void getTransposeMatrix(float mat[16]);

    /*!
     * \brief Returns this matrix inverted
     * \param mat target
     */
    bool getInvert(float mat[16]);

    /*!
     * \brief Multiplies two matrices
     * \param a first matrix
     * \param b second matrix
     * \returns resultant matrix
     */
    static Matrix multiply(const Matrix& a, const Matrix& b);

    /*!
     * \brief Multiplies v vector and this matrix
     * \param v vector
     * \param result where the result will be stored, may be same as v
     */
    void multiply4v(float v[4], float result[4]);

    /*!
     * \brief Multiplies v vector and this matrix
     * \param v vector
     * \param result where the result will be stored, may be same as v
     */
    void multiply3v(float v[3], float result[3]);

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
    Matrix operator *(const Matrix& a);

    /*!
     * \brief Multiply vector by this matrix
     * \param v Vector to multiply with
     * \returns resultant vector (mult)
     */
    Vec3 operator *(Vec3 v);

    /*!
     * \brief Sets to identity matrix
     */
    void setIdentity();

    /*!
     * \brief S et the matrix
     * \fixme dangerous, scary, boo!
     * \param mat new matrix
     */
    void setMatrix(float mat[16]);

    /*!
     * \brief Rotate object in 3D space
     * \param x x rotation in radians
     * \param y y rotation in radians
     * \param z z rotation in radians
     */
    void rotate(float x, float y, float z);

    /*!
     * \brief Rotate object in 3D space
     * \param xyz rotation in radians
     */
    void rotate(const float* xyz);

    /*!
     * \brief Scale object in 3D space
     * \param x x scaling
     * \param y y scaling
     * \param z z scaling
     */
    void scale(float x, float y, float z);

    /*!
     * \brief Scale object in 3D space
     * \param xyz scaling factors
     */
    void scale(const float* xyz);

    /*!
     * \brief Translate (move) object in 3D space
     * \param x x translation
     * \param y y translation
     * \param z z translation
     */
    void translate(float x, float y, float z);

    /*!
     * \brief Translate (move) object in 3D space
     * \param xyz translations
     */
    void translate(const float* xyz);

    /*!
     * \brief Transpose this matrix
     */
    void transpose();

  private:

    /*!
     * \brief Copys value from source to dest
     * \param source source
     * \param dest destination
     */
    static void copy(float source[16], float dest[16]);

    /*!
     * \brief Multiplies matrices a and b. Neither a or b is also the result.
     * \param a first matrix
     * \param b second matrix
     * \param result wil be set to resultant matrix value
     */
    static void multiply(const float a[16], const float b[16], float result[16]);

    float mMatrix[16];
};

#endif

