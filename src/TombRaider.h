/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : OpenRaider
 * Author  : Mongoose
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : OpenRaider
 * License : GPL, See file COPYING for details
 * Comments: Loads TR 1, 2, 3, and 4 maps, meshes, and textures
 *
 *           WARNING: No endian routines as of yet
 *
 *           Define ZLIB_SUPPORT to build TR4 support
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History ------------------------------------------------
 *
 * 2003.05.13:
 * Mongoose - New API, maintance cost was becoming so high
 *            it was needed to sort out methods in groups
 *            like my newer source code -- of course some
 *            methods were altered or removed ( clean up )
 *
 * 2003.05.19:
 * Mongoose - I'm now using Roy's documented TR5 structures, so
 *            I can fill in my blanks and share common structures
 *            with other TR5 pak loaders using it - I've removed
 *            most of my old TR5 test code - it may have been more
 *            correct for DX lights ( not sure ) it's still in CVS
 *
 * 2003.05.13:
 * Mongoose - New API, maintance cost was becoming so high
 *            it was needed to sort out methods in groups
 *            like my newer source code -- of course some
 *            methods were altered or removed ( clean up )
 *
 * 2001.06.19:
 * Mongoose - New texture API for the new TR4/TR5 bump map
 *            support, also purged textile exposure
 *
 *
 * 2001.05.21:
 * Mongoose - Added to project OpenRaider, more documentation
 *            than Freyja version I wrote ( 3d modeler )
 *
 *            TR4 texel and texture fixes
 *
 *            Runtime debug output toggle
 *
 *
 * 2000.05.13:
 * Mongoose - Added gcc and VC++ pragmas for packing
 *
 *            id style typedefs for structs
 *
 *            Heavy clean up and ported to C++
 *
 *            I saved yuri's notes as best I could and
 *            reformatted and corrected as needed.
 *
 * Mongoose - Created, based on:
 *                tr_view's tr2io.c by Yuri Zhivago, PhD,
 *                TR Rosetta Stone ( TombRaider pak format specs )
 ================================================================*/

#ifndef __OPENRAIDER_MONGOOSE_TOMBRAIDER_H_
#define __OPENRAIDER_MONGOOSE_TOMBRAIDER_H_

#ifdef WIN32
// Have to remove GCC packing, by removing in preprocessor
#define __attribute__(packed)

// MSVC++ pragma to pack structs
#define TR2_H 1
#pragma pack(push, tr2_h, 1)
#endif

//#include "TombRaider1.h"

typedef enum
{
  TR_VERSION_UNKNOWN,
  TR_VERSION_1,
  TR_VERSION_2,
  TR_VERSION_3,
  TR_VERSION_4,
  TR_VERSION_5

} tr2_version_type;


typedef enum
{
	TR_FREAD_NORMAL = 0,
	TR_FREAD_COMPRESSED

} tr_fread_mode_t;


typedef enum
{
	tombraiderLight_typeDirectional   = 1,
	tombraiderLight_typeSpot          = 2,
	tombraiderLight_typePoint         = 3

} tombraiderLightType;

typedef enum
{
	tombraiderLight_useCutoff         = 1,
	tombraiderLight_useAttenuation    = 2

} tombraiderLightFlags;

typedef enum
{
	tombraiderRoom_underWater         = 1
} tombraiderRoomFlags;

typedef enum
{
	tombraiderSector_wall = 1
} tombraiderSectorFlags;

typedef enum
{
	tombraiderFace_Alpha          = 1,
	tombraiderFace_Colored        = 2,
	tombraiderFace_PartialAlpha   = 4

} tombraiderFace_Flags;


#define TR_SOUND_FOOTSTEP0    1
#define TR_SOUND_F_PISTOL    12
#define TR_SOUND_F_PISTOL    12

enum TR4_Objects
{
	TR4_LARA = 0,
	TR4_PISTOLS_ANIM = 1,
	TR4_UZI_ANIM = 2,
	TR4_SHOTGUN_ANIM = 3,
	TR4_CROSSBOW_ANIM = 4,
	TR4_GRENADE_GUN_ANIM = 5,
	TR4_SIXSHOOTER_ANIM = 6,
	TR4_FLARE_ANIM = 7,
	TR4_LARA_SKIN = 8,
	TR4_LARA_SKIN_JOINTS = 9,
	TR4_LARA_SCREAM = 10,
	TR4_LARA_CROSSBOW_LASER = 11,
	TR4_LARA_REVOLVER_LASER = 12,
	TR4_LARA_HOLSTERS = 13,
	TR4_LARA_HOLSTERS_PISTOLS = 14,
	TR4_LARA_HOLSTERS_UZIS = 15,
	TR4_LARA_HOLSTERS_SIXSHOOTER = 16,
	TR4_LARA_SPEECH_HEAD1 = 17,
	TR4_LARA_SPEECH_HEAD2 = 18,
	TR4_LARA_SPEECH_HEAD3 = 19,
	TR4_LARA_SPEECH_HEAD4 = 20
};

#define TR_ANIAMTION_RUN               0
#define TR_ANIAMTION_STAND            11
#define TR_ANIAMTION_TURN_L           12
#define TR_ANIAMTION_TURN_R           13
#define TR_ANIAMTION_HIT_WALL_FRONT   53
#define TR_ANIAMTION_SWIM_IDLE        87
#define TR_ANIAMTION_SWIM             86
#define TR_ANIAMTION_SWIM_L          143
#define TR_ANIAMTION_SWIM_R          144
#define TR_ANIAMTION_GRAB_LEDGE       96
#define TR_ANIAMTION_PULLING_UP       97

/*---------------------------------------------------------------
 * Basic 24-bit colour structure
 * (although it appears that only 6 bits
 * per colour are actually used, making it 18-bit colour)
 ---------------------------------------------------------------*/
typedef struct tr2_colour_s
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
}  __attribute__ ((packed)) tr2_colour_t;


/*--------------------------------------------------------------
 * Basic vertex structure.
 * Note that all vertices are relative coordinates;
 * each mesh (room, object, etc.) has its own offset by
 * which the vertex coordinates are translated.
 --------------------------------------------------------------*/
typedef struct tr2_vertex_s
{
  short x;
  short y;
  short z;
}  __attribute__ ((packed))   tr2_vertex_t;


/*--------------------------------------------------------------
 * A rectangular (quad) face definition.
 * Four vertices (the values are indices into the
 * appropriate vertex list) and a texture (an index
 * into the texture list) or colour (index into 8-bit palette).
 *
 * I've seen a few coloured polygons where Texture is
 * greater than 255, but ANDing the value with 0xFF
 * seems to produce acceptable results.
 --------------------------------------------------------------*/
typedef struct tr2_quad_s /* was tr2_face4  */
{
  unsigned short vertices[4];
  unsigned short texture;
}  __attribute__ ((packed))   tr2_quad_t;

typedef struct tr4_quad_s
{
	unsigned short vertices[4];  // The 4 vertices of a quad
	unsigned short texture;      // Object-texture index
	unsigned short lighting;     // transparency flag & strength of
	                             // the hilight
	// Bit 0: if set, alpha channel = intensity (same meaning that when the Attribute field of tr2_object_texture is 2. Cf TRosetta stone document)
	// Bit 1-7: strength of the hilight. In TR4, objects can exhibit some kind of light reflection when seen from some particular angles. These bits give the strength of this effect: the more bigger the value is, the more visible is the effect.
}  __attribute__ ((packed))   tr4_quad_t;

/*--------------------------------------------------------------
 * A triangular face definition.
 * Three vertices (the values are indices into the
 * appropriate vertex list) and a texture (an index into the
 * texture list) or colour (index into palette).
 *
 * In the case of a colour, (Texture & 0xff) is the index
 * into the 8-bit palette, while (Texture >> 8) is
 * the index into the 16-bit palette.
 --------------------------------------------------------------*/
typedef struct tr2_tri_s /* was tr2_face3  */
{
  unsigned short vertices[3];
  unsigned short texture;
}  __attribute__ ((packed))   tr2_tri_t;

typedef struct tr4_tri_s
{
	unsigned short vertices[3];  // The 3 vertices of a tri
	unsigned short texture;      // Object-texture index
	unsigned short lighting;     // transparency flag & strength of
	                             // the hilight
}  __attribute__ ((packed))   tr4_tri_t;

/*--------------------------------------------------------------
 * An 8-bit texture tile (65536 bytes).
 *
 * Each byte represents a pixel whose colour
 * is in the 8-bit palette.
 --------------------------------------------------------------*/
typedef struct tr2_textile8_s
{
  unsigned char tile[256 * 256];
}  __attribute__ ((packed))   tr2_textile8_t;


/*--------------------------------------------------------------
 * A 16-bit texture tile (131072 bytes).
 *
 * Each word represents a pixel
 * whose colour is of the form ARGB, MSB-to-LSB:
 *
 *    1-bit transparency (0: transparent, 1: opaque)
 *    5-bit red channel
 *    5-bit green channel
 *    5-bit blue channel
 --------------------------------------------------------------*/
typedef struct tr2_textile16_s
{
   unsigned short tile[256 * 256];
}   __attribute__ ((packed))  tr2_textile16_t;


