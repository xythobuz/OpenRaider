/*!
 * \file src/Mesh.cpp
 * \brief OpenGL Mesh
 *
 * \author Mongoose
 */

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "Mesh.h"


////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

Mesh::Mesh()
{
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


Mesh::~Mesh()
{
    unsigned int i;


    if (mVertices)
    {
        delete [] mVertices;
    }

    if (mNormals)
    {
        delete [] mNormals;
    }

    if (mColors)
    {
        delete [] mColors;
    }

    if (mTris)
    {
        for (i = 0; i < mNumTris; ++i)
        {
            if (mTris[i].triangles)
                delete [] mTris[i].triangles;

            if (mTris[i].alpha_triangles)
                delete [] mTris[i].alpha_triangles;

            if (mTris[i].texcoors)
                delete [] mTris[i].texcoors;

            if (mTris[i].texcoors2)
                delete [] mTris[i].texcoors2;
        }

        delete [] mTris;
    }

    if (mQuads)
    {
        for (i = 0; i < mNumQuads; ++i)
        {
            if (mQuads[i].quads)
                delete [] mQuads[i].quads;

            if (mQuads[i].alpha_quads)
                delete [] mQuads[i].alpha_quads;

            if (mQuads[i].texcoors)
                delete [] mQuads[i].texcoors;

            if (mQuads[i].texcoors2)
                delete [] mQuads[i].texcoors2;
        }

        delete [] mQuads;
    }

    if (mVertexArray)
    {
        delete [] mVertexArray;
    }

    if (mNormalArray)
    {
        delete [] mNormalArray;
    }

    if (mColorArray)
    {
        delete [] mColorArray;
    }

    if (mTriangleTextures)
    {
        delete [] mTriangleTextures;
    }

    if (mTriangleIndices)
    {
        delete [] mTriangleIndices;
    }

    if (mTriangleFlags)
    {
        delete [] mTriangleFlags;
    }

    if (mTriangleTexCoordArray)
    {
        delete [] mTriangleTexCoordArray;
    }
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////

void Mesh::drawAlpha()
{
    unsigned int i, j, k, index;


    // Render quadralaterals
    for (mQuads ? i = 0 : i = mNumQuads; i < mNumQuads; ++i)
    {
        switch (mMode)
        {
            case MeshModeWireframe:
                glColor3f(0.0, 0.0, 1.0);
                glBindTexture(GL_TEXTURE_2D, 0);
                break;
            case MeshModeSolid:
                // Bind WHITE texture for solid colors
                glBindTexture(GL_TEXTURE_2D, 0);
                break;
            case MeshModeTexture:
            case MeshModeMultiTexture:
                // Bind texture id for textures
                glBindTexture(GL_TEXTURE_2D, mQuads[i].texture+1);
                break;
        }

        glBegin(GL_QUADS);

        for (j = 0; j < mQuads[i].num_alpha_quads; ++j)
        {
            for (k = 0; k < 4; ++k)
            {
                index = mQuads[i].alpha_quads[j*4+k];

                glTexCoord2fv(mQuads[i].texcoors2[j*4+k]);
                glColor4fv(mColors[index]);
                glVertex3fv(mVertices[index]);
            }
        }

        glEnd();
    }

    // Render triangles
    for (mTris ? i = 0 : i = mNumTris; i < mNumTris; ++i)
    {
        switch (mMode)
        {
            case MeshModeWireframe:
                glColor3f(0.0, 1.0, 0.0);
                glBindTexture(GL_TEXTURE_2D, 0);
                break;
            case MeshModeSolid:
                // Bind WHITE texture for solid colors
                glBindTexture(GL_TEXTURE_2D, 0);
                break;
            case MeshModeTexture:
            case MeshModeMultiTexture:
                // Bind texture id for textures
                glBindTexture(GL_TEXTURE_2D, mTris[i].texture+1);
                break;
        }

        glBegin(GL_TRIANGLES);

        for (j = 0; j < mTris[i].num_alpha_triangles; ++j)
        {
            for (k = 0; k < 3; ++k)
            {
                index = mTris[i].alpha_triangles[j*3+k];

                glTexCoord2fv(mTris[i].texcoors2[j*3+k]);
                glColor4fv(mColors[index]);
                glVertex3fv(mVertices[index]);
            }
        }

        glEnd();
    }
}


void Mesh::drawSolid()
{
    unsigned int i, j, k, index;


    if (mFlags & fMesh_UseVertexArray)
    {
        //glEnableClientState(GL_VERTEX_ARRAY);
        //glVertexPointer(3, GL_FLOAT, 0, mVertexArray);

        glPointSize(2.0f);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLES);

        for (i = 0; i < mTriangleCount*3; ++i)
        {
            //glArrayElement(mTriangleIndices[i]);
            glVertex3fv(mVertexArray+mTriangleIndices[i]);
        }

        glEnd();

        glPointSize(1.0f);

        //! \fixme
        /*
        for (j = 0; j < mQuads[i].num_quads; ++j)
        {
            for (k = 0; k < 4; ++k)
            {
                index = mQuads[i].quads[j*4+k];

                glTexCoord2fv(mQuads[i].texcoors[j*4+k]);
                glArrayElement(mQuads[i].quads[j*4+k]);
            }
        }
        */

        return;
    }

    // Render quadralaterals
    for (mQuads ? i = 0 : i = mNumQuads; i < mNumQuads; ++i)
    {
        switch (mMode)
        {
            case MeshModeSolid:
                glColor3f(0.0, 0.0, 0.0);
                break;
            case MeshModeWireframe:
                // Bind WHITE texture for solid colors
                glBindTexture(GL_TEXTURE_2D, 0);
                break;
#ifdef MULTITEXTURE
            case MeshModeMultiTexture:
                glActiveTextureARB(GL_TEXTURE0_ARB);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, mQuads[i].texture+1);

                glActiveTextureARB(GL_TEXTURE1_ARB);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, mQuads[i].bumpmap+1);
                break;
#else
            case MeshModeMultiTexture:
#endif
            case MeshModeTexture:
                // Bind texture id for textures
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                glBindTexture(GL_TEXTURE_2D, mQuads[i].texture+1);
                break;
        }

        glBegin(GL_QUADS);

        for (j = 0; j < mQuads[i].num_quads; ++j)
        {
            for (k = 0; k < 4; ++k)
            {
                index = mQuads[i].quads[j*4+k];

                glColor4fv(mColors[index]);

#ifdef MULTITEXTURE
                if (mMode == MeshModeMultiTexture)
                {
                    glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,
                            mQuads[i].texcoors[j*4+k]);
                    glMultiTexCoord2fvARB(GL_TEXTURE1_ARB,
                            mQuads[i].texcoors[j*4+k]);
                }
                else
#endif
                    glTexCoord2fv(mQuads[i].texcoors[j*4+k]);

                glVertex3fv(mVertices[index]);
            }
        }

        glEnd();
    }

    // Render triangles
    for (mTris ? i = 0 : i = mNumTris; i < mNumTris; ++i)
    {
        switch (mMode)
        {
            case MeshModeSolid:
                glColor3f(1.0, 0.0, 0.0);
                break;
            case MeshModeWireframe:
                // Bind WHITE texture for solid colors
                glBindTexture(GL_TEXTURE_2D, 0);
                break;
#ifdef MULTITEXTURE
            case MeshModeMultiTexture:
                glActiveTextureARB(GL_TEXTURE0_ARB);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, mTris[i].texture+1);

                glActiveTextureARB(GL_TEXTURE1_ARB);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, mTris[i].bumpmap+1);
                break;
