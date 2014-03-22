/*!
 * \file include/TombRaider.h
 * \brief Loads maps, meshes, textures...
 *
 * \todo WARNING: No endian routines as of yet
 * \author Mongoose
 */

#ifndef _TOMBRAIDER_H_
#define _TOMBRAIDER_H_

#include <cstdint>

#ifdef WIN32
// Have to remove GCC packing, by removing in preprocessor
#define __attribute__(packed)

// MSVC++ pragma to pack structs
#define TR2_H 1
#pragma pack(push, tr2_h, 1)
#endif

//#include "games/TombRaider1.h"

typedef enum {
    TR_VERSION_UNKNOWN,
    TR_VERSION_1,
    TR_VERSION_2,
    TR_VERSION_3,
    TR_VERSION_4,
    TR_VERSION_5
} tr2_version_type;

typedef enum {
    TR_FREAD_NORMAL = 0,
    TR_FREAD_COMPRESSED
} tr_fread_mode_t;

typedef enum {
    tombraiderLight_typeDirectional = 1,
    tombraiderLight_typeSpot        = 2,
    tombraiderLight_typePoint       = 3
} tombraiderLightType;

typedef enum {
    tombraiderLight_useCutoff      = 1,
    tombraiderLight_useAttenuation = 2
} tombraiderLightFlags;

typedef enum {
    tombraiderRoom_underWater = 1
} tombraiderRoomFlags;

typedef enum {
    tombraiderSector_wall = 1
} tombraiderSectorFlags;

typedef enum {
    tombraiderFace_Alpha        = (1 << 0),
    tombraiderFace_Colored      = (1 << 1),
    tombraiderFace_PartialAlpha = (1 << 2)
} tombraiderFace_Flags;

#define TR_SOUND_FOOTSTEP0    1
#define TR_SOUND_F_PISTOL    12

