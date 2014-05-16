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

#endif

