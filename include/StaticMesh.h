/*!
 * \file include/StaticMesh.h
 * \brief Static Model Meshes
 *
 * \author xythobuz
 */

#ifndef _STATIC_MODEL_H_
#define _STATIC_MODEL_H_

#include <vector>
#include "math/math.h"
#include "TombRaider.h"

class TexturedTriangle {
public:
    TexturedTriangle(int i[3], vec_t s[6], int tex, unsigned short trans);
    bool operator< (TexturedTriangle &t);

    void display(vec_t *vertices, vec_t *colors, vec_t *normals);

private:
    int index[3];
    vec_t st[6];
    int texture;
    unsigned short transparency;
};

class StaticMesh {
public:
    StaticMesh(TombRaider &tr, unsigned int index);
    ~StaticMesh();

    void display();
    vec_t getRadius();

private:
    bool dontshow;
    vec3_t center;
    vec_t radius;

    unsigned int vertexCount;
    unsigned int colorCount;
    unsigned int normalCount;

    vec_t *vertices;
    vec_t *colors;
    vec_t *normals;

    std::vector<TexturedTriangle *> texturedTriangles;
    std::vector<TexturedTriangle *> coloredTriangles;
    std::vector<TexturedTriangle *> texturedRectangles;
    std::vector<TexturedTriangle *> coloredRectangles;
};

#endif

