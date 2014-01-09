/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : Hel
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Entity
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: This is the base thing class
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- Test Defines -----------------------------------------------
 *
 * UNIT_TEST_ENTITY - Builds Entity class as a console unit test
 *
 *-- History ------------------------------------------------
 *
 * 2002.11.09:
 * Mongoose - Created
 ================================================================*/


#ifndef GUARD__HEL_MONGOOSE_ENTITY_H_
#define GUARD__HEL_MONGOOSE_ENTITY_H_

#include <hel/math.h>
#include <hel/BoundingVolume.h>
#include <hel/Quaternion.h>
#include <hel/Vector3d.h>
#include <hel/Mass.h>


typedef enum
{
	EntityFlag_Visible    = 1,
	EntityFlag_Collision  = 2,
	EntityFlag_Particle   = 4,
	EntityFlag_World      = 8

} EntityFlag;


class Entity
{
 public:

	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////


	unsigned int mId;          /* Unique identifier */

	Vector3d mPos;             /* World position */

	Quaternion mOrientation;	/* Orientation (Pitch, Yaw, Roll) */

	BoundingVolume mVolume;		/* Bounding volume of entity */

	Mass mMass;						/* Mass of this entity */

	EntityFlag mFlag;	 			/* Flags of this entity */

 private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////
};

#endif