enum TR4_Objects {
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

/*!
 * \brief Basic 24-bit colour structure.
 *
 * It appears that only 6 bits per colour are actually
 * used, making it 18-bit colour.
 */
typedef struct {
    unsigned char r; //!< Red part
    unsigned char g; //!< Green part
    unsigned char b; //!< Blue part
} __attribute__ ((packed)) tr2_colour_t;

/*!
 * \brief Basic vertex structure.
 *
 * Note that all vertices are relative coordinates;
 * each mesh (room, object, etc.) has its own offset by
 * which the vertex coordinates are translated.
 */
typedef struct {
    short x;
    short y;
    short z;
} __attribute__ ((packed)) tr2_vertex_t;

/*!
 * \brief A rectangular (quad) face definition.
 *
 * Four vertices (the values are indices into the
 * appropriate vertex list) and a texture (an index
 * into the texture list) or colour (index into 8-bit palette).
 *
 * I've seen a few coloured polygons where Texture is
 * greater than 255, but ANDing the value with 0xFF
 * seems to produce acceptable results.
 */
typedef struct {
    unsigned short vertices[4];
    unsigned short texture;
} __attribute__ ((packed)) tr2_quad_t; // was tr2_face4

typedef struct {
    unsigned short vertices[4]; //!< The 4 vertices of a quad
    unsigned short texture;     //!< Object-texture index
    unsigned short lighting;    //!< Transparency flag & strength of the highlight
    /*!<
     * Bit 0: if set, alpha channel = intensity (same meaning that when the
     * Attribute field of tr2_object_texture is 2. Cf TRosetta stone document)
     *
     * Bit 1-7: strength of the highlight. In TR4, objects can exhibit some kind
     * of light reflection when seen from some particular angles. These bits give
     * the strength of this effect:
     * the more bigger the value is, the more visible is the effect.
     */
} __attribute__ ((packed)) tr4_quad_t;

/*!
 * \brief A triangular face definition.
 *
 * Three vertices (the values are indices into the
 * appropriate vertex list) and a texture (an index into the
 * texture list) or colour (index into palette).
 *
 * In the case of a colour, (Texture & 0xff) is the index
 * into the 8-bit palette, while (Texture >> 8) is
 * the index into the 16-bit palette.
 */
typedef struct {
    unsigned short vertices[3];
    unsigned short texture;
} __attribute__ ((packed)) tr2_tri_t; // was tr2_face3

typedef struct {
    unsigned short vertices[3]; //!< The 3 vertices of a tri
    unsigned short texture;     //!< Object-texture index
    unsigned short lighting;    //!< Transparency flag & strength of the highlight
} __attribute__ ((packed)) tr4_tri_t;

/*!
 * \brief An 8-bit texture tile (65536 bytes).
 *
 * Each byte represents a pixel whose colour
 * is in the 8-bit palette.
 */
typedef struct {
    unsigned char tile[256 * 256];
} __attribute__ ((packed)) tr2_textile8_t;

/*!
 * \brief A 16-bit texture tile (131072 bytes).
 *
 * Each word represents a pixel
 * whose colour is of the form ARGB, MSB-to-LSB:
 *
 *  * 1-bit transparency (0: transparent, 1: opaque)
 *  * 5-bit red channel
 *  * 5-bit green channel
 *  * 5-bit blue channel
 */
typedef struct {
    unsigned short tile[256 * 256];
} __attribute__ ((packed)) tr2_textile16_t;

/*!
 * \brief A 32-bit texture tile (262144 bytes).
 *
 * BGRA with 4bytes each channel.
 */
typedef struct {
    unsigned int tile[256 * 256];
} __attribute__ ((packed)) tr2_textile32_t;

/*!
 * \brief The "header" of a room.
 *
 * X/Z indicate the base position of the room mesh in world coordinates.
 *
 * yLowest and yHighest indicate the lowest and highest points in this room
 * (even though their actual values appear to be reversed, since a "high"
 * point will have a smaller value than a "low" point).
 *
 * When positioning objects/items, as well as the room meshes
 * themselves, y is always 0-relative (not room-relative).
 */
typedef struct {
    int x;        //!< X-offset of room (world coordinates)
    int z;        //!< Z-offset of room (world coordinates)
    int y_bottom; //!< Y-offset of lowest point in room (world coordinates, actually highest value)
    int y_top;    //!< Y-offset of highest point in room (world coordinates, actually lowest value)
} __attribute__ ((packed)) tr2_room_info_t;

/*!
 * \brief Portal structure.
 *
 * This defines every viable exit from a given "room".
 *
 * Note that "rooms" are really just areas.  They aren't
 * necessarily enclosed.  The door structure below defines
 * areas of egress, not the actual moveable mesh,
 * texture, and action (if any).
 */
typedef struct {
    unsigned short adjoining_room; //!< Which room this "door" leads to
    tr2_vertex_t normal;           //!< Which way the "door" faces
    tr2_vertex_t vertices[4];      //!< The corners of the "door"
} __attribute__ ((packed)) tr2_room_portal_t;

/*!
 * \brief Room sector structure.
 *
 * Sectors are 1024 * 1024 (world coordinates). Floor and
 * Ceiling are signed number of 256 units of height
 * (relative to 0), e.g. Floor 0x04 corresponds to
 * Y = 1024 in world coordinates.
 *
 * Note: this implies that, while X and Z can be quite large,
 * Y is constrained to -32768..32512. Floor/Ceiling value of
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
 */
typedef struct {
    unsigned short fd_index;   //!< Index into FloorData[]
    unsigned short box_index;  //!< Index into Boxes[]/Zones[] (-1 if none)
    unsigned char room_below;  //!< The number of the room below this one (-1 or 255 if none)
    char floor;                //!< Absolute height of floor (Multiply by 256 for world coordinates)
    unsigned char  room_above; //!< The number of the room above this one (-1 or 255 if none)
    char ceiling;              //!< Absolute height of ceiling (multiply by 256 for world coordinates)
} __attribute__ ((packed)) tr2_room_sector_t;

/*!
 * \brief Room lighting structure.
 *
 * X/Y/Z are in world coordinates.
 *
 * Lighting values seem to range from 0..8192.
 */
typedef struct {
    int x;
    int y;
    int z;
    unsigned short intensity1;
    unsigned short intensity2;
    unsigned int fade1;
    unsigned int fade2;
} __attribute__ ((packed)) tr2_room_light_t;

typedef struct {
    int xPosition;           //!< World coords
    int yPosition;           //!< World coords
    int zPosition;           //!< World coords
    tr2_colour_t color;      //!< Three bytes rgb values
    unsigned char lightType; //!< Same as D3D (i.e. 2 is for spotlight)
    unsigned char unknown;   //!< Always 0xff?
    unsigned char intensity;
    float in;
    float out;
    float length;
    float cutoff;
    float xDir, yDir, zDir;  //!< Direction?
} __attribute__ ((packed)) tr4_room_light_t;

/*!
 * \brief Room vertex structure.
 *
 * This defines the vertices within a room.
 */
typedef struct {
    tr2_vertex_t vertex;
    short lighting1;           //!< Values range from 0 to 32767 in TR3, 0=dark.
                               /*!< I think the values ranged from 0 to 8192
                                * in TR1/2, 0=bright.  */
    unsigned short attributes; /*!<
                                * * 0x8000 Something to do with water surface
                                * * 0x4000 Under water lighting modulation
                                *          and movement if viewed from
                                *          above water surface
                                * * 0x2000 Water/quicksand surface movement
                                * * 0x1fef Nothing?
                                * * 0x0010 Everything?
                                */

    short lighting2;           //!< Seems to be the same as lighting1
} __attribute__ ((packed)) tr2_vertex_room_t;

/*!
 * \brief Sprite structure
 */
typedef struct {
    short vertex;  //!< Offset into vertex list
    short texture; //!< Offset into texture list
} __attribute__ ((packed)) tr2_room_sprite_t;

/*!
 * \brief Room mesh structure.
 *
 * This is the geometry of the "room," including
 * walls, floors, rocks, water, etc. It does _not_ include
 * objects that Lara can interact with (keyboxes,
 * moveable blocks, moveable doors, etc.)
 */
typedef struct tr2_room_data_s {
    short num_vertices;          //!< Number of vertices in the following list
    tr2_vertex_room_t *vertices; //!< List of vertices (relative coordinates)
    short num_rectangles;        //!< Number of textured rectangles
    tr2_quad_t *rectangles;      //!< List of textured rectangles
    short num_triangles;         //!< Number of textured triangles
    tr2_tri_t *triangles;        //!< List of textured triangles
    short num_sprites;           //!< Number of sprites
    tr2_room_sprite_t *sprites;  //!< List of sprites
} __attribute__ ((packed)) tr2_room_data_t;

/*!
 * \brief Room static mesh data.
 *
 * Positions and IDs of static meshes
 * (e.g. skeletons, spiderwebs, furniture)
 */
typedef struct {
    int  x;                    //!< Absolute position in world coordinates
    int  y;
    int  z;
    unsigned short rotation;   //!< High two bits (0xc000) indicate steps of 90 degrees
    unsigned short intensity1;
    unsigned short intensity2;
    unsigned short object_id;  //!< Which StaticMesh item to draw
} __attribute__ ((packed)) tr2_room_staticmesh_t;


/*!
 * \brief Room structure.
 *
 * Here's where all the room data comes together.
 */
typedef struct {
    tr2_room_info_t info;                 //!< where the room exists, in world coordinates
    unsigned int num_data_words;          //!< number of data words (bitu16)
    unsigned char *data;                  //!< the raw data from which the rest of this is derived
    tr2_room_data_t room_data;            //!< the room mesh
    unsigned short num_portals;           //!< number of visibility portals that leave this room
    tr2_room_portal_t *portals;           //!< list of visibility portals
    unsigned short num_zsectors;          //!< width of sector list
    unsigned short num_xsectors;          //!< height of sector list
    tr2_room_sector_t *sector_list;       //!< list of sectors in this room
    short intensity1;
    short intensity2;
    short light_mode;
    unsigned short num_lights;            //!< number of lights in this room
    tr2_room_light_t *lights;             //!< list of lights
    unsigned short num_static_meshes;     //!< number of static meshes
    tr2_room_staticmesh_t *static_meshes; //!< static meshes
    short  alternate_room;
    short  flags;                         /*!< * 0x0001 - room is filled with water
                                           *   * 0x0020 - Lara's ponytail gets blown by the wind */
    tr2_colour_t room_light_colour;       //!< TR3 ONLY!
    tr4_room_light_t *tr4Lights;          //!< TR4 ONLY!
} __attribute__ ((packed)) tr2_room_t;

/*!
 * \brief Animation structure up to TR3.
 */
typedef struct {
    unsigned int frame_offset;          //!< byte offset into Frames[] (divide by 2 for Frames[i])
    unsigned char frame_rate;           //!< "ticks" per frame
    unsigned char frame_size;           //!< number of words in Frames[] used by this animation
    short state_id;
    short unknown1;
    short unknown2;
    short unknown3;
    short unknown4;
    unsigned short frame_start;         //!< first frame in this animation
    unsigned short frame_end;           //!< last frame in this animation (numframes = (End - Start) + 1)
    unsigned short next_animation;
    unsigned short next_frame;
    unsigned short num_state_changes;
    unsigned short state_change_offset; //!< offset into StateChanges[]
    unsigned short num_anim_commands;
    unsigned short anim_command;        //!< offset into AnimCommands[]
} __attribute__ ((packed)) tr2_animation_t;

/*!
 * \brief Data for an animation structure (40 bytes in TR4 vice 32 in TR1/2/3)
 */
typedef struct {
    unsigned int frame_offset;          //!< same meaning as in TR3
    unsigned char frame_rate;           //!< same meaning as in TR3
    unsigned char frame_size;           //!< same meaning as in TR3
    unsigned short state_id;            //!< same meaning as in TR3
    short unknown;                      //!< same meaning as in TR3
    short speed;                        //!< same meaning as in TR3
    unsigned short accel_lo;            //!< same meaning as in TR3
    short accel_hi;                     //!< same meaning as in TR3
    unsigned char unknown2[8];          //!< new in TR4
    unsigned short frame_start;         //!< same meaning as in TR3
    unsigned short frame_end;           //!< same meaning as in TR3
    unsigned short next_animation;      //!< same meaning as in TR3
    unsigned short next_frame;          //!< same meaning as in TR3
    unsigned short num_state_changes;   //!< same meaning as in TR3
    unsigned short state_change_offset; //!< same meaning as in TR3
    unsigned short num_anim_commands;   //!< same meaning as in TR3
    unsigned short anim_command;        //!< same meaning as in TR3
} __attribute__ ((packed)) tr4_animation_t;

/*!
 * \brief State Change structure
 */
typedef struct {
    unsigned short state_id;
    unsigned short num_anim_dispatches; //!< Number of dispatches (seems to always be 1..5)
    unsigned short anim_dispatch;       //!< Offset into AnimDispatches[]
} __attribute__ ((packed)) tr2_state_change_t;

/*!
 * \brief Animation Dispatch structure
 */
typedef struct {
    short low;
    short high;
    short next_animation;
    short next_frame;
} __attribute__ ((packed)) tr2_anim_dispatch_t;

/*!
 * \brief AnimCommand structure
 */
typedef struct {
    short value;
} __attribute__ ((packed)) tr2_anim_command_t;

/*!
 * \brief MeshTree structure.
 *
 * MeshTree[] is actually groups of four ints.  The first one is a
 * "flags" word;  bit 1 (0x0002) indicates "make previous mesh an
 * anchor (e.g. PUSH)";  bit 0 (0x0001) indicates "return to previous
 * anchor (e.g. POP)".
 * The next three ints are X, Y, Z offsets from the last mesh position.
 */
typedef struct {
    int flags; //!< 0x0001 = POP, 0x0002 = PUSH
    int x;
    int y;
    int z;
} __attribute__ ((packed)) tr2_meshtree_t;

/*!
 * \brief Moveable structure.
 *
 * This defines a list of contiguous meshes that
 * comprise one object, e.g. in WALL.TR2,
 *
 *     moveable[0]  = Lara (StartingMesh 0, NumMeshes 15),
 *     moveable[13] = Tiger (StartingMesh 215, NumMeshes 27)
 *     moveable[15] = Spikes (StartingMesh 249, NumMeshes 1)
 *     moveable[16] = Boulder (StartingMesh 250, NumMeshes 1)
 *     moveable[20] = Rolling Blade (StartingMesh 254, NumMeshes 1)
 */
typedef struct {
    unsigned int object_id;       //!< Item Identifier
    unsigned short num_meshes;    //!< number of meshes in this object
    unsigned short starting_mesh; //!< first mesh
    unsigned int mesh_tree;       //!< offset into MeshTree[]
    unsigned int frame_offset;    //!< byte offset into Frames[] (divide by 2 for Frames[i])
    unsigned short animation;     //!< offset into Animations[]
} __attribute__ ((packed)) tr2_moveable_t;

/*!
 * \brief StaticMesh structure.
 *
 * This defines meshes that don't move (e.g. skeletons
 * lying on the floor, spiderwebs, etc.)
 */
typedef struct {
    unsigned int object_id;          //!< Item Identifier
    unsigned short starting_mesh;    //!< first mesh
    tr2_vertex_t bounding_box[2][2];
    unsigned short flags;
} __attribute__ ((packed)) tr2_staticmesh_t;

/*!
 * \brief Object texture vertex structure.
 *
 * Maps coordinates into texture tiles.
 */
typedef struct {
    unsigned char xcoordinate;
    unsigned char xpixel;
    unsigned char ycoordinate;
    unsigned char ypixel;
} __attribute__ ((packed)) tr2_object_texture_vert_t;

/*!
 * \brief Object texture structure.
 */
typedef struct {
    unsigned short transparency_flags;     /*!< * 0: Opaque
                                            *   * 1: Use transparency
                                            *   * 2: Use partial transparency
                                            *        [grayscale intensity :: transparency]
                                            */
    unsigned short tile;                   //!< index into textile list
    tr2_object_texture_vert_t vertices[4]; //!< the four corners of the texture
} __attribute__ ((packed)) tr2_object_texture_t;

/*!
 * \brief Sprite texture structure.
 */
typedef struct {
    unsigned short tile;
    unsigned char x;
    unsigned char y;
    unsigned short width;  //!< Actually, (width * 256) + 255
    unsigned short height; //!< Actually, (height * 256) + 255
    short left_side;
    short top_side;
    short right_side;
    short bottom_side;
} __attribute__ ((packed)) tr2_sprite_texture_t;

/*!
 * \brief Sprite Sequence structure
 */
typedef struct {
    int object_id;         //!< Item identifier (same numbering as in tr2_moveable)
    short negative_length; //!< Negative of "how many sprites are in this sequence"
    short offset;          //!< Where (in sprite texture list) this sequence starts
} __attribute__ ((packed)) tr2_sprite_sequence_t;

/*!
 * \brief Mesh structure.
 *
 * The mesh list contains the mesh info for Lara (in all her
 * various incarnations), blocks, enemies (tigers, birds, bad guys),
 * moveable blocks, zip line handles, boulders, spinning blades,
 * you name it.
 *
 * If NumNormals is negative, Normals[] represent vertex
 * lighting values (one per vertex).
 */
typedef struct {
    tr2_vertex_t centre;             /*!< \brief This seems to describe the
                                      * approximate geometric centre
                                      * of the mesh (possibly the centre of gravity?)
                                      * (relative coordinates, just like the vertices) */
    int collision_size;              //!< radius of collisional sphere
    short num_vertices;              //!< number of vertices in this mesh
    tr2_vertex_t *vertices;          //!< list of vertices (relative coordinates)
    short num_normals;               //!< number of normals in this mesh (should always equal NumVertices)
    tr2_vertex_t *normals;           //!< list of normals (NULL if NumNormals < 0)
    short *mesh_lights;              //!< if NumNormals < 0
    short num_textured_rectangles;   //!< number of textured rectangles
    tr2_quad_t *textured_rectangles; //!< list of textured rectangles
    short num_textured_triangles;    //!< number of textured triangles in this mesh
    tr2_tri_t *textured_triangles;   //!< list of textured triangles
    short num_coloured_rectangles;   //!< number of coloured rectangles
    tr2_quad_t *coloured_rectangles; //!< list of coloured rectangles
    short num_coloured_triangles;    //!< number of coloured triangles in this mesh
    tr2_tri_t *coloured_triangles;   //!< list of coloured triangles
} __attribute__ ((packed)) tr2_mesh_t;

/*!
 * \brief Frame structure.
 *
 * Frames indicates how composite meshes are positioned and rotated.
 * They work in conjunction with Animations[] and Bone2[].
 *
 * A given frame has the following format:
 *
 *     short BB1x, BB1y, BB1z           // bounding box (low)
 *     short BB2x, BB2y, BB2z           // bounding box (high)
 *     short OffsetX, OffsetY, OffsetZ  // starting offset for this moveable
 *     (TR1 ONLY: short NumValues       // number of angle sets to follow)
 *     (TR2/3: NumValues is implicitly NumMeshes (from moveable))
 *
 * What follows next is a list of angle sets.  In TR2/3, an angle set can
 * specify either one or three axes of rotation.  If either of the high two
 * bits (0xc000) of the first angle unsigned short are set, it's one axis:
 *
 *     only one  unsigned short,
 *     low 10 bits (0x03ff),
 *     scale is 0x100 == 90 degrees;
 *
 * the high two  bits are interpreted as follows:
 *
 *     0x4000 == X only, 0x8000 == Y only,
 *     0xC000 == Z only.
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
 */
typedef struct {
    tr2_vertex_t vector[3];
    int num_words;
    unsigned short *words;
} __attribute__ ((packed)) tr2_frame_t;

/*!
 * \brief Item structure
 */
typedef struct {
    short object_id;
    short room;
    int x;
    int y;
    int z;
    short angle;
    short intensity1;
    short intensity2;
    short flags;      //!< 0x0100 indicates "inactive" or "invisible"
} __attribute__ ((packed)) tr2_item_t;

/*!
 * \brief SoundSource structure
 */
typedef struct {
    int x;                   //!< position of sound source
    int y;
    int z;
    unsigned short sound_id; //!< internal sound index
    unsigned short flags;    //!< 0x40, 0x80, or 0xc0
} __attribute__ ((packed)) tr2_sound_source_t;

/*!
 * \brief Boxes structure
 */
typedef struct {
    unsigned char zmin;  //!< sectors (* 1024 units)
    unsigned char zmax;
    unsigned char xmin;
    unsigned char xmax;
    short true_floor;    //!< Y value (no scaling)
    short overlap_index; //!< index into Overlaps[]
} __attribute__ ((packed)) tr2_box_t;

/*!
 * \brief AnimatedTexture structure.
 *
 * - really should be simple short[], since it's variable length
 */
typedef struct {
    short num_texture_ids; //!< Number of Texture IDs - 1
    short *texture_list;   //!< list of textures to cycle through
} __attribute__ ((packed)) tr2_animated_texture_t;

/*!
 * \brief Camera structure
 */
typedef struct {
    int x;
    int y;
    int z;
    short room;
    unsigned short unknown1; //!< correlates to Boxes[]?
} __attribute__ ((packed)) tr2_camera_t;

/*
 * \brief Data for a flyby camera (40 bytes)
 */
typedef struct {
    int pos[6];                //!< Positions ? (x1,y1,z1,x2,y2,z2) roatations?
    unsigned char index[2];    //!< A pair of indices
    unsigned short unknown[5]; //!< ??
    int id;                    //!< Index of something
} __attribute__ ((packed)) tr4_extra_camera_t;

/*!
 * \brief Sound sample structure
 */
typedef struct {
    short sample;
    short volume;
    short sound_range;
    short flags;       /*!< \ brief bits 8-15: priority?, 2-7: number of sound samples
                        * in this group, bits 0-1: channel number */
} __attribute__ ((packed)) tr2_sound_details_t;

/*!
 * \brief Cutscene Camera structure
 */
typedef struct {
    short roty;     //!< Rotation about Y axis, +/-32767 ::= +/- 180 degrees
    short rotz;     //!< Rotation about Z axis, +/-32767 ::= +/- 180 degrees
    short rotz2;    //!< Rotation about Z axis (why two?), +/-32767 ::= +/- 180 degrees
    short posz;     //!< Z position of camera, relative to something
    short posy;     //!< Y position of camera, relative to something
    short posx;     //!< X position of camera, relative to something
    short unknown1;
    short rotx;     //!< Rotation about X axis, +/-32767 ::= +/- 180 degrees
} __attribute__ ((packed)) tr2_cinematic_frame_t;

/*!
 * \brief Data for a AI object (24 bytes).
 *
 * this field replaces the bitu16 NumCinematicFrames of TR1/2/3 levels
 *
 * Used to read TR4 AI data
 */
typedef struct {
    unsigned short int object_id; //!< the objectID from the AI object
                                  //!< (AI_FOLLOW is 402)
    unsigned short int room;
    int x, y, a;
    unsigned short int ocb;
    unsigned short int flags;     //!< The trigger flags
                                  //!< (button 1-5, first button has value 2)
    int angle;                    //!< rotation
} __attribute__ ((packed)) tr4_ai_object_t;

/*!
 * \brief Used to read packed TR4 texels
 */
typedef struct {
    unsigned short attribute;              //!< same meaning as in TR3
    unsigned short tile;                   //!< same meaning as in TR3
    unsigned short flags;                  //!< new in TR4
    tr2_object_texture_vert_t vertices[4]; //!< same meaning as in TR3
    unsigned int unknown1, unknown2;       //!< new in TR4: x & y offset in something
    unsigned int xsize, ysize;             //!< new in TR4: width-1 & height-1 of the object texture
} __attribute__ ((packed)) tr4_object_texture_t;

/*!
 * \brief TR5 Room Layer (56 bytes)
 */
typedef struct {
    uint32_t numLayerVertices;   //!< number of vertices in this layer (4 bytes)
    uint16_t unknownL1;          //!< unknown (2 bytes)
    uint16_t numLayerRectangles; //!< number of rectangles in this layer (2 bytes)
    uint16_t numLayerTriangles;  //!< number of triangles in this layer (2 bytes)
    uint16_t unknownL2;          //!< appears to be the number of 2 sided textures
                                  //!< in this layer, however is sometimes 1 off (2 bytes)
    uint16_t filler;             //!< always 0 (2 bytes)
    uint16_t filler2;            //!< always 0 (2 bytes)

    float layerBoundingBoxX1;     //!< These 6 floats (4 bytes each) define the bounding box for the layer
    float layerBoundingBoxY1;
    float layerBoundingBoxZ1;
    float layerBoundingBoxX2;
    float layerBoundingBoxY2;
    float layerBoundingBoxZ2;
    uint32_t filler3;            //!< always 0 (4 bytes)
    uint32_t unknownL6;          //!< unknown (4 bytes)
    uint32_t unknownL7;          //!< unknown (4 bytes)
    uint32_t unknownL8;          //!< unknown. Always the same throughout the level. (4 bytes)
} tr5_room_layer_t;

/*!
 * \brief TR5 Quad (12 bytes)
 */
typedef struct {
    uint16_t vertices[4]; //!< the values are the indices into the
                           //!< appropriate layer vertice list. (2 bytes each)
    uint16_t texture;     //!< the texture number for this face. Needs to be masked
                           //!< with 0xFFF as the high 4 bits are flags (2 bytes)
    uint16_t unknownF4;   //!< unknown (2 bytes)
} tr5_face4_t;

/*!
 * \brief TR5 triangular face (10 bytes)
 */
typedef struct {
    uint16_t vertices[3]; //!< the values are the indices into the
                           //!< appropriate layer vertice list (2 bytes each)
    uint16_t texture;     //!< the texture number for this face. Needs to be masked
                           //!< with 0xFFF as the high 4 bits are flags (2 bytes)
    uint16_t unknownF3;   //!< unknown (2 bytes)
} tr5_face3_t;

/*!
 * \brief TR5 Vertex (28 bytes)
 */
typedef struct {
    float x;          //!< x of vertex (4 bytes)
    float y;          //!< y of vertex (4 bytes)
    float z;          //!< z of vertex (4 bytes)
    float nx;         //!< x of vertex normal (4 bytes)
    float ny;         //!< y of vertex normal (4 bytes)
    float nz;         //!< z of vertex normal (4 bytes)
    uint32_t vColor; //!< vertex color ARGB format (4 bytes)
} tr5_vertex_t;

/*!
 * \brief This is to help store and manage TR5 layer based polgons for rooms
 */
typedef struct {
    tr5_face4_t *quads;
    tr5_face3_t *tris;
    tr5_vertex_t *verts;
} tr5_room_geometry_t;

/*!
 * \brief TR5 light (88 bytes)
 */
typedef struct {
    float x; //!< x position of light (4 bytes)
    float y; //!< y position of light (4 bytes)
    float z; //!< z position of light (4 bytes)
    /*!< Maybe wrong: The (x, y, z) floats specify the position of the light
     * in world coordinates.
     *
     * The sun type light should not use these but seems to have a
     * large x value (9 million, give or take)
     * a zero y value, and a small z value (4 - 20) in the original TR5 levels
     */

    float red;   //!< color of red spectrum (4 bytes)
    float green; //!< color of green spectrum (4 bytes)
    float blue;  //!< color of blue spectrum (4 bytes)

    uint32_t seperator; //!< not used 0xCDCDCDCD (4 bytes)

    float input;  //!< cosine of the IN value for light / size of IN value (4 bytes)
    float output; //!< cosine of the OUT value for light / size of OUT value (4 bytes)
    /*!< At this point the info diverges dependant
     * on which type of light being used:
     *
     *     0 = sun, 1 = light, 2 = spot, 3 = shadow
     *
     * The sun type doesn't use input and output.
     *
     * For the spot type these are the hotspot and falloff angle cosines.
     *
     * For the light and shadow types these are the TR units
     * for the hotspot/falloff (1024=1sector).
     */

    float radInput;  //!< (IN radians) * 2 (4 bytes)
    float radOutput; //!< (OUT radians) * 2 (4 bytes)
    //!< radInput and radOutput are only used by the spot type light

    float range; //!< Range of light (4 bytes), only used by the spot type light

    float directionVectorX; //!< light direction (4 bytes)
    float directionVectorY; //!< light direction (4 bytes)
    float directionVectorZ; //!< light direction (4 bytes)
    /*!< The 3 directionVector floats are used only by the 'sun' and 'spot' type lights.
     * They describe the directional vector of the light.
     * This can be obtained by:
     *
     *     if both x and y LightDirectionVectorX = cosX * sinY
     *
     *     LightDirectionVectorY = sinX
     *     LightDirectionVectorZ = cosX * cosY
     */

    int32_t x2; //!< x position of light (4 bytes)
    int32_t y2; //!< y position of light (4 bytes)
    int32_t z2; //!< z position of light (4 bytes)
    /*!<
     * x2, y2, z2 and the directionVectors-2 repeat some of the
     * previous information in long data types vice floats
     */
    int32_t directionVectorX2; //!< light direction (4 bytes)
    int32_t directionVectorY2; //!< light direction (4 bytes)
    int32_t directionVectorZ2; //!< light direction (4 bytes)
    //!< 16384 = 1.0 for the rotations (1/16384)

    uint8_t lightType; //!< type of light as specified above (1 byte)

    uint8_t seperator2[3]; //!< 0xCDCDCD (3 bytes)
} tr5_light_t;

/*!
 * \brief TR5 Room.
 *
 * First number is offset in bytes from start of room structure.
 */
typedef struct {
    uint32_t checkXELA;           //!< "XELA" (4 bytes)
    uint32_t roomDataSize;        /*!< size of the following data ( use this vice
                                    * 'walking thru' to get next room) (4 bytes) */
    uint32_t seperator;           //!< 0xCDCDCDCD (4 bytes)
    uint32_t endSDOffset;         /*!< usually this number +  216 will give you the
                                    * offset from the start of the room data to the end
                                    * of the Sector Data. HOWEVER have seen where this
                                    * bitu32 is equal to -1 so it is better to use the
                                    * following bitu32 and + 216 +
                                    * ((NumXSectors * NumZSectors)*8) if you need
                                    * to obtain this information. (4 bytes) */
    uint32_t startSDOffset;       /*!< this number + 216 will give you the offset from
                                    * the start of the room to the start of the
                                    * sector data. (4 bytes) */
    uint32_t seperator2;          //!< will either be 0x00000000 or 0xCDCDCDCD (4 bytes)
    uint32_t endPortalOffset;     /*!< this number + 216 will give you the offset
                                    * from the start of the room to the end of the
                                    * portal data. (4 bytes) */
    int32_t x;                     //!< X position of room ( world coordinates) (4 bytes)
    int32_t seperator3;            //!< 0x00000000 (4 bytes)
    int32_t z;                     //!< Z position of room (world coordinates) (4 bytes)
    int32_t yBottom;               //!< lowest point in room (4 bytes)
    int32_t yTop;                  //!< highest point in room (4 bytes)
    uint16_t numZSectors;         //!< sector table width (2 bytes)
    uint16_t numXSectors;         //!< sector table height (2 bytes)
    uint32_t roomAmbientColor;    //!< ARGB format (blue is least significant byte) (4 bytes)
    uint16_t numRoomLights;       //!< number of lights in this room (2 bytes)
    uint16_t numStaticMeshes;     //!< number of static meshes in this room (2 bytes)
    uint16_t unknownR1;           //!< usually 0x0001 however high byte is sometimes non zero (2 bytes)
    uint16_t unknownR2;           //!< usually 0x0000 however low byte is sometimes non zero (2 bytes)
    uint32_t filler;              //!< always 0x00007FFF (4 bytes)
    uint32_t filler2;             //!< always 0x00007FFF (4 bytes)
    uint32_t seperator4;          //!< 0xCDCDCDCD (4 bytes)
    uint32_t seperator5;          //!< 0xCDCDCDCD (4 bytes)
    unsigned char seperator6[6];   //!< 6 bytes 0xFF
    uint16_t roomFlag;            //!< 0x01 = water, 0x20 = wind, others unknown (2 bytes)
    uint16_t unknownR5;           //!< unknown (2 bytes)
    unsigned char seperator7[10];  //!< 10 bytes 0x00
    uint32_t seperator8;          //!< 0xCDCDCDCD (4 bytes)
    uint32_t unknownR6;           //!< unknown (4 bytes)
    float roomX;                   //!< X position of room in world coordinates
                                   //!< If  null room then this data will be 0xCDCDCDCD (4 bytes)
    uint32_t seperator9;          //!< 0x00000000 or 0xCDCDCDCD if null room. (4 bytes)
    float roomZ;                   //!< Z position of room in world coordinates
                                   //!< If null room then will be bitu32 0xCDCDCDCD (4 bytes)
    uint32_t seperator10;         //!< 0xCDCDCDCD (4 bytes)
    uint32_t seperator11;         //!< 0xCDCDCDCD (4 bytes)
    uint32_t seperator12;         //!< 0xCDCDCDCD (4 bytes)
    uint32_t seperator13;         //!< 0xCDCDCDCD (4 bytes)
    uint32_t seperator14;         //!< 0x00000000 unless null room then 0xCDCDCDCD (4 bytes)
    uint32_t seperator15;         //!< 0xCDCDCDCD (4 bytes)
    uint32_t numRoomTriangles;    //!< total number of triangles this room (4 bytes)
    uint32_t numRoomRectangles;   //!< total number of rectangles this room (4 bytes)
    uint32_t seperator16;         //!< 0x00000000 (4 bytes)
    uint32_t lightSize;           //!< size of light data (number of lights * 88) (4 bytes)
    uint32_t numTotalRoomLights;  //!< total number of lights this room (4 bytes)
    uint32_t unknownR7;           //!< unknown, usually equals 0, 1, 2, or 3 (4 bytes)
    int32_t unknownR8;             //!< usually equals room yTop. Sometimes a few blocks off.
                                   //!< If null room then 0xCDCDCDCD
    int32_t lyBottom;              //!< equals room yBottom. If null room then 0xCDCDCDCD (4 bytes)
    uint32_t numLayers;           //!< number of layers (pieces) this room (4 bytes)
    uint32_t layerOffset;         //!< this number + 216 will give you an offset from the start
                                   //!< of the room data to the start of the layer data (4 bytes)
    uint32_t verticesOffset;      //!< this number + 216 will give you an offset from the start
                                   //!< of the room data to the start of the verex data (4 bytes)
    uint32_t polyOffset;          //!< this number + 216 will give you an offset from the start
                                   //!< of the room data to the start of the rectangle/triangle data (4 bytes)
    uint32_t polyOffset2;         //!< same as above ? (4 bytes)
    uint32_t verticesSize;        //!< size of vertices data block (4 bytes)
    uint32_t seperator17;         //!< 0xCDCDCDCD (4 bytes)
    uint32_t seperator18;         //!< 0xCDCDCDCD (4 bytes)
    uint32_t seperator19;         //!< 0xCDCDCDCD (4 bytes)
    uint32_t seperator20;         //!< 0xCDCDCDCD (4 bytes)
    tr5_light_t *lights;           //!< [LightSize];
                                   //!< data for the lights (88 bytes * NumRoomLights)
    tr2_room_sector_t *sectors;    //!< Data[(NumXSectors * NumZSectors) * 8];
                                   //!< normal sector data structure
    uint16_t numDoors;            //!< number of portals (2 bytes)
    tr2_room_portal_t *doors;      //!< [NumDoors];
                                   //!< normal portal structure (32 bytes * NumDoors)
    uint16_t seperator21;         //!< 0xCDCD (2 bytes)
    tr2_room_staticmesh_t *meshes; //!< [NumStaticMeshes];
                                   //!< normal room static mesh structure (20 bytes * NumRoomStaticMeshes)
    tr5_room_layer_t *layers;      //!< [NumLayers];
                                   //!< data for the room layers (pieces) (56 bytes * NumLayers)
    tr5_room_geometry_t *faces;    //!< [NumRoomRectangles + NumRoomTriangles];
                                   /* Data for the room polys (face4 and face3).
                                    * Structured as first layers rectangles
                                    * then triangles, followed by the
                                    * next layers rectangles and triangles, etc.,
                                    * until all layers are done.
                                    * (12 bytes each rectangle. 10 bytes each triangle)
                                    */
    tr5_vertex_t *vertices;        //!< [VerticesSize];
                                   /*!< Data for the room vertices.
                                    * Structured as vertices for the first layer,
                                    * then vertices for the second layer, etc.,
                                    * until all layers are done.
                                    * (28 bytes each vertex.
                                    */
} tr5_room_t;

/*!
 * \brief TR5 Object Texture Vertex (4 bytes)
 */
typedef struct {
    uint8_t xCoordinate; //!< 0 if Xpixel is the low value,
                          //!< 255 if Xpixel is the high value in the object texture (1 byte)
    uint8_t xPixel;      //!< (1 byte)
    uint8_t yCoordinate; //!< 0 if Ypixel is the low value,
                          //!< 255 if Ypixel is the high value in the object texture (1 byte)
    uint8_t yPixel;      //!< (1 byte)
} tr5_object_texture_vertex_t;

/*!
 * \brief TR5 Object Texture (40 bytes)
 */
typedef struct {
    uint16_t attribute;                     //!< 0, 1, or 2 (2 means 2 sided) (2 bytes)
    uint32_t tile;                          //!< need to mask with 0xFF as other bits are flags.
                                             //!< ie int15_t seems to indicate triangle (4 bytes)
    tr5_object_texture_vertex_t vertices[4]; //!< Vertices[4] (16 bytes)
    uint32_t uv1;                           //!< unknown how used (4 bytes)
    uint32_t uv2;                           //!< unknown how used (4 bytes)
    uint32_t xSize;                         //!< unknown how used, x size (4 bytes)
    uint32_t ySize;                         //!< unknown how used, y size (4 bytes)
    uint16_t seperator;                     //!< always 0x0000 (2 bytes)
} tr5_object_texture_t;

/*!
 * \brief TR5 Flyby camera (40 bytes)
 */
typedef struct {
    int32_t cameraX;      //!< x position of camera in world coordinates (4 bytes)
    int32_t cameraY;      //!< y position of camera in world coordinates (4 bytes)
    int32_t cameraZ;      //!< z  position of camera in world coordinates (4 bytes)
    int32_t targetX;      //!< x position of aiming point in world coords (4 bytes)
    int32_t targetY;      //!< y position of aiming point in world coords (4 bytes)
    int32_t targetZ;      //!< z  position of aiming point in world coords (4 bytes)
    int8_t sequence;      //!< sequence # of camera (1 byte)
    int8_t cameraNumber;  //!< camera # (1 byte)
    uint16_t fov;        //!< fov of camera ( .0054945 for each degree ) (2 bytes)
    uint16_t roll;       //!< roll of camera ( .0054945 for each degree ) (2 bytes)
    uint16_t timer;      //!< timer number (2 bytes)
    uint16_t speed;      //!< ( .000015259 each ) (2 bytes)
    uint16_t flags;      //!< ( see your handy TRLE manual for the specs ) (2 bytes)
    uint32_t room;       //!< room number (4 bytes)
} tr5_flyby_camera_t;

/*!
 * \brief TR5 Moveable (20 bytes).
 *
 * Same as old structure but has uint16_t filler at the end
 */
typedef struct {
    uint32_t objectId;     //!< object identifier ( matched in Items[] )
    uint16_t numMeshes;    //!< number of meshes in this object
    uint16_t startingMesh; //!< starting mesh ( offset into MeshPointers[] )
    uint32_t meshTree;     //!< offset into MeshTree[] )
    uint32_t frameOffset;  //!< byte offset into Frames[] ( divide by 2 for Frames[i] )
    uint16_t animation;    //!< offset into Animations[]
    uint16_t filler;       //!< always equal to 65519 ( 0xFFEF )
} tr5_moveable_t;

typedef struct {
    tr2_vertex_t center;             //!< relative coordinates of mesh centre (6 bytes)
    uint8_t unknown1[4];            //!< unknown (4 bytes)
    int16_t numVertices;             //!< number of vertices to follow (2 bytes)
    tr2_vertex_t *vertices;          //!< list of vertices (NumVertices * 6 bytes)
    int16_t numNormals;              //!< number of normals to follow (2 bytes)
    tr2_vertex_t *normals;           //!< list of normals (NumNormals * 6 bytes)
                                     //!< (becomes Lights if NumNormals < 0; 2 bytes)
    int16_t numTexturedRectangles;   //!< number of textured rectangles to follow (2 bytes)
    tr5_face4_t *texturedRectangles; //!< list of textured rectangles (NumTexturedRectangles * 12 bytes)
    int16_t numTexturedTriangles;    //!< number of textured triangles to follow (2 bytes)
    tr5_face3_t *texturedTriangles;  //!< list of textured triangles (NumTexturedTriangles * 10 bytes)
} tr5_mesh_t;

/*!
 * \brief TR5 Animation (40 bytes).
 *
 * Same as old structure but has 8 bytes before FrameStart.
 */
typedef struct {
    uint32_t FrameOffset;       //!< byte offset into Frames[] ( divide by 2 for Frames[i] )
    uint8_t FrameRate;          //!< Engine ticks per frame
    uint8_t FrameSize;          //!< number of int16_t's in Frames[] used by this animation
    uint16_t StateId;
    int16_t Unknown;
    int16_t Speed;               //!< Evengi Popov found this but I never seen what he said it was for
    uint16_t AccelLo;           //!< same as above
    int16_t AccelHi;             //!< same as above
    uint8_t AUnknown[8];        //!< Unknown
    uint16_t FrameStart;        //!< first frame in this animation
    uint16_t FrameEnd;          //!< last frame in this animation ( numframes = ( End - Start) + 1 )
    uint16_t NextAnimation;
    uint16_t NextFrame;
    uint16_t NumStateChanges;
    uint16_t StateChangeOffset; //!< offset into StateChanges[]
    uint16_t NumAnimCommands;   //!< how many of them to use
    uint16_t AnimCommand;       //!< offset into AnimCommand[]
} tr5_animation_t;

typedef struct {
    unsigned int unknown[24];
} tr5_cinematic_frame_t;

#ifdef WIN32
#pragma pack(pop, tr2_h, 1)
#endif

/*!
 * \brief Loads maps, meshes, textures...
 */
class TombRaider {
public:

    /*!
     * \brief Constructs an object of TombRaider
     */
    TombRaider();

    /*!
     * \brief Deconstructs an object of TombRaider
     */
    ~TombRaider();

    ////////////////////////////////////////
    // Wash me -- not part of cleaned API //
    ////////////////////////////////////////

    int NumRooms();

    int NumMoveables();

    int NumTextures();

    /*!
     * \brief Get number of _special_ textures/images
     * \returns number of special textures/images
     */
    int NumSpecialTextures();

    int NumAnimations();

    unsigned int NumFrames();

    int NumStaticMeshes();

    int NumSprites();

    int NumSpriteSequences();

    int NumItems();

    tr2_version_type Engine();

    unsigned short *Frame();

    tr2_animation_t *Animation();

    tr2_item_t *Item();

    tr2_object_texture_t *ObjectTextures();

    /*!
     * \brief Get number of boxes
     * \returns number of boxes
     */
    unsigned int getNumBoxes();

    tr2_box_t *Box();

    tr2_mesh_t *Mesh();

    /*!
     * \brief Get number of animations for a moveable
     * \param moveable_index valid moveable id
     * \returns number of animations for specified moveable
     */
    int getNumAnimsForMoveable(int moveable_index);

    tr2_staticmesh_t *StaticMesh();