/* --------------------------------------------------------------
 * 32bit textiles
 * BGRA with 4bytes each channel
 --------------------------------------------------------------*/
typedef struct tr2_textile32_s
{
   unsigned int tile[256 * 256];
}   __attribute__ ((packed))  tr2_textile32_t;


/*--------------------------------------------------------------
 * The "header" of a room.
 * + X/Z indicate the base position of the room
 *   mesh in world coordinates.
 * + YLowest and yHighest indicate the lowest and
 * + Highest points in this room
 *    (even though their actual values appear to
 *     be reversed, since a "high" point will have a
 *     smaller value than a "low" point).
 *
 * When positioning objects/items, as well as the room meshes
 * themselves, y is always 0-relative (not room-relative).
 --------------------------------------------------------------*/
typedef struct tr2_room_info_s
{
  int x;             // X-offset of room (world coordinates)
  int z;             // Z-offset of room (world coordinates)
  int y_bottom;      // Y-offset of lowest point in room (world coordinates)
                     //   (actually highest value)
  int y_top;         // Y-offset of highest point in room (world coordinates)
                     //   (actually lowest value)
}  __attribute__ ((packed))   tr2_room_info_t;


/*--------------------------------------------------------------
 * Portal structure.
 * This defines every viable exit from a given "room".
 *
 * Note that "rooms" are really just areas.  They aren't
 * necessarily enclosed.  The door structure below defines
 * areas of egress, not the actual moveable mesh,
 * texture, and action (if any).
 --------------------------------------------------------------*/
typedef struct tr2_room_portal_s
{
   unsigned short adjoining_room;     // Which room this "door" leads to
   tr2_vertex_t normal;               // Which way the "door" faces
   tr2_vertex_t vertices[4];          // The corners of the "door"
}  __attribute__ ((packed))   tr2_room_portal_t;


/*--------------------------------------------------------------
 * Room sector structure.
 *
 * Sectors are 1024 * 1024 (world coordinates).  Floor and
 * Ceiling are signed number of 256 units of height
 * (relative to 0), e.g. Floor 0x04 corresponds to
 * Y = 1024 in world coordinates.
 *
 * Note: this implies that, while X and Z can be quite large,
 * Y is constrained to -32768..32512.  Floor/Ceiling value of
 * 0x81 indicates impenetrable wall.
 *
 * Floor values are used by the game engine to determine what
 * objects Lara can traverse and how.
 *
 * Relative steps of 1 (-256) can be walked up;
 * steps of 2..7 (-512..-1792) can/must be climbed;
 * steps larger than 7 (-2048..-32768) cannot be climbed (too tall)
 *
 * In TR3, BoxIndex is more complicated. Only bits 4-14 are the
 * "real" index; bits 0-3 are most likely some kind of flag,
 * such as what kind of footstep sound to make (wood, metal, snow).
 * Furthermore, there is a special value of the "real" index,
 * 2047, or 0x7ff.
 *
 * RoomAbove and RoomBelow indicate what neighboring rooms are
 * in these directions; if RoomAbove is not "none", then the
 * ceiling is a collisional portal to that room, while if
 * RoomBelow is not "none", then the floor is a collisional
 * portal to that room.
 --------------------------------------------------------------*/
typedef struct tr2_room_sector_s
{
	unsigned short fd_index;    // Index into FloorData[]
	unsigned short box_index;   // Index into Boxes[]/Zones[] ( -1 if none )
	unsigned char room_below;   // The number of the room below this one
	                            // ( -1 or 255 if none )
	char floor;                 // Absolute height of floor
	                            // ( Multiply by 256 for world coordinates )
	unsigned char  room_above;  // The number of the room above this one
	                            // ( -1 or 255 if none )
	char ceiling;               // Absolute height of ceiling
	                            // ( Multiply by 256 for world coordinates )
}  __attribute__ ((packed))   tr2_room_sector_t;


/*--------------------------------------------------------------
 * Room lighting structure.
 * X/Y/Z are in world coordinates.
 * Lighting values seem to range from 0..8192.
 --------------------------------------------------------------*/
typedef struct tr2_room_light_s
{
  int  x;
  int  y;
  int  z;
  unsigned short intensity1;
  unsigned short intensity2;
  unsigned int fade1;
  unsigned int fade2;
}   __attribute__ ((packed))  tr2_room_light_t;

typedef struct tr4_room_light_s
{
	int xPosition;  // world coords
	int yPosition;  // world coords
	int zPosition;  // world coords
   tr2_colour_t color; // three bytes rgb values
   unsigned char lightType;  // same as D3D (i.e. 2 is for spotlight)
   unsigned char unknown;    // always 0xff?
   unsigned char intensity;
   float in;
	float out;
	float length;
	float cutoff;
	float xDir, yDir, zDir;    // direction?

}   __attribute__ ((packed))  tr4_room_light_t;


/*--------------------------------------------------------------
 * Room vertex structure.
 * This defines the vertices within a room.
 --------------------------------------------------------------*/
typedef struct tr2_vertex_room_s
{
   tr2_vertex_t vertex;
                               // Following 3 entries
   short lighting1;            // Values range from 0 to 32767,
                               //   0 = total darkness. (TR3)
                               //   I think the values ranged from
                               //   0 to 8192 in TR1/2, 0=total brightness

   unsigned short attributes;  // 0x8000 Something to do with water surface
                               // 0x4000 Under water lighting modulation
                               //        and movement if viewed from
                               //        above water surface
                               // 0x2000 Water/quicksand surface movement
                               // 0x1fef Nothing?
                               // 0x0010 Everything?

   short lighting2;            // Seems to be the same as lighting1
}  __attribute__ ((packed))  tr2_vertex_room_t;


/*--------------------------------------------------------------
 * Sprite structure
 --------------------------------------------------------------*/
typedef struct tr2_room_sprite_s
{
   short vertex;     // offset into vertex list
   short texture;    // offset into texture list
}  __attribute__ ((packed))   tr2_room_sprite_t;


/*--------------------------------------------------------------
 * Room mesh structure.
 * This is the geometry of the "room," including
 * walls, floors, rocks, water, etc.  It does _not_ include
 * objects that Lara can interact with (keyboxes,
 * moveable blocks, moveable doors, etc.)
 --------------------------------------------------------------*/
typedef struct tr2_room_data_s
{
  short           num_vertices;    // number of vertices in the following list
  tr2_vertex_room_t *vertices;     // list of vertices (relative coordinates)
  short           num_rectangles;  // number of textured rectangles
  tr2_quad_t       *rectangles;    // list of textured rectangles
  short           num_triangles;   // number of textured triangles
  tr2_tri_t      *triangles;       // list of textured triangles
  short           num_sprites;     // number of sprites
  tr2_room_sprite_t *sprites;      // list of sprites
}  __attribute__ ((packed)) tr2_room_data_t;


/*--------------------------------------------------------------
 * Room static mesh data.
 * Positions and IDs of static meshes
 * (e.g. skeletons, spiderwebs, furniture)
 --------------------------------------------------------------*/
typedef struct tr2_room_staticmesh_s
{
   int  x;                   // absolute position in world coordinates
   int  y;
   int  z;
   unsigned short rotation;  // high two bits (0xc000)
                             //  indicate steps of 90 degrees
   unsigned short intensity1;
   unsigned short intensity2;
   unsigned short object_id; // which StaticMesh item to draw
}  __attribute__ ((packed)) tr2_room_staticmesh_t;


/*--------------------------------------------------------------
 * Room structure.
 * Here's where all the room data come together.
 --------------------------------------------------------------*/
typedef struct tr2_room_s
{
  tr2_room_info_t info;          // where the room exists, in world coordinates
  unsigned int num_data_words;   // number of data words (bitu16)
  unsigned char *data;           // the raw data from which the rest
                                 //  of this is derived
  tr2_room_data_t room_data;     // the room mesh
  unsigned short num_portals;    // number of visibility portals
                                 //  that leave this room
  tr2_room_portal_t *portals;    // list of visibility portals
  unsigned short num_zsectors;   // width of sector list
  unsigned short num_xsectors;   // height of sector list
  tr2_room_sector_t *sector_list;// list of sectors in this room
  short intensity1;
  short intensity2;
  short light_mode;
  unsigned short num_lights;            // number of lights in this room
  tr2_room_light_t *lights;             // list of lights
  unsigned short num_static_meshes;     // number of static meshes
  tr2_room_staticmesh_t *static_meshes; // static meshes
  short  alternate_room;
  short  flags;                         // 0x0001 - room is filled with water
                                        // 0x0020 - Lara's ponytail
                                        // gets blown by the wind

	tr2_colour_t room_light_colour;      // TR3 ONLY!
	tr4_room_light_t *tr4Lights;         // TR4 ONLY!

}  __attribute__ ((packed))  tr2_room_t;

/*--------------------------------------------------------------
 * Animation structure.
 --------------------------------------------------------------*/
