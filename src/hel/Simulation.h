/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 * 
 * Project : Hel
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Simulation
 * License : No use w/o permission (C) 2003 Mongoose
 * Comments: This class simulates mass physics
 *
 *
 *           This file was generated using Mongoose's C++ 
 *           template generator script.  <stu7440@westga.edu>
 * 
 *-- Test Defines -----------------------------------------------
 *           
 * UNIT_TEST_SIMULATION - Builds Simulation class as a console unit test 
 *
 *-- History ------------------------------------------------ 
 *
 * 2003.06.08:
 * Mongoose - Generic collision and basic resolution w/o generic
 *            Hel Resolvers
 *
 * 2003.06.02:
 * Mongoose - Created, based on algorithms from Erkin Tunca
 ================================================================*/


#ifndef GUARD__HEL_MONGOOSE_SIMULATION_H_
#define GUARD__HEL_MONGOOSE_SIMULATION_H_

#include <mstl/Vector.h>
#include <hel/CollisionObject.h>
#include <hel/Mass.h>
#include <hel/math.h>


class Simulation
{
 public:

	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	Simulation();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Constructs an object of Simulation
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.02: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual ~Simulation();
	/*------------------------------------------------------
	 * Pre  : Simulation object is allocated
	 * Post : Deconstructs an object of Simulation
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.02: 
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////

	Mass *getMass(unsigned int i);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Get <i>th Mass element of this simulation
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.02:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	virtual void attachCollisionObject(CollisionObject *obj);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Adds <Obj> to the collision list for this simulation
	 *
	 * TODO : Allow multiple types of collision to be excluded?
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void clear();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Removes all masses from this simulation
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.02:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual bool collisionDetected(Mass *mass);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Returns true if <Mass> collided with object
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void generateMasses(unsigned int count, vec_t mass);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : <Count> number of masses with <mass> value are added
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.02:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void generateMasses(unsigned int count, vec_t mass,
										 const Vector3d &position, 
										 const Vector3d &velocity);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : <Count> number of masses with 
	 *        <mass>, <position>, and <velocity> are added
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.02:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void init();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Reset forces acting on masses
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.02:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void resolveCollision(Mass *mass, CollisionObject *obj);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Resolves a collision of <Obj> and <Mass>
	 *
	 *        The default resolution here is to set mass pos
	 *        to intersection point with the object, there
	 *        is no affect on velocity
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void run(vec_t timeDelta);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Does full init, solve, simulate cycle
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.02:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void simulate(vec_t timeDelta);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Update change in time for each mass
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.02:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void solve ();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Have forces act on masses  
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.02:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	Vector<Mass *> mMasses;			/* It's a collection of masses, silly */

	Vector<CollisionObject *> mObjects;	/* Object(s) to test collision with */

	unsigned int mMassCount;		/* Cache of number of masses in collection  */


 private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////
};


class GravitySimulation : public Simulation
{
public:

	GravitySimulation() : Simulation()      
	{
		gravitation = Vector3d(0.0f, -9.81f, 0.0f);
	}


	virtual void solve()                                                    
	{
		Mass *m;

		// Apply gravitational force to each mass
		for (mMasses.start(); mMasses.forward(); mMasses.next())
		{
			m = mMasses.current();

			// Gravitational force, Force = (Mass)(Gravity)
			m->applyForce(gravitation * m->mMass);
			
			if (collisionDetected(m))
			{
				m->mVelocity = Vector3d(0, 0, 0);
			}
		}
	}


	Vector3d gravitation;     /* Gravitational acceleration */
};


class SpringSimulation : public Simulation
{
public:

	SpringSimulation() : Simulation()
	{
	}

	
	virtual void generateMasses(unsigned int count, vec_t mass,
										 const Vector3d &position, 
										 const Vector3d &velocity)
	{
		Simulation::generateMasses(count, mass, 
											connectionPos + position, velocity);
	}


	virtual void solve()
	{
		Mass *m;

		// Apply spring force to each mass
		for (mMasses.start(); mMasses.forward(); mMasses.next())
		{
			m = mMasses.current();

			// Vector from mass to spring connection
			Vector3d springVector = m->mPos - connectionPos;
			
			// Apply spring force, spring force formulation
			m->applyForce(-springVector * springConstant);

			if (collisionDetected(m))
			{
				//m->mVelocity = Vector3d(0, 0, 0);
			}
		}
	}

	vec_t springConstant;     /* Stiffness, higher values more spring force */

	Vector3d connectionPos;   /* Location where mass is connected to spring */
};
#endif