    tr2_moveable_t *Moveable();

    tr2_meshtree_t *MeshTree();

    /*!
     * \brief Get the sprites
     * \returns the sprite array
     */
    tr2_sprite_texture_t *Sprite();

    tr2_sprite_sequence_t *SpriteSequence();

    /*!
     * \brief Makes a 32bit RGBA image from a stexture/bmap
     * \param texture valid index into tex_special list
     * \returns 32bit RGBA image or NULL on error
     */
    unsigned char *SpecialTexTile(int texture);

    /*!
     * \brief Get copies of texture and it's bumpmap
     * \param texture valid textile index
     * \param image will be set to texture if found, or NULL
     * \param bumpmap will be set to bumpmap if found, or NULL
     */
    void Texture(int texture, unsigned char **image, unsigned char **bumpmap);

    unsigned int *Palette16();

    unsigned char *Palette8();

    tr2_room_t *Room();

    /*!
     * \brief Check if a file is a TombRaider pak
     * \param filename file to check
     * \returns 0 if it is a TombRaider pak
     */
    int checkMime(char *filename);

    /*!
     * \brief Loads TombRaider 1-5 pak into memory
     * and does some processing.
     *
     * At 10% all textures are loaded. The exact figure
     * '10' will always be passed to allow for texture
     * caching while meshes load for TR4.
     * \param filename points to valid TombRaider pak
     * \param percent load progress callback
     * \returns 0 on success, < 0 on error
     * \sa TombRaider::loadTR5()
     */
    int Load(char *filename, void (*percent)(int, void *), void *obj);