typedef struct tr2_animation_s
{
  unsigned int frame_offset;    // byte offset into Frames[]
                                //   (divide by 2 for Frames[i])
  unsigned char frame_rate;     // "ticks" per frame
  unsigned char frame_size;     // number of words in Frames[]
                                //   used by this animation
  short state_id;
  short unknown1;
  short unknown2;
  short unknown3;
  short unknown4;

  unsigned short frame_start;   // first frame in this animation
  unsigned short frame_end;     // last frame in this animation
                                //  (numframes = (End - Start) + 1)
  unsigned short next_animation;
  unsigned short next_frame;
  unsigned short num_state_changes;

  unsigned short state_change_offset;  // offset into StateChanges[]
  unsigned short num_anim_commands;
  unsigned short anim_command;         // offset into AnimCommands[]
}  __attribute__ ((packed))   tr2_animation_t;

// Data for an animation structure (40 bytes vice 32 in TR1/2/3)
typedef struct tr4_animation_s
{
	unsigned int frame_offset;// same meaning than in TR3
	unsigned char frame_rate;// same meaning than in TR3
	unsigned char frame_size;// same meaning than in TR3
	unsigned short state_id;// same meaning than in TR3
	short unknown;// same meaning than in TR3
	short speed;// same meaning than in TR3
	unsigned short accel_lo;// same meaning than in TR3
	short accel_hi;// same meaning than in TR3
	unsigned char unknown2[8];// new in TR4

	unsigned short frame_start;// same meaning than in TR3
	unsigned short frame_end;// same meaning than in TR3
	unsigned short next_animation;// same meaning than in TR3
	unsigned short next_frame;// same meaning than in TR3

	unsigned short num_state_changes;// same meaning than in TR3
	unsigned short state_change_offset;// same meaning than in TR3
	unsigned short num_anim_commands;// same meaning than in TR3
	unsigned short anim_command;// same meaning than in TR3
}  __attribute__ ((packed))   tr4_animation_t;

/*--------------------------------------------------------------
 * State Change structure
 --------------------------------------------------------------*/
typedef struct tr2_state_change_s
{
  unsigned short state_id;
  unsigned short num_anim_dispatches;  // number of dispatches
                                       //  (seems to always be 1..5)
  unsigned short anim_dispatch;        // Offset into AnimDispatches[]
}  __attribute__ ((packed))   tr2_state_change_t;


/*--------------------------------------------------------------
 * Animation Dispatch structure
 --------------------------------------------------------------*/
typedef struct tr2_anim_dispatch_s
{
  short low;
  short high;
  short next_animation;
  short next_frame;
}   __attribute__ ((packed))  tr2_anim_dispatch_t;


/*--------------------------------------------------------------
 * AnimCommand structure
 --------------------------------------------------------------*/
typedef struct tr2_anim_command_s
{
   short value;
}  __attribute__ ((packed))   tr2_anim_command_t;


/*--------------------------------------------------------------
 * MeshTree structure
 *
 * MeshTree[] is actually groups of four ints.  The first one is a
 * "flags" word;  bit 1 (0x0002) indicates "make previous mesh an
 * anchor (e.g. PUSH)";  bit 0 (0x0001) indicates "return to previous
 * anchor (e.g. POP)".
 * The next three ints are X, Y, Z offsets from the last mesh position.
 --------------------------------------------------------------*/
typedef struct tr2_meshtree_s
{
  int flags;   // 0x0001 = POP, 0x0002 = PUSH
  int x;
  int y;
  int z;
}  __attribute__ ((packed))  tr2_meshtree_t;


/*--------------------------------------------------------------
 * Moveable structure.  This defines a list of contiguous meshes that
 * comprise one object, e.g. in WALL.TR2,
 *    moveable[0]  = Lara (StartingMesh 0, NumMeshes 15),
 *    moveable[13] = Tiger (StartingMesh 215, NumMeshes 27)
 *    moveable[15] = Spikes (StartingMesh 249, NumMeshes 1)
 *    moveable[16] = Boulder (StartingMesh 250, NumMeshes 1)
 *    moveable[20] = Rolling Blade (StartingMesh 254, NumMeshes 1)
 --------------------------------------------------------------*/
typedef struct tr2_moveable_s
{
  unsigned int object_id;        // Item Identifier
  unsigned short num_meshes;     // number of meshes in this object
  unsigned short starting_mesh;  // first mesh
  unsigned int mesh_tree;        // offset into MeshTree[]
  unsigned int frame_offset;     // byte offset into Frames[]
                                 //  (divide by 2 for Frames[i])
  unsigned short animation;      // offset into Animations[]
}  __attribute__ ((packed))  tr2_moveable_t;


/*--------------------------------------------------------------
 * StaticMesh structure.
 *
 * This defines meshes that don't move (e.g. skeletons
 * lying on the floor, spiderwebs, etc.)
 --------------------------------------------------------------*/
typedef struct tr2_staticmesh_s
{
  unsigned int object_id;             // Item Identifier
  unsigned short starting_mesh;       // first mesh
  tr2_vertex_t bounding_box[2][2];
  unsigned short flags;
}  __attribute__ ((packed))  tr2_staticmesh_t;


/*--------------------------------------------------------------
 * Object texture vertex structure.
 *
 * Maps coordinates into texture tiles.
 --------------------------------------------------------------*/
typedef struct tr2_object_texture_vert_s
{
  unsigned char xcoordinate;
  unsigned char xpixel;
  unsigned char ycoordinate;
  unsigned char ypixel;
}  __attribute__ ((packed))  tr2_object_texture_vert_t;


/*--------------------------------------------------------------
 * Object texture structure.
 --------------------------------------------------------------*/
typedef struct tr2_object_texture_s
{
  unsigned short transparency_flags;  // 0: Opaque
                                      // 1: Use transparency
                                      // 2: Use partial transparency
                                      //    [grayscale intensity :: transparency]

  unsigned short tile;                     // index into textile list
  tr2_object_texture_vert_t vertices[4];   // the four corners of the texture
}  __attribute__ ((packed))   tr2_object_texture_t;


/*--------------------------------------------------------------
 * Sprite texture structure.
 --------------------------------------------------------------*/
typedef struct tr2_sprite_texture_s
{
  unsigned short tile;
  unsigned char  x;
  unsigned char  y;
  unsigned short width;     // actually, (width * 256) + 255
  unsigned short height;    // actually, (height * 256) + 255
  short  left_side;
  short  top_side;
  short  right_side;
  short  bottom_side;
}  __attribute__ ((packed))   tr2_sprite_texture_t;


/*--------------------------------------------------------------
 * Sprite Sequence structure
 --------------------------------------------------------------*/
typedef struct tr2_sprite_sequence_s
{
  int object_id;         // Item identifier (same numbering as in tr2_moveable)
  short negative_length; // negative of "how many sprites are in this sequence"
  short offset;          // where (in sprite texture list) this sequence starts
}  __attribute__ ((packed))   tr2_sprite_sequence_t;


/*--------------------------------------------------------------
 * Mesh structure.
 *
 * The mesh list contains the mesh info for Lara (in all her
 * various incarnations), blocks, enemies (tigers, birds, bad guys),
 * moveable blocks, zip line handles, boulders, spinning blades,
 * you name it.
 *
 * If NumNormals is negative, Normals[] represent vertex
 * lighting values (one per vertex).
 --------------------------------------------------------------*/
typedef struct tr2_mesh_s
{
  tr2_vertex_t centre;       // this seems to describe the
                             //  approximate geometric centre
                             //  of the mesh (possibly the centre of gravity?)
                             // (relative coordinates, just like the vertices)
  int      collision_size;   // radius of collisional sphere
  short     num_vertices;    // number of vertices in this mesh
  tr2_vertex_t *vertices;    // list of vertices (relative coordinates)
  short     num_normals;     // number of normals in this mesh
                             //  (should always equal NumVertices)
  tr2_vertex_t *normals;     // list of normals (NULL if NumNormals < 0)
  short      *mesh_lights;   // if NumNormals < 0

  short num_textured_rectangles;   // number of textured rectangles
  tr2_quad_t *textured_rectangles; // list of textured rectangles

  short num_textured_triangles;    // number of textured triangles in this mesh
  tr2_tri_t  *textured_triangles;  // list of textured triangles

  short  num_coloured_rectangles;  // number of coloured rectangles
  tr2_quad_t *coloured_rectangles; // list of coloured rectangles

  short    num_coloured_triangles; // number of coloured triangles in this mesh
  tr2_tri_t  *coloured_triangles;  // list of coloured triangles
}  __attribute__ ((packed))   tr2_mesh_t;


