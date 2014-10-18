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

/*!
 * \brief OpenGL Mesh
 */
class Mesh {
  public:

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

    /*!
     * \brief Constructs an object of Mesh
     */
    Mesh();

    /*!
     * \brief Deconstructs an object of Mesh
     */
    ~Mesh();

    void drawAlpha();

    void drawSolid();

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