    /*!
     * \brief Makes a clamped 0.0 to 1.0 texel from coord pair
     * \param texel texel value, is modified, divided by 255.0 and returned
     * \param offset if offset is negative, texel is decreased by one, else increased
     * \returns modified texel divided by 255.0
     */
    float adjustTexel(unsigned char texel, char offset);

    /*!
     * \brief Compute rotation angles from moveable animation data
     * \param frame moveable animation data
     * \param frame_offset moveable animation data
     * \param angle_offset moveable animation data
     * \param x will be set to computed angle
     * \param y will be set to computed angle
     * \param z will be set to computed angle
     */
    void computeRotationAngles(unsigned short **frame,
                                unsigned int *frame_offset,
                                unsigned int *angle_offset,
                                float *x, float *y, float *z);

    /*!
     * \brief Returns computed UV in u and v
     * \param st object texture vert, its coordinates are added to the pixels and divided by 255.0
     * \param u will contain calculated x value
     * \param v will contain calculated y value
     */
    void computeUV(tr2_object_texture_vert_t *st, float *u, float *v);

    /*!
     * \brief Get number of bump maps in loaded pak
     * \returns number of bump maps
     */
    int getBumpMapCount();

    void getColor(int index, float color[4]);

    tr2_version_type getEngine();

    /*!
     * \brief Get the collision sphere for a mesh
     * \param meshIndex mesh index
     * \param center will be filled with center coordinates, not NULL
     * \param radius will be filled with radius, not NULL
     */
    void getMeshCollisionInfo(unsigned int meshIndex,
                                float center[3], float *radius);

