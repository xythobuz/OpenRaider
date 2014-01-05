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
 *-- History -------------------------------------------------
 *
 * 2003.06.02:
 * Mongoose - Created, based on algorithms from Erkin Tunca
 =================================================================*/

#include <math.h>
#include "Simulation.h"


////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

Simulation::Simulation()
{
	mMassCount = 0;
}


Simulation::~Simulation()
{
	clear();
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////

Mass *Simulation::getMass(unsigned int i)
{
	return mMasses[i];
}


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

void Simulation::attachCollisionObject(CollisionObject *obj)
{
	mObjects.pushBack(obj);
}


void Simulation::clear()
{
	mMasses.erase();
}


bool Simulation::collisionDetected(Mass *mass)
{
	bool detected = false;

	for (mObjects.start(); mObjects.forward(); mObjects.next())
	{
		if ((mObjects.current())->intersectPoint(mass->mPos))
		{
			detected = true;
			resolveCollision(mass, mObjects.current());
		}
	}

	return detected;
}


void Simulation::generateMasses(unsigned int count, vec_t mass)
{
	generateMasses(count, mass, Vector3d(0, 0, 0), Vector3d(0, 0, 0));
}


void Simulation::generateMasses(unsigned int count, vec_t mass,
										  const Vector3d &position,
										  const Vector3d &velocity)
{
	unsigned int i;


	mMassCount = count;
	//mMasses.reserve(mMasses.size() + count);

	for (i = 0; i < count; ++i)
	{
		mMasses.pushBack(new Mass(mass, position, velocity));
	}
}


void Simulation::init()
{
	Mass *m;

	for (mMasses.start(); mMasses.forward(); mMasses.next())
	{
		m = mMasses.current();

		if (!m)
			continue;

		m->rest();
	}
}


void Simulation::resolveCollision(Mass *mass, CollisionObject *obj)
{
	mass->mPos = obj->mIntersection;
}


void Simulation::run(vec_t timeDelta)
{
	// Reset forces
	init();

	// Apply external forces
	solve();

	// Update change in time
	simulate(timeDelta);
}


void Simulation::simulate(vec_t timeDelta)
{
	Mass *m;

	for (mMasses.start(); mMasses.forward(); mMasses.next())
	{
		m = mMasses.current();

		if (!m)
			continue;

		m->simulate(timeDelta);
	}
}


void Simulation::solve()
{
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

#ifdef UNIT_TEST_SIMULATION

#define SDL_UNIT_TEST
#include "SDLUnitTest.cpp"

Simulation *gEmitter;
GravitySimulation gGravityEmitter;
SpringSimulation gSpringEmitter;
SpringSimulation gSpringEmitter2;
bool gOn = false;
float gSpeed = 0.1f;


// Draw particles
#define ENABLE_TEX_PARTICLES
void renderScene(float time, float lastTime)
{
	Mass *m;
	Vector3d last;
	vec_t x, y, z;

	glBindTexture(GL_TEXTURE_2D, 1);

	if (gOn)
		gEmitter->run((time - lastTime) * 0.1 * gSpeed); // slow it down

#ifdef ENABLE_TEX_PARTICLES
	// Alpha is determined by life, which is affected by blend amount
	//glColor4f(mColor, g, b, life);

	// Render tristrip quad
	for (gEmitter->mMasses.start(); gEmitter->mMasses.forward();
		  gEmitter->mMasses.next())
	{
		m = gEmitter->mMasses.current();

		if (!m)
			continue;

		if (gEmitter == &gSpringEmitter)
		{
			if (gEmitter->mMasses.getCurrentIndex() == 0)
			{
				glDisable(GL_TEXTURE_2D);

				glColor4f(0.0, 1.0, 0.0, 1.0);
				glBegin(GL_POINTS);
				glVertex3fv(gSpringEmitter.connectionPos.mVec);
				glEnd();
			}

			glColor4f(gEmitter->mMasses.getCurrentIndex()*0.33, 0.0, 1.0, 1.0);
			glBegin(GL_POINTS);
			glVertex3fv(m->mPos.mVec);
			glEnd();
			glBegin(GL_LINES);
			glVertex3fv(m->mPos.mVec);
			glVertex3fv(gSpringEmitter.connectionPos.mVec);
			glEnd();
		}
		else
		{
			x = m->mPos.mVec[0];
			y = m->mPos.mVec[1];
			z = m->mPos.mVec[2];

			glBegin(GL_TRIANGLE_STRIP);

			glTexCoord2d(1.0, 1.0);
			glVertex3f(x + 25, y + 25, z);

			glTexCoord2d(0.0, 1.0);
			glVertex3f(x - 25, y + 25, z);

			glTexCoord2d(1.0, 0.0);
			glVertex3f(x + 25, y - 25, z);

			glTexCoord2d(0.0, 0.0);
			glVertex3f(x - 25, y - 25, z);

			glEnd();
		}
	}

#else
	glBegin(GL_POINTS);

	for (gEmitter.mMasses.start(); gEmitter.mMasses.forward();
		  gEmitter.mMasses.next())
	{
		m = gEmitter.mMasses.current();

		if (!m)
			continue;

		glColor4f(0.0, 1.0, 1.0, 1.0);
		glVertex3fv(m->mPos.mVec);
	}

	glEnd();
#endif
}


void handleKey(int key)
{
	Mass *m;

	switch (key)
	{
	case ' ':
		m = gGravityEmitter.getMass(0);

		if (m)
		{
			m->mVelocity = Vector3d(0.0f, 120.0f, 0.0f);
		}
		break;
	case 'r':
		gGravityEmitter.clear();

		gGravityEmitter.generateMasses(1, 1.0f,
												 Vector3d(0.0f, 0.0f, 0.0f),
												 Vector3d(0.0f, 96.0f, -64.0f));
		gGravityEmitter.generateMasses(1, 1.0f,
												 Vector3d(-128.0f, 0.0f, 0.0f),
												 Vector3d(32.0f, 96.0f, 0.0f));
		gGravityEmitter.generateMasses(1, 1.0f,
												 Vector3d(128.0f, 0.0f, 0.0f),
												 Vector3d(-32.0f, 96.0f, 0.0f));
		break;
	case 't':
		if (gEmitter == &gGravityEmitter)
		{
			gEmitter = &gSpringEmitter;
		}
		else
		{
			gEmitter = &gGravityEmitter;
		}
		break;
	case 'f':
		gGravityEmitter.gravitation *= 10.0;
		printf("G is now %f\n", gGravityEmitter.gravitation.mVec[1]);
		break;
	case 'g':
		if (fabs(gGravityEmitter.gravitation.mVec[1] * 0.1) > 0.0000001)
			gGravityEmitter.gravitation *= 0.1;

		printf("G is now %f\n", gGravityEmitter.gravitation.mVec[1]);
		break;
	case 's':
		gOn = !gOn;
		break;
	case '[':
		gSpringEmitter.springConstant += 0.5f;
		printf("Spring constant = %f\n", gSpringEmitter.springConstant);
		break;
	case ']':
		if (gSpringEmitter.springConstant - 0.5 > 0.0f)
			gSpringEmitter.springConstant -= 0.5f;
		printf("Spring constant = %f\n", gSpringEmitter.springConstant);
		break;
	case '.':
		gSpeed *= 10.0;
		printf("Time is at %f speed\n", gSpeed);
		break;
	case ',':
		if (gSpeed * 0.1 > 0.0000001)
			gSpeed *= 0.1;
		printf("Time is at %f speed\n", gSpeed);
		break;
	case 'h':
		printf("\n");
		printf("----------------------------------\n");
		printf("t -       Toggle emitter\n");
		printf("r -       Reset gravity emitter test\n");
		printf("[ ] -     Adjust spring stiffness\n");
		printf(", . -     Adjust time delta\n");
		printf("g -       Scale gravity down\n");
		printf("f -       Scale gravity up\n");
		printf("s -       Start/stop simulation\n");
		break;
	}
}


void handleArgKey(char key)
{
	switch (key)
	{
	default:
		;
	}
}


void initScene(int argc, char *argv[])
{
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);    // Q3A uses front face culling
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_LIGHTING);

	glPointSize(5.0f);
	gYawOn = false;
	gScale = 0.3f;

	// Texture setup
	gTexture.reset();
	gTexture.setFlag(Texture::fUseMipmaps);
	gTexture.setMaxTextureCount(64);
	printf("Loading textures: ");
	gTexture.loadTGA("data/snow.tga");
	gTexture.loadTGA("data/snow2.tga");
	printf(" done\n\n");
}


