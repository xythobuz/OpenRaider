/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : OpenRaider
 * Author  : Mongoose
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : OpenRaider
 * License : No use w/o permission (C) 2001 Mongoose
 * Comments: This is the main class for OpenRaider
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History ------------------------------------------------
 *
 * 2002.08.24:
 * Mongoose - Finally made into Singleton
 *
 * 2001.05.21:
 * Mongoose - Created
 ================================================================*/


#ifndef GUARD__OPENRAIDER_MONGOOSE_OPENRAIDER_H_
#define GUARD__OPENRAIDER_MONGOOSE_OPENRAIDER_H_


#include "mstl/List.h"
#include "mstl/Map.h"
#include "mstl/Vector.h"
#include "TombRaider.h"
#include "Camera.h"
#include "Render.h"
#include "Sound.h"
#include "SDLSystem.h"
#include "Network.h"
#include "World.h"


typedef enum
{
	OpenRaiderKey_console = 1, // Reserved 0 event for console in System
	OpenRaiderKey_attack,
	OpenRaiderKey_forward,
	OpenRaiderKey_left,
	OpenRaiderKey_right,
	OpenRaiderKey_backward,
	OpenRaiderKey_jump,
	OpenRaiderKey_tiltUp,
	OpenRaiderKey_tiltDown,
	OpenRaiderKey_panLeft,
	OpenRaiderKey_panRight,
	OpenRaiderKey_crouch

} OpenRaider_KeyEvent;


typedef enum
{
	OpenRaider_ShowFPS      =   1,
	OpenRaider_DebugMap     =   2,
	OpenRaider_DebugModel   =   4,
	OpenRaider_EnableSound  =   8,
	OpenRaider_DumpTexture  =  16,
	OpenRaider_FullScreen   =  32,
	OpenRaider_Loading      =  64,
	OpenRaider_GameRunning  = 128,

} OpenRaiderFlags;

typedef enum
{
	OpenRaiderSound_Ambient = 1

} OpenRaider_SoundEventType;


class OpenRaider : public SDLSystem
{
 public:

	////////////////////////////////////////////////////////////
	// Constructors ( Singleton )
	////////////////////////////////////////////////////////////

	static OpenRaider *Instance();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Constructs the object of OpenRaider or
	 *        returns pointer to it if previously allocated
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.08.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~OpenRaider();
	/*------------------------------------------------------
	 * Pre  : OpenRaider object is allocated
	 * Post : Deconstructs an object of OpenRaider
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.21:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	void start();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Init the Game
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.21:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void handleMouseMotionEvent(float x, float y);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Mouse motion input used
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.06.04:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void handleBoundKeyPressEvent(unsigned int key);
	/*------------------------------------------------------
	 * Pre  : <Key> is a valid keyboard code
	 *
	 * Post : Recieves <Event> bound to <Cmd> from <Key> press
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void handleBoundKeyReleaseEvent(unsigned int key);
	/*------------------------------------------------------
	 * Pre  : <Key> is a valid keyboard code
	 *
	 * Post : Recieves <Event> bound to <Cmd> from <Key> release
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	 void handleCommand(char *command, unsigned int mode);
	/*------------------------------------------------------
	 * Pre  : <Command> is valid keyword optionally followed
	 *        by ' ' (space) seperated and argument(s)
	 *
	 *        <Mode> is the current type or resource mode
	 *
	 * Post : Executes valid command based on keyword
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.03.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	 void handleConsoleKeyPressEvent(unsigned int key, unsigned int mod);
	/*------------------------------------------------------
	 * Pre  : <Key> is a valid keyboard code
	 *
	 * Post : Recieves <Key> code from text input in console mode
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void handleKeyPressEvent(unsigned int key, unsigned int mod);
	void handleKeyReleaseEvent(unsigned int key, unsigned int mod);
	/*------------------------------------------------------
	 * Pre  : key and mod are valid key command
	 * Post : executes command acc with key, if any
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.27:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void initGL();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void gameFrame();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : One game physics/time frame passes
	 *        Also renders one frame per call
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.21:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void print(bool dump_stdout, const char *format, ...);
	/*------------------------------------------------------
	 * Pre  : Use like printf
	 *
	 * Post : Outputs message in game console
	 *
	 *        If dump_stdout is true, print to stdout also
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/


 protected:

	////////////////////////////////////////////////////////////
	// Constructors ( Singleton )
	////////////////////////////////////////////////////////////

	OpenRaider();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Constructs an object of OpenRaider
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.21:
	 * Mongoose - Created
	 ------------------------------------------------------*/


 private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////

	void consoleCommand(char *cmd);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * ????.??.??:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void soundEvent(int type, int id, vec3_t pos, vec3_t angles);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.16:
	 * Mongoose - Created
	 ------------------------------------------------------*/

//	void entityEvent(entity_t &e, RaiderEvent event);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.16:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void processPakSounds();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.06.04:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void initSound();
	/*------------------------------------------------------
	 * Pre  : Sound system has been init
	 * Post : Loads and positions level sounds and music
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.06.04:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void initTextures();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Generates all textures or mipmaps
	 *        needed for fonts, splash, ext particles
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.28:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void processTextures();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Generates tombraider textures or mipmaps
	 *        needed for sprites, rooms, and models
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.28:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void processSprites();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Generates render sprite sequences
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.06.06:
	 * Mongoose - All new functionality
	 *
	 * 2001.05.28:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void processMoveables();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.06.08:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void processMoveable(int index, int i, int *ent, List <skeletal_model_t *> &cache2, List <unsigned int> &cache, int object_id);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.04.06:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void processModel(int index);
	/*------------------------------------------------------
	 * Pre  : valid model index
	 * Post : Generates render mesh and any textures
	 *        needed for model[index]
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.26:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void processRoom(int index);
	/*------------------------------------------------------
	 * Pre  : valid room index
	 * Post : Generates render mesh and any textures
	 *        needed for room[index]
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.21:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void loadLevel(char *filename);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Loads validated level pak from diskfile
	 *        using maplist
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.21:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	static OpenRaider *mInstance; /* Singleton use */

	TombRaider m_tombraider;      /* Tombraider data support */

	Sound mSound;                 /* 3d Audio support */

	Render m_render;              /* Rendering support */

	Camera m_camera;              /* Client camera support */

	GLString *mText;              /* Hook to textbox like output */

	int mNoClipping;              /* 0 - Clipping, 1 - No Clipping,
											   2 - No Clipping w/o gravity */

	// RC vars //////////////////

	unsigned int mMode[8];			/* Translate System's mode ids to OR's */

	unsigned int m_flags;			/* Set options by flags */

	int m_testSFX;						/* Used for mixed channel sound tests */

	float m_mouseX, m_mouseY;		/* XY axis rotation deltas */

	unsigned int m_texOffset;		/* Offset of TombRaider textures in list */

	unsigned int mLevelTextureOffset;

	// Game vars //////////////

	Vector <char *> mMusicList;  /* List of game level music */

	Vector <char *> mMapList;    /* List of game maps */

	char m_mapName[32];          /* Current map filename */

	char *m_pakDir;              /* Current pak directory */

	char *m_audioDir;            /* Current audio directory */

	char *m_homeDir;             /* Current home directory */

	Vector<entity_t *> mClients; /* Player entity/clients */

	unsigned int mMaxClients;    /* Max number of clients */
};

#endif
