/*!
 * \file src/Mesh.cpp
 * \brief OpenGL Mesh
 *
 * \author xythobuz
 */

#include "global.h"
#include "TextureManager.h"
#include "system/Window.h"
#include "Mesh.h"

Mesh::Mesh(const std::vector<glm::vec3>& vert,
           const std::vector<IndexedRectangle>& rect,
           const std::vector<IndexedRectangle>& tri,
           const std::vector<IndexedColoredRectangle>& coloredRect,
           const std::vector<IndexedColoredRectangle>& coloredTri) {
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

    for (auto& t : coloredRect) {
        indicesColor.push_back(0);
        verticesColor.push_back(glm::vec3(vert.at(t.v1).x, vert.at(t.v1).y, vert.at(t.v1).z));
        verticesColor.push_back(glm::vec3(vert.at(t.v2).x, vert.at(t.v2).y, vert.at(t.v2).z));
        verticesColor.push_back(glm::vec3(vert.at(t.v3).x, vert.at(t.v3).y, vert.at(t.v3).z));
        verticesColor.push_back(glm::vec3(vert.at(t.v4).x, vert.at(t.v4).y, vert.at(t.v4).z));
        colors.push_back(glm::vec3(t.r, t.g, t.b));
    }

    for (auto& t : coloredTri) {
        indicesColor.push_back(1);
        verticesColor.push_back(glm::vec3(vert.at(t.v1).x, vert.at(t.v1).y, vert.at(t.v1).z));
        verticesColor.push_back(glm::vec3(vert.at(t.v2).x, vert.at(t.v2).y, vert.at(t.v2).z));
        verticesColor.push_back(glm::vec3(vert.at(t.v3).x, vert.at(t.v3).y, vert.at(t.v3).z));
        colors.push_back(glm::vec3(t.r, t.g, t.b));
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
    assert((ind.size() % 3) == 0);
    assert(vert.size() == tex.size());
    assert(vert.size() == uvs.size());
    indices = ind;
    vertices = vert;
    textures = tex;

    std::vector<unsigned short> indC;
    std::vector<glm::vec3> vertC;
    std::vector<glm::vec3> col;
    std::map<PackedColoredVertex, unsigned short> vertexMapC;
    vertIndex = 0;
    for (int i = 0; i < indicesColor.size(); i++) {
        for (int v = 0; v < ((indicesColor.at(i) == 0) ? 4 : 3); v++) {
            PackedColoredVertex p(verticesColor.at(vertIndex + v), colors.at(i));
            unsigned short s;
            if (findSimilarVertex(p, vertexMapC, s)) {
                indC.push_back(s); // Vertex already cached
            } else {
                vertexMapC[p] = vertC.size();
                indC.push_back(vertC.size());
                vertC.push_back(p.pos);
                col.push_back(p.col);
            }
        }

        if (indicesColor.at(i) == 0) {
            indC.push_back(indC.at(indC.size() - 2));
            indC.push_back(indC.at(indC.size() - 5));
        }

        vertIndex += (indicesColor.at(i) == 0) ? 4 : 3;
    }
    assert((indC.size() % 3) == 0);
    assert(vertC.size() == col.size());
    indicesColor = indC;
    verticesColor = vertC;
    colors = col;
}

void Mesh::display(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
    glm::mat4 MVP = projection * view * model;

    if (indices.size() > 0) {
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

    if (indicesColor.size() > 0)
        Window::drawGL(verticesColor, colors, indicesColor, MVP);
}

