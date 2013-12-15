/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 * 
 * Project : Freyja
 * Author  : Mongoose
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Camera
 * License : No use w/o permission (C) 2001 Mongoose
 * Comments: OpenGL camera class for Freyja
 *
 *           This file was generated using Mongoose's C++ 
 *           template generator script.  <stu7440@westga.edu>
 * 
 *-- History ------------------------------------------------- 
 *
 * 2002.12.16:
 * Mongoose - Removed perspective setting and OpenGL dependency
 *            API changes to reflect new direction of this object:
 *              Removing outdated algorithms and code 
 *              And refactoring the class in general
 *
 * 2001.06.06:
 * Mongoose - Moving GLU code into here to setup break up
 *            into Camera base class, DynamicCamera,
 *            and GLUCamera child classes
 *
 * 2001.06.04:
 * Mongoose - Quaternion based compile option
 *
 * 2001.05.18:
 * Mongoose - Created
 =================================================================*/

#include <math.h>
#include <hel/math.h>
#include "Camera.h"

#ifdef DEBUG_MEMEORY
#   include "memeory_test.h"
#endif


unsigned int Camera::mCounter = 0;

////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

Camera::Camera()
{
	mId = ++mCounter;
	mFlags = 0;
	mViewDistance = 14.0;
	mTranslateDelta = 256.0;
	mRotateDelta = helDegToRad(15.0);
 	mRotateDelta2 = helDegToRad(5.0);
	mFlags &= Camera_FlyMode;
	reset();
}


