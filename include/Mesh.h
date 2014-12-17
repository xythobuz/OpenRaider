/*!
 * \file include/Mesh.h
 * \brief OpenGL Mesh
 *
 * \author xythobuz
 */

#ifndef _MESH_H_
#define _MESH_H_

#include <map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Mesh {

};

// --------------------------------------

struct PackedVertex {
    glm::vec3 pos;
    glm::vec2 uv;
    unsigned int tex;

    PackedVertex(glm::vec3 p, glm::vec2 u, unsigned int t) : pos(p), uv(u), tex(t) { }
    bool operator<(const PackedVertex& v) const { return memcmp(this, &v, sizeof(PackedVertex)) > 0; }
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