/*--------------------------------------------------------------
 * Frame structure.
 *
 * Frames indicates how composite meshes are positioned and rotated.
 * They work in conjunction with Animations[] and Bone2[].
 *
 * A given frame has the following format:
 *    short BB1x, BB1y, BB1z           // bounding box (low)
 *    short BB2x, BB2y, BB2z           // bounding box (high)
 *    short OffsetX, OffsetY, OffsetZ  // starting offset for this moveable
 *    (TR1 ONLY: short NumValues       // number of angle sets to follow)
 *    (TR2/3: NumValues is implicitly NumMeshes (from moveable))
 *
 * What follows next is a list of angle sets.  In TR2/3, an angle set can
 * specify either one or three axes of rotation.  If either of the high two
 * bits (0xc000) of the first angle unsigned short are set, it's one axis:
 *  only one  unsigned short,
 *  low 10 bits (0x03ff),
 *  scale is 0x100 == 90 degrees;
 * the high two  bits are interpreted as follows:
 *  0x4000 == X only, 0x8000 == Y only,
 *  0xC000 == Z only.
 *
 * If neither of the high bits are set, it's a three-axis rotation.  The next
 * 10 bits (0x3ff0) are the X rotation, the next 10 (including the following
 * unsigned short) (0x000f, 0xfc00) are the Y rotation,
 * the next 10 (0x03ff) are the Z rotation, same scale as
 * before (0x100 == 90 degrees).
 *
 * Rotations are performed in Y, X, Z order.
 * TR1 ONLY: All angle sets are two words and interpreted like the two-word
 * sets in TR2/3, EXCEPT that the word order is reversed.
 --------------------------------------------------------------*/
typedef struct tr2_frame_s
{
  tr2_vertex_t vector[3];
  int        num_words;
  unsigned short     *words;
}  __attribute__ ((packed))   tr2_frame_t;


/*--------------------------------------------------------------
 * Item structure
 --------------------------------------------------------------*/
typedef struct tr2_item_s
{
  short object_id;
  short room;
  int x;
  int y;
  int z;
  short angle;
  short intensity1;
  short intensity2;
  short flags;   // 0x0100 indicates "inactive" or "invisible"
}  __attribute__ ((packed))   tr2_item_t;


/*--------------------------------------------------------------
 * SoundSource structure
 --------------------------------------------------------------*/
typedef struct tr2_sound_source_s
{
  int x;                           // position of sound source
  int y;
  int z;
  unsigned short sound_id;         // internal sound index
  unsigned short flags;            // 0x40, 0x80, or 0xc0
}  __attribute__ ((packed))   tr2_sound_source_t;


/*--------------------------------------------------------------
 * Boxes structure
 --------------------------------------------------------------*/
typedef struct tr2_boxes_s
{
  unsigned char zmin;             // sectors (* 1024 units)
  unsigned char zmax;
  unsigned char xmin;
  unsigned char xmax;
  short true_floor;        // Y value (no scaling)
  short overlap_index;     // index into Overlaps[]
}  __attribute__ ((packed))   tr2_box_t;


/*--------------------------------------------------------------
 * AnimatedTexture structure
 * - really should be simple short[], since it's variable length
 --------------------------------------------------------------*/
typedef struct tr2_animated_texture_s
{
  short num_texture_ids;    // Number of Texture IDs - 1
  short *texture_list;      // list of textures to cycle through
}  __attribute__ ((packed))   tr2_animated_texture_t;


/*--------------------------------------------------------------
 * Camera structure
 --------------------------------------------------------------*/
typedef struct tr2_camera_s
{
  int x;
  int y;
  int z;
  short room;
  unsigned short unknown1;  // correlates to Boxes[]?
} __attribute__ ((packed))    tr2_camera_t;

// Data for a flyby camera (40 bytes)
typedef struct tr4_extra_camera_s
{
	int pos[6];  // Positions ? (x1,y1,z1,x2,y2,z2) roatations?
	unsigned char index[2]; // A pair of indices
	unsigned short unknown[5];// ??
	int id ;// Index of something
} __attribute__ ((packed))    tr4_extra_camera_t;


/*--------------------------------------------------------------
 * Sound sample structure
 --------------------------------------------------------------*/
typedef struct tr2_sound_details_s
{
  short sample;
  short volume;
  short sound_range;
  short flags;         // bits 8-15: priority?, 2-7: number of sound samples
                       // in this group, bits 0-1: channel number
} __attribute__ ((packed)) tr2_sound_details_t;


/*--------------------------------------------------------------
 * Cutscene Camera structure
 --------------------------------------------------------------*/
typedef struct tr2_cinematic_frame_s
{
  short roty;      // Rotation about Y axis, +/-32767 ::= +/- 180 degrees
  short rotz;      // Rotation about Z axis, +/-32767 ::= +/- 180 degrees
  short rotz2;     // Rotation about Z axis (why two?),
                   //    +/-32767 ::= +/- 180 degrees
  short posz;      // Z position of camera, relative to something
  short posy;      // Y position of camera, relative to something
  short posx;      // X position of camera, relative to something
  short unknown1;
  short rotx;      // Rotation about X axis, +/-32767 ::= +/- 180 degrees
} __attribute__ ((packed)) tr2_cinematic_frame_t;


/*--------------------------------------------------------------
 * Used to read TR4 AI data
 *  this field replaces the bitu16 NumCinematicFrames of TR1/2/3 levels
 *  Data for a AI object (24 bytes)
 --------------------------------------------------------------*/
typedef struct tr4_ai_object_s
{
	unsigned short int object_id; // the objectID from the AI object
	                              // (AI_FOLLOW is 402)
	unsigned short int room;
	int x, y, a;
	unsigned short int ocb;
	unsigned short int flags; // The trigger flags
	                          // (button 1-5, first button has value 2)
	int angle;  // rotation
} __attribute__ ((packed)) tr4_ai_object_t;


/*--------------------------------------------------------------
 * Used to read packed TR4 texels
 --------------------------------------------------------------*/
typedef struct tr4_object_texture_s
{
	unsigned short	attribute; // same meaning than in TR3
	unsigned short tile;      // same meaning than in TR3
	unsigned short flags;// new in TR4
	tr2_object_texture_vert_t vertices[4];// same meaning than in TR3
	unsigned int unknown1, unknown2;  // new in TR4: x & y offset in something
	unsigned int xsize,ysize;// new in TR4: width-1 & height-1 of the object texture

}  __attribute__ ((packed)) tr4_object_texture_t;


typedef struct tr5_room_layer_s // (56 bytes)
{
   u_int32_t numLayerVertices; // number of vertices in this layer (4 bytes)

   u_int16_t unknownL1; // unknown (2 bytes)

   u_int16_t numLayerRectangles; // number of rectangles in this layer (2 bytes)

   u_int16_t numLayerTriangles; // number of triangles in this layer (2 bytes)

	u_int16_t unknownL2; // appears to be the number of 2 sided textures in this layer, however is sometimes 1 off (2 bytes)

	u_int16_t filler; // always 0 (2 bytes)

	u_int16_t filler2; // always 0 (2 bytes)

	// The following 6 floats (4 bytes each)
	//  define the bounding box for the layer
	float layerBoundingBoxX1;
	float layerBoundingBoxY1;
	float layerBoundingBoxZ1;
	float layerBoundingBoxX2;
	float layerBoundingBoxY2;
	float layerBoundingBoxZ2;

	u_int32_t filler3; // always 0 (4 bytes)

	u_int32_t unknownL6; // unknown (4 bytes)

	u_int32_t unknownL7; // unknown (4 bytes)

	u_int32_t unknownL8; // unknown. Always the same throughout the level. (4 bytes)

} tr5_room_layer_t;


typedef struct tr5_face4_s // (12 bytes)
{
	u_int16_t vertices[4]; // the values are the indices into the appropriate layer vertice list. (2 bytes each)

	u_int16_t texture; // the texture number for this face. Needs to be masked with 0xFFF as the high 4 bits are flags.(2 bytes)

	u_int16_t unknownF4; // unknown (2 bytes)

} tr5_face4_t;


typedef struct tr5_face3_s // (10 bytes)
{
	u_int16_t vertices[3]; // the values are the indices into the appropriate layer vertice list (2 bytes each)

	u_int16_t texture; // the texture number for this face. Needs to be masked with 0xFFF as the high 4 bits are flags (2 bytes)

	u_int16_t unknownF3; // unknown (2 bytes)

} tr5_face3_t;

typedef struct tr5_vertex_s  //  (28 bytes)
{
	float x; // x of vertex (4 bytes)
	float y; // y of vertex (4 bytes)
	float z; // z of vertex (4 bytes)

	float nx; // x of vertex normal (4 bytes)
	float ny; // y of vertex normal (4 bytes)
	float nz; // z of vertex normal (4 bytes)

	u_int32_t vColor; // vertex color ARGB format (4 bytes)

} tr5_vertex_t;

typedef struct tr5_room_geometry_s
{
	// This is to help store and manage TR5 layer based polgons for rooms
	tr5_face4_t *quads;
	tr5_face3_t *tris;
	tr5_vertex_t *verts;

} tr5_room_geometry_t;


