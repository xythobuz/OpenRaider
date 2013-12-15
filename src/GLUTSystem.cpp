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
 *-- History -------------------------------------------------
 *
 * 2002.08.09:
 * Mongoose - Created
 =================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#ifdef PS2_LINUX
#   include "ps2.h"
#endif

#include "GLUTSystem.h"


// Mongoose 2003.06.03, Updated to modern times since the wheel
//          was invented, no longer propagate crap up to System
GLUTSystem gGlutSystem;


////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

GLUTSystem::GLUTSystem() : System()
{
}


GLUTSystem::~GLUTSystem()
{
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

void display(void)
{
	gGlutSystem.gameFrame();
}


void reshape(int w, int h)
{
	gGlutSystem.resizeGL(w, h);
}


void key(unsigned char k, int x, int y)
{
	gGlutSystem.handleKeyEvent(k, 0);
}


void special(int key, int x, int y)
{
	gGlutSystem.handleKeyEvent(key, 0);
}


void GLUTSystem::shutdown(int i)
{
	//#ifdef DEBUG_MEMEORY
	//printf("[Mongoose MEMEORY_DEBUG]\nUnfreed memory table:\n");
	//dump_memory_report();
	//#endif

	printf("\n\n\tThanks for testing %s\n", VERSION);
	printf("\tPlease file bug reports and submit video card performance\n\n");
	printf("\tBuild date : %s @ %s\n", __DATE__, __TIME__);
	printf("\tBuild host : %s\n", BUILD_HOST);
	printf("\tEmail bugs : mongoose@users.sourceforge.net\n");
	printf("\tWeb site   : http://openraider.sourceforge.net\n\n");

	exit(0);
}


void GLUTSystem::initVideo(unsigned int width, unsigned int height,
								  bool fullscreen)
{
	glutInit(NULL, 0);
	printf("@Created OpenGL Context...\n");

	m_width = width;
	m_height = height;

	if (fullscreen)
	{
	}

   glutReshapeFunc(reshape);
   glutDisplayFunc(display);
   glutKeyboardFunc(key);
   glutSpecialFunc(special);

	// Start game renderer
	initGL();

	// Resize context
	resizeGL(width, height);
}


void GLUTSystem::runGame()
{
	glutMainLoop();
}


void GLUTSystem::toggleFullscreen()
{
}


void GLUTSystem::swapBuffersGL()
{
	glutSwapBuffers();
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

#ifdef UNIT_TEST_GLUTSYSTEM
int runGLUTSystemUnitTest(int argc, char *argv[])
{
	return 0;
}


int main(int argc, char *argv[])
{
	GLUTSystem test;


	printf("[GLUTSystem class test]\n");

	return runGLUTSystemUnitTest(argc, argv);
}
#endif
