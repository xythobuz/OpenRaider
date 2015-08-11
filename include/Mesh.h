/*!
 * \file include/Mesh.h
 * \brief Textured/Colored Mesh
 *
 * \author xythobuz
 */

#ifndef _MESH_H_
#define _MESH_H_

#include <vector>

#include "BoundingSphere.h"
#include "system/Shader.h"

struct IndexedRectangle {
    unsigned int v1, v2, v3, v4; // Vertex list indices
    unsigned int texture; // Index into object-texture list

    IndexedRectangle(unsigned int t, unsigned int _v1,
                     unsigned int _v2, unsigned int _v3, unsigned int _v4 = 0)
        : v1(_v1), v2(_v2), v3(_v3), v4(_v4), texture(t) { }
};

struct IndexedColoredRectangle {
    unsigned int v1, v2, v3, v4, index;

    IndexedColoredRectangle(unsigned int paletteIndex,
                            unsigned int _v1, unsigned int _v2,
                            unsigned int _v3, unsigned int _v4 = 0)
        : v1(_v1), v2(_v2), v3(_v3), v4(_v4), index(paletteIndex) { }
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
    void display(glm::mat4 MVP, ShaderTexture* shaderTexture = nullptr);

    BoundingSphere& getBoundingSphere() { return sphere; }

  private:
    std::vector<unsigned short> indicesBuff;
    std::vector<glm::vec3> verticesBuff;
    std::vector<glm::vec2> uvsBuff;
    std::vector<unsigned int> texturesBuff;

    std::vector<unsigned short> indicesColorBuff;
    std::vector<glm::vec3> verticesColorBuff;
    std::vector<glm::vec3> colorsBuff;
    std::vector<unsigned int> colorsIndexBuff;

    BoundingSphere sphere;
};

#endif

