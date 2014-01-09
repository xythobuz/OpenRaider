/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : Freyja
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Light
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: This is the GL light class
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History ------------------------------------------------
 *
 * 2002.01.27:
 * Mongoose - Created
 ================================================================*/


#ifndef __FREYJA_MONGOOSE_LIGHT_H_
#define __FREYJA_MONGOOSE_LIGHT_H_

#include "hel/math.h"


class Light
{
 public:

	typedef enum
   {
		typePoint = 1,
		typeSpot = 2,
		typeDirectional = 3
	} FreyjaLightType;


	Light();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Constructs an object of Light
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.01.27:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~Light();
	/*------------------------------------------------------
	 * Pre  : Light object is allocated
	 * Post : Deconstructs an object of Light
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.01.27:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	float mAmbient[4];          /* Ambient color */

	float mDiffuse[4];          /* Diffuse color */

	float mSpecular[4];         /* Specular color */

	vec4_t mPos;                /* Light position in 3 space */

	vec3_t mDir;                /* Light direction */

	float mAtt;                 /* */

	vec4_t mColor;              /* Color of light */

	vec_t mCutoff;              /* Fade out distance */

	FreyjaLightType mType;	    /* Type of light */

 private:

	//unsigned int mFlags;        /* Flags for this light */

	//unsigned int mId;           /* Unique identifier of this light */

	//char *mName;                /* Light name */
};

#endif
