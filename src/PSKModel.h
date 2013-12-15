/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 * 
 * Project : Freyja
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : PSKModel
 * License : No use w/o permission (C) 2003 Mongoose
 * Comments: Unreal Tournament skeletal model
 *
 *
 *           This file was generated using Mongoose's C++ 
 *           template generator script.  <stu7440@westga.edu>
 * 
 *-- Test Defines -----------------------------------------------
 *           
 * UNIT_TEST_PSKMODEL - Builds PSKModel class as a console unit test 
 *
 *-- History ------------------------------------------------ 
 *
 * 2003.07.12:
 * Mongoose - UT Package prototype code moved into 'UTPackage'
 *            API changes to structs to clearify meaning
 *
 * 2003.06.12:
 * Mongoose - PSA keyframes fixed, special thanks to:
 *            Steven Fuller and Forest Hale for help picking
 *            apart those tough little 8 bytes   =)
 *
 * 2003.06.11:
 * Mongoose - Broke up rendering and model loading into seperate
 *            classes ( Closely coupled )
 *
 *            PSA experimental loader
 *
 * 2003.06.10:
 * Mongoose - Skeletal format fixed, should load perfectly
 *
 * 2003.01.20:
 * Mongoose - Fixed triangle rendering thanks to debugging by
 *            Steven Fuller, who found the tris -> UV -> vert 
 *            connection
 *
 *            Finished up basic rendering, lots of 
 *            small fixes/features
 *
 * 2003.01.06:
 * Mongoose - Created
 ================================================================*/


#ifndef GUARD__FREYJA_MONGOOSE_PSKMODEL_H_
#define GUARD__FREYJA_MONGOOSE_PSKMODEL_H_


typedef struct /* 13 bytes */
{
	unsigned short x, y, z;
	unsigned char material;
	unsigned int flags;
	unsigned short unknown;

} psk_face_t;


typedef struct /* 16 bytes */
{
	unsigned short vertex;
	unsigned short unknown1;
	float uv[2];
	unsigned int unknown2;

} psk_vtxw_t;


typedef struct  /* 88 bytes */
{
	char name[64];

	unsigned int unknown1;
	unsigned int unknown2;
	unsigned int unknown3;
	unsigned int unknown4;
	unsigned int unknown5;
	unsigned int unknown6;

} psk_material_t;

typedef struct /* 120 bytes */
{
	char name[64];
	unsigned int flags;         /* Option flags? */
	unsigned int numChildren;
	unsigned int parentIndex;
	float restDir[4];           /* Quaternion x,y,z,w */
	float restLoc[3];
	int unknown[4];             /* Scale? */

} psk_bone_t;


typedef struct  /* 12 bytes */
{
	float weight;
	unsigned int vertexIndex;
	unsigned int boneIndex;

} psk_weight_t;


typedef struct /* 168 bytes */
{
	char name[64];
	char name2[64];
	unsigned int numBones;
	unsigned int rootId; // ?
	unsigned int key[2];
	float keyf;
	float time[2];
	float unknown[3];

} psa_anim_info_t;


typedef struct /* 32 bytes */
{
	float trans[3];	// xyz
	float dir[4];		// xyzw
	float scale;		// ?

} psa_key_frame_t;


class PSKModel
{
 public:

	enum PSKModelFlag
	{
		fDebugWeightLoad		= 1,
		fDebugPointLoad		= 2,
		fDebugFaceLoad			= 4,
		fDebugUVLoad			= 8,
		fDebugMattLoad			= 16,
		fDebugBoneLoad			= 32
	};


	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	PSKModel();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Constructs an object of PSKModel
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.01.06: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~PSKModel();
	/*------------------------------------------------------
	 * Pre  : PSKModel object is allocated
	 * Post : Deconstructs an object of PSKModel
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.01.06: 
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////

	void printSkeletion(bool printNames);
	/*------------------------------------------------------
	 * Pre  : <PrintNames> if true use names instead of Ids
	 *
	 * Post : Prints skeletion bone trace one bone per line
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.10:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	int load(char *filename);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Loads PSK model from disk
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.01.06:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned int mFlags;

	unsigned int mNumFrames;

	unsigned int mNumVertices;

	unsigned int mNumFaces;

	unsigned int mNumVTXWs;

	unsigned int mNumMaterials;

	unsigned int mNumBones;

	unsigned int mNumWeights;

	float *mVertices;							/* Vertices of the model */

	psk_vtxw_t *mVTXWs;						/* UV Wedges */

	psk_face_t *mFaces;						/* The triangle mesh */

	psk_material_t *mMaterials;			/* Materials/textures */

	psk_bone_t *mBones;						/* The bones of the model */

	psk_weight_t *mWeights;					/* The weights of the bones */


 private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////
};