    /*!
     * \brief Get SIGNED mesh count (TR encoded < 0 errors)
     * \returns signed mesh count
     */
    int getMeshCount();

    /*!
     * \brief This method is made to let you fill arrays or
     * create single faces. There may be an allocatin version that
     * passes back arrays later.
     * Returns Quads/Rects as 2 triangles,
     * because returning quads would be too trivial :)
     * \param meshIndex mesh index
     * \param faceIndex face index
     * \param index allocated RGBA
     * \param color allocated RGBA
     * \fixme This method interface may change later
     */
    void getMeshColoredRectangle(unsigned int meshIndex,
                                    unsigned int faceIndex,
                                    int index[6], float color[4]);

    /*!
     * \brief This method is made to let you fill arrays or
     * create single faces. There may be an allocating version that
     * passes back arrays later.
     * \param meshIndex mesh index
     * \param faceIndex face index
     * \param index allocated RGBA
     * \param color allocated RGBA
     * \fixme This method interface may change later
     */
    void getMeshColoredTriangle(unsigned int meshIndex,
                                unsigned int faceIndex,
                                int index[3], float color[4]);

    /*!
     * \brief This method is made to let you fill arrays or
     * create single faces. There may be an allocatin version that
     * passes back arrays later.
     * Returns Quads/Rects as 2 triangles,
     * because returning quads would be too trivial :)
     * \param meshIndex mesh index
     * \param faceIndex face index
     * \param index allocated
     * \param st allocated
     * \param texture allocated
     * \param transparency allocated
     * \fixme This method interface may change later
     */
    void getMeshTexturedRectangle(unsigned int meshIndex,
                                    unsigned int faceIndex,
                                    int index[6], float st[12], int *texture,
                                    unsigned short *transparency);

    /*!
     * \brief This method is made to let you fill arrays or
     * create single faces. There may be an allocatin version that
     * passes back arrays later.
     * \param meshIndex mesh index
     * \param faceIndex face index
     * \param index allocated
     * \param st allocated
     * \param texture allocated
     * \param transparency allocated
     * \fixme This method interface may change later
     */
    void getMeshTexturedTriangle(unsigned int meshIndex,
                                    unsigned int faceIndex,
                                    int index[3], float st[6], int *texture,
                                    unsigned short *transparency);

    /*!
     * \brief Get face counts for a given mesh.
     *
     * \todo This method interface may change later...
     * \param meshIndex mesh index
     * \returns number of textured triangles in mesh
     */
    int getMeshTexturedTriangleCount(unsigned int meshIndex);

    /*!
     * \brief Get face counts for a given mesh.
     * \param meshIndex mesh index
     * \returns number of colored triangles in mesh
     */
    int getMeshColoredTriangleCount(unsigned int meshIndex);

    /*!
     * \brief Get face counts for a given mesh.
     * \param meshIndex mesh index
     * \returns number of textured rectangles in mesh
     */
    int getMeshTexturedRectangleCount(unsigned int meshIndex);

    /*!
     * \brief Get face counts for a given mesh.
     * \returns number if colored rectangles in mesh
     */
    int getMeshColoredRectangleCount(unsigned int meshIndex);

    /*!
     * \brief Get vertex, normal and color arrays for a mesh
     * \param meshIndex mesh index
     * \param vertexCount will be set to length of vertex array
     * \param verts will be set to allocated vertex array (XYX)
     * \param normalCount will be set to length of normal array
     * \param norms will be set to allocated normal array (IJK)
     * \param colorCount will be set to length of color array
     * \param colors will be set to allocated color array (I)
     */
    void getMeshVertexArrays(unsigned int meshIndex,
                                unsigned int *vertexCount, float **verts,
                                unsigned int *normalCount, float **norms,
                                unsigned int *colorCount, float **colors);

    /*!
     * \brief Get a single collision box from room (unified)
     * \param roomIndex room index
     * \param index index of box in room
     * \param xyzA will contain first corner of box
     * \param xyzB will contain second corner of box
     * \param xyzC will contain third corner of box
     * \param xyzD will contain fourth corner of box
     * \returns 0 on success, < 0 on error
     */
    int getRoomBox(unsigned int roomIndex, unsigned int index,
                    float *xyzA, float *xyzB, float *xyzC, float *xyzD);

