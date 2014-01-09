/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 * 
 * Project : hel
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : ViewVolume
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: This is the viewing volume for culling use
 *
 *
 *           This file was generated using Mongoose's C++ 
 *           template generator script.  <stu7440@westga.edu>
 * 
 *-- Test Defines -----------------------------------------------
 *           
 * UNIT_TEST_VIEWVOLUME - Builds ViewVolume class as a console unit test 
 *
 *-- History ------------------------------------------------ 
 *
 * 2002.12.15:
 * Mongoose - Created
 ================================================================*/


#ifndef GUARD__HEL_MONGOOSE_VIEWVOLUME_H_
#define GUARD__HEL_MONGOOSE_VIEWVOLUME_H_

#include <hel/Matrix.h>
#include <hel/BoundingVolume.h>

class ViewVolume
{
 public:

	enum ViewVolumeSide
	{
		rightSide  = 0, // Right
		leftSide   = 1, // Left
		bottomSide = 2, // Bottom
		topSide    = 3, // Top
		farSide    = 4, // Back
		nearSide   = 5  // Front
	};

	enum ViewVolumePlane
	{
		planeA = 0, // X value of normal
		planeB = 1, // Y value of normal
		planeC = 2, // Z value of normal
		planeD = 3  // Distance to origin
	};


	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	ViewVolume();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Constructs an object of ViewVolume
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.15: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~ViewVolume();
	/*------------------------------------------------------
	 * Pre  : ViewVolume object is allocated
	 * Post : Deconstructs an object of ViewVolume
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.15: 
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////

	bool isBoundingVolumeInFrustum(BoundingVolume bvol);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Returns true if frustum contains the given 
	 *        bounding volume
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.15: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	bool isBoundingSphereInFrustum(BoundingSphere bvol);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Returns true if frustum contains the given 
	 *        bounding volume
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.15: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	bool isBoundingBoxInFrustum(BoundingBox bvol);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Returns true if frustum contains the given 
	 *        bounding volume
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.15: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	bool isPointInFrustum(vec_t x, vec_t y, vec_t z);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : 
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.15: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	bool isSphereInFrustum(vec_t x, vec_t y, vec_t z, vec_t radius);
	/*------------------------------------------------------
	 * Pre  : Abstract sphere is valid
	 * Post : Returns true if abstract sphere in view volume
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.15: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	bool isBboxInFrustum(vec3_t min, vec3_t max);
	/*------------------------------------------------------
	 * Pre  : Abstract bounding box is valid
	 * Post : Returns true if abstract bounding box in view volume
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.15: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	vec_t getDistToSphereFromNear(vec_t x, vec_t y, vec_t z, vec_t radius);
	/*------------------------------------------------------
	 * Pre  : Abstract sphere is valid
	 * Post : Returns distance to abstract sphere 
	 *        bounding volume
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.15: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	vec_t getDistToBboxFromNear(vec3_t min, vec3_t max);
	/*------------------------------------------------------
	 * Pre  : Abstract bounding box is valid
	 * Post : Returns distance to abstract box
	 *        bounding volume
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.15: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getFrustum(vec_t frustum[6][4]);
	/*------------------------------------------------------
	 * Pre  : Frustum is allocated 6x4 vec_t
	 * Post : Get a copy of the view volume
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.26:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getPlane(ViewVolumeSide p, vec4_t plane);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Get a copy of a given plane in view volume
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.26:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	
	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	void updateFrame(matrix_t proj, matrix_t mdl);
	/*------------------------------------------------------
	 * Pre  : Given valid projection and model matrices
	 * Post : Updates view volume for this frame
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.15:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void updateFrame();
	/*------------------------------------------------------
	 * Pre  : Called setModel() and setProjection()
	 * Post : Updates view volume for this frame
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.15:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setModel(matrix_t mdl);
	/*------------------------------------------------------
	 * Pre  : It's valid model matrix
	 * Post : Sets this class' model matrix
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.15:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setProjection(matrix_t proj);
	/*------------------------------------------------------
	 * Pre  : It's the valid projection matrix
	 * Post : Sets this class' projection matrix
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.15:
	 * Mongoose - Created
	 ------------------------------------------------------*/

 private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////

	void updateClip();
	/*------------------------------------------------------
	 * Pre  : model and projection matrices are set
	 * Post : Computes clipping matrix
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.15:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void updateFrustum();
	/*------------------------------------------------------
	 * Pre  : model, projection, clip matrices are set
	 * Post : Computes planes of frustum
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.15:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	Matrix mProjection;       	/* Projection matrix */

	Matrix mModel;         	   /* Model matrix */

	Matrix mClip;	            /* Clipping matrix */

	vec_t mFrustum[6][4];      /* View volume */
};

#endif
