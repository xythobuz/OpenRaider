/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : Render
 * Author  : Mongoose
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Render
 * License : No use w/o permission (C) 2001 Mongoose
 * Comments: This is the renderer class for OpenRaider
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History ------------------------------------------------
 *
 * 2001.05.21:
 * Mongoose - Created
 ================================================================*/


#ifndef GUARD__RENDER_MONGOOSE_RENDER_H_
#define GUARD__RENDER_MONGOOSE_RENDER_H_

#include "mstl/List.h"
#include "mstl/Vector.h"
#include "hel/Matrix.h"
#include "hel/ViewVolume.h"
#include "Light.h"
#include "World.h"
#include "SkeletalModel.h"
#include "OpenGLMesh.h"
#include "Texture.h"
#include "Camera.h"
#include "GLString.h"

#ifdef USING_EMITTER
#   include "Emitter.h"
#endif


class RenderRoom
{
public:
	RenderRoom()
	{
		room = 0x0;
	}

	~RenderRoom()
	{
		// FIXME: hangs when erasing - might be shared pointers somewhere
		//lights.erase();
	}

	void computeCenter()
	{
		if (room)
		{
			helMidpoint3v(room->bbox_min, room->bbox_max, center);
		}
	}

	vec_t dist;              /* Distance to near plane, move to room?  */

	vec3_t center;           /* Center of bbox, move to room? */

	room_mesh_t *room;       /* Physical room stored in World,
										 Very dangerous as allocated and used */

	Vector<Light *> lights;  /* List of lights in this room */

	OpenGLMesh mesh;         /* OpenGL mesh that represents this room */
};


class Render
{
 public:

	typedef enum
   {
		modeDisabled    = 0,
		modeLoadScreen  = 1,
		modeVertexLight = 2,
		modeSolid       = 3,
		modeWireframe   = 4,
		modeTexture     = 5
	} RenderMode;

	typedef enum
   {
		fParticles		= 1,
		fPortals			= 2,
		fRoomAlpha		= 4,
		fViewModel    = 8,
		fSprites      = 16,
		fRoomModels   = 32,
		fEntityModels = 64,
		fFog          = 128,
		fUsePortals   = 256,
		fFastCard     = 1024,
		fGL_Lights    = 2048,
		fOneRoom      = 4096,
		fRenderPonytail = 8192,
		fMultiTexture = 16384,
		fUpdateRoomListPerFrame = 32768,
		fAnimateAllModels = 65536,
		fAllRooms      =  131072
	} RenderFlags;

	typedef enum
	{
		roomMesh = 1,
		skeletalMesh = 2

	} RenderMeshType;

	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	Render();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Constructs an object of Render
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.21:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~Render();
	/*------------------------------------------------------
	 * Pre  : Render object is allocated
	 * Post : Deconstructs an object of Render
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.21:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////

	void screenShot(char *filenameBase);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Makes a screenshot, writes to disk as file
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created, factored out of OpenRaider class
	 ------------------------------------------------------*/

	int getMode();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Gets current rendering mode
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.21:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	void addRoom(RenderRoom *rRoom);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.21:
	 * Mongoose - Created, factored out of World
	 ------------------------------------------------------*/

	void Init(int width, int height, bool fastCard);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Starts and sets up OpenGL target
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.21:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void loadTexture(unsigned char *image,
						  unsigned int width, unsigned int height,
						  unsigned int id);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Loads textures in a certian id slot
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created, factored out of OpenRaider class
	 ------------------------------------------------------*/

	void initTextures(char *textureDir, unsigned int *numLoaded,
							unsigned int *nextId);
	/*------------------------------------------------------
	 * Pre  : textureDir is valid and exists with textures
	 * Post : Sets up textures for OpenRaider
	 *        Returns number of loaded textures and
	 *
	 *        numLoaded will update number of
	 *        external textures loaded
	 *
	 *        nextId will update next level texture id
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created, factored out of OpenRaider class
	 ------------------------------------------------------*/