typedef struct tr5_light_s //  (88 bytes)
{
	/* Maybe wrong: The first three floats specify the position of the light
		in world coordinates

		The sun type light should not use these but seems to have a
		large x value (9 million, give or take)
		a zero y value, and a small z value (4 - 20) in the original TR5 levels
	*/

	float x; // x position of light (4 bytes)
	float y; // y position of light (4 bytes)
	float z; // z position of light (4 bytes)

	// The next three specify the color of the light
	float red; // color of red spectrum (4 bytes)
	float green; // color of green spectrum (4 bytes)
	float blue; // color of blue spectrum (4 bytes)

	u_int32_t seperator; // not used 0xCDCDCDCD (4 bytes)

	/* At this point the following info diverges dependant
		on which type of light being used:

		0 = sun,  1 = light,  2 = spot,  3 = shadow

		The sun type doesn't use the next two.

		For the spot type these are the hotspot and falloff angle cosines

		For the light and shadow types these are the TR units
		for the hotspot/falloff (1024=1sector)
	*/

	float input; // cosine of the IN value for light / size of IN value (4 bytes)
	float output; // cosine of the OUT value for light / size of OUT value (4 bytes)

	//  The next two are only used by the spot type light
	float radInput; // (IN radians) * 2 (4 bytes)
	float radOutput; // (OUT radians) * 2 (4 bytes)

	//  The next is also only used by the spot type light
	float range; // Range of light (4 bytes)

	/* The final 3 floats are used only by the 'sun' and 'spot' type lights.
		They describe the directional vector of the light.
		This can be obtained by :

		if both x and y LightDirectionVectorX = cosX * sinY

		LightDirectionVectorY = sinX
		LightDirectionVectorZ = cosX * cosY
	*/

	float directionVectorX; // light direction (4 bytes)
	float directionVectorY; // light direction (4 bytes)
	float directionVectorZ; // light direction (4 bytes)

	//  The next six values repeat some of the previous information in long data types vice floats

	int32_t x2; // x position of light (4 bytes)
	int32_t y2; // y position of light (4 bytes)
	int32_t z2; // z position of light (4 bytes)

	//  16384 = 1.0 for the rotations ( 1/16384 )
	int32_t directionVectorX2;  // light direction (4 bytes)
	int32_t directionVectorY2; // light direction (4 bytes)
	int32_t directionVectorZ2; // light direction (4 bytes)

	//  The next char specifies the type of light as specified above
	u_int8_t lightType; // type of light. (1 byte)

	u_int8_t seperator2[3]; // 0xCDCDCD (3 bytes)

} tr5_light_t;

// [ FIRST NUMBER IS OFFSET IN BYTES FROM START OF ROOM STRUCTURE]
typedef struct tr5_room_s
{
	u_int32_t checkXELA; // "XELA" (4 bytes)

	u_int32_t roomDataSize; /* size of the following data ( use this vice
										'walking thru' to get next room) (4 bytes) */

	u_int32_t seperator; // 0xCDCDCDCD (4 bytes)

	u_int32_t endSDOffset; /* usually this number +  216 will give you the
									  offset from the start of the room data to the end
									  of the Sector Data. HOWEVER have seen where this
									  bitu32 is equal to -1 so it is better to use the
									  following bitu32 and + 216 +
									  ((NumXSectors * NumZSectors)*8) if you need
									  to obtain this information. (4 bytes) */

	u_int32_t startSDOffset; /* this number + 216 will give you the offset from
										 the start of the room to the start of the
										 sector data. (4 bytes) */

	u_int32_t seperator2; // will either be 0x00000000 or 0xCDCDCDCD (4 bytes)

	u_int32_t endPortalOffset; /* this number + 216 will give you the offset
											from the start of the room to the end of the
											portal data. (4 bytes) */

	int32_t x; // X position of room ( world coordinates) (4 bytes)

	int32_t seperator3; // 0x00000000 (4 bytes)

	int32_t z; // Z position of room (world coordinates) (4 bytes)

	int32_t yBottom; // lowest point in room (4 bytes)

	int32_t yTop; // highest point in room (4 bytes)

	u_int16_t numZSectors; // sector table width (2 bytes)

	u_int16_t numXSectors; // sector table height (2 bytes)

	u_int32_t roomAmbientColor; // ARGB format (blue is least significant byte) (4 bytes)

	u_int16_t numRoomLights; // number of lights in this room (2 bytes)

	u_int16_t numStaticMeshes; // number of static meshes in this room (2 bytes)

	u_int16_t unknownR1; // usually 0x0001 however high byte is sometimes non zero (2 bytes)

	u_int16_t unknownR2; // usually 0x0000 however low byte is sometimes non zero (2 bytes)

	u_int32_t filler; // always 0x00007FFF (4 bytes)

	u_int32_t filler2; // always 0x00007FFF (4 bytes)

	u_int32_t seperator4; // 0xCDCDCDCD (4 bytes)

	u_int32_t seperator5; // 0xCDCDCDCD (4 bytes)

	unsigned char seperator6[6]; // 6 bytes 0xFF

	u_int16_t roomFlag; // 0x01 = water, 0x20 = wind, others unknown (2 bytes)

	u_int16_t unknownR5; // unknown (2 bytes)

	unsigned char seperator7[10]; // 10 bytes 0x00

	u_int32_t seperator8; // 0xCDCDCDCD (4 bytes)

	u_int32_t unknownR6; // unknown (4 bytes)

	float roomX; // X position of room in world coordinates * If  null room then this data will be 0xCDCDCDCD (4                                bytes)

	u_int32_t seperator9; // 0x00000000 or 0xCDCDCDCD if null room. (4 bytes)

	float roomZ; // Z position of room in world coordinates * If null room then will be bitu32 0xCDCDCDCD (4                                bytes)

	u_int32_t seperator10; // 0xCDCDCDCD (4 bytes)

	u_int32_t seperator11; // 0xCDCDCDCD (4 bytes)

	u_int32_t seperator12; // 0xCDCDCDCD (4 bytes)

	u_int32_t seperator13; // 0xCDCDCDCD (4 bytes)

	u_int32_t seperator14; // 0x00000000 unless null room then 0xCDCDCDCD (4 bytes)

	u_int32_t seperator15; // 0xCDCDCDCD (4 bytes)

	u_int32_t numRoomTriangles; // total number of triangles this room (4 bytes)

	u_int32_t numRoomRectangles; // total number of rectangles this room (4 bytes)

	u_int32_t seperator16; // 0x00000000 (4 bytes)

	u_int32_t lightSize; //  size of light data (number of lights * 88) (4 bytes)

	u_int32_t numTotalRoomLights; // total number of lights this room (4 bytes)

	u_int32_t unknownR7; // unknown, usually equals 0, 1, 2, or 3 (4 bytes)

	int32_t unknownR8; // usually equals room yTop. Sometimes a few blocks off. If null room then 0xCDCDCDCD

	int32_t lyBottom; // equals room yBottom. If null room then 0xCDCDCDCD (4 bytes)

	u_int32_t numLayers; // number of layers (pieces) this room (4 bytes)

	u_int32_t layerOffset; // this number + 216 will give you an offset from the start of the room data to the start of the                                         layer data (4 bytes)

	u_int32_t verticesOffset; // this number + 216 will give you an offset from the start of the room data to the start of the verex data (4 bytes)

	u_int32_t polyOffset; // this number + 216 will give you an offset from the start of the room data to the start of the rectangle/triangle data (4 bytes)

	u_int32_t polyOffset2; // same as above ? (4 bytes)

	u_int32_t verticesSize; // size of vertices data block (4 bytes)

	u_int32_t seperator17; // 0xCDCDCDCD (4 bytes)

	u_int32_t seperator18; // 0xCDCDCDCD (4 bytes)

	u_int32_t seperator19; // 0xCDCDCDCD (4 bytes)

	u_int32_t seperator20; // 0xCDCDCDCD (4 bytes)

	tr5_light_t *lights; //[LightSize]; // data for the lights (88 bytes * NumRoomLights)

	tr2_room_sector_t *sectors; //Data[(NumXSectors * NumZSectors) * 8]; // normal sector data structure

	u_int16_t numDoors; // number of portals (2 bytes)

	tr2_room_portal_t *doors;//[NumDoors]; // normal portal structure (32 bytes * NumDoors)

	u_int16_t seperator21; // 0xCDCD (2 bytes)

	tr2_room_staticmesh_t *meshes; //[NumStaticMeshes]; // normal room static mesh structure (20 bytes * NumRoomStaticMeshes)

	tr5_room_layer_t *layers; // [NumLayers]; // data for the room layers (pieces) (56 bytes * NumLayers)

	tr5_room_geometry_t *faces; //[NumRoomRectangles + NumRoomTriangles];
	/* Data for the room polys (face4 and face3).
		Structured as first layers rectangles then triangles, followed by the
		next layers rectangles and triangles, etc., until all layers are done.
		(12 bytes each rectangle. 10 bytes each triangle) */

	tr5_vertex_t *vertices; //[VerticesSize];
	/* data for the room vertices. Structured as vertices for the first layer,
		then vertices for the second layer, etc., until all layers are done.
		(28 bytes each vertex. */

} tr5_room_t;

typedef struct tr5_object_texture_vertex_s // (4 bytes)
{
	u_int8_t xCoordinate; // 0 if Xpixel is the low value, 255 if Xpixel is the high value in the object texture (1 byte)

	u_int8_t xPixel; // (1 byte)

	u_int8_t yCoordinate; // 0 if Ypixel is the low value, 255 if Ypixel is the high value in the object texture (1 byte)

	u_int8_t yPixel; // (1 byte)

} tr5_object_texture_vertex_t;

