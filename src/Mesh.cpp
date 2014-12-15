/*!
 * \file src/Mesh.cpp
 * \brief OpenGL Mesh
 *
 * \author Mongoose
 */

#include <stdlib.h>

#include "global.h"
#include "TextureManager.h"
#include "Mesh.h"

void Mesh::addTexturedRectangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
                                uint16_t textile) {
    texturedRectangles.emplace_back(a, b, c, d, textile);
}

void Mesh::addTexturedTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, uint16_t textile) {
    texturedTriangles.emplace_back(a, b, c, glm::vec3(), textile);
}

void Mesh::addColoredRectangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, float re,
                               float gr, float bl) {
    coloredRectangles.emplace_back(a, b, c, d, -1, re, gr, bl);
}

void Mesh::addColoredTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, float re, float gr, float bl) {
    coloredTriangles.emplace_back(a, b, c, glm::vec3(), -1, re, gr, bl);
}

void Mesh::addNormal(glm::vec3 n) {
    normals.emplace_back(n);
}

void Mesh::drawAlpha() {
    if ((texturedRectangles.size() == 0)
        && (texturedTriangles.size() == 0)
        && (coloredRectangles.size() == 0)
        && (coloredTriangles.size() == 0)
        && (normals.size() == 0)) {
        drawAlphaOld();
        return;
    }

    // TODO
}

void Mesh::drawSolid() {
    if ((texturedRectangles.size() == 0)
        && (texturedTriangles.size() == 0)
        && (coloredRectangles.size() == 0)
        && (coloredTriangles.size() == 0)
        && (normals.size() == 0)) {
        drawSolidOld();
        return;
    }

    for (auto& q : texturedRectangles) {
        if (mMode == MeshModeWireframe) {
            getTextureManager().bindTextureId(TEXTURE_WHITE, TextureManager::TextureStorage::SYSTEM);
            /*
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(q.a.x, q.a.y, q.a.z);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(q.b.x, q.b.y, q.b.z);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(q.c.x, q.c.y, q.c.z);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(q.d.x, q.d.y, q.d.z);
            glEnd();
            */
        } else if (mMode == MeshModeSolid) {
            // TODO
        } else if (mMode == MeshModeTexture) {
            //getTextureManager().getTile(q.texture).displayRectangle(q.a, q.b, q.c, q.d);
        }
    }

    for (auto& t : texturedTriangles) {
        if (mMode == MeshModeWireframe) {
            getTextureManager().bindTextureId(TEXTURE_WHITE, TextureManager::TextureStorage::SYSTEM);
            /*
            glBegin(GL_TRIANGLES);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(t.a.x, t.a.y, t.a.z);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(t.b.x, t.b.y, t.b.z);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(t.c.x, t.c.y, t.c.z);
            glEnd();
            */
        } else if (mMode == MeshModeSolid) {
            // TODO
        } else if (mMode == MeshModeTexture) {
            //getTextureManager().getTile(t.texture).displayTriangle(t.a, t.b, t.c);
        }
    }

    for (auto& q : coloredRectangles) {
        if (mMode == MeshModeWireframe) {
            getTextureManager().bindTextureId(TEXTURE_WHITE, TextureManager::TextureStorage::SYSTEM);
            /*
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(q.a.x, q.a.y, q.a.z);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(q.b.x, q.b.y, q.b.z);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(q.c.x, q.c.y, q.c.z);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(q.d.x, q.d.y, q.d.z);
            glEnd();
            */
        } else if (mMode == MeshModeSolid) {
            // TODO
        } else if (mMode == MeshModeTexture) {
            // TODO
        }
    }

    for (auto& t : coloredTriangles) {
        if (mMode == MeshModeWireframe) {
            getTextureManager().bindTextureId(TEXTURE_WHITE, TextureManager::TextureStorage::SYSTEM);
            /*
            glBegin(GL_TRIANGLES);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(t.a.x, t.a.y, t.a.z);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(t.b.x, t.b.y, t.b.z);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(t.c.x, t.c.y, t.c.z);
            glEnd();
            */
        } else if (mMode == MeshModeSolid) {
            // TODO
        } else if (mMode == MeshModeTexture) {
            // TODO
        }
    }
}


////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

