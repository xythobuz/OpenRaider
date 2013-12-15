/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 * 
 * Project : Hel
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
 *-- History ------------------------------------------------ 
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
 * Mongoose - Created, based on my old GL camera code
 *            that has been used in GooseEgg since alpha
 *            and algorithms from Yuri Zhivago's trview
 ================================================================*/


#ifndef __HEL_MONGOOSE_CAMERA_H_
#define __HEL_MONGOOSE_CAMERA_H_

#include <hel/math.h>
#include <hel/Matrix.h>
#include <hel/Quaternion.h>

enum camera_command                /* Interactive camera control */
{
  CAMERA_MOVE_FORWARD = 1,
  CAMERA_MOVE_BACKWARD,
  CAMERA_MOVE_UP,
  CAMERA_MOVE_DOWN,
  CAMERA_ROTATE_RIGHT,
  CAMERA_ROTATE_LEFT,
  CAMERA_SPEED_UP,
  CAMERA_SPEED_DOWN,
  CAMERA_ROTATE_UP,
  CAMERA_ROTATE_DOWN,
  CAMERA_MOVE_LEFT,
  CAMERA_MOVE_RIGHT
};

enum CameraFlags
{
	Camera_FlyMode = 1
};


class Camera
{
 public:

	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	Camera();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Constructs an object of Camera
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.18: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~Camera();
	/*------------------------------------------------------
	 * Pre  : Camera object is allocated
	 * Post : Deconstructs an object of Camera
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.18: 
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////

	unsigned int getId();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Returns this camera's id
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.18: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getPosition(vec3_t pos);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Returns current position
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.16:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getUp(vec3_t up);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Returns up vector
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.18: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getTarget(vec3_t target);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Returns target ( look at pos )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.18: 
	 * Mongoose - Created
	 ------------------------------------------------------*/
	
	float getYaw();
	/*------------------------------------------------------
	 * Pre  : Get current yaw in degrees
	 * Post : 
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.22:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	double getRadianYaw();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Returns theta angle/yaw of camera
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.26: 
	 * Mongoose - Created
	 ------------------------------------------------------*/
	
	float getPitch();
	/*------------------------------------------------------
	 * Pre  : Get current pitch in degrees
	 * Post : 
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.22:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	double getRadianPitch();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Returns phi angle/pitch of camera
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.26: 
	 * Mongoose - Created
	 ------------------------------------------------------*/
	
	bool isBehind(int x, int z);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Returns true if (x, z) is behind camera eye
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.26: 
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	void rotate(float angle, float x, float y, float z);
	/*------------------------------------------------------
	 * Pre  : x,y,z axis; angle in radians
	 * Post : Rotates camera
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.06.04: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void translate(float x, float y, float z);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Camera position is set to x,y,z
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.18: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void reset();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Camera is set to inital state
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.18: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setSensitivityX(float angle);
	/*------------------------------------------------------
	 * Pre  : angle is theta's rotation delta in degrees
	 * Post : Sets rotation delta
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.06.04: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setSensitivityY(float angle);
	/*------------------------------------------------------
	 * Pre  : angle is theta's rotation delta in degrees
	 * Post : Sets rotation delta
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.06.04: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void command(enum camera_command cmd);
	/*------------------------------------------------------
	 * Pre  : Command is valid camera command
	 * Post : Sends interactive command to camera
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.18: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

   void setSpeed(float s);
   /*------------------------------------------------------
    * Pre  : s is 256 or greater in general
    * Post : Sets 'speed'
    *
    *-- History ------------------------------------------
    *
    * 2002.01.02: 
    * Mongoose - Created
    ------------------------------------------------------*/

	void update();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Updates view target
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.18: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setPosition(vec3_t pos);
	/*------------------------------------------------------
	 * Pre  : Set current position
	 * Post : 
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.16:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setUp(vec3_t up);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Sets up vector
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.18: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setTarget(vec3_t target);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Sets target ( look at pos )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.18: 
	 * Mongoose - Created
	 ------------------------------------------------------*/


 private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////

	unsigned int mId;           /* Unquie id */

	Quaternion mQ;              /* Quaternion for rotation */

	unsigned int mFlags;        /* For testing with flags  */
		
	double mPos[4];             /* Location in 3 space (aka eye) */
	
	double mTarget[4];          /* Postition we're looking at  */
	
	double mUp[4];              /* Up vector  */
	
	double mSide[4];            /* Side vector  */
	
	double mViewDistance;      /* Distance from target */
	
	double mTranslateDelta;    /* Step size to move */
	
	double mRotateDelta;       /* Radians to rotate  Y */
	
	double mTheta;              /* View angle  Y */

	double mRotateDelta2;      /* Radians to rotate  Z */
	
	double mTheta2;             /* View angle  Z */
	
	bool mUpdate;               /* Check to see if view needs updating */

	static unsigned int mCounter;   /* Id system use */
};

#endif
