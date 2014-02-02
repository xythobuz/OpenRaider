/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : Hel
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Quaternion
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: Quaternion now in C++ class form fresh from the grove
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- Test Defines -----------------------------------------------
 *
 * UNIT_TEST_QUATERNION - Builds Quaternion class as a console unit test
 *
 *-- History ------------------------------------------------
 *
 * 2002.12.16:
 * Mongoose - Created, based on mtk3d ( freyja )
 ================================================================*/


#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include <MatMath.h>

class Quaternion
{
 public:

    ////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////

    Quaternion();
    /*------------------------------------------------------
     * Pre  :
     * Post : Constructs an object of Quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.12.16:
     * Mongoose - Created
     ------------------------------------------------------*/

    Quaternion(vec_t w, vec_t x, vec_t y, vec_t z);
    /*------------------------------------------------------
     * Pre  :
     * Post : Constructs an object of Quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.12.16:
     * Mongoose - Created
     ------------------------------------------------------*/

    Quaternion(vec4_t v);
    /*------------------------------------------------------
     * Pre  : v { w, x, y, z }
     * Post : Constructs an object of Quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.12.16:
     * Mongoose - Created
     ------------------------------------------------------*/

    ~Quaternion();
    /*------------------------------------------------------
     * Pre  : Quaternion object is allocated
     * Post : Deconstructs an object of Quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.12.16:
     * Mongoose - Created
     ------------------------------------------------------*/


    ////////////////////////////////////////////////////////////
    // Public Accessors
    ////////////////////////////////////////////////////////////

    void getMatrix(matrix_t m);
    /*------------------------------------------------------
     * Pre  : Matrix is valid
     * Post : Returns col order matrix equiv of this quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    Quaternion &operator =(const Quaternion &q);
    /*------------------------------------------------------
     * Pre  :
     * Post : Asigns Q to this quaternion
     *        returns (this) resultant quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    Quaternion operator *(const Quaternion &q);
    /*------------------------------------------------------
     * Pre  :
     * Post : Multiplies Q and this quaternion
     *        returns resultant quaternion
     *        ( Use normalize() call for unit quaternion )
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    Quaternion operator /(const Quaternion &q);
    /*------------------------------------------------------
     * Pre  :
     * Post : Dividess Q from this quaternion
     *        returns quotient quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    Quaternion operator +(const Quaternion &q);
    /*------------------------------------------------------
     * Pre  :
     * Post : Adds Q and this quaternion
     *        returns resultant quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    Quaternion operator -(const Quaternion &q);
    /*------------------------------------------------------
     * Pre  :
     * Post : Subtracts Q from this quaternion
     *        returns resultant quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    bool operator ==(const Quaternion &q);
    /*------------------------------------------------------
     * Pre  :
     * Post : Compares Q to this quaternion
     *        returns boolean true if equal, otherwise false
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    Quaternion conjugate();
    /*------------------------------------------------------
     * Pre  :
     * Post : Returns conjugate of this quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    Quaternion scale(vec_t s);
    /*------------------------------------------------------
     * Pre  :
     * Post : Returns scaled result of this quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    Quaternion inverse();
    /*------------------------------------------------------
     * Pre  :
     * Post : Returns inverse of this quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    static vec_t dot(Quaternion a, Quaternion b);
    /*------------------------------------------------------
     * Pre  :
     * Post : Returns dot product of A and B quaternions
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    vec_t magnitude();
    /*------------------------------------------------------
     * Pre  :
     * Post : Returns magnitude this quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    static Quaternion slerp(Quaternion a, Quaternion b, vec_t time);
    /*------------------------------------------------------
     * Pre  :
     * Post : Interpolates between A and B rotations and
     *        returns resultant quaternion using
     *        spherical linear interpolation:
     *
     *        I = (((B . A)^ -1)^ Time) A
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
     * Post : Sets this quaternion to identity
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void set(vec_t angle, vec_t x, vec_t y, vec_t z);
    /*------------------------------------------------------
     * Pre  :
     * Post : Sets this quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void normalize();
    /*------------------------------------------------------
     * Pre  :
     * Post : Normalize this quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void copy(Quaternion q);
    /*------------------------------------------------------
     * Pre  :
     * Post : Set this quaternion using q
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    void setByMatrix(matrix_t m);
    /*------------------------------------------------------
     * Pre  : Matrix is valid column order
     * Post : Sets matrix equiv of this quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/


 private:

    ////////////////////////////////////////////////////////////
    // Private Accessors
    ////////////////////////////////////////////////////////////

    Quaternion multiply(Quaternion a, Quaternion b);
    /*------------------------------------------------------
     * Pre  :
     * Post : Multiplies A and B quaternions
     *        returns resultant quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    Quaternion divide(Quaternion a, Quaternion b);
    /*------------------------------------------------------
     * Pre  :
     * Post : Divides B from A quaternion
     *        returns quotient quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    Quaternion add(Quaternion a, Quaternion b);
    /*------------------------------------------------------
     * Pre  :
     * Post : Adds A and B quaternions
     *        returns resultant quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    Quaternion subtract(Quaternion a, Quaternion b);
    /*------------------------------------------------------
     * Pre  :
     * Post : Subtracts B from A quaternion
     *        returns resultant quaternion
     *
     *-- History ------------------------------------------
     *
     * 2002.05.08:
     * Mongoose - Created
     ------------------------------------------------------*/

    ////////////////////////////////////////////////////////////
    // Private Mutators
    ////////////////////////////////////////////////////////////

    vec_t mW, mX, mY, mZ;          /* Quaternion */
};

#endif
