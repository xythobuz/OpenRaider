/*!
 * \file include/StaticMesh.h
 * \brief Static Model Meshes
 *
 * \author xythobuz
 */

#ifndef _STATIC_MODEL_H_
#define _STATIC_MODEL_H_

#include <vector>
#include "TombRaider.h"

class TexturedTriangle {
  public:
    TexturedTriangle(int i[3], float s[6], int tex, unsigned short trans);
    void display(float* vertices, float* colors, float* normals);

  private:
    int index[3];
    float st[6];
    int texture;
    unsigned short transparency;
};

class StaticMesh {
  public:
    StaticMesh(TombRaider& tr, unsigned int index);
    ~StaticMesh();
    void display();
    float getRadius();

  private:
    bool dontshow;
    float center[3];
    float radius;

    unsigned int vertexCount;
    unsigned int colorCount;
    unsigned int normalCount;

    float* vertices;
    float* colors;
    float* normals;

    std::vector<TexturedTriangle*> triangles;
};

#endif

