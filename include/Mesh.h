/*!
 * \file include/Mesh.h
 * \brief OpenGL Mesh
 *
 * \author xythobuz
 */

#ifndef _MESH_H_
#define _MESH_H_

#include <map>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

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
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> textures;

    std::vector<unsigned short> indicesColor;
    std::vector<glm::vec3> verticesColor;
    std::vector<glm::vec3> colors;
};

// --------------------------------------

struct PackedVertex {
    glm::vec3 pos;
    glm::vec2 uv;
    unsigned int tex;

    PackedVertex(glm::vec3 p, glm::vec2 u, unsigned int t) : pos(p), uv(u), tex(t) { }

    bool operator<(const PackedVertex& v) const {
        return memcmp(this, &v, sizeof(PackedVertex)) > 0;
    }
};

struct PackedColoredVertex {
    glm::vec3 pos;
    glm::vec3 col;

    PackedColoredVertex(glm::vec3 p, glm::vec3 c) : pos(p), col(c) { }

    bool operator<(const PackedColoredVertex& v) const {
        return memcmp(this, &v, sizeof(PackedColoredVertex)) > 0;
    }
};

template <typename T>
bool findSimilarVertex(T& v,
                       std::map<T, unsigned short> m,
                       unsigned short& s) {
    auto it = m.find(v);
    if (it == m.end())
        return false;
    else {
        s = it->second;
        return true;
    }
}

#endif

