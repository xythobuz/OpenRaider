/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 * 
 * Project : Hel
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Mass
 * License : No use w/o permission (C) 2003 Mongoose
 * Comments: This is the class that has the mass - oh yass
 *
 *
 *           This file was generated using Mongoose's C++ 
 *           template generator script.  <stu7440@westga.edu>
 * 
 *-- History ------------------------------------------------- 
 *
 * 2003.06.02:
 * Mongoose - Created, based on algorithms from Erkin Tunca
 =================================================================*/

#include "Mass.h"


////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

Mass::Mass()
{
	mMass = 0.0;
}


Mass::Mass(vec_t mass)
{
	mMass = mass;
}


Mass::Mass(vec_t mass, const Vector3d &position, const Vector3d &velocity)
{
	mMass = mass;
	mPos = position;
	mVelocity = velocity;
}


Mass::~Mass()
{
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

void Mass::applyForce(Vector3d force)
{
	mForce += force;
}


void Mass::rest()
{
	mForce = Vector3d(0, 0, 0);
}


void Mass::simulate(vec_t timeDelta)
{
	// The Euler Method, hhmmm... for now

	// Update velocity
	mVelocity += (mForce / mMass) * timeDelta;

	// Update position, position = velocity * time delta
	mPos += mVelocity * timeDelta;
}


////////////////////////////////////////////////////////////
// Private Accessors
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Private Mutators
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Unit Test code
////////////////////////////////////////////////////////////

#ifdef UNIT_TEST_MASS
int runMassUnitTest(int argc, char *argv[])
{
	Mass test;

	return 0;
}


int main(int argc, char *argv[])
{
	printf("[Mass class test]\n");
	runMassUnitTest(argc, argv);

	return 0;
}
#endif
