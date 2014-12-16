/*!
 * \file src/Mesh.cpp
 * \brief OpenGL Mesh
 *
 * \author Mongoose
 */

#include <map>
#include <stdlib.h>

#include "global.h"
#include "TextureManager.h"
#include "system/Window.h"
#include "Mesh.h"

Mesh::Mesh(const std::vector<RoomVertexTR2>& vert,
     const std::vector<RoomRectangleTR2>& rect,
     const std::vector<RoomTriangleTR2>& tri) {
    for (auto& t : rect) {
        indices.push_back(0);
        vertices.push_back(glm::vec3(vert.at(t.v1).x, vert.at(t.v1).y, vert.at(t.v1).z));
        vertices.push_back(glm::vec3(vert.at(t.v2).x, vert.at(t.v2).y, vert.at(t.v2).z));
        vertices.push_back(glm::vec3(vert.at(t.v3).x, vert.at(t.v3).y, vert.at(t.v3).z));
        vertices.push_back(glm::vec3(vert.at(t.v4).x, vert.at(t.v4).y, vert.at(t.v4).z));
        textures.push_back(t.texture);
    }

    for (auto& t : tri) {
        indices.push_back(1);
        vertices.push_back(glm::vec3(vert.at(t.v1).x, vert.at(t.v1).y, vert.at(t.v1).z));
        vertices.push_back(glm::vec3(vert.at(t.v2).x, vert.at(t.v2).y, vert.at(t.v2).z));
        vertices.push_back(glm::vec3(vert.at(t.v3).x, vert.at(t.v3).y, vert.at(t.v3).z));
        textures.push_back(t.texture);
    }
}

struct PackedVertex {
    glm::vec3 pos;
    glm::vec2 uv;
    unsigned int tex;

    PackedVertex(glm::vec3 p, glm::vec2 u, unsigned int t) : pos(p), uv(u), tex(t) { }
    bool operator<(const PackedVertex& v) const { return memcmp(this, &v, sizeof(PackedVertex)) > 0; }
};

static bool findSimilarVertex(PackedVertex& v,
                              std::map<PackedVertex, unsigned short> m,
                              unsigned short& s) {
    auto it = m.find(v);
    if (it == m.end())
        return false;
    else {
        s = it->second;
        return true;
    }
}

void Mesh::prepare() {
    std::vector<unsigned short> ind;
    std::vector<glm::vec3> vert;
    std::vector<unsigned int> tex;

    std::map<PackedVertex, unsigned short> vertexMap;

    int vertIndex = 0;
    for (int i = 0; i < indices.size(); i++) {
        unsigned int texture = getTextureManager().getTile(textures.at(i)).getTexture();
        for (int v = 0; v < ((indices.at(i) == 0) ? 4 : 3); v++) {
            glm::vec2 uv = getTextureManager().getTile(textures.at(i)).getUV(v);
            PackedVertex p(vertices.at(vertIndex + v), uv, texture);
            unsigned short s;
            if (findSimilarVertex(p, vertexMap, s)) {
                ind.push_back(s); // Vertex already cached
            } else {
                vertexMap[p] = vert.size();
                ind.push_back(vert.size());
                vert.push_back(p.pos);
                uvs.push_back(p.uv);
                tex.push_back(p.tex);
            }
        }

        if (indices.at(i) == 0) {
            ind.push_back(ind.at(ind.size() - 2));
            ind.push_back(ind.at(ind.size() - 5));
        }

        vertIndex += (indices.at(i) == 0) ? 4 : 3;
    }

    indices = ind;
    vertices = vert;
    textures = tex;

    assert((indices.size() % 3) == 0);
}

void Mesh::display(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
    glm::mat4 MVP = projection * view * model;

    unsigned int indexStart = 0;
    unsigned int indexPos = 1;
    unsigned int texture = textures.at(indices.at(0));

    while ((indexStart != indexPos) && (indexPos < indices.size())) {
        while ((indexPos < indices.size()) && (textures.at(indices.at(indexPos)) == texture))
            indexPos++;

        std::vector<unsigned short> ind(indices.begin() + indexStart, indices.begin() + indexPos);
        Window::drawGL(vertices, uvs, ind, MVP, texture);

        if (indexPos < indices.size()) {
            indexStart = indexPos;
            indexPos += 1;
            texture = textures.at(indices.at(indexStart));
        }
    }
}

