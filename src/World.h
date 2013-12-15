/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 * 
 * Project : OpenRaider
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : World
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: The game world ( model )
 *
 *
 *           This file was generated using Mongoose's C++ 
 *           template generator script.  <stu7440@westga.edu>
 * 
 *-- Test Defines -----------------------------------------------
 *           
 * UNIT_TEST_WORLD - Builds World class as a console unit test 
 *
 *-- History ------------------------------------------------ 
 *
 * 2002.12.16:
 * Mongoose - Created
 ================================================================*/


#ifndef GUARD__OPENRAIDER_MONGOOSE_WORLD_H_
#define GUARD__OPENRAIDER_MONGOOSE_WORLD_H_

#define BAD_BLOOD  // For temp rendering use

#ifdef BAD_BLOOD
#   include "SkeletalModel.h"
#endif

#include <mstl/List.h>
#include <mstl/Vector.h>
#include <hel/math.h>
#include <hel/Mass.h>


// Mirrors TombRaider class' room flags really
typedef enum
{
	roomFlag_underWater    = 0x0001

} room_flags_t;

typedef enum
{
	worldMoveType_walkNoSwim   = -1,
	worldMoveType_walk         = 0,
	worldMoveType_noClipping   = 1,
	worldMoveType_fly          = 2,
	worldMoveType_swim         = 3

} worldMoveType;


typedef struct vertex_s
{
	vec3_t pos;

} vertex_t;


typedef struct uv_s
{
	vec2_t uv;

} uv_t;

typedef struct texel_s
{
	vec2_t st;

} texel_t;


typedef struct color_s
{
	vec4_t rgba;

} color_t;


typedef struct sprite_s
{
	int num_verts; // 4 == Quad, 3 == Triangle, renderered as triangles 
	vertex_t vertex[4];
	texel_t texel[4];
	float pos[3];
	float radius; // yeah, I know
	int texture;	

} sprite_t;


typedef struct sprite_seq_s
{
	int num_sprites;
	sprite_t *sprite;

} sprite_seq_t;


// FIXME: For now shaders are textures on tex objects
//        and materials on color objects.  If -1
//        then it doesn't have that information yet.

typedef struct texture_tri_s
{
	int index[3];
	vec_t st[6];
	int texture;
	unsigned short transparency;

} texture_tri_t;


typedef struct model_mesh_s
{
	Vector<texture_tri_t *> texturedTriangles;
	Vector<texture_tri_t *> coloredTriangles;
	Vector<texture_tri_t *> texturedRectangles;
	Vector<texture_tri_t *> coloredRectangles;

	vec3_t center;
	float radius;

	unsigned int vertexCount;
	vec_t *vertices;

	unsigned int colorCount;
	vec_t *colors;

	unsigned int normalCount;
	vec_t *normals;

} model_mesh_t;


////////////////////////////////////////////////////////////////


typedef struct entity_s
{
	int id;                    // Unique identifier
	float pos[3];              // World position
	float angles[3];           // Eular angles (pitch, yaw, roll)
	int type;                  // {(0x00, item), (0x01, ai), (0x02, player)}
	int room;                  // Current room entity is in
	worldMoveType moveType;    // Type of motion/clipping
	bool moving;               // In motion?
	struct entity_s *master;   // Part of entity chain?

	int state;      // State of the Player, AI, or object
	int objectId;              // What kind of entity?

	int modelId;               // Animation model
	void *tmpHook;
	bool animate;

	Mass mass;
	/*
	  float time, lastTime;
	  int state, lastState;
	  int event, lastEvent;
	  int goal;
	 */

} entity_t;


typedef struct static_model_s
{
	int index;     // model_mesh index
	float yaw;     // angle of rotation on Y
	float pos[3];  // position
	
	//vec3_t bboxMax;
	//vec3_t bboxMin;

} static_model_t;



typedef struct portal_s
{
	float vertices[4][3];
	float normal[3];
	int adjoining_room;

} portal_t;


typedef struct box_s
{
	vertex_t a;
	vertex_t b;
	vertex_t c;
	vertex_t d;

} box_t;

typedef struct sector_s
{
	vec_t floor;
	vec_t ceiling;

	bool wall;

} sector_t;

// FIXME: No room mesh list or sprites and etc
typedef struct room_mesh_s
{
	Vector<int> adjacentRooms;
	Vector<portal_t *> portals;
	Vector<static_model_t *> models;
	Vector<sprite_t *> sprites;
	Vector<box_t *> boxes;
	Vector<sector_t *> sectors;

	int id;
	unsigned int flags;
	unsigned int numXSectors;
	unsigned int numZSectors;
	float pos[3];
	vec3_t bbox_min;
	vec3_t bbox_max;

} room_mesh_t;


// Workout generic entity and a client class from these entities
typedef struct world_entity_s 
{
	vec3_t pos;
	vec3_t lastPos;
	vec3_t angle;
	vec_t ttl;

	int type;
	int state;

	//struct world_entity_s *master;

} world_entity_t;


