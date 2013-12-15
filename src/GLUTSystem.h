/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 * 
 * Project : UnRaider
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : GLUTSystem
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: 
 *
 *
 *           This file was generated using Mongoose's C++ 
 *           template generator script.  <stu7440@westga.edu>
 * 
 *-- Test Defines -----------------------------------------------
 *           
 * UNIT_TEST_GLUTSYSTEM - Builds GLUTSystem class as a console unit test 
 *
 *-- History ------------------------------------------------ 
 *
 * 2002.08.09:
 * Mongoose - Created
 ================================================================*/


#ifndef GUARD__UNRAIDER_MONGOOSE_GLUTSYSTEM_H_
#define GUARD__UNRAIDER_MONGOOSE_GLUTSYSTEM_H_

#include "System.h"


class GLUTSystem : public System
{
 public:

	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	GLUTSystem();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Constructs an object of GLUTSystem
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~GLUTSystem();
	/*------------------------------------------------------
	 * Pre  : GLUTSystem object is allocated
	 * Post : Deconstructs an object of GLUTSystem
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09: 
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	void shutdown(int i);

	void initVideo(unsigned int width, unsigned int height, 
						bool fullscreen);

	void runGame();

	void toggleFullscreen();

	void swapBuffersGL();

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
