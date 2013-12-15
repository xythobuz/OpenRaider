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
 *-- Test Defines -----------------------------------------------
 *           
 * UNIT_TEST_MASS - Builds Mass class as a console unit test 
 *
 *-- History ------------------------------------------------ 
 *
 * 2003.06.02:
 * Mongoose - Created, based on algorithms from Erkin Tunca
 ================================================================*/


#ifndef GUARD__HEL_MONGOOSE_MASS_H_
#define GUARD__HEL_MONGOOSE_MASS_H_

#include <hel/math.h>
#include <hel/Vector3d.h>

class Mass
{
 public:

	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	Mass();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Constructs an object of Mass
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.02: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	Mass(vec_t mass);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Constructs an object of Mass
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.02: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	Mass(vec_t mass, const Vector3d &position, const Vector3d &velocity);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Constructs an object of Mass
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.02: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual ~Mass();
	/*------------------------------------------------------
	 * Pre  : Mass object is allocated
	 * Post : Deconstructs an object of Mass
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.02: 
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	virtual void applyForce(Vector3d force);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : External force acts on... the mass
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.02:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void rest();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Reset forces acting on this mass
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.02:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void simulate(vec_t timeDelta);
	/*------------------------------------------------------
	 * Pre  : Given change in time
	 * Post : Adjust velocity and position with respect to dt
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.02:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	vec_t mMass;          /* The value of the mass */

	Vector3d mPos;        /* Location in 3 space of... the mass */

	Vector3d mVelocity;   /* Velocity of... the mass */

	Vector3d mForce;      /* Force acting on the mass */


 private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////

};

#endif
