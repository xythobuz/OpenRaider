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

typedef enum {
    worldMoveType_walkNoSwim = -1,
    worldMoveType_walk       = 0,
    worldMoveType_noClipping = 1,
    worldMoveType_fly        = 2,
    worldMoveType_swim       = 3
} worldMoveType;

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

typedef struct {
    int id;                  //!< Unique identifier
    float pos[3];            //!< World position
    float angles[3];         //!< Euler angles (pitch, yaw, roll)
    int room;                //!< Current room entity is in
    worldMoveType moveType;  //!< Type of motion/clipping

    int state;               //!< State of the Player, AI, or object
    int objectId;            //!< What kind of entity?

    int modelId;             //!< Animation model
    SkeletalModel *tmpHook;
    bool animate;
} entity_t;

#endif

