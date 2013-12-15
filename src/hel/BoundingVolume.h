/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 * 
 * Project : libHel
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : BoundingVolume
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: Bounding volume class for world/vis 
 *           culling/clipping/collision stuff
 *
 *
 *           This file was generated using Mongoose's C++ 
 *           template generator script.  <stu7440@westga.edu>
 * 
 *-- Test Defines -----------------------------------------------
 *           
 * UNIT_TEST_BOUNDINGVOLUME - Builds BoundingVolume class unit test 
 *
 *-- History ------------------------------------------------ 
 *
 * 2002.11.27:
 * Mongoose - Created
 ================================================================*/


#ifndef GUARD__LIBHEL_MONGOOSE_BOUNDINGVOLUME_H_
#define GUARD__LIBHEL_MONGOOSE_BOUNDINGVOLUME_H_

#include <hel/math.h>


class BoundingSphere
{
public:
	vec3_t mCenter;            /* Center of bounding sphere */

	vec_t mRadius;             /* Raduis of bounding sphere */	
};


class BoundingBox
{
public:
	vec3_t mMin;           /* Bounding box MIN point */

	vec3_t mMax;           /* Bounding box MAX point */
};


class BoundingVolume
{
 public:

	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	BoundingVolume();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Constructs an object of BoundingVolume
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.11.27: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~BoundingVolume();
	/*------------------------------------------------------
	 * Pre  : BoundingVolume object is allocated
	 * Post : Deconstructs an object of BoundingVolume
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.11.27: 
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////


	BoundingSphere mSphere;    /* Bounding sphere of this volume */

	BoundingBox mBox;;         /* Bounding box of this volume */

 private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////


	/* */
};

#endif