typedef struct tr5_object_texture_s //  (40 bytes)
{
	u_int16_t attribute; //0, 1, or 2 (2 means 2 sided) (2 bytes)

	u_int32_t tile; // need to mask with 0xFF as other bits are flags. ie int15_t seems to indicate triangle (4 bytes)

	tr5_object_texture_vertex_t vertices[4]; // Vertices[4] (16 bytes)

	u_int32_t uv1; // unknown how used (4 bytes)

	u_int32_t uv2; // unknown how used (4 bytes)

	u_int32_t xSize; // unknown how used, x size (4 bytes)

	u_int32_t ySize; // unknown how used, y size (4 bytes)

	u_int16_t seperator; // always 0x0000 (2 bytes)

} tr5_object_texture_t;

typedef struct tr5_flyby_camera_s //  (40 bytes)
{
	int32_t cameraX; // x position of camera in world coordinates (4 bytes)
	int32_t cameraY; // y position of camera in world coordinates (4 bytes)
	int32_t cameraZ; // z  position of camera in world coordinates (4 bytes)

	int32_t targetX; // x position of aiming point in world coords (4 bytes)
	int32_t targetY; // y position of aiming point in world coords (4 bytes)
	int32_t targetZ; // z  position of aiming point in world coords (4 bytes)

	int8_t  sequence; // sequence # of camera (1 byte)

	int8_t  cameraNumber; // camera # (1 byte)

	u_int16_t fov; // fov of camera ( .0054945 for each degree ) (2 bytes)

	u_int16_t roll; // roll of camera ( .0054945 for each degree ) (2 bytes)

	u_int16_t timer; // timer number (2 bytes)

	u_int16_t speed; // ( .000015259 each ) (2 bytes)

	u_int16_t flags; // ( see your handy TRLE manual for the specs ) (2 bytes)

	u_int32_t room; // room number (4 bytes)

} tr5_flyby_camera_t;

typedef struct tr5_moveable_s // ( 20 bytes ) ( same as old structure but has u_int16_t filler at the end )
{
	u_int32_t objectId; // object identifier ( matched in Items[] )

	u_int16_t numMeshes; // number of meshes in this object

	u_int16_t startingMesh; // starting mesh ( offset into MeshPointers[] )

	u_int32_t meshTree; // offset into MeshTree[] )

	u_int32_t frameOffset; // byte offset into Frames[] ( divide by 2 for Frames[i] )

	u_int16_t animation; // offset into Animations[]

	u_int16_t filler; // always equal to 65519 ( 0xFFEF )

} tr5_moveable_t;


typedef struct tr5_mesh_s
{
	tr2_vertex_t center; // relative coordinates of mesh centre (6 bytes)
	u_int8_t unknown1[4]; // unknown (4 bytes)
	int16_t numVertices; // number of vertices to follow (2 bytes)
	tr2_vertex_t *vertices; //[NumVertices]; // list of vertices (NumVertices * 6 bytes)
	int16_t numNormals; // number of normals to follow (2 bytes)
	tr2_vertex_t *normals; //[NumNormals]; // list of normals (NumNormals * 6 bytes) (becomes Lights if NumNormals < 0; 2 bytes)
	int16_t numTexturedRectangles; // number of textured rectangles to follow (2 bytes)
	tr5_face4_t *texturedRectangles; //[NumTexturedRectangles]; // list of textured rectangles (NumTexturedRectangles * 12 bytes)
	int16_t numTexturedTriangles; // number of textured triangles to follow (2 bytes)
	tr5_face3_t *texturedTriangles; //[NumTexturedTriangles]; // list of textured triangles (NumTexturedTriangles * 10 bytes)

} tr5_mesh_t;


typedef struct tr5_animation_s // ( 40 bytes ) ( same as old structure but has 8 bytes before FrameStart )
{
	u_int32_t  FrameOffset; // byte offset into Frames[] ( divide by 2 for Frames[i] )

	u_int8_t    FrameRate; // Engine ticks per frame

	u_int8_t    FrameSize; // number of int16_t's in Frames[] used by this animation

	u_int16_t  StateId; //

	int16_t    Unknown;

	int16_t    Speed; // Evengi Popov found this but I never seen what he said it was for

	u_int16_t  AccelLo; // same as above

	int16_t    AccelHi; // same as above

	u_int8_t    AUnknown[8]; // Unknown

	u_int16_t  FrameStart; // first frame in this animation

	u_int16_t  FrameEnd; // last frame in this animation ( numframes = ( End - Start) + 1 )

	u_int16_t  NextAnimation;

	u_int16_t  NextFrame;

	u_int16_t  NumStateChanges;

	u_int16_t  StateChangeOffset; // offset into StateChanges[]

	u_int16_t  NumAnimCommands; // how many of them to use

	u_int16_t  AnimCommand; // offset into AnimCommand[]

} tr5_animation_t;

typedef struct tr5_cinematic_frame_s
{
	unsigned int unknown[24];
} tr5_cinematic_frame_t;

#ifdef WIN32
#pragma pack(pop, tr2_h, 1)
#endif

class TombRaider
{
 public:

	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	TombRaider();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Constructs an object of TombRaider
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~TombRaider();
	/*------------------------------------------------------
	 * Pre  : TombRaider object is allocated
	 * Post : Deconstructs an object of TombRaider
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	////////////////////////////////////////////////////////////
	// Wash me -- not part of cleaned API
	////////////////////////////////////////////////////////////

	int NumRooms();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int NumMoveables();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int NumTextures();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int NumSpecialTextures();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns number of "special" textures/images
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int NumAnimations();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned int NumFrames();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int NumStaticMeshes();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int NumSprites();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int NumSpriteSequences();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.06.06:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int NumItems();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.06.06:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	tr2_version_type Engine();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned short *Frame();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	tr2_animation_t *Animation();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	tr2_item_t *Item();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	tr2_object_texture_t *ObjectTextures();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
   ------------------------------------------------------*/

	unsigned int getNumBoxes();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : returns number of boxes
	 *
	 *-- History ------------------------------------------
	 *
	 * 200?.??.??:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	tr2_box_t *Box();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	tr2_mesh_t *Mesh();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int getNumAnimsForMoveable(int moveable_index);
	/*------------------------------------------------------
	 * Pre  : moveable_id is valid
	 * Post : Returns number of animations for a moveable
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.15:
	 * Mongoose - Using signed values now ( like it should have )
	 *
	 * 2002.04.04:
	 * Mongoose - Created, based on TRViewer algorithm
	 ------------------------------------------------------*/

	tr2_staticmesh_t *StaticMesh();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	tr2_moveable_t *Moveable();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	tr2_meshtree_t *MeshTree();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	tr2_sprite_texture_t *Sprite();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns the sprite array
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.06.02:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	tr2_sprite_sequence_t *SpriteSequence();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.06.06:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned char *SpecialTexTile(int texture);
	/*------------------------------------------------------
	 * Pre  : texture is valid index into tex_special list
	 *
	 * Post : Makes a 32bit RGBA image from a stexture/bmap
	 *        and returns it.  Returns NULL on error.
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void Texture(int texture, unsigned char **image, unsigned char **bumpmap);
	/*------------------------------------------------------
	 * Pre  : texture is valid textile index
	 * Post : Returns copies of texture and it's bumpmap
	 *        if found, otherwise returns NULL
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned int *Palette16();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned char *Palette8();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	tr2_room_t *Room();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int checkMime(char *filename);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns 0 if TombRaider pak file
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.07.05:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int Load(char *filename, void (*percent)(int));
	/*------------------------------------------------------
	 * Pre  : filename is valid string and points
	 *        to vaild TombRaider 1-5 pak
	 *
	 * Post : Loads TombRaider 1-5 pak into memory
	 *        and does some processing
	 *
	 *        Returns 0 on sucess, and less than 0 on error
	 *
	 *        At 10% all textures are loaded
	 *        the exact figure '10' will always be passed to
	 *        allow for texture caching while meshes load
	 *        for TR4
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.01.02:
	 * Mongoose - New callback for percent loaded feedback
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/



	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////

	float adjustTexel(unsigned char texel, char offset);
	/*------------------------------------------------------
	 * Pre  : Passed the correct coor/pixel pair from
	 *        object texture
	 *
	 * Post : Makes a clamped 0.0 to 1.0 texel from coord pair
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.06.21:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void computeRotationAngles(unsigned short **frame,
										unsigned int *frame_offset,
										unsigned int *angle_offset,
										float *x, float *y, float *z);
	/*------------------------------------------------------
	 * Pre  : Given moveable animation data
	 * Post : Returns computed angles in x,y,z pointers
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.04.05:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void computeUV(tr2_object_texture_vert_t *st, float *u, float *v);
	/*------------------------------------------------------
	 * Pre  : Args are valid pointers
	 * Post : Returns computed UV in u and v
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.04.05:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int getBumpMapCount();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns number of bump maps in loaded pak
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getColor(int index, float color[4]);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.30:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	tr2_version_type getEngine();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Obsoloetes Engine() method, name change
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getMeshCollisionInfo(unsigned int meshIndex,
									  float center[3], float *radius);
	/*------------------------------------------------------
	 * Pre  : float Center[3], float radius allocated
	 *
	 * Post : Returns Center and radius of collision sphere
	 *        for mesh[index]
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.16:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int getMeshCount();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns SIGNED mesh count ( TR encoded < 0 errs )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getMeshColoredRectangle(unsigned int meshIndex,
										  unsigned int faceIndex,
										  int index[6], float color[4]);
	/*------------------------------------------------------
	 * Pre  : This method interface may change later
	 *
	 *        Pass allocated index[6], color[4] ( RGBA )
	 *
	 * Post : This method is made to let you fill arrays or
	 *        create single faces, their may be an allocating
	 *        version that passes back arrays later
	 *
	 *        Returns Quads/Rects as 2 triangles, because
	 *        returning quads would be too trival  =)
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.16:
	 * Mongoose - Created, based on method from OpenRaider
	 ------------------------------------------------------*/

