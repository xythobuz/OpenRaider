/*!
 * \file include/Mesh.h
 * \brief OpenGL Mesh
 *
 * \author xythobuz
 */

#ifndef _MESH_H_
#define _MESH_H_

#include <vector>

#include "system/Shader.h"

struct IndexedRectangle {
    unsigned int v1, v2, v3, v4; // Vertex list indices
    unsigned int texture; // Index into object-texture list

    IndexedRectangle(unsigned int t, unsigned int _v1,
                     unsigned int _v2, unsigned int _v3, unsigned int _v4 = 0)
        : v1(_v1), v2(_v2), v3(_v3), v4(_v4), texture(t) { }
};

struct IndexedColoredRectangle {
    unsigned int v1, v2, v3, v4;
    unsigned char r, g, b;

    IndexedColoredRectangle(unsigned char _r, unsigned char _g, unsigned char _b,
                            unsigned int _v1, unsigned int _v2,
                            unsigned int _v3, unsigned int _v4 = 0)
        : v1(_v1), v2(_v2), v3(_v3), v4(_v4), r(_r), g(_g), b(_b) { }
};

// --------------------------------------

class Mesh {
  public:
    Mesh(const std::vector<glm::vec3>& vertices,
         const std::vector<IndexedRectangle>& rectangles,
         const std::vector<IndexedRectangle>& triangles,
         const std::vector<IndexedColoredRectangle>& coloredRectangles,
         const std::vector<IndexedColoredRectangle>& coloredTriangles);
    void prepare();
    void display(glm::mat4 MVP);

  private:
    std::vector<unsigned short> indicesBuff;
    std::vector<glm::vec3> verticesBuff;
    std::vector<unsigned int> texturesBuff;

    std::vector<unsigned short> indicesColorBuff;
    std::vector<glm::vec3> verticesColorBuff;
    std::vector<glm::vec3> colorsBuff;

    ShaderBuffer indices, vertices, uvs;
    ShaderBuffer indicesColor, verticesColor, colors;
};

#endif