	void initEmitter(const char *name, unsigned int size,
						  unsigned int snowTex1, unsigned int snowTex2);
	/*------------------------------------------------------
	 * Pre  : Textures are init and these args are valid
	 * Post : Emitter is set up for rendering with 2 textures
	 *        in a overhead rain or snow like pattern
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void ClearWorld();
  /*------------------------------------------------------
   * Pre  :
   * Post : Removes current world/entity/etc geometery
   *
   *-- History ------------------------------------------
   *
   * 2001.05.21:
   * Mongoose - Created
   ------------------------------------------------------*/

	void toggleFlag(unsigned int flag);
	void clearFlags(unsigned int flags);
	void setFlags(unsigned int flags);
  /*------------------------------------------------------
   * Pre  : (Un)Sets bitflags for various control use
   * Post : Changes state of renderer in some way
   *
   *-- History ------------------------------------------
   *
   * 2002.03.21:
   * Mongoose - Created
   ------------------------------------------------------*/

	void setMode(int n);
  /*------------------------------------------------------
   * Pre  :
   * Post :
   *
   *-- History ------------------------------------------
   *
   * 2001.05.21:
   * Mongoose - Created
   ------------------------------------------------------*/

	void Update(int width, int height);
  /*------------------------------------------------------
   * Pre  :
   * Post :
   *
   *-- History ------------------------------------------
   *
   * 2001.05.21:
   * Mongoose - Created
   ------------------------------------------------------*/

	void Display();
  /*------------------------------------------------------
   * Pre  :
   * Post : Renders a single game frame
   *
   *-- History ------------------------------------------
   *
   * 2001.05.21:
   * Mongoose - Created
   ------------------------------------------------------*/

	void setSkyMesh(int index, bool rot);
  /*------------------------------------------------------
   * Pre  :
   * Post :
   *
   *-- History ------------------------------------------
   *
   * 2001.05.21:
   * Mongoose - Created
   ------------------------------------------------------*/

	void ViewModel(entity_t *ent, int index);
  /*------------------------------------------------------
   * Pre  :
   * Post :
   *
   *-- History ------------------------------------------
   *
   * 2001.05.21:
   * Mongoose - Created
   ------------------------------------------------------*/

	void RegisterCamera(Camera *camera);
  /*------------------------------------------------------
   * Pre  :
   * Post :
   *
   *-- History ------------------------------------------
   *
   * 2001.05.21:
   * Mongoose - Created
   ------------------------------------------------------*/

   GLString *GetString();
   /*------------------------------------------------------
    * Pre  :
    * Post : Returns GL text output agent
    *
    *-- History ------------------------------------------
    *
    * 2002.01.04:
    * Mongoose - Created
    ------------------------------------------------------*/

	// FIXME: Should be private
   void drawLoadScreen();
   /*------------------------------------------------------
    * Pre  : Texture is init
    * Post : Renders load screen
    *
    *-- History ------------------------------------------
    *
    * 2002.01.01:
    * Mongoose - Created
    ------------------------------------------------------*/

	void addSkeletalModel(SkeletalModel *mdl);
   /*------------------------------------------------------
    * Pre  :
    * Post :
    *
    *-- History ------------------------------------------
    *
    * 2002.01.01:
    * Mongoose - Created
    ------------------------------------------------------*/


private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////

	bool isVisible(float bboxMin[3], float bboxMax[3]);
	/*------------------------------------------------------
	 * Pre  : Abstract bounding box must be valid
	 * Post : If object's bounding box is in viewing volume
	 *        return true, else false
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.16:
	 * Mongoose - Moved to Render class
	 *
	 * 2001.06.06:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	bool isVisible(float x, float y, float z);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Is point in view volume?
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.16:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	bool isVisible(float x, float y, float z, float radius);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Is sphere in view volume?
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.16:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////

	void newRoomRenderList(int index);
   /*------------------------------------------------------
    * Pre  : room index is valid
    * Post : Build a visible room list starting at index
    *
    *-- History ------------------------------------------
    *
    * 2002.01.01:
    * Mongoose - Created
    ------------------------------------------------------*/