    /*!
     * \brief Get number of collision boxes in room (unified)
     * \param roomIndex room index
     * \returns number of collision boxes in room
     */
    unsigned int getRoomBoxCount(unsigned int roomIndex);

    void getRoomInfo(unsigned int index,
                        unsigned int *flags, float pos[3],
                        float bboxMin[3], float bboxMax[3]);

    /*!
     * \brief Get a single light from a room (unified)
     * \param roomIndex valid room index
     * \param index valid light index in room
     * \param pos allocated, will contain position
     * \param color allocated, will contain color
     * \param dir allocated, will contain direction
     * \param attenuation will contain attenuation
     * \param cutoffAngle will contain cutoff angle
     * \param type will contain type
     * \param flags will contain flags
     * \returns 0 on success, < 0 on error
     */
    int getRoomLight(unsigned int roomIndex, unsigned int index,
                        float pos[4], float color[4], float dir[3],
                        float *attenuation, float *cutoffAngle,
                        unsigned int *type, unsigned int *flags);

    /*!
     * \brief Get number of lights in room (unified)
     * \param roomIndex room index
     * \returns number of lights in room
     */
    unsigned int getRoomLightCount(unsigned int roomIndex);

    /*!
     * \brief Get a single model info from a room
     * \param roomIndex valid room index
     * \param index valid model index in room
     * \param modelIndex will contain starting mesh
     * \param pos will contain pos
     * \param yaw will contain yaw angle
     * \returns 0 on success, < 0 on error
     */
    int getRoomModel(unsigned int roomIndex, unsigned int index,
                        int *modelIndex, float pos[3], float *yaw);

    /*!
     * \brief Get number of room models in room (unified)
     * \param roomIndex room index
     * \returns number of room models in room
     */
    unsigned int getRoomModelCount(unsigned int roomIndex);

    /*!
     * \brief Get a single portal from room (unified)
     * \param roomIndex valid room index
     * \param index valid portal index in room
     * \param adjoiningRoom will contain adjoining room index
     * \param normal allocated, will contain normal vector
     * \param vertices allocated, will contain vertices
     * \returns 0 on success, < 0 on error
     */
    int getRoomPortal(unsigned int roomIndex, unsigned int index,
                        int *adjoiningRoom, float normal[3], float vertices[12]);

    /*!
     * \brief Get number of portals from room (unified)
     * \param roomIndex room index
     * \returns number of portals from room
     */
    unsigned int getRoomPortalCount(unsigned int roomIndex);

    /*!
     * \brief Get rectangle data with texCoords for non-matching
     * vertex/uv for each vertex in TombRaider room (unified)
     * \param roomIndex valid room index
     * \param rectangleIndex rectangle index in room
     * \param indices will contain indices
     * \param texCoords will contain texCoords
     * \param texture will contain texture
     * \param flags will contain flags
     */
    void getRoomRectangle(unsigned int roomIndex, unsigned int rectangleIndex,
                            unsigned int *indices, float *texCoords, int *texture,
                            unsigned int *flags);

    /*!
     * \brief Get number of rectangles from room (unified)
     * \param roomIndex room index
     * \returns number of rectangles from room
     */
    unsigned int getRoomRectangleCount(unsigned int roomIndex);

    /*!
     * \brief Get a single sector from room (unified)
     * \param roomIndex room index
     * \param index sector index
     * \param flags will contain flags
     * \param ceiling will contain ceiling
     * \param floor will contain floor
     * \param floorDataIndex will contain floor data index
     * \param boxIndex will contain boxIndex
     * \param roomBelow will contain roomBelow
     * \param roomAbove will contain roomAbove
     * \return 0 on success, < 0 on error
     */
    int getRoomSector(unsigned int roomIndex, unsigned int index,
                        unsigned int *flags, float *ceiling, float *floor,
                        int *floorDataIndex, int *boxIndex,
                        int *roomBelow, int *roomAbove);

    /*!
     * \brief Get number of sectors in room (unified)
     * \param roomIndex room index
     * \param zSectorsCount will contain width of sector list
     * \param xSectorsCount will contain height of sector list
     * \returns number of sectors in room
     */
    unsigned int getRoomSectorCount(unsigned int roomIndex,
                                    unsigned int *zSectorsCount,
                                    unsigned int *xSectorsCount);

    void getRoomSprite(unsigned int roomIndex, unsigned int index,
                        float scale, int *texture,
                        float *pos, float *vertices, float *texcoords);

    /*!
     * \brief Get number of sprites in room (unified)
     * \param roomIndex room index
     * \returns number of sprites in room
     */
    unsigned int getRoomSpriteCount(unsigned int roomIndex);

    /*!
     * \brief Gets triangle data with texCoords for non-matching
     * vertex/uv for each vertex in TombRaider room (unified)
     * \param roomIndex room index
     * \param triangleIndex triangle index
     * \param indices will contain indices
     * \param texCoords will contain texCoords
     * \param texture will contain texture
     * \param flags will contain flags
     */
    void getRoomTriangle(unsigned int roomIndex, unsigned int triangleIndex,
                            unsigned int *indices, float *texCoords, int *texture,
                            unsigned int *flags);

    /*!
     * \brief Gets triangle data with texCoords for non-matching
     * vertex/uv for each vertex in TombRaider room.
     *
     * This is used with vertices, colors, etc. to do partial array
     * rendering, since the texcoords will never match vertives
     * (Tomb Raider is textile based).
     * \param index room index
     * \param textureOffset texture offset
     * \param count will contain count
     * \param indices will contain indices
     * \param texCoords will contain texCoords
     * \param textures will contain textures
     * \param flags will contain flags
     */
    void getRoomTriangles(unsigned int index, int textureOffset,
                            unsigned int *count, unsigned int **indices,
                            float **texCoords, int **textures,
                            unsigned int **flags);

    /*!
     * \brief Gets triangle data with duplicated vertex/color/normal
     * data for each face vertex to match the textile based texcoords.
     *
     * This uses more memory, but lets you do direct array rendering
     * with OpenGL, D3D, etc.
     * \param roomIndex room index
     * \param textureOffset texture offset
     * \param count will contain count
     * \param indices will contain indices
     * \param vertices will contain vertices
     * \param texCoords will contain texCoords
     * \param colors will contain colors
     * \param textures will contain textures
     * \param flags will contain flags
     */
    void getRoomTriangles(unsigned int roomIndex, int textureOffset,
                            unsigned int *count,
                            unsigned int **indices, float **vertices,
                            float **texCoords, float **colors,
                            int **textures, unsigned int **flags);

    /*!
     * \brief Get number of triangles from room (unified)
     * \param roomIndex room index
     * \returns number of triangles from room
     */
    unsigned int getRoomTriangleCount(unsigned int roomIndex);

    /*!
     * \brief Gets vertex position and color
     * \param roomIndex room index
     * \param vertexIndex vertex index
     * \param xyz will contain vertex position, has to be allocated
     * \param rgba will contain vertex color, has to be allocated
     */
    void getRoomVertex(unsigned int roomIndex, unsigned int vertexIndex,
                        float *xyz, float *rgba);

    /*!
     * \brief Get allocated vertex and color arrays and their element counts (unified)
     * \param roomIndex valid room index
     * \param vertexCount will contain vertex array length
     * \param vertices will contain vertex array
     * \param normalCount will contain normal array length
     * \param normals will contain normal array
     * \param colorCount will contain color array length
     * \param colors will contain color array
     */
    void getRoomVertexArrays(unsigned int roomIndex,
                                unsigned int *vertexCount, float **vertices,
                                unsigned int *normalCount, float **normals,
                                unsigned int *colorCount, float **colors);

    /*!
     * \brief Get number of lights in room (unified)
     * \param roomIndex room index
     * \returns number of lights in room
     */
    unsigned int getRoomVertexCount(unsigned int roomIndex);

    /*!
     * \brief Get sky mesh ID
     * \returns moveable id of sky mesh or -1 if none
     */
    int getSkyModelId();

    void getSprites();

    /*!
     * \brief Get a copy of a sound sample and its byte size
     * \param index sound sample index
     * \param bytes will contain byte size of sound sample
     * \param data will contain sound sample
     */
    void getSoundSample(unsigned int index,
                        unsigned int *bytes, unsigned char **data);

    /*!
     * \brief Get number of loaded sound samples
     * \returns number of sound samples loaded
     */
    unsigned int getSoundSamplesCount();

    /*!
     * \brief Check if a mesh is valid
     * \param index mesh index (?)
     * \returns true if mesh is valid
     */
    bool isMeshValid(int index);

    /*!
     * \brief Check if a room is valid (TRC support)
     * \param index room index
     * \returns true if room is valid
     */
    bool isRoomValid(int index);

    /*!
     * \brief Load an external sound pak for TR2 and TR3
     * \param filename pak to load
     * \returns < 0 on error
     */
    int loadSFX(char *filename);

    void reset();

    void setDebug(bool toggle);

    /*!
     * \brief Sets lighting factor for each vertex color per room in TR3 paks
     * \param f new lighting factor
     */
    void setRoomVertexLightingFactor(float f);