Mesh::Mesh() {
    mNumVertices = 0;
    mVertices = 0x0;

    mNumNormals = 0;
    mNormals = 0x0;

    mNumColors = 0;
    mColors = 0x0;

    mNumTris = 0;
    mTris = 0x0;

    mNumQuads = 0;
    mQuads = 0x0;

    mVertexArray = 0x0;
    mNormalArray = 0x0;
    mColorArray = 0x0;

    mTriangleCount = 0;
    mTriangleTextures = 0x0;
    mTriangleIndices = 0x0;
    mTriangleFlags = 0x0;
    mTriangleTexCoordArray = 0x0;

    mFlags = 0;
    mMode = MeshModeTexture;
}


Mesh::~Mesh() {
    for (unsigned int i = 0; i < mNumVertices; i++)
        delete [] mVertices[i];
    delete [] mVertices;
    mVertices = nullptr;

    for (unsigned int i = 0; i < mNumNormals; i++)
        delete [] mNormals[i];
    delete [] mNormals;
    mNormals = nullptr;

    for (unsigned int i = 0; i < mNumColors; i++)
        delete [] mColors[i];
    delete [] mColors;
    mColors = nullptr;

    if (mTris) {
        for (unsigned int i = 0; i < mNumTris; ++i) {
            delete [] mTris[i].triangles;
            delete [] mTris[i].alpha_triangles;
            delete [] mTris[i].texcoors;
            delete [] mTris[i].texcoors2;
        }

        delete [] mTris;
        mTris = nullptr;
    }

    if (mQuads) {
        for (unsigned int i = 0; i < mNumQuads; ++i) {
            delete [] mQuads[i].quads;
            delete [] mQuads[i].alpha_quads;
            delete [] mQuads[i].texcoors;
            delete [] mQuads[i].texcoors2;
        }

        delete [] mQuads;
        mQuads = nullptr;
    }

    delete [] mVertexArray;
    mVertexArray = nullptr;

    delete [] mNormalArray;
    mNormalArray = nullptr;

    delete [] mColorArray;
    mColorArray = nullptr;

    delete [] mTriangleTextures;
    mTriangleTextures = nullptr;

    delete [] mTriangleIndices;
    mTriangleIndices = nullptr;

    delete [] mTriangleFlags;
    mTriangleFlags = nullptr;

    delete [] mTriangleTexCoordArray;
    mTriangleTexCoordArray = nullptr;
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////

void Mesh::drawAlphaOld() {
    unsigned int i, j, k, index;

    /*

    // Render quadralaterals
    for (mQuads ? i = 0 : i = mNumQuads; i < mNumQuads; ++i) {
        switch (mMode) {
            case MeshModeWireframe:
                glColor3f(0.0, 0.0, 1.0);
                getTextureManager().bindTextureId(TEXTURE_WHITE, TextureManager::TextureStorage::SYSTEM);
                break;
            case MeshModeSolid:
                // Bind WHITE texture for solid colors
                getTextureManager().bindTextureId(TEXTURE_WHITE, TextureManager::TextureStorage::SYSTEM);
                break;
            case MeshModeTexture:
            case MeshModeMultiTexture:
                // Bind texture id for textures
                getTextureManager().bindTextureId(mQuads[i].texture);
                break;
        }

        glBegin(GL_QUADS);

        for (j = 0; j < mQuads[i].num_alpha_quads; ++j) {
            for (k = 0; k < 4; ++k) {
                index = mQuads[i].alpha_quads[j * 4 + k];

                glTexCoord2fv(mQuads[i].texcoors2[j * 4 + k]);
                glColor4fv(mColors[index]);
                glVertex3fv(mVertices[index]);
            }
        }

        glEnd();
    }

    // Render triangles
    for (mTris ? i = 0 : i = mNumTris; i < mNumTris; ++i) {
        switch (mMode) {
            case MeshModeWireframe:
                glColor3f(0.0, 1.0, 0.0);
                getTextureManager().bindTextureId(TEXTURE_WHITE, TextureManager::TextureStorage::SYSTEM);
                break;
            case MeshModeSolid:
                // Bind WHITE texture for solid colors
                getTextureManager().bindTextureId(TEXTURE_WHITE, TextureManager::TextureStorage::SYSTEM);
                break;
            case MeshModeTexture:
            case MeshModeMultiTexture:
                // Bind texture id for textures
                getTextureManager().bindTextureId(mTris[i].texture);
                break;
        }

        glBegin(GL_TRIANGLES);

        for (j = 0; j < mTris[i].num_alpha_triangles; ++j) {
            for (k = 0; k < 3; ++k) {
                index = mTris[i].alpha_triangles[j * 3 + k];

                glTexCoord2fv(mTris[i].texcoors2[j * 3 + k]);
                glColor4fv(mColors[index]);
                glVertex3fv(mVertices[index]);
            }
        }

        glEnd();
    }

    */
}


void Mesh::drawSolidOld() {
    unsigned int i, j, k, index;

    /*


    // Render quadralaterals
    for (mQuads ? i = 0 : i = mNumQuads; i < mNumQuads; ++i) {
        switch (mMode) {
            case MeshModeSolid:
                glColor3f(0.0, 0.0, 0.0);
                break;
            case MeshModeWireframe:
                // Bind WHITE texture for solid colors
                getTextureManager().bindTextureId(TEXTURE_WHITE, TextureManager::TextureStorage::SYSTEM);
                break;
#ifdef MULTITEXTURE
            case MeshModeMultiTexture:
                glActiveTextureARB(GL_TEXTURE0_ARB);
                glEnable(GL_TEXTURE_2D);
                getTextureManager().bindTextureId(mQuads[i].texture);

                glActiveTextureARB(GL_TEXTURE1_ARB);
                glEnable(GL_TEXTURE_2D);
                getTextureManager().bindTextureId(mQuads[i].bumpmap);
                break;
#else
            case MeshModeMultiTexture:
#endif
            case MeshModeTexture:
                // Bind texture id for textures
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                getTextureManager().bindTextureId(mQuads[i].texture);
                break;
        }

        glBegin(GL_QUADS);

        for (j = 0; j < mQuads[i].num_quads; ++j) {
            for (k = 0; k < 4; ++k) {
                index = mQuads[i].quads[j * 4 + k];

                glColor4fv(mColors[index]);

#ifdef MULTITEXTURE
                if (mMode == MeshModeMultiTexture) {
                    glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,
                                          mQuads[i].texcoors[j * 4 + k]);
                    glMultiTexCoord2fvARB(GL_TEXTURE1_ARB,
                                          mQuads[i].texcoors[j * 4 + k]);
                } else
#endif
                    glTexCoord2fv(mQuads[i].texcoors[j * 4 + k]);

                glVertex3fv(mVertices[index]);
            }
        }

        glEnd();
    }

    // Render triangles
    for (mTris ? i = 0 : i = mNumTris; i < mNumTris; ++i) {
        switch (mMode) {
            case MeshModeSolid:
                glColor3f(1.0, 0.0, 0.0);
                break;
            case MeshModeWireframe:
                // Bind WHITE texture for solid colors
                getTextureManager().bindTextureId(TEXTURE_WHITE, TextureManager::TextureStorage::SYSTEM);
                break;
#ifdef MULTITEXTURE
            case MeshModeMultiTexture:
                glActiveTextureARB(GL_TEXTURE0_ARB);
                glEnable(GL_TEXTURE_2D);
                getTextureManager().bindTextureId(mTris[i].texture);

                glActiveTextureARB(GL_TEXTURE1_ARB);
                glEnable(GL_TEXTURE_2D);
                getTextureManager().bindTextureId(mTris[i].bumpmap);
                break;
#else
            case MeshModeMultiTexture:
#endif
            case MeshModeTexture:
                // Bind texture id for textures
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                getTextureManager().bindTextureId(mTris[i].texture);
                break;
        }

        glBegin(GL_TRIANGLES);

        for (j = 0; j < mTris[i].num_triangles; ++j) {
            for (k = 0; k < 3; ++k) {
                index = mTris[i].triangles[j * 3 + k];

#ifdef MULTITEXTURE
                if (mMode == MeshModeMultiTexture) {
                    glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,
                                          mTris[i].texcoors[j * 3 + k]);
                    glMultiTexCoord2fvARB(GL_TEXTURE1_ARB,
                                          mTris[i].texcoors[j * 3 + k]);
                } else
#endif
                    glTexCoord2fv(mTris[i].texcoors[j * 3 + k]);

                glColor4fv(mColors[index]);
                glVertex3fv(mVertices[index]);
            }
        }

        glEnd();
    }

#ifdef MULTITEXTURE
    if (mMode == MeshModeMultiTexture) {
        glDisable(GL_TEXTURE_2D);
        glActiveTextureARB(GL_TEXTURE0_ARB);
    }
#endif

    */
}


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

void Mesh::allocateColors(unsigned int n) {
    if (mColors) {
        for (unsigned int i = 0; i < mNumColors; i++)
            delete [] mColors[i];
        delete [] mColors;
        mNumColors = 0;
    }

    if (!n) {
        return;
    }

    mNumColors = n;
    mColors = new float *[mNumColors];
    for (unsigned int i = 0; i < mNumColors; i++)
        mColors[i] = new float[4];
}


void Mesh::allocateNormals(unsigned int n) {
    if (mNormals) {
        for (unsigned int i = 0; i < mNumNormals; i++)
            delete [] mNormals[i];
        delete [] mNormals;
        mNumNormals = 0;
    }

    if (!n) {
        return;
    }

    mNumNormals = n;
    mNormals = new float *[mNumNormals];
    for (unsigned int i = 0; i < mNumNormals; i++)
        mNormals[i] = new float[3];
}


void Mesh::allocateRectangles(unsigned int n) {
    if (mQuads) {
        mNumQuads = 0;
        delete [] mQuads;
    }

    if (!n) {
        return;
    }

    mNumQuads = n;
    mQuads = new rect_t[mNumQuads];
}


void Mesh::allocateTriangles(unsigned int n) {
    if (mTris) {
        mNumTris = 0;
        delete [] mTris;
    }

    if (!n) {
        return;
    }

    mNumTris = n;
    mTris = new tris_t[mNumTris];
}


void Mesh::allocateVertices(unsigned int n) {
    if (mVertices) {
        for (unsigned int i = 0; i < mNumVertices; i++)
            delete [] mVertices[i];
        delete [] mVertices;
        mNumVertices = 0;
    }

    if (!n) {
        return;
    }

    mNumVertices = n;
    mVertices = new float *[mNumVertices];
    for (unsigned int i = 0; i < mNumVertices; i++)
        mVertices[i] = new float[3];
}


void Mesh::bufferColorArray(unsigned int colorCount, float* colors) {
    if (mColors) {
        for (unsigned int i = 0; i < mNumColors; i++)
            delete [] mColors[i];
        delete [] mColors;
        mNumColors = 0;
    }

    if (!colorCount) {
        return;
    }

    mNumColors = colorCount;
    mColorArray = colors;
}


void Mesh::bufferNormalArray(unsigned int normalCount, float* normals) {
    if (mNormals) {
        for (unsigned int i = 0; i < mNumNormals; i++)
            delete [] mNormals[i];
        delete [] mNormals;
        mNumNormals = 0;
    }

    if (!normalCount) {
        return;
    }

    mNumNormals = normalCount;
    mNormalArray = normals;
}


void Mesh::bufferTriangles(unsigned int count,
                           unsigned int* indices, float* texCoords,
                           int* textures, unsigned int* flags) {

    mTriangleCount = count;
    mTriangleTextures = textures;
    mTriangleIndices = indices;
    mTriangleFlags = flags;
    mTriangleTexCoordArray = texCoords;

    //! \fixme sortTrianglesByTexture();
}


void Mesh::bufferVertexArray(unsigned int vertexCount, float* vertices) {
    if (mVertices) {
        for (unsigned int i = 0; i < mNumVertices; i++)
            delete [] mVertices[i];
        delete [] mVertices;
        mNumVertices = 0;
    }

    if (!vertexCount) {
        return;
    }

    mNumVertices = vertexCount;
    mVertexArray = vertices;
    mFlags |= fMesh_UseVertexArray;
}


void Mesh::setColor(unsigned int index,
                    float r, float g, float b, float a) {
    assert(index < mNumColors);

    mColors[index][0] = r;
    mColors[index][1] = g;
    mColors[index][2] = b;
    mColors[index][3] = a;
}


void Mesh::setColor(unsigned int index, float rgba[4]) {
    assert(index < mNumColors);

    mColors[index][0] = rgba[0];
    mColors[index][1] = rgba[1];
    mColors[index][2] = rgba[2];
    mColors[index][3] = rgba[3];
}


void Mesh::setNormal(unsigned int index, float i, float j, float k) {
    assert(index < mNumNormals);

    mNormals[index][0] = i;
    mNormals[index][1] = j;
    mNormals[index][2] = k;
}


void Mesh::setVertex(unsigned int index, float x, float y, float z) {
    assert(index < mNumVertices);

    mVertices[index][0] = x;
    mVertices[index][1] = y;
    mVertices[index][2] = z;
}


////////////////////////////////////////////////////////////
// Private Accessors
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Private Mutators
////////////////////////////////////////////////////////////