	void getMeshColoredTriangle(unsigned int meshIndex,
										 unsigned int faceIndex,
										 int index[3], float color[4]);
	/*------------------------------------------------------
	 * Pre  : This method interface may change later
	 *
	 *        Pass allocated index[3], color[4] ( RGBA )
	 *
	 * Post : This method is made to let you fill arrays or
	 *        create single faces, their may be an allocating
	 *        version that passes back arrays later
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.16:
	 * Mongoose - Created, based on method from OpenRaider
	 ------------------------------------------------------*/

	void getMeshTexturedRectangle(unsigned int meshIndex,
											unsigned int faceIndex,
											int index[6], float st[12], int *texture,
											unsigned short *transparency);
	/*------------------------------------------------------
	 * Pre  : This method interface may change later
	 *
	 *        Pass allocated index[6], st[12], texture,
	 *        and transparency
	 *
	 * Post : This method is made to let you fill arrays or
	 *        create single faces, their may be an allocating
	 *        version that passes back arrays later
	 *
	 *        Returns Quads/Rects as 2 triangles, because
	 *        returning quads would be too trival  =)
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.16:
	 * Mongoose - Created, based on method from OpenRaider
	 ------------------------------------------------------*/

	void getMeshTexturedTriangle(unsigned int meshIndex,
										  unsigned int faceIndex,
										  int index[3], float st[6], int *texture,
										  unsigned short *transparency);
	/*------------------------------------------------------
	 * Pre  : This method interface may change later
	 *
	 *        Pass allocated index[3], st[6], texture,
	 *        and transparency
	 *
	 * Post : This method is made to let you fill arrays or
	 *        create single faces, their may be an allocating
	 *        version that passes back arrays later
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.16:
	 * Mongoose - Created, based on method from OpenRaider
	 ------------------------------------------------------*/

	int getMeshTexturedTriangleCount(unsigned int meshIndex);
	int getMeshColoredTriangleCount(unsigned int meshIndex);
	int getMeshTexturedRectangleCount(unsigned int meshIndex);
	int getMeshColoredRectangleCount(unsigned int meshIndex);
	/*------------------------------------------------------
	 * Pre  : This method interface may change later
	 * Post : Get face counts for a given mesh
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.16:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getMeshVertexArrays(unsigned int meshIndex,
									 unsigned int *vertexCount, float **verts,
									 unsigned int *normalCount, float **norms,
									 unsigned int *colorCount, float **colors);
	/*------------------------------------------------------
	 * Pre  : Given mesh index
	 *
	 * Post : Returns allocated vertex, normal, and color arrays,
	 *        and their element counts,
	 *
	 *        Colors has been changed to be an intesity for now
	 *
	 *        verts - XYX, norms - IJK, colors - I
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.13:
	 * Mongoose - Created, based on method from OpenRaider
	 ------------------------------------------------------*/

	int getRoomBox(unsigned int roomIndex, unsigned int index,
						float *xyzA, float *xyzB, float *xyzC, float *xyzD);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns a single collision box from room ( unified )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned int getRoomBoxCount(unsigned int roomIndex);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns number of collision boxes in room ( unified )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getRoomInfo(unsigned int index,
						  unsigned int *flags, float pos[3],
						  float bboxMin[3], float bboxMax[3]);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int getRoomLight(unsigned int roomIndex, unsigned int index,
						  float pos[4], float color[4], float dir[3],
						  float *attenuation, float *cutoffAngle,
						  unsigned int *type, unsigned int *flags);
	/*------------------------------------------------------
	 * Pre  : All parms are allocated
	 * Post : Returns a single light from a room ( unified )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned int getRoomLightCount(unsigned int roomIndex);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns number of lights in room ( unified )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int getRoomModel(unsigned int roomIndex, unsigned int index,
						  int *modelIndex, float pos[3], float *yaw);
	/*------------------------------------------------------
	 * Pre  : All parms are allocated
	 * Post : Returns a single model info from a room ( unified )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned int getRoomModelCount(unsigned int roomIndex);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns number of room models in room ( unified )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int getRoomPortal(unsigned int roomIndex, unsigned int index,
							int *adjoiningRoom, float normal[3], float vertices[12]);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns a single portal from room ( unified )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned int getRoomPortalCount(unsigned int roomIndex);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns number of portals from room ( unified )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getRoomRectangle(unsigned int roomIndex, unsigned int rectangleIndex,
								 unsigned int *indices, float *texCoords, int *texture,
								 unsigned int *flags);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Gets rectangle data with texCoords for non-matching
	 *        vertex/uv for each vertex in TombRaider room ( unified )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned int getRoomRectangleCount(unsigned int roomIndex);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns number of rectangles from room ( unified )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int getRoomSector(unsigned int roomIndex, unsigned int index,
							unsigned int *flags, float *ceiling, float *floor,
							int *floorDataIndex, int *boxIndex,
							int *roomBelow, int *roomAbove);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns a single sector from room ( unified )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned int getRoomSectorCount(unsigned int roomIndex,
											  unsigned int *zSectorsCount,
											  unsigned int *xSectorsCount);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns number of sectors in room ( unified )
	 *        zSectorsCount - width of sector list
	 *        xSectorsCount - height of sector list
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getRoomSprite(unsigned int roomIndex, unsigned int index,
							 float scale, int *texture,
							 float *pos, float *vertices, float *texcoords);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned int getRoomSpriteCount(unsigned int roomIndex);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns number of sprites in room ( unified )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getRoomTriangle(unsigned int roomIndex, unsigned int triangleIndex,
								unsigned int *indices, float *texCoords, int *texture,
								unsigned int *flags);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Gets triangle data with texCoords for non-matching
	 *        vertex/uv for each vertex in TombRaider room ( unified )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getRoomTriangles(unsigned int index, int textureOffset,
								 unsigned int *count, unsigned int **indices,
								 float **texCoords, int **textures,
								 unsigned int **flags);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Gets triangle data with texCoords for non-matching
	 *        vertex/uv for each vertex in TombRaider room
	 *
	 *        This is used with room vertices, colors, etc
	 *        to do partial array rendering, since the texcoords
	 *        will never match vertices ( Tomb Raider is textile based )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getRoomTriangles(unsigned int roomIndex, int textureOffset,
								 unsigned int *count,
								 unsigned int **indices, float **vertices,
								 float **texCoords, float **colors,
								 int **textures, unsigned int **flags);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Gets triangle data with duplicated
	 *        vertex/color/normal data for each face vertex
	 *        to match the textile based texcoords
	 *
	 *        This uses more memory, but lets you do direct
	 *        array rendering with OpenGL, D3D, etc
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned int getRoomTriangleCount(unsigned int roomIndex);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns number of triangles from room ( unified )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getRoomVertex(unsigned int roomIndex, unsigned int vertexIndex,
							 float *xyz, float *rgba);
	/*------------------------------------------------------
	 * Pre  : Given room index and vertex index and
	 *         xyz is allocated float[3]
	 *         rbga is allocated float[4]
	 *
	 * Post : Returns vertex position xyz and color rbga
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.13:
	 * Mongoose - Created, based on method from OpenRaider
	 ------------------------------------------------------*/

	void getRoomVertexArrays(unsigned int roomIndex,
									 unsigned int *vertexCount, float **vertices,
									 unsigned int *normalCount, float **normals,
									 unsigned int *colorCount, float **colors);
	/*------------------------------------------------------
	 * Pre  : Given room index
	 *
	 * Post : Returns allocated vertex and color arrays,
	 *        and their element counts  ( unified )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Unified API
	 *
	 * 2003.05.13:
	 * Mongoose - Created, based on method from OpenRaider
	 ------------------------------------------------------*/

	unsigned int getRoomVertexCount(unsigned int roomIndex);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns number of lights in room ( unified )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int getSkyModelId();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns moveable id of sky mesh or -1 if none
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.28:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getSprites();

