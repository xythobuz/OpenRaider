/*!
 * \file include/Mesh.h
 * \brief OpenGL Mesh
 *
 * \author Mongoose
 *
 * \todo Unify the parallel systems here, arrays and the allocate/set
 */

#ifndef _MESH_H_
#define _MESH_H_

#include "math/Vec3.h"

/*!
 * \brief OpenGL Mesh
 */
class Mesh {
  public:

    struct rectangle_t {
        Vec3 a, b, c, d;
        uint16_t texture;
        float red, green, blue;

        rectangle_t(Vec3 _a, Vec3 _b, Vec3 _c, Vec3 _d, uint16_t t,
                    float re = 0.0f, float gr = 0.0f, float bl = 0.0f)
            : a(_a), b(_b), c(_c), d(_d), texture(t), red(re), green(gr), blue(bl) { }
    };

    Mesh();
    ~Mesh();

    void drawAlpha();
    void drawSolid();

    void addTexturedRectangle(Vec3 a, Vec3 b, Vec3 c, Vec3 d, uint16_t textile);
    void addTexturedTriangle(Vec3 a, Vec3 b, Vec3 c, uint16_t textile);

    void addColoredRectangle(Vec3 a, Vec3 b, Vec3 c, Vec3 d, float re, float gr, float bl);
    void addColoredTriangle(Vec3 a, Vec3 b, Vec3 c, float re, float gr, float bl);

    void addNormal(Vec3 n);

    std::vector<rectangle_t> texturedRectangles;
    std::vector<rectangle_t> coloredRectangles;
    std::vector<rectangle_t> texturedTriangles;
    std::vector<rectangle_t> coloredTriangles;
    std::vector<Vec3> normals;


    // Old API

    typedef enum {
        MeshModeSolid,
        MeshModeWireframe,
        MeshModeTexture,
        MeshModeMultiTexture
    } MeshMode;

    typedef enum {
        fMesh_UseVertexArray = (1 << 0)
    } MeshFlags;

    typedef struct {
        int texture;
#ifdef MULTITEXTURE
        int bumpmap;
#endif

        unsigned int cnum_triangles;
        unsigned int cnum_alpha_triangles;

        unsigned int num_texcoors;
        float** texcoors; // 2D

        unsigned int num_texcoors2;
        float** texcoors2; // 2D

        //! Arrays of triangle indices sorted by texture
        unsigned int num_triangles;
        unsigned int* triangles; //!< ABCABCABC...

        //! Arrays of alpha triangle indices sorted by texture
        unsigned int num_alpha_triangles;
        unsigned int* alpha_triangles; //!< ABCABCABC...
    } tris_t;

    typedef struct {

        int texture;
#ifdef MULTITEXTURE
        int bumpmap;
#endif

        unsigned int cnum_quads;
        unsigned int cnum_alpha_quads;

        unsigned int num_texcoors;
        float** texcoors; // 2D

        unsigned int num_texcoors2;
        float** texcoors2; // 2D

        //! Arrays of rectangle indices sorted by texture
        unsigned int num_quads;
        unsigned int* quads; //!< ABCDABCDABCD...

        //! Arrays of alpha rectangle indices sorted by texture
        unsigned int num_alpha_quads;
        unsigned int* alpha_quads; //!< ABCDABCDABCD...

    } rect_t;

    void drawAlphaOld();
    void drawSolidOld();

    void allocateColors(unsigned int n);

    void allocateNormals(unsigned int n);

    void allocateRectangles(unsigned int n);

    void allocateTriangles(unsigned int n);

    void allocateVertices(unsigned int n);

    void bufferColorArray(unsigned int colorCount, float* colors);

    void bufferNormalArray(unsigned int normalCount, float* normals);

    void bufferTriangles(unsigned int count,
                         unsigned int* indices, float* texCoords,
                         int* textures, unsigned int* flags);

    void bufferVertexArray(unsigned int vertexCount, float* vertices);

    void setColor(unsigned int index, float r, float g, float b, float a);

    void setColor(unsigned int index, float rgba[4]);

    void setNormal(unsigned int index, float i, float j, float k);

    void setVertex(unsigned int index, float x, float y, float z);

#if 0
    void sortFacesByTexture();

    void addFace(int textureIndex, int textureIndexB, unsigned int flags,
                 unsigned int vertexIndexCount, float* vertexIndices);

    void addTexTiledFace(int textureIndex, int textureIndexB,
                         unsigned int flags, unsigned int indexCount,
                         float* vertexIndices, float* texcoords);

    void bufferTexcoords(unsigned int texcoordCount, float* texcoords);

    void duplicateArraysForTexTiledTexcoords();
#endif

    unsigned int mFlags;

    MeshMode mMode;

    unsigned int mNumVertices;
    float** mVertices; //!< XYZ

    unsigned int mNumNormals;
    float** mNormals; //!< IJK

    unsigned int mNumColors;
    float** mColors; //!< RGBA

    unsigned int mNumTris;
    tris_t* mTris;

    unsigned int mNumQuads;
    rect_t* mQuads;

    unsigned int mTriangleCount;
    int* mTriangleTextures;
    unsigned int* mTriangleIndices;
    unsigned int* mTriangleFlags;
    float* mTriangleTexCoordArray;

    float* mVertexArray;
    float* mNormalArray;
    float* mColorArray;
};

#endif
