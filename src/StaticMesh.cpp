/*!
 * \file src/StaticMesh.cpp
 * \brief Static Model Meshes
 *
 * \author xythobuz
 */

#include "global.h"
#include "Game.h"
#include "Render.h"
#include "TextureManager.h"
#include "World.h"
#include "utils/pixel.h"
#include "StaticMesh.h"

TexturedTriangle::TexturedTriangle(int i[3], float s[6], int tex, unsigned short trans) {
    index[0] = i[0];
    index[1] = i[1];
    index[2] = i[2];
    st[0] = s[0];
    st[1] = s[1];
    st[2] = s[2];
    st[3] = s[3];
    st[4] = s[4];
    st[5] = s[5];
    texture = tex;
    transparency = trans;
}

void TexturedTriangle::display(float* vertices, float* colors, float* normals) {
    /*
    assert(vertices != nullptr);

    if ((Render::getMode() != RenderMode::Wireframe)
        && (Render::getMode() != RenderMode::Solid)) {
        getTextureManager().bindTextureId(texture);
    }

    glBegin(GL_TRIANGLES);

    switch (Render::getMode()) {
        case RenderMode::Solid:
            //case RenderMode::VertexLight:
            if (colors != nullptr) {
                glColor3fv(colors + index[0]);
                glTexCoord2fv(st);
                glVertex3fv(vertices + (index[0] * 3));

                glColor3fv(colors + index[1]);
                glTexCoord2fv(st + 2);
                glVertex3fv(vertices + (index[1] * 3));

                glColor3fv(colors + index[2]);
                glTexCoord2fv(st + 4);
                glVertex3fv(vertices + (index[2] * 3));
            } else if (normals != nullptr) {
                glNormal3fv(normals + (index[0] * 3));
                glTexCoord2fv(st);
                glVertex3fv(vertices + (index[0] * 3));

                glNormal3fv(normals + (index[1] * 3));
                glTexCoord2fv(st + 2);
                glVertex3fv(vertices + (index[1] * 3));

                glNormal3fv(normals + (index[2] * 3));
                glTexCoord2fv(st + 4);
                glVertex3fv(vertices + (index[2] * 3));
            } else {
                glTexCoord2fv(st);
                glVertex3fv(vertices + (index[0] * 3));
                glTexCoord2fv(st + 2);
                glVertex3fv(vertices + (index[1] * 3));
                glTexCoord2fv(st + 4);
                glVertex3fv(vertices + (index[2] * 3));
            }
            break;

        case RenderMode::Wireframe:
            glVertex3fv(vertices + (index[0] * 3));
            glVertex3fv(vertices + (index[1] * 3));
            glVertex3fv(vertices + (index[2] * 3));
            break;

        default:
            glTexCoord2fv(st);
            glVertex3fv(vertices + (index[0] * 3));
            glTexCoord2fv(st + 2);
            glVertex3fv(vertices + (index[1] * 3));
            glTexCoord2fv(st + 4);
            glVertex3fv(vertices + (index[2] * 3));
    }

    glEnd();
    */
}

// ----------------------------------------------------------------------------

StaticMesh::StaticMesh(int i, int m) : id(i), mesh(m) {
    vertices = colors = normals = nullptr;
}

StaticMesh::~StaticMesh() {
    while (!triangles.empty()) {
        delete triangles.back();
        triangles.pop_back();
    }

    delete [] vertices;
    delete [] normals;
    delete [] colors;
}

void StaticMesh::display() {
    /*
    if ((id != -1) && (mesh != -1)) {
        getWorld().getMesh(mesh).drawSolid();
        return;
    }

    if (!dontshow) {
        //! \fixme Duh, vis tests need to be put back
        //if (!isVisible(center, radius, bbox))
        //   return;

        //! \fixme 'AMBIENT' -- Mongoose 2002.01.08
        glColor3ubv(WHITE);

        if (Render::getMode() == RenderMode::Wireframe)
            glColor3ubv(WHITE);

        getTextureManager().bindTextureId(TEXTURE_WHITE, TextureManager::TextureStorage::SYSTEM);

        for (unsigned int i = 0; i < triangles.size(); i++)
            triangles.at(i)->display(vertices, colors, normals);
    }
    */
}

float StaticMesh::getRadius() {
    assert((id == -1) && (mesh == -1));
    return radius;
}