	void getSoundSample(unsigned int index,
							  unsigned int *bytes, unsigned char **data);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns a copy of a sound sample and
	 *        its byte size by it's  sound sample index
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.10:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned int getSoundSamplesCount();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns number of sound samples loaded
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.10:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	bool isMeshValid(int index);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns true if mesh is valid
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.16:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	bool isRoomValid(int index);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns true if room is valid ( TRC support )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	int loadSFX(char *filename);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Loads an external sound pak for TR2 and TR3
	 *        games.  Returns < 0 if error.
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.12:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void reset();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.13:
	 * Mongoose - New API, was Clear()
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setDebug(bool toggle);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setRoomVertexLightingFactor(float f);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Sets lighting factor for each vertex color per
	 *        room in TR3 paks
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setTexelScalingFactor(float f);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Sets scaling for sprite texel alignment, etc
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.22:
	 * Mongoose - Created
	 ------------------------------------------------------*/


 private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////

	void extractMeshes(unsigned char *mesh_data,
							 unsigned int num_mesh_pointers,
							 unsigned int *mesh_pointers);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int Fread(void *buffer, size_t size, size_t count, FILE *f);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getRiffData(unsigned int *bytes, unsigned char **data);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns a copy of the sound samples and
	 *        their size
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.10:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void getRiffDataTR4(unsigned int index,
							  unsigned int *bytes, unsigned char **data);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns a copy of a TR4 sound sample
	 *        and its size by it's sound sample index
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.10:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int getRiffOffsets(unsigned char *riffData, unsigned int riffDataBytes,
							 unsigned int **offsets, unsigned int numOffsets);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns an array of offsets for a contigous
	 *        RIFF data stream in chunks.
	 *
	 *        Offsets will be allocated with enough space
	 *        to hold expected number of offsets.
	 *        ( Should be known number, otherwise not
	 *          all RIFFs will be parsed. )
	 *
	 *        Returns number of RIFFs found
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.12:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned char *getTexTile(int texture);
	/*------------------------------------------------------
	 * Pre  : texture is valid index into textile list
	 *
	 * Post : Makes a 32bit RGBA image from a textile
	 *        and returns it.  Returns NULL on error.
	 *
	 *        This handles all selection and conversion
	 *        including alpha layering flags now.
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.28:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int loadTR5(FILE *f, void (*percent)(int));
	/*------------------------------------------------------
	 * Pre  : f is a valid FILE and percent is allocated
	 * Post : Loads a TR5 pak into memory
	 *
	 *        At 10% all textures are loaded
	 *        the exact figure '10' will always be passed to
	 *        allow for texture caching while meshes load
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.01.02:
	 * Mongoose - New callback for percent loaded feedback
	 *
	 * 2001.06.18:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void print(const char *methodName, const char *s, ...);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.13:
	 * Mongoose - New API clean up
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void printDebug(const char *methodName, const char *s, ...);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.13:
	 * Mongoose - New API clean up
	 *
	 * 2000.05.13:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////


	bool mReset;                        /* Guard multiple calls to reset() */

	bool mDebug;                        /* Debug output toggle */

	unsigned int mPakVersion;           /* TombRaider pak file header version */

	tr2_version_type mEngineVersion;    /* TombRaider engine version  */

	tr2_colour_t _palette8[256];        /* 8-bit palette */

	unsigned int _palette16[256];       /* 16-bit palette */

	unsigned int _num_textiles;         /* Total number of texture tiles */

	unsigned short	_num_room_textures;     /* Num textures only for room use? */

	unsigned short _num_misc_textures;     /* Num of textures for misc use? */

	unsigned short _num_bump_map_textures; /* Num of textures that are
														   bump map, texture pairs */

	tr2_textile8_t *_textile8;          /* 8-bit (palettised) textiles */

	tr2_textile16_t *_textile16;        /* 16-bit (ARGB) textiles */

	tr2_textile32_t *_textile32;        /* 32-bit (BGRA) textiles */

	unsigned int _num_tex_special;     /* Special textures and bump maps count */

	unsigned char *_tex_special;       /* Special textures and bump maps */

	unsigned int _unknown_t;            /* 32-bit unknown
														(always 0 in real TR2 levels) */

	unsigned short _num_rooms;          /* Number of rooms in this level */

	tr2_room_t *_rooms;                 /* List of rooms (TR1,TR2,TR3,TR4) */

	tr5_room_t *mRoomsTR5;              /* Rooms ( TR5 / TRC ) Only */

	unsigned int _num_floor_data;       /* Num of words of floor data
														this level */

	unsigned short *_floor_data;        /* Floor data */

	int mMeshCount;                     /* Number of meshes this level */

	tr2_mesh_t *mMeshes;                /* list of meshes */

	unsigned int _num_animations;       /* number of animations this level */

	tr2_animation_t *_animations;       /* list of animations */

	unsigned int _num_state_changes;    /* number of structures(?) this level */

	tr2_state_change_t *_state_changes; /* list of structures */

	unsigned int _num_anim_dispatches;  /* number of ranges(?) this level */

  tr2_anim_dispatch_t *_anim_dispatches; /* list of ranges */

	unsigned int _num_anim_commands;       /* number of Bone1s this level */

	tr2_anim_command_t *_anim_commands;    /* list of Bone1s */

	unsigned int _num_mesh_trees;          /* number of Bone2s this level */

	tr2_meshtree_t *_mesh_trees;      /* list of Bone2s */

	unsigned int _num_frames;         /* num of words of frame data this level */

	unsigned short *_frames;          /* frame data */

	unsigned int _num_moveables;      /* number of moveables this level */

	tr2_moveable_t *_moveables;       /* list of moveables */

	u_int32_t numMoveablesTR5;
	tr5_moveable_t *moveablesTR5;

	u_int32_t numAnimationsTR5;
	tr5_animation_t *animationsTR5;

	u_int32_t numObjectTexturesTR5;
	tr5_object_texture_t *objectTexturesTR5;

	u_int32_t numCinematicFramesTR5;
	tr5_cinematic_frame_t *cinematicFramesTR5;

	u_int32_t numFlyByCamerasTR5;
	tr5_flyby_camera_t *flyByCamerasTR5;

	unsigned int _num_static_meshes;  /* number of static meshes this level */

	tr2_staticmesh_t *_static_meshes; /* static meshes */

	unsigned int _num_object_textures;   /* number of object textures this level */

	tr2_object_texture_t *_object_textures; /* list of object textures */


	unsigned int _num_sprite_textures;      /* num of sprite textures this level */

	tr2_sprite_texture_t *_sprite_textures; /* list of sprite textures */

	unsigned int _num_sprite_sequences;     /* num of sprite sequences this level */

	tr2_sprite_sequence_t *_sprite_sequences;  /* sprite sequence data */

	int  _num_cameras;                         /* Number of Cameras */

	tr2_camera_t  *_cameras;                   /* cameras */

	int  _num_sound_sources;                   /* Number of Sounds */

	tr2_sound_source_t *_sound_sources;          /* sounds */

	int  _num_boxes;                           /* Number of Boxes */


	tr2_box_t *_boxes;                    /* boxes - looks like
														  struct { unsigned short value[4]; }
														  - value[0..2] might be a vector;
														  value[3] seems to be index into
														  Overlaps[] */

	int  _num_overlaps;                       /* Number of Overlaps */

	short  *_overlaps;                        /* Overlaps -
																looks like ushort; 0x8000 is flag
																of some sort appears to be an
																offset into Boxes[] and/or
																Boxes2[] */

	short  *_zones;                           /* Boxes2 */

	int  _num_animated_textures;              /* Number of AnimTextures */

	short  *_animated_textures;               /* Animtextures */

	int  _num_items;                          /* Number of Items */

	tr2_item_t *_items;                       /* Items */

	unsigned char  *_light_map;               /* Colour-light maps */

	unsigned int _num_cinematic_frames;       /* Number of cut-scene frames */

	tr2_cinematic_frame_t *_cinematic_frames; /* Cut-scene frames */

	short  _num_demo_data;                    /* Number of Demo Data */

	unsigned char  *_demo_data;               /* Demo data */

	float mRoomVertexLightingFactor;

	float mTexelScale;


	// Sound data

	short  *mSoundMap;                       /* Sound map */

	int  mNumSoundDetails;                   /* Number of SampleModifiers */

	tr2_sound_details_t *mSoundDetails;      /* Sample modifiers */

	int  mNumSampleIndices;                  /* Number of Sample Indices */

	int  *mSampleIndices;                    /* Sample indices */

	unsigned int *mSampleIndicesTR5;

	bool mRiffAlternateLoaded;               /* Is a TR2,TR3 SFX loaded? */

	unsigned int *mRiffAlternateOffsets;     /* After parsing this will
															  hold byte offsets for TR2,TR3
														     RIFFs in the buffered SFX */

	int mRiffDataSz;                         /* Byte size of a loaded SFX */

	unsigned char *mRiffData;                /* SFX RIFF data in chunks */

	unsigned int mNumTR4Samples;

	unsigned char **mTR4Samples;

	unsigned int *mTR4SamplesSz;


	// For packed Fread emu/wrapper

	unsigned char *mCompressedLevelData;     /* Buffer used to emulate fread
															  with uncompressed libz data */

	unsigned int mCompressedLevelDataOffset; /* Offset into buffer */

	unsigned int mCompressedLevelSize;       /* Size of buffer */

	tr_fread_mode_t mFreadMode;              /* Fread mode file|buffer */
};

#endif