Camera::~Camera()
{
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////

unsigned int Camera::getId()
{
	return mId;
}


void Camera::getPosition(vec3_t pos)
{
	pos[0] = mPos[0];
	pos[1] = mPos[1];
	pos[2] = mPos[2];
}


void Camera::getUp(vec3_t up)
{
	up[0] = mUp[0];
	up[1] = mUp[1];
	up[2] = mUp[2];
}


void Camera::getTarget(vec3_t target)
{
	target[0] = mTarget[0];
	target[1] = mTarget[1];
	target[2] = mTarget[2];
}


float Camera::getYaw()
{
	return helRadToDeg(mTheta);
}


double Camera::getRadianYaw()
{
	return mTheta;
}


double Camera::getRadianPitch()
{
	return mTheta2;
}


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

void Camera::rotate(float angle, float x, float y, float z)
{
	Quaternion t, n;
	Matrix matrix;
	double side[4] = { 1, 0,  0, 1 };
	double up[4] =   { 0, 1,  0, 1 };
	double look[4] = { 0, 0, -1, 1 };
 	unsigned int i;
	matrix_t m;
   

	t.set(angle, x, y, z);
	n = mQ * t;
	n.normalize();

	n.getMatrix(m);
	matrix.setMatrix(m);
	matrix.multiply4d(side, mSide);
	matrix.multiply4d(look, mTarget);
	matrix.multiply4d(up, mUp);

	for (i = 0; i < 3; ++i)
	{
		mSide[i] += mPos[i];
		mTarget[i] += mPos[i];
		mUp[i] += mPos[i];
	}
    
	mQ = n;
}


void Camera::translate(float x, float y, float z)
{
	int i;
	double result[4];
	double v[4];
	matrix_t m;
	Matrix matrix;


	v[0] = x;
	v[1] = y;
	v[2] = -z;
	v[3] = 1;

	m[0] = mSide[0] - mPos[0];
	m[1] = mUp[0] - mPos[0];
	m[2] = mTarget[0] - mPos[0];
	m[3] = 0;
	m[4] = mSide[1] - mPos[1];
	m[5] = mUp[1] - mPos[1];
	m[6] = mTarget[1] - mPos[1];
	m[7] = 0;
	m[8] = mSide[2] - mPos[2];
	m[9] = mUp[2] - mPos[2];
	m[10] = mTarget[2] - mPos[2];
	m[11] = 0;
	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;

	matrix.setMatrix(m);
	matrix.multiply4d(v, result);

	for (i = 0; i < 3; ++i)
	{
		mSide[i] += result[i];
		mUp[i] += result[i];
		mTarget[i] += result[i];
		mPos[i] += result[i];
	}

	mPos[0] = x;
	mPos[1] = y;
	mPos[2] = z;	
}


void Camera::reset()
{
	mTheta = 0.0;
	mTheta2 = 0.0;

	mPos[0] = 0.0;
	mPos[1] = 0.0;
	mPos[2] = 0.0;

	mTarget[0] = 0.0;
	mTarget[1] = 0.0;
	mTarget[2] = mViewDistance;

	mSide[0] = 1.0;
	mSide[1] = 0.0;
	mSide[2] = 0.0;

	mUp[0] = 0.0;
	mUp[1] = 1.0;
	mUp[2] = 0.0;

	mQ.setIdentity();
	translate(0.0, 0.0, 0.0);
}


void Camera::setSensitivityY(float angle)
{
	mRotateDelta2 = helDegToRad(angle);
}


void Camera::setSensitivityX(float angle)
{
	mRotateDelta = helDegToRad(angle);
}

////////
void Camera::command(enum camera_command cmd)
{
	switch (cmd)
	{
	case CAMERA_MOVE_FORWARD:
		if (mFlags & Camera_FlyMode)
		{
			mPos[2] += (mTranslateDelta * cos(mTheta));
		}
		
	  mPos[0] += (mTranslateDelta * sin(mTheta));
	  mPos[1] += (mTranslateDelta * sin(mTheta2));
	  break;
  case CAMERA_MOVE_BACKWARD:
	  if (mFlags & Camera_FlyMode)
	  {
		  mPos[2] -= (mTranslateDelta * cos(mTheta));
	  }

	  mPos[0] -= (mTranslateDelta * sin(mTheta));
	  mPos[1] -= (mTranslateDelta * sin(mTheta2));
	  break;
  case CAMERA_MOVE_LEFT:
#ifdef OBSOLETE
	  //mPos[0] -= (mTranslateDelta * sin(mTheta2+MTK_PI_OVER_4));
	  mPos[1] -= (mTranslateDelta * cos(mTheta));

	  if (mFlags & Camera_FlyMode)
	  {
		  mPos[2] -= (mTranslateDelta * cos(mTheta2+MTK_PI_OVER_4));
	  }
#else
	  mPos[0] -= (mTranslateDelta * sin(mTheta - 90.0f));
	  mPos[2] -= (mTranslateDelta * cos(mTheta - 90.0f));
#endif
	  break;
  case CAMERA_MOVE_RIGHT:
#ifdef OBSOLETE  // I have no idea when it got out of hand
	  //mPos[0] += (mTranslateDelta * sin(mTheta2+MTK_PI_OVER_4));
	  mPos[1] += (mTranslateDelta * cos(mTheta));

	  if (mFlags & Camera_FlyMode)
	  {
		  mPos[2] += (mTranslateDelta * cos(mTheta2+MTK_PI_OVER_4));
	  }
#else
	  mPos[0] -= (mTranslateDelta * sin(mTheta + 90.0f));
	  mPos[2] -= (mTranslateDelta * cos(mTheta + 90.0f));
#endif
	  break;
 case CAMERA_ROTATE_UP:
	 mTheta2 += mRotateDelta2;
	 rotate(mTheta2, 1.0, 0.0, 0.0);
	 break;
 case CAMERA_ROTATE_DOWN:
	 mTheta2 -= mRotateDelta2;
	 rotate(mTheta2, 1.0, 0.0, 0.0);
	 break;
 case CAMERA_ROTATE_RIGHT:
	 mTheta += mRotateDelta;
	 rotate(mTheta, 0.0, 1.0, 0.0);
	 break;
  case CAMERA_ROTATE_LEFT:
	  mTheta -= mRotateDelta;
	  rotate(mTheta, 0.0, 1.0, 0.0);
	  break;
  case CAMERA_MOVE_UP:
	  mPos[1] -= mTranslateDelta / 2.0;   
	  mTarget[1] -= mTranslateDelta / 2.0;
	  break;
  case CAMERA_MOVE_DOWN:
	  mPos[1] += mTranslateDelta / 2.0;
	  mTarget[1] += mTranslateDelta / 2.0;
	  break;
  case CAMERA_SPEED_UP:
    ++mTranslateDelta;
    break;
  case CAMERA_SPEED_DOWN:
    if (--mTranslateDelta < 0.0)
      mTranslateDelta = 1.0;
  default:
    ;
  }
}


// FIXME: Mostly invalid for QUAT_CAM (can rotate on XYZ)
bool Camera::isBehind(int x, int z)
{
  double bTheta, bCameraX, bCameraZ, Distance;


  // Set up a "virtual camera" a huge distance behind us
  bTheta = mTheta + HEL_PI;

  if (bTheta > HEL_PI)
    bTheta -= HEL_2_PI;

   // 64k is a fair distance away...
  bCameraX = (65536.0 * sin(bTheta)) + mPos[0];
  bCameraZ = (65536.0 * cos(bTheta)) + mPos[2];

  bCameraX -= x;
  bCameraZ -= z;
  Distance = sqrt((bCameraX * bCameraX) + (bCameraZ * bCameraZ));

  return (Distance < 65536.0);
}


void Camera::setSpeed(float s)
{
	mTranslateDelta = s;
}


void Camera::update()
{
   mTarget[2] = (mViewDistance * cos(mTheta)) + mPos[2];
   mTarget[0] = (mViewDistance * sin(mTheta)) + mPos[0];
	mTarget[1] = (mViewDistance * sin(mTheta2)) + mPos[1]; // + height_offset;
}


void Camera::setPosition(vec3_t pos)
{
	mPos[0] = pos[0];
	mPos[1] = pos[1];
	mPos[2] = pos[2];
}


void Camera::setUp(vec3_t up)
{
	mUp[0] = up[0];
	mUp[1] = up[1];
	mUp[2] = up[2];
}


void Camera::setTarget(vec3_t target)
{
	mTarget[0] = target[0];
	mTarget[1] = target[1];
	mTarget[2] = target[2];
}


////////////////////////////////////////////////////////////
// Private Accessors
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Private Mutators
////////////////////////////////////////////////////////////

