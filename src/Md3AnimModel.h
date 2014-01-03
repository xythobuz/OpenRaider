/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : Freyja
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Md3AnimModel
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: Md3 animation model class
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *
 *-- Test Defines -----------------------------------------------
 *
 * UNIT_TEST_MD3 - Builds Md3 class as a console unit test
 *
 *-- History ----------------------------------------------------
 *
 * 2002.06.19:
 * Mongoose - Created
 ================================================================*/


#ifndef __FREYJA_MONGOOSE_MD3ANIMMODEL_H_
#define __FREYJA_MONGOOSE_MD3ANIMMODEL_H_

#include "Md3.h"


typedef enum
{
	MD3_LOD_HIGH = 0,
	MD3_LOD_MED  = 1,
	MD3_LOD_LOW  = 2,

} md3_lod_t;

typedef enum
{
	MD3_SEX_MALE = 0,
	MD3_SEX_FEMALE  = 1,
	MD3_SEX_NEUTER  = 2,

} md3_sex_t;

typedef enum
{
	UNSUPPORTED = -1,
	BOTH_DEATH1 = 0,		// The first twirling death animation
	BOTH_DEAD1,				// The end of the first twirling death animation
	BOTH_DEATH2,			// The second twirling death animation
	BOTH_DEAD2,				// The end of the second twirling death animation
	BOTH_DEATH3,			// The back flip death animation
	BOTH_DEAD3,				// The end of the back flip death animation
	TORSO_GESTURE,			// The torso's gesturing animation
	TORSO_ATTACK,			// The torso's attack1 animation
	TORSO_ATTACK2,			// The torso's attack2 animation
	TORSO_DROP,				// The torso's weapon drop animation
	TORSO_RAISE,			// The torso's weapon pickup animation
	TORSO_STAND,			// The torso's idle stand animation
	TORSO_STAND2,			// The torso's idle stand2 animation
	LEGS_WALKCR,			// The legs's crouching walk animation
	LEGS_WALK,				// The legs's walk animation
	LEGS_RUN,				// The legs's run animation
	LEGS_BACK,				// The legs's running backwards animation
	LEGS_SWIM,				// The legs's swimming animation
	LEGS_JUMP,				// The legs's jumping animation
	LEGS_LAND,				// The legs's landing animation
	LEGS_JUMPB,				// The legs's jumping back animation
	LEGS_LANDB,				// The legs's landing back animation
	LEGS_IDLE,				// The legs's idle stand animation
	LEGS_IDLECR,			// The legs's idle crouching animation
	LEGS_TURN				// The legs's turn animation
	//	MD3_ANIM_SIZE

} md3_animation_id_t;

typedef struct md3_texture_s
{
	char *name;
	unsigned int name_len;
	int gl_texture_id;

} md3_texture_t;

typedef struct md3_animation_s
{
	md3_animation_id_t id;

	unsigned int first_frame;
	unsigned int end_frame;
	unsigned int num_frames;
	unsigned int looping_frames;
	unsigned int frames_per_second;

} md3_animation_t;


class Md3AnimModel
{
 public:

	enum Md3AnimModelFlags
	{
		fDisableTagInterpolate  = 1,
		fAnimate                = 2,
		fRenderBones            = 4,
		fDrawWeapon             = 8,
		fDisableMeshInterpolate = 16,
		fRenderingWarnings      = 32,
		fUseNormals             = 64
	};


	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	Md3AnimModel();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Md3AnimModel object is deconstructed
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~Md3AnimModel();
	/*------------------------------------------------------
	 * Pre  : Md3AnimModel object is allocated
	 * Post : Md3AnimModel object is deconstructed
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////

	unsigned int getFlags();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Gets option flags
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.29:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	md3_animation_id_t getAnimUpper();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Get animation for upper model
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.22:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	md3_animation_id_t getAnimLower();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Get animation for lower model
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.22:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	void setFlag(Md3AnimModelFlags flag);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Sets option flag
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.29:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void toggleFlag(Md3AnimModelFlags flag);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Clear/Set option flag
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.29:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setAnimUpper(md3_animation_id_t anim);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Animation is set for upper model
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.22:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setAnimLower(md3_animation_id_t anim);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Animation is set for lower model
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.22:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setDebug(unsigned char level);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : level 0 = errors, 1 = warnings, 2 = debug1
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int load(char *modelPath, char *skin, md3_lod_t modelLoD);
	/*------------------------------------------------------
	 * Pre  : modelPath - Path to directory containg data files
	 *
	 *           eg '/usr/local/games/quake3/qbase3/players/slash'
	 *
	 *        skin - name of skin base to use
	 *
	 *           eg 'default' or 'red' or 'blue' etc
	 *
	 *        modelLoD - Level of detail to use,
	 *
	 * Post : Error code is returned, 0 no error
	 *        Md3s are read into memory
	 *
	 *-- History ------------------------------------------
	 *
	 *
	 * 2002.12.29:
	 * Mongoose - Progressively loads lower LoDs if it can't
	 *            find passed LoD
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int loadAnimations(char *modelPath);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Loads animations for composite model
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int loadWeapon(char *modelPath, const char *name);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Loads a weapon model
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void render();
	/*------------------------------------------------------
	 * Pre  : Build with USING_OPENGL
	 * Post : Renders entire composite model
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void renderTag(unsigned int id);
	/*------------------------------------------------------
	 * Pre  : Build with USING_OPENGL
	 * Post : Renders tag and it's slaves
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void renderModel(Md3 &model);
	/*------------------------------------------------------
	 * Pre  : Build with USING_OPENGL
	 * Post : Renders single Md3 model
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	unsigned int texNumTest;        /* Number of texTests */

	md3_texture_t texTest[32];      /* Texture loader/id trans test */


private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////

	void setCurrentTime(Md3 &model);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : time is set for model animation
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void updateModel(Md3 &model);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Sets up md3 for next render pass
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.23:
	 * Mongoose - Created, obsoletes old anim arrays
	 ------------------------------------------------------*/

	void initMd3(Md3 &model, unsigned int id);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Sets up md3 init animation values
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.23:
	 * Mongoose - Created, obsoletes old anim arrays
	 ------------------------------------------------------*/

	int addAnim(char *modelPath, md3_animation_id_t id,
					unsigned int firstFrame, unsigned int numFrames,
					unsigned int loopingFrames, unsigned int framesPerSecond);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Sets up animation array
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.22:
	 * Mongoose - Created, forked off load()
	 ------------------------------------------------------*/

	int loadMd3(Md3 &model, const char *base, char *modelPath, char *skin,
					md3_lod_t modelLoD);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Loads model and if given skin
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.22:
	 * Mongoose - Created, forked off load()
	 ------------------------------------------------------*/

	int loadSkin(Md3 &model, char *filename);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Loads skin for model
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int loadShader(Md3 &model, char *filename);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Loads shader for model
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void connectModels(char *tagName, Md3 &modelA, Md3 &modelB);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Connects models as A slave to B master
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int cacheTexture(char *texture);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Sets up texture filename caching
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned int mFlags;       /* Model options */

	Md3 m_head;                /* Head model */

	Md3 m_upper;               /* Upper body model */

	Md3 m_lower;               /* Lower body model */

	Md3 m_weapon;              /* Weapon model */

	unsigned int m_animCount;  /* Number of animations loaded */

	unsigned char m_debug;     /* Set level of debug info to stdout */

	md3_sex_t m_sex;           /* Sex of model */

	md3_animation_t m_anim[32];        /* This models animation list */

	md3_animation_id_t m_upperAnim;    /* Current upper model animation */

	md3_animation_id_t m_lowerAnim;    /* Current lower model animation */
};

#endif