//////////////////////////////////////////////////////////////////////
// PSAAnimation Class
//////////////////////////////////////////////////////////////////////

class PSAAnimation
{
public:

	enum PSKModelRendererFlag
	{
		fReserved1			= 1,
		fReserved2			= 2,
		fDebugBones			= 4,
		fDebugAnimInfos	= 8,
		fDebugKeyFrames	= 16
	};

	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	PSAAnimation();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Constructs an object of PSAAnimation
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.11: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~PSAAnimation();
	/*------------------------------------------------------
	 * Pre  : PSAAnimation object is allocated
	 * Post : Deconstructs an object of PSAAnimation
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.11: 
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////

	int load(char *filename);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Loads PSA animation from disk
	 *
	 *        Returns < 0 on error
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.01.06:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	unsigned int mFlags;

	unsigned int mNumFrames;

	unsigned int mNumBones;

	unsigned int mNumAnimInfos;

	unsigned int mNumKeyFrames;

	psk_bone_t *mBones;

	psa_anim_info_t *mAnimInfos;

	psa_key_frame_t *mKeyFrames;


private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////

};


//////////////////////////////////////////////////////////////////////
// PSKModelRenderer Class
//////////////////////////////////////////////////////////////////////

class PSKModelRenderer
{
 public:

	enum PSKModelRendererFlag
	{
		fRenderFaces		= 1,
		fRenderTexture		= 2,
		fRenderPoints		= 4,
		fRenderBones 		= 8,
		fConvertEuler 		= 16,
		fDebugFaceRender 	= 32
	};


	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	PSKModelRenderer();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Constructs an object of PSKModelRenderer
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.01.06: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~PSKModelRenderer();
	/*------------------------------------------------------
	 * Pre  : PSKModelRenderer object is allocated
	 * Post : Deconstructs an object of PSKModelRenderer
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.01.06: 
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////

	void render();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Renders PSK model in OpenGL
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.01.06:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void renderBone(unsigned int id);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Renders PSK model's bone in OpenGL, 
	 *        ( Recursive calling )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.01.06:
	 * Mongoose - Created, from nonpublic render()
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	void convertBoneAngles();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Converts bone angles for rendering use
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.10:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void convertBoneAnglesPSA(unsigned int frame);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Converts PSA bone angles for rendering use
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.10:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void copyVertices();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Resets vertices to match PSKModel's default
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.10:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setAnimation(PSAAnimation *anim);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Sets PSA animation to render, and sets up cache
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.11:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setModel(PSKModel *model);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Sets PSK model to render, and sets up cache
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.10:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setupRestMatrices(unsigned int id);
	/*------------------------------------------------------
	 * Pre  : <Id> the bone to setup matching invert matrix 
	 *
	 * Post : Generates inverted matrices to render deformed
	 *        mesh by recursive call from root bone
	 *
	 *        OpenGL accelerated where avalibable
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.07.07:
	 * Mongoose - Created, was boneTransforms
	 ------------------------------------------------------*/

	void setupWorldMatrices(unsigned int id);
	/*------------------------------------------------------
	 * Pre  : <Id> the bone to setup matching transform matrix 
	 *
	 * Post : Generates transform matrices to render deformed
	 *        mesh by recursive call from root bone
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.10:
	 * Mongoose - Created, was boneTransforms
	 ------------------------------------------------------*/

	void transformVertices();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Transforms vertices to deformed mesh for 
	 *        current bone frame
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.10:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned int mFlags;

	int mTextures[8];							/* Texture Ids of loaded materials */

	unsigned int mAnimationFrame;			/* Used to keep up with animation 
														frame externally, 
													
														And updated	internally

														Should be moved to 
														private+acessor method
													*/

 private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////

	float *mBoneRotationCache;				/* Cache of the computed axis angles */

	float *mVertexTransformCache;			/* Cache of vertex transforms */

	float **mInvertedMatrices;          /* Cache of computed inverted 
														bone transform matrices for the
														vertices at rest position */

	float **mWorldMatrices;

	unsigned int mNumMatrices;


	/* Simulate inheritence here, cheesy waste */

	unsigned int mNumFrames;

	unsigned int mNumVertices;

	unsigned int mNumFaces;

	unsigned int mNumVTXWs;

	unsigned int mNumMaterials;

	unsigned int mNumBones;

	unsigned int mNumWeights;

	PSKModel *mModel;							/* Current PSK model to render */

	PSAAnimation *mAnimation;				/* Current PSA anim to render */

	psk_vtxw_t *mVTXWs;						/* UV Wedges */

	psk_face_t *mFaces;						/* The triangle mesh */

	psk_material_t *mMaterials;			/* Materials/textures */

	psk_bone_t *mBones;						/* The bones of the model */

	psk_weight_t *mWeights;					/* The weights of the bones */
};

#endif
