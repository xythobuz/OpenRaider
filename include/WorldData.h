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

#endif