	void buildRoomRenderList(RenderRoom *room);
   /*------------------------------------------------------
    * Pre  : room is valid
    * Post : Build a visible room list starting from room
	 *        and only consider it's linked rooms and their
	 *        linked rooms
    *
    *-- History ------------------------------------------
    *
    * 2002.01.01:
    * Mongoose - Created
    ------------------------------------------------------*/

	void drawObjects();
   /*------------------------------------------------------
    * Pre  : Texture is init
    * Post : Renders visible world objects
    *
    *-- History ------------------------------------------
    *
    * 2002.01.01:
    * Mongoose - Created
    ------------------------------------------------------*/

	void drawSkyMesh(float scale);
   /*------------------------------------------------------
    * Pre  : Texture is init
	 *        scale is correct for map size
    * Post : Renders Sky domes/boxes/etc by scaling factor
    *
    *-- History ------------------------------------------
    *
    * 2002.01.01:
    * Mongoose - Created
    ------------------------------------------------------*/

	void drawModel(SkeletalModel *model);
   /*------------------------------------------------------
    * Pre  : Texture is init
    * Post : Renders a skeletal model
    *
    *-- History ------------------------------------------
    *
    * 2002.01.01:
    * Mongoose - Created
    ------------------------------------------------------*/

	void drawRoom(RenderRoom *rRoom, bool draw_alpha);
   /*------------------------------------------------------
    * Pre  : Texture is init
	 *        Draw all rooms with alpha false, then
	 *        draw with alpha true
	 *
    * Post : Renders a room in 2 seperate passes to
	 *        handle alpha, currently doesn't sort alpha
	 *        but looks pretty good
    *
    *-- History ------------------------------------------
    *
    * 2002.01.01:
    * Mongoose - Created
    ------------------------------------------------------*/

	void drawRoomModel(static_model_t *mesh);
   /*------------------------------------------------------
    * Pre  : Texture is init
    * Post : Renders static room model
    *
    *-- History ------------------------------------------
    *
    * 2002.01.01:
    * Mongoose - Created
    ------------------------------------------------------*/

	void drawModelMesh(model_mesh_t *r_mesh, RenderMeshType type);
   /*------------------------------------------------------
    * Pre  : Texture is init, type is object containing mesh
    * Post : Renders a mesh
    *
    *-- History ------------------------------------------
    *
    * 2002.01.01:
    * Mongoose - Created
    ------------------------------------------------------*/

	void drawSprite(sprite_t *sprite);
   /*------------------------------------------------------
    * Pre  : Texture is init
    * Post : Renders a sprite
    *
    *-- History ------------------------------------------
    *
    * 2002.01.01:
    * Mongoose - Created
    ------------------------------------------------------*/

	void updateViewVolume();
	/*------------------------------------------------------
	 * Pre  : Call once per render frame
	 * Post : Updated view volume
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.16:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void tmpRenderModelMesh(model_mesh_t *r_mesh, texture_tri_t *ttri);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Let them eat cake...
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/



#ifdef USING_EMITTER
	Emitter *mEmitter;      /* Particle emitter test */
#endif

	Texture mTexture;       /* Texture subsystem */

	Camera *mCamera;        /* Camera subsystem */

	GLString mString;       /* GL Text subsystem */

	Vector<RenderRoom *> mRoomRenderList;

	Vector<RenderRoom *> mRooms;

	Vector<SkeletalModel *> mModels;

	unsigned int mFlags;    /* Rendering flags */

	unsigned int mWidth;    /* Viewport width */

	unsigned int mHeight;   /* Viewport height */

	unsigned int mMode;     /* Rendering mode */

	unsigned int *mNumTexturesLoaded;

	unsigned int *mNextTextureId;

	// float mSplash;

	int mLock;

	int mSkyMesh;           /* Skymesh model id */

	bool mSkyMeshRotation;  /* Should Skymesh be rotated? */
};

#endif
