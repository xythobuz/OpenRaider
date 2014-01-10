/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : Hel
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Vector3d
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: Math vector
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- Test Defines -----------------------------------------------
 *
 * UNIT_TEST_VECTOR3D - Builds Vector3d class as a console unit test
 *
 *-- History ------------------------------------------------
 *
 * 2002.12.24:
 * Mongoose - Created
 ================================================================*/


#ifndef GUARD__HEL_MONGOOSE_VECTOR3D_H_
#define GUARD__HEL_MONGOOSE_VECTOR3D_H_


#include <MatMath.h>

class Vector3d
{
 public:

	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	Vector3d();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Constructs an object of Vector3d
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	Vector3d(vec3_t v);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Constructs an object of Vector3d
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	Vector3d(vec_t x, vec_t y, vec_t z);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Constructs an object of Vector3d
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~Vector3d();
	/*------------------------------------------------------
	 * Pre  : Vector3d object is allocated
	 * Post : Deconstructs an object of Vector3d
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////

	static vec_t dot(const Vector3d &u, const Vector3d &v);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns dot product of U and V vectors
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.05.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	static Vector3d cross(const Vector3d &u, const Vector3d &v);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns cross product of U and V vectors
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.05.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	vec_t magnitude();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns magnitude this vector
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.05.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	Vector3d unit();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns normalized copy of this vector
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.05.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	static Vector3d zeroVector();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns the Zero vector <0, 0, 0>
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	Vector3d operator +(const Vector3d &v);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns a vector = this vector + v
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.05.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	Vector3d operator -(const Vector3d &v);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns a vector = this vector - v
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.05.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	Vector3d operator -();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns a copy of this vector, negated
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.05.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	Vector3d operator *(vec_t s);
	/*------------------------------------------------------
	 * Pre  : S is scalar to scale by
	 * Post : Returns scale by S of this vector ( mult )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.05.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	Vector3d operator /(vec_t s);
	/*------------------------------------------------------
	 * Pre  : S is scalar to scale by
	 * Post : Returns scale by S of this vector ( div )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.05.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	vec_t operator *(const Vector3d &v);
	/*------------------------------------------------------
	 * Pre  : V is vector to dot by
	 * Post : Returns dot by V with this vector
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.05.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	void normalize();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Normalizes *this vector
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.05.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void zero();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :This is set to the Zero vector <0, 0, 0>
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void operator =(const Vector3d &v);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : this = v, values are assigned   =)
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.05.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void operator +=(const Vector3d &v);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : this += v, values are sumed, assigned   =)
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.05.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void operator -=(const Vector3d &v);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : this -= v, values are diffed, assigned   =)
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.05.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void operator *=(vec_t s);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : this *= s, values are scaled, assigned   =)
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.05.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	vec3_t mVec;   /* Vector data */

 private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////

};

#endif
