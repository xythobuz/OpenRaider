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
    assert(vertices != NULL);

    if ((getRender().getMode() != Render::modeWireframe)
        && (getRender().getMode() != Render::modeSolid)) {
        getTextureManager().bindTextureId(texture);
    }

    glBegin(GL_TRIANGLES);

    switch (getRender().getMode()) {
        case Render::modeSolid:
        case Render::modeVertexLight:
            if (colors != NULL) {
                glColor3fv(colors + index[0]);
                glTexCoord2fv(st);
                glVertex3fv(vertices + (index[0] * 3));

                glColor3fv(colors + index[1]);
                glTexCoord2fv(st + 2);
                glVertex3fv(vertices + (index[1] * 3));

                glColor3fv(colors + index[2]);
                glTexCoord2fv(st + 4);
                glVertex3fv(vertices + (index[2] * 3));
            } else if (normals != NULL) {
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

        case Render::modeWireframe:
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
}

// ----------------------------------------------------------------------------

StaticMesh::StaticMesh(int i, int m) : id(i), mesh(m) { }

#ifdef EXPERIMENTAL

#include <map>
std::map<unsigned int, unsigned int> gColorTextureHACK;

int setupTextureColor(float* colorf) {
    unsigned char color[4];
    unsigned int colorI;
    unsigned int texture;

    color[0] = (unsigned char)(colorf[0] * 255.0f);
    color[1] = (unsigned char)(colorf[1] * 255.0f);
    color[2] = (unsigned char)(colorf[2] * 255.0f);
    color[3] = (unsigned char)(colorf[3] * 255.0f);

    ((unsigned char*)(&colorI))[3] = color[0];
    ((unsigned char*)(&colorI))[2] = color[1];
    ((unsigned char*)(&colorI))[1] = color[2];
    ((unsigned char*)(&colorI))[0] = color[3];

    try {
        texture = gColorTextureHACK.at(colorI);
    } catch (...) {
        unsigned char* image = generateColorTexture(color, 32, 32, 32);
        texture = getTextureManager().loadBufferSlot(image, 32, 32, RGBA, 32);
        delete [] image;
    }

    return texture;
}

#endif

StaticMesh::StaticMesh(TombRaider& tr, unsigned int index) {
    int count, texture;
    int vertexIndices[6];
    float st[12];
    float color[4];
    unsigned short transparency;

    if (!tr.isMeshValid(index)) {
        dontshow = true;
        return;
    } else {
        dontshow = false;
    }

    // Mongoose 2002.08.30, Testing support for 'shootable' models ( traceable )
    tr.getMeshCollisionInfo(index, center, &radius);

    //! \fixme Arrays don't work either  =)
    // Mesh geometery, colors, etc
    tr.getMeshVertexArrays(index,
                           &vertexCount, &vertices,
                           &normalCount, &normals,
                           &colorCount,  &colors);

    // Textured Triangles
    count = tr.getMeshTexturedTriangleCount(index);
    for (int i = 0; i < count; i++) {
        tr.getMeshTexturedTriangle(index, i,
                                   vertexIndices, st,
                                   &texture, &transparency);
        triangles.push_back(
            new TexturedTriangle(vertexIndices, st, texture, transparency));
    }

    // Coloured Triangles
    count = tr.getMeshColoredTriangleCount(index);
    for (int i = 0; i < count; i++) {
        tr.getMeshColoredTriangle(index, i,
                                  vertexIndices, color);
        st[0] = color[0];
        st[1] = color[1];
        st[2] = color[2];
        st[3] = color[3];
        st[4] = 1.0;
        st[5] = 1.0;

#ifdef EXPERIMENTAL
        texture = setupTextureColor(color);
#else
        texture = 0; // White texture
#endif
        transparency = 0;

        triangles.push_back(
            new TexturedTriangle(vertexIndices, st, texture, transparency));
    }

    // Textured Rectangles
    count = tr.getMeshTexturedRectangleCount(index);
    for (int i = 0; i < count; i++) {
        tr.getMeshTexturedRectangle(index, i,
                                    vertexIndices, st,
                                    &texture, &transparency);
        triangles.push_back(
            new TexturedTriangle(vertexIndices, st, texture, transparency));
        triangles.push_back(
            new TexturedTriangle(vertexIndices + 3, st + 6, texture, transparency));
    }

    // Coloured Rectangles
    count = tr.getMeshColoredRectangleCount(index);
    for (int i = 0; i < count; i++) {
        tr.getMeshColoredRectangle(index, i,
                                   vertexIndices, color);

        st[0] = color[0];
        st[1] = color[1];
        st[2] = color[2];
        st[3] = color[3];
        st[4] = 1.0;
        st[5] = 1.0;

#ifdef EXPERIMENTAL
        texture = setupTextureColor(color);
#else
        texture = 0; // White texture
#endif
        transparency = 0;

        triangles.push_back(
            new TexturedTriangle(vertexIndices, st, texture, transparency));
        triangles.push_back(
            new TexturedTriangle(vertexIndices + 3, st, texture, transparency));
    }

    id = mesh = -1;
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

        if (getRender().getMode() == Render::modeWireframe)
            glColor3ubv(WHITE);

        getTextureManager().bindTextureId(TEXTURE_WHITE, TextureManager::TextureStorage::SYSTEM);

        for (unsigned int i = 0; i < triangles.size(); i++)
            triangles.at(i)->display(vertices, colors, normals);
    }
}

float StaticMesh::getRadius() {
    assert((id != -1) && (mesh != -1));
    return radius;
}