#else
            case MeshModeMultiTexture:
#endif
            case MeshModeTexture:
                // Bind texture id for textures
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                glBindTexture(GL_TEXTURE_2D, mTris[i].texture+1);
                break;
        }

        glBegin(GL_TRIANGLES);

        for (j = 0; j < mTris[i].num_triangles; ++j)
        {
            for (k = 0; k < 3; ++k)
            {
                index = mTris[i].triangles[j*3+k];

#ifdef MULTITEXTURE
                if (mMode == MeshModeMultiTexture)
                {
                    glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,
                            mTris[i].texcoors[j*3+k]);
                    glMultiTexCoord2fvARB(GL_TEXTURE1_ARB,
                            mTris[i].texcoors[j*3+k]);
                }
                else
#endif
                    glTexCoord2fv(mTris[i].texcoors[j*3+k]);

                glColor4fv(mColors[index]);
                glVertex3fv(mVertices[index]);
            }
        }

        glEnd();
    }

#ifdef MULTITEXTURE
    if (mMode == MeshModeMultiTexture)
    {
        glDisable(GL_TEXTURE_2D);
        glActiveTextureARB(GL_TEXTURE0_ARB);
    }
#endif
}


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

void Mesh::allocateColors(unsigned int n)
{
    if (mColors)
    {
        mNumColors = 0;
        delete [] mColors;
    }

    if (!n)
    {
        return;
    }

    mNumColors = n;
    mColors = new vec4_t[mNumColors];
}


