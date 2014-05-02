/*!
 * \file include/WorldData.h
 * \brief Structs and enums for the game world (model)
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _WORLDDATA_H_
#define _WORLDDATA_H_

#include "math/math.h"
#include "SkeletalModel.h"

// Mirrors TombRaider class' room flags really
typedef enum {
    roomFlag_underWater    = 0x0001
} room_flags_t;

typedef enum {
    worldMoveType_walkNoSwim   = -1,
    worldMoveType_walk         = 0,
    worldMoveType_noClipping   = 1,
    worldMoveType_fly          = 2,
    worldMoveType_swim         = 3
} worldMoveType;

typedef struct {
    vec3_t pos;
} vertex_t;

typedef struct {
    vec2_t st;
} texel_t;

typedef struct {
    int num_verts;      //!< 4 == Quad, 3 == Triangle, rendered as triangles
    vertex_t vertex[4];
    texel_t texel[4];
    float pos[3];
    float radius;       //!< \fixme yeah, I know (I don't? --xythobuz)
    int texture;
} sprite_t;

typedef struct {
    int num_sprites;
    sprite_t *sprite;
} sprite_seq_t;

/*! \fixme For now shaders are textures on tex objects
 * and materials on color objects. If -1
 * then it doesn't have that information yet.
 */
typedef struct {
    int index[3];
    vec_t st[6];
    int texture;
    unsigned short transparency;
} texture_tri_t;

typedef struct {
    std::vector<texture_tri_t *> texturedTriangles;
    std::vector<texture_tri_t *> coloredTriangles;
    std::vector<texture_tri_t *> texturedRectangles;
    std::vector<texture_tri_t *> coloredRectangles;

    vec3_t center;
    float radius;

    unsigned int vertexCount;
    vec_t *vertices;

    unsigned int colorCount;
    vec_t *colors;

    unsigned int normalCount;
    vec_t *normals;
} model_mesh_t;

typedef struct entity_s {
    int id;                  //!< Unique identifier
    float pos[3];            //!< World position
    float angles[3];         //!< Euler angles (pitch, yaw, roll)
    int type;                //!< {(0x00, item), (0x01, ai), (0x02, player)}
    int room;                //!< Current room entity is in
    worldMoveType moveType;  //!< Type of motion/clipping
    bool moving;             //!< In motion?
    struct entity_s *master; //!< Part of entity chain?

    int state;               //!< State of the Player, AI, or object
    int objectId;            //!< What kind of entity?

    int modelId;             //!< Animation model
    SkeletalModel *tmpHook;
    bool animate;

    /*
    float time, lastTime;
    int state, lastState;
    int event, lastEvent;
    int goal;
    */
} entity_t;

typedef struct {
    int index;    //!< model_mesh index
    float yaw;    //!< angle of rotation on Y
    float pos[3]; //!< position

    //vec3_t bboxMax;
    //vec3_t bboxMin;
} static_model_t;

typedef struct {
    float vertices[4][3];
    float normal[3];
    int adjoining_room;
} portal_t;

typedef struct {
    vertex_t a;
    vertex_t b;
    vertex_t c;
    vertex_t d;
} box_t;

typedef struct {
    vec_t floor;
    vec_t ceiling;

    bool wall;
} sector_t;

//! \fixme No room mesh list or sprites and etc
typedef struct {
    std::vector<int> adjacentRooms;
    std::vector<portal_t *> portals;
    std::vector<static_model_t *> models;
    std::vector<sprite_t *> sprites;
    std::vector<box_t *> boxes;
    std::vector<sector_t *> sectors;

    int id;
    unsigned int flags;
    unsigned int numXSectors;
    unsigned int numZSectors;
    float pos[3];
    vec3_t bbox_min;
    vec3_t bbox_max;
} room_mesh_t;

#endif

