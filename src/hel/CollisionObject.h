/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : libHel
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : CollisionObject
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


#ifndef GUARD__LIBHEL_MONGOOSE_COLLISIONOBJECT_H_
#define GUARD__LIBHEL_MONGOOSE_COLLISIONOBJECT_H_

#include <hel/Vector3d.h>

class CollisionObject
{
 public:

	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	CollisionObject();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Constructs an object of CollisionObject
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.11.27:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~CollisionObject();
	/*------------------------------------------------------
	 * Pre  : CollisionObject object is allocated
	 * Post : Deconstructs an object of CollisionObject
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.11.27:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////

    bool intersectPoint(Vector3d p);

	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

    Vector3d mIntersection;


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
