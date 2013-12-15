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
 *-- History -------------------------------------------------
 *
 * 2002.11.27:
 * Mongoose - Created
 =================================================================*/

#include "CollisionObject.h"


////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

CollisionObject::CollisionObject()
{
}


CollisionObject::~CollisionObject()
{
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////

bool CollisionObject::intersectPoint(Vector3d p) {
    return 0;
}

////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Private Accessors
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Private Mutators
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Unit Test code
////////////////////////////////////////////////////////////

#ifdef UNIT_TEST_CollisionObject
int runCollisionObjectUnitTest()
{
	CollisionObject col;

	return 0;
}


int main(int argc, char *argv[])
{
	printf("[CollisionObject class test]\n");

	runCollisionObjectUnitTest();

	return 0;
}
#endif