    /*!
     * \brief Set scaling for sprite texel alignment, etc.
     * \param f new scaling factor
     */
    void setTexelScalingFactor(float f);

private:

    void extractMeshes(unsigned char *mesh_data,
                        unsigned int num_mesh_pointers,
                        unsigned int *mesh_pointers);

    int Fread(void *buffer, size_t size, size_t count, FILE *f);

    /*!
     * \brief Get a copy of the sound samples
     * \param bytes will contain size of sound samples
     * \param data will contain sound samples themselves
     */
    void getRiffData(unsigned int *bytes, unsigned char **data);

    /*!
     * \brief Get a copy of a TR4 sound sample
     * \param index sound sample index
     * \param bytes will contain length of sound sample
     * \param data will contain sound sample itself
     */
    void getRiffDataTR4(unsigned int index,
                        unsigned int *bytes, unsigned char **data);

    /*!
     * \brief Get an array of offsets for a contiguous RIFF data stream in chunks.
     *
     * Offsets will be allocated with enough space to hold expected
     * number of offsets. (Should be known number, otherwise not all RIFFs
     * will be parsed.)
     * \param riffData riff data
     * \param riffDataBytes length of riff data
     * \param offsets will contain offsets
     * \param numOffsets known number
     * \returns number of RIFFs found
     */
    int getRiffOffsets(unsigned char *riffData, unsigned int riffDataBytes,
                        unsigned int **offsets, unsigned int numOffsets);

    /*!
     * \brief Makes a 32bit RGBA image from a textile.
     *
     * This handles all selection and conversion, including
     * alpha layering flags, now.
     * \param texture valid index into textile list
     * \returns 32bit RGBA image or NULL on error
     */
    unsigned char *getTexTile(int texture);

    /*!
     * \brief Loads a TR5 pak into memory.
     *
     * At 10% all textures are loaded
     * the exact figure '10' will always be passed to
     * allow for texture caching while meshes load.
     * \param f valid FILE
     * \param percent callback for loading progress
     * \returns 0 on success, < 0 on error
     */
    int loadTR5(FILE *f, void (*percent)(int, void *), void *obj);

    void print(const char *methodName, const char *s, ...) __attribute__((format(printf, 3, 4)));

    void printDebug(const char *methodName, const char *s, ...) __attribute__((format(printf, 3, 4)));

    bool mReset;                           //!< Guard multiple calls to reset()
    bool mDebug;                           //!< Debug output toggle
    unsigned int mPakVersion;              //!< TombRaider pak file header version
    tr2_version_type mEngineVersion;       //!< TombRaider engine version
    tr2_colour_t _palette8[256];           //!< 8-bit palette
    unsigned int _palette16[256];          //!< 16-bit palette
    unsigned int _num_textiles;            //!< Total number of texture tiles
    unsigned short  _num_room_textures;    //!< Num textures only for room use?
    unsigned short _num_misc_textures;     //!< Num of textures for misc use?
    unsigned short _num_bump_map_textures; //!< Num of textures that are bump map, texture pairs
    tr2_textile8_t *_textile8;             //!< 8-bit (palettised) textiles
    tr2_textile16_t *_textile16;           //!< 16-bit (ARGB) textiles
    tr2_textile32_t *_textile32;           //!< 32-bit (BGRA) textiles
    unsigned int _num_tex_special;         //!< Special textures and bump maps count
    unsigned char *_tex_special;           //!< Special textures and bump maps
    unsigned int _unknown_t;               //!< 32-bit unknown (always 0 in real TR2 levels)
    unsigned short _num_rooms;             //!< Number of rooms in this level
    tr2_room_t *_rooms;                    //!< List of rooms (TR1,TR2,TR3,TR4)
    tr5_room_t *mRoomsTR5;                 //!< Rooms ( TR5 / TRC ) Only
    unsigned int _num_floor_data;          //!< Num of words of floor data this level
    unsigned short *_floor_data;           //!< Floor data
    int mMeshCount;                        //!< Number of meshes this level
    tr2_mesh_t *mMeshes;                   //!< list of meshes
    unsigned int _num_animations;          //!< number of animations this level
    tr2_animation_t *_animations;          //!< list of animations
    unsigned int _num_state_changes;       //!< number of structures(?) this level
    tr2_state_change_t *_state_changes;    //!< list of structures
    unsigned int _num_anim_dispatches;     //!< number of ranges(?) this level
    tr2_anim_dispatch_t *_anim_dispatches; //!< list of ranges
    unsigned int _num_anim_commands;       //!< number of Bone1s this level
    tr2_anim_command_t *_anim_commands;    //!< list of Bone1s
    unsigned int _num_mesh_trees;          //!< number of Bone2s this level
    tr2_meshtree_t *_mesh_trees;           //!< list of Bone2s
    unsigned int _num_frames;              //!< num of words of frame data this level
    unsigned short *_frames;               //!< frame data
    unsigned int _num_moveables;           //!< number of moveables this level
    tr2_moveable_t *_moveables;            //!< list of moveables

    uint32_t numMoveablesTR5;
    tr5_moveable_t *moveablesTR5;

    uint32_t numAnimationsTR5;
    tr5_animation_t *animationsTR5;

    uint32_t numObjectTexturesTR5;
    tr5_object_texture_t *objectTexturesTR5;

    uint32_t numCinematicFramesTR5;
    tr5_cinematic_frame_t *cinematicFramesTR5;

    uint32_t numFlyByCamerasTR5;
    tr5_flyby_camera_t *flyByCamerasTR5;

    unsigned int _num_static_meshes;        //!< number of static meshes this level
    tr2_staticmesh_t *_static_meshes;       //!< static meshes
    unsigned int _num_object_textures;      //!< number of object textures this level
    tr2_object_texture_t *_object_textures; //!< list of object textures

    unsigned int _num_sprite_textures;        //!< num of sprite textures this level
    tr2_sprite_texture_t *_sprite_textures;   //!< list of sprite textures
    unsigned int _num_sprite_sequences;       //!< num of sprite sequences this level
    tr2_sprite_sequence_t *_sprite_sequences; //!< sprite sequence data
    int  _num_cameras;                        //!< Number of Cameras
    tr2_camera_t  *_cameras;                  //!< cameras
    int  _num_sound_sources;                  //!< Number of Sounds
    tr2_sound_source_t *_sound_sources;       //!< sounds
    int  _num_boxes;                          //!< Number of Boxes
    tr2_box_t *_boxes;                        /*!< boxes - looks like
                                               * struct { unsigned short value[4]; }
                                               * - value[0..2] might be a vector;
                                               * value[3] seems to be index into
                                               * Overlaps[] */
    int  _num_overlaps;                       //!< Number of Overlaps
    short  *_overlaps;                        /*!< Overlaps -
                                               * looks like ushort; 0x8000 is flag
                                               * of some sort appears to be an
                                               * offset into Boxes[] and/or
                                               * Boxes2[] */
    short  *_zones;                           //!< Boxes2
    int  _num_animated_textures;              //!< Number of AnimTextures
    short  *_animated_textures;               //!< Animtextures
    int  _num_items;                          //!< Number of Items
    tr2_item_t *_items;                       //!< Items
    unsigned char  *_light_map;               //!< Colour-light maps
    unsigned int _num_cinematic_frames;       //!< Number of cut-scene frames
    tr2_cinematic_frame_t *_cinematic_frames; //!< Cut-scene frames
    short  _num_demo_data;                    //!< Number of Demo Data
    unsigned char  *_demo_data;               //!< Demo data
    float mRoomVertexLightingFactor;
    float mTexelScale;

    // Sound data
    short  *mSoundMap;                   //!< Sound map
    int  mNumSoundDetails;               //!< Number of SampleModifiers
    tr2_sound_details_t *mSoundDetails;  //!< Sample modifiers
    int  mNumSampleIndices;              //!< Number of Sample Indices
    int  *mSampleIndices;                //!< Sample indices
    unsigned int *mSampleIndicesTR5;
    bool mRiffAlternateLoaded;           //!< Is a TR2,TR3 SFX loaded?
    unsigned int *mRiffAlternateOffsets; //!< After parsing this will
                                         //!< hold byte offsets for TR2,TR3
                                         //!< RIFFs in the buffered SFX
    int mRiffDataSz;                     //!< Byte size of a loaded SFX
    unsigned char *mRiffData;            //!< SFX RIFF data in chunks
    unsigned int mNumTR4Samples;
    unsigned char **mTR4Samples;
    unsigned int *mTR4SamplesSz;

    // For packed Fread emu/wrapper
    unsigned char *mCompressedLevelData;     //!< Buffer used to emulate fread with uncompressed libz data
    unsigned int mCompressedLevelDataOffset; //!< Offset into buffer
    unsigned int mCompressedLevelSize;       //!< Size of buffer
    tr_fread_mode_t mFreadMode;              //!< Fread mode file|buffer
};

#endif