typedef struct actor_entity_s 
{
	vec3_t pos;
	vec3_t lastPos;
	vec3_t angle;
	char clipping;
	float time, eventTime, eventTimer;
	int state, nextState;
	float health;

	// Client
	unsigned int uid;
	char name[32];
	int actor, enemy;

	// Render
	unsigned int model;
	unsigned int skin;
	unsigned int animFrame;

} actor_entity_t;

enum OpenRaiderEvent
{
	eNone            = 0,
	eWeaponDischarge,
	eDying,
	eDead,
	eWounded,
	eRunForward,
	eRunBackward,
	eJump,
	eCrouchWalk,
	eIdle,
	eTaunt,
	eTurn,
	eRespawn,
	eLand
};


class World
{
 public:

	enum WorldFlag
	{
		fEnableHopping = 1
	};

	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	World();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Constructs an object of World
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.16: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~World();
	/*------------------------------------------------------
	 * Pre  : World object is allocated
	 * Post : Deconstructs an object of World
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.16: 
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////

	int getRoomByLocation(int index, float x, float y, float z);
	/*------------------------------------------------------
	 * Pre  : index - Guessed room index
	 * Post : Returns correct room index or -1 for unknown
	 *
	 *        NOTE: If it fails to be in a room it gives
	 *        closest overlapping room
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created, factored out of Render class 
	 ------------------------------------------------------*/

	int getRoomByLocation(float x, float y, float z);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Returns correct room index or -1 for unknown
	 *
	 *        NOTE: If it fails to be in a room it gives 
	 *        closest overlapping room
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created, factored out of Render class 
	 ------------------------------------------------------*/

	int getAdjoiningRoom(int index, 
								float x, float y, float z,
								float x2, float y2, float z2);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Looks for portal crossings from xyz to xyz2 segment
	 *        from room[index] returns index of adjoined room or -1
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.29:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int getSector(int room, float x, float z);
	int getSector(int room, float x, float z, float *floor, float *ceiling);
	/*------------------------------------------------------
	 * Pre  : room - valid room index
	 * Post : Gets the sector index of the position in room
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created, factored out of Render class 
	 ------------------------------------------------------*/

	unsigned int getRoomInfo(int room);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : 
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.28:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	bool isWall(int room, int sector);
	/*------------------------------------------------------
	 * Pre  : room - valid room index
	 *        sector - valid sector index
	 * Post : Returns true if this sector is a wall
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created, factored out of Render class 
	 ------------------------------------------------------*/

	bool getHeightAtPosition(int index, float x, float *y, float z);
	/*------------------------------------------------------
	 * Pre  : index - valid room index
	 * Post : Returns true if position is in a room
	 *        and sets y to the world height in that room
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created, factored out of Render class 
	 ------------------------------------------------------*/

	// Temp methods for rendering use until more refactoring is done
#ifdef BAD_BLOOD
	model_mesh_t *getMesh(int index);
	skeletal_model_t *getModel(int index);
	room_mesh_t *getRoom(int index);
	Vector<entity_t *> *getEntities();
	Vector<sprite_seq_t *> *getSprites();
	Vector<room_mesh_t *> *getRooms();
#endif

	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////
	
	void setFlag(WorldFlag flag);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Sets option flag
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created, factored out of Render class 
	 ------------------------------------------------------*/

	void clearFlag(WorldFlag flag);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Clears option flag
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created, factored out of Render class 
	 ------------------------------------------------------*/

	void destroy();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Clears all data in world, in future will check
	 *        if data is in use before clearing
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void addRoom(room_mesh_t *room);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Adds object to world
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created, factored out of Render class 
	 ------------------------------------------------------*/
	
	void addMesh(model_mesh_t *model);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Adds object to world
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created, factored out of Render class 
	 ------------------------------------------------------*/

	void addEntity(entity_t *e);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Adds object to world
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created, factored out of Render class 
	 ------------------------------------------------------*/

	int addModel(skeletal_model_t *model);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Adds object to world, returns next model Id
	 *        or -1 if failed to add model to world
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created, factored out of Render class 
	 ------------------------------------------------------*/

	void addSprite(sprite_seq_t *sprite);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Adds object to world
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created, factored out of Render class 
	 ------------------------------------------------------*/

	void moveEntity(entity_t *e, char movement);
	/*------------------------------------------------------
	 * Pre  : movement - 'f' orward
	 *                   'b' ackward
	 *                   'l' eft
	 *                   'r' ight
	 *
	 * Post : Move entity e in a given direction, unless
	 *        a collision ocurrs
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Moved to WOrld class
	 *
	 * 2002.09.02:
	 * Mongoose - Created
	 ------------------------------------------------------*/


 private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////

	void clear();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Clears all data in world
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.20:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	bool mClearLock;

	unsigned int mFlags;                   /* World flags */

	Vector<entity_t *> mEntities;	         /* World entities */

	Vector<room_mesh_t *> mRooms;	         /* Map data and meshes */

	Vector<model_mesh_t *> mMeshes;	      /* Unanimated meshes */

	Vector<sprite_seq_t *> mSprites;	      /* Sprites */

	Vector<skeletal_model_t *> mModels; 	/* Skeletal animation models */
};

#endif