int runSimulationUnitTest(int argc, char *argv[])
{
	gEmitter = &gGravityEmitter;

	// Masses created above the origin, traveling down on Y axis
	// as if it where in a vacuum pulled by gravity
	gGravityEmitter.generateMasses(1, 1.0f,
											 Vector3d(0.0f, 0.0f, 0.0f),
											 Vector3d(0.0f, 96.0f, 0.0f));
	gGravityEmitter.generateMasses(1, 1.0f,
											 Vector3d(-64.0f, 1424.0f, 0.0f),
											 Vector3d(-9.8f, 0.0f, 0.0f));
	gGravityEmitter.generateMasses(1, 1.0f,
											 Vector3d(64.0f, 1424.0f, 0.0f),
											 Vector3d(9.8f, 0.0f, 0.0f));

	//InternalBBoxCollisionObject *obj =
	//new InternalBBoxCollisionObject(-9999, 0, -9999,
	//										 9999, 9999, 9999);

	HeightCollisionObject *obj = new HeightCollisionObject(0, 9999);

	/* Don't fall out ot the box world  =) */
	gGravityEmitter.attachCollisionObject(obj);
	gSpringEmitter.attachCollisionObject(obj);

	// Spring
	gSpringEmitter.connectionPos = Vector3d(0.0f, 200.0f, 0.0f);
	gSpringEmitter.springConstant = 0.3f;
	gSpringEmitter.generateMasses(1, 0.5f,
											Vector3d(0.0f, -512.0f, 0.0f),
											Vector3d(3.0f, 0.0f, 0.0f));
	gSpringEmitter.generateMasses(1, 0.5f,
											Vector3d(-512.0f, -128.0f, 0.0f),
											Vector3d(0.0f, 3.0f, 0.0f));
	gSpringEmitter.generateMasses(1, 0.5f,
											Vector3d(512.0f, -128.0f, 0.0f),
											Vector3d(0.0f, 0.0f, 0.3f));

	updateWindowTitle("[Simulation particle class test]");
	runSDLUnitTest(argc, argv);

	return 0;
}


int main(int argc, char *argv[])
{
	printf("[Simulation class test]\n");
	runSimulationUnitTest(argc, argv);

	return 0;
}
#endif
