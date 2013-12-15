/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 * 
 * Project : Hel
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Particle
 * License : No use w/o permission (C) 2001-2003 Mongoose
 * Comments: Partcle system's atomic base
 *
 *
 *           This file was generated using Mongoose's C++ 
 *           template generator script.  <stu7440@westga.edu>
 * 
 *-- History ------------------------------------------------ 
 *
 * 2003.06.02: 
 * Mongoose - Now using Hel mass system
 *
 * 2001.08.13:
 * Mongoose - Created
 ================================================================*/


#ifndef __HEL_MONGOOSE_PARTICLEMASS_H_
#define __HEL_MONGOOSE_PARTICLEMASS_H_

#include <hel/Mass.h>


class ParticleMass
{
public:

	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	ParticleMass();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Constructs an object of Particle
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.08.13: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~ParticleMass();
	/*------------------------------------------------------
	 * Pre  : Particle object is allocated
	 * Post : Deconstructs an object of Particle
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.08.13: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void simulate(vec_t timeDelta)
	{
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Ajusts for particle life cycle
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.08.13: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

		mLife -= mBlend * timeDelta;
		
		// Reset 'dead' OR fully blended particles
		if (mLife < 0.0)
		{
			mLife = 1.0;
		}

		mMass->simulate(timeDelta);
	}


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	Mass *mMass;				/* Particle mass */

	bool mActive;           /* Is this particle in use? */
	
	int mTextureId;         /* Texture Id to use on polygon */	
	
	vec_t mLife;            /* Life of particle  */
	
	vec_t mBlend;           /* Blend amount or fade per sec */

	vec3_t mColor;          /* Current color */

private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////

};

#endif
