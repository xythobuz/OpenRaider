/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : Freyja
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Matrix
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: 3d Matrix in class form
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History ------------------------------------------------
 *
 * 2003.06.17:
 * Mongoose - Now in column order to match OpenGL user needs,
 *            use transpose() to get row order back  =)
 *
 * 2002.05.11:
 * Mongoose - Created, based on my mtk3d matrix
 ================================================================*/


#ifndef _MATRIX_H_
#define _MATRIX_H_


#include <MatMath.h>
#include <Quaternion.h>
#include <Vector3d.h>

    ///////////////////////////////////////////////
    // Multidim map for row order encoding       //
    ///////////////////////////////////////////////
    // 0,0 - 0;   0,1 - 1;   0,2 - 2;   0,3 - 3  //
    // 1,0 - 4;   1,1 - 5;   1,2 - 6;   1,3 - 7  //
    // 2,0 - 8;   2,1 - 9;   2,2 - 10;  2,3 - 11 //
    // 3,0 - 12;  3,1 - 13;  3,2 - 14;  3,3 - 15 //
    ///////////////////////////////////////////////

    ///////////////////////////////////////////////
    // Multidim map for column order encoding    //
    ///////////////////////////////////////////////
    // 0,0 - 0;   0,1 - 4;   0,2 - 8;   0,3 - 12 //
    // 1,0 - 1;   1,1 - 5;   1,2 - 9;   1,3 - 13 //
    // 2,0 - 2;   2,1 - 6;   2,2 - 10;  2,3 - 14 //
    // 3,0 - 3;   3,1 - 7;   3,2 - 11;  3,3 - 15 //
    ///////////////////////////////////////////////


class Matrix
{
 public:

    ////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////

    Matrix();
    /*------------------------------------------------------
     * Pre  :
     * Post : Constructs an object of Matrix
     *
     *-- History ------------------------------------------
     *
     * 2002.05.11:
     * Mongoose - Created
     ------------------------------------------------------*/

    Matrix(matrix_t mat);
    /*------------------------------------------------------
     * Pre  :
     * Post : Constructs an object of Matrix
     *
     *-- History ------------------------------------------
     *
     * 2002.05.11:
     * Mongoose - Created
     ------------------------------------------------------*/

    Matrix(Quaternion &q);
    /*------------------------------------------------------
     * Pre  :
     * Post : Converts and asigns Q to a Matrix
     *        returns quaternion as Matrix
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    ~Matrix();
    /*------------------------------------------------------
     * Pre  : Matrix object is allocated
     * Post : Deconstructs an object of Matrix
     *
     *-- History ------------------------------------------
     *
     * 2002.05.11:
     * Mongoose - Created
     ------------------------------------------------------*/

    ////////////////////////////////////////////////////////////
    // Public Accessors
    ////////////////////////////////////////////////////////////

    void getMatrix(matrix_t mat);
    /*------------------------------------------------------
     * Pre  :
     * Post : Returns this matrix copy
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void getTransposeMatrix(matrix_t mat);
    /*------------------------------------------------------
     * Pre  :
     * Post : Returns this matrix transposed
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    bool getInvert(matrix_t mat);
    /*------------------------------------------------------
     * Pre  :
     * Post : Returns this matrix inverted
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    Matrix multiply(const Matrix &a, const Matrix &b);
    /*------------------------------------------------------
     * Pre  : Multiplies 2 matrices
     * Post : Returns resultant matrix
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void multiply4d(double *v, double *result);
    /*------------------------------------------------------
     * Pre  : Multiplies <V> vector (double[4]) and <This> matrix
     *
     * Post : Returns <Result> vector,
     *        <V> and <Result> maybe be the same vector
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void multiply4v(vec4_t v, vec4_t result);
    /*------------------------------------------------------
     * Pre  : Multiplies <V> vector and <This> matrix
     *
     * Post : Returns <Result> vector,
     *        <V> and <Result> maybe be the same vector
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void multiply3v(vec3_t v, vec3_t result);
    /*------------------------------------------------------
     * Pre  : Multiplies <V> vector and <This> matrix
     *
     * Post : Returns <Result> vector,
     *        <V> and <Result> maybe be the same vector
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void print();
    /*------------------------------------------------------
     * Pre  :
     * Post : Prints matrix values to stdout
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    bool isIdentity();
    /*------------------------------------------------------
     * Pre  :
     * Post : Is this matrix the identity matrix?
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    Matrix operator *(const Matrix &a);
    /*------------------------------------------------------
     * Pre  : Multiplies A and this matrices
     * Post : Returns resultant matrix
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    Vector3d operator *(Vector3d v);
    /*------------------------------------------------------
     * Pre  : <V> is vector to multiply by this matrix
     * Post : Returns resultant vector ( mult )
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/


    ////////////////////////////////////////////////////////////
    // Public Mutators
    ////////////////////////////////////////////////////////////

    void setIdentity();
    /*------------------------------------------------------
     * Pre  :
     * Post : Sets to identity matrix
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void setMatrix(matrix_t mat);
    /*------------------------------------------------------
     * Pre  :
     * Post : Set the matrix ( dangerous, scary boo )
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void rotate(vec_t x, vec_t y, vec_t z);
    /*------------------------------------------------------
     * Pre  : Radian input
     * Post : Rotates object in 3 space
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void rotate(const vec_t *xyz);
    /*------------------------------------------------------
     * Pre  : Rotates object in 3 space, Radian input
     * Post : Returns true on sucess
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void scale(vec_t x, vec_t y, vec_t z);
    /*------------------------------------------------------
     * Pre  :
     * Post : Scales object in 3 space
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void scale(const vec_t *xyz);
    /*------------------------------------------------------
     * Pre  :
     * Post : Scales object in 3 space
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void translate(vec_t x, vec_t y, vec_t z);
    /*------------------------------------------------------
     * Pre  :
     * Post : Translates ( moves ) object in 3 space
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void translate(const vec_t *xyz);
    /*------------------------------------------------------
     * Pre  :
     * Post : Translates ( moves ) object in 3 space
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void transpose();
    /*------------------------------------------------------
     * Pre  :
     * Post : Transpose this matrix
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    matrix_t mMatrix;      /* Data model, moved public for faster
                                      external renderer feedback use */

 private:

    ////////////////////////////////////////////////////////////
    // Private Accessors
    ////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////
    // Private Mutators
    ////////////////////////////////////////////////////////////

    void copy(matrix_t source, matrix_t dest);
    /*------------------------------------------------------
     * Pre  :
     * Post : Copys value of source to dest
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void multiply(const matrix_t a, const matrix_t b, matrix_t result);
    /*------------------------------------------------------
     * Pre  : Multiplies matrices a and b
     *        Neither a or b is also the result
     *
     * Post : Sets resultant matrix value ( result )
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/
};

#endif