void Mesh::allocateNormals(unsigned int n)
{
    if (mNormals)
    {
        mNumNormals = 0;
        delete [] mNormals;
    }

    if (!n)
    {
        return;
    }

    mNumNormals = n;
    mNormals = new vec3_t[mNumNormals];
}


void Mesh::allocateRectangles(unsigned int n)
{
    if (mQuads)
    {
        mNumQuads = 0;
        delete [] mQuads;
    }

    if (!n)
    {
        return;
    }

    mNumQuads = n;
    mQuads = new rect_t[mNumQuads];
}


void Mesh::allocateTriangles(unsigned int n)
{
    if (mTris)
    {
        mNumTris = 0;
        delete [] mTris;
    }

    if (!n)
    {
        return;
    }

    mNumTris = n;
    mTris = new tris_t[mNumTris];
}


void Mesh::allocateVertices(unsigned int n)
{
    if (mVertices)
    {
        mNumVertices = 0;
        delete [] mVertices;
    }

    if (!n)
    {
        return;
    }

    mNumVertices = n;
    mVertices = new vec3_t[mNumVertices];
}


void Mesh::bufferColorArray(unsigned int colorCount, vec_t *colors)
{
    if (mColors)
    {
        mNumColors = 0;
        delete [] mColors;
    }

    if (!colorCount)
    {
        return;
    }

    mNumColors = colorCount;
    mColorArray = colors;
}


void Mesh::bufferNormalArray(unsigned int normalCount, vec_t *normals)
{
    if (mNormals)
    {
        mNumNormals = 0;
        delete [] mNormals;
    }

    if (!normalCount)
    {
        return;
    }

    mNumNormals = normalCount;
    mNormalArray = normals;
}


void Mesh::bufferTriangles(unsigned int count,
        unsigned int *indices, vec_t *texCoords,
        int *textures, unsigned int *flags)
{

    mTriangleCount = count;
    mTriangleTextures = textures;
    mTriangleIndices = indices;
    mTriangleFlags = flags;
    mTriangleTexCoordArray = texCoords;

    //! \fixme sortTrianglesByTexture();
}


void Mesh::bufferVertexArray(unsigned int vertexCount, vec_t *vertices)
{
    if (mVertices)
    {
        mNumVertices = 0;
        delete [] mVertices;
    }

    if (!vertexCount)
    {
        return;
    }

    mNumVertices = vertexCount;
    mVertexArray = vertices;
    mFlags |= fMesh_UseVertexArray;
}


void Mesh::setColor(unsigned int index,
        float r, float g, float b, float a)
{
    if (index > mNumColors)
    {
        return;
    }

    mColors[index][0] = r;
    mColors[index][1] = g;
    mColors[index][2] = b;
    mColors[index][3] = a;
}


void Mesh::setColor(unsigned int index, float rgba[4])
{
    if (index > mNumColors)
    {
        return;
    }

    mColors[index][0] = rgba[0];
    mColors[index][1] = rgba[1];
    mColors[index][2] = rgba[2];
    mColors[index][3] = rgba[3];
}


void Mesh::setNormal(unsigned int index, float i, float j, float k)
{
    if (index > mNumNormals)
    {
        return;
    }

    mNormals[index][0] = i;
    mNormals[index][1] = j;
    mNormals[index][2] = k;
}


void Mesh::setVertex(unsigned int index, float x, float y, float z)
{
    if (index > mNumVertices)
    {
        return;
    }

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

