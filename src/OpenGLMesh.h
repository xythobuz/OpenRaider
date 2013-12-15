/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : OpenRaider
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : OpenGLMesh
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments:
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- Test Defines -----------------------------------------------
 *
 * UNIT_TEST_OPENGLMESH - Builds OpenGLMesh class as a console unit test
 *
 *-- History ------------------------------------------------
 *
 * 2002.08.23:
 * Mongoose - Created
 ================================================================*/


#ifndef GUARD__OPENRAIDER_MONGOOSE_OPENGLMESH_H_
#define GUARD__OPENRAIDER_MONGOOSE_OPENGLMESH_H_

#include "hel/math.h"

// TODO: Unify the parallel systems here, arrays and the allocate/set


typedef enum
{
	OpenGLMeshModeSolid          = 0,
	OpenGLMeshModeWireframe      = 1,
	OpenGLMeshModeTexture        = 2,
	OpenGLMeshModeMultiTexture   = 3

} OpenGLMeshMode;

enum OpenGLMeshFlags
{
	fOpenGLMesh_Transparent      = 1,
	fOpenGLMesh_BumpMap          = 2,
	fOpenGLMesh_UseVertexArray   = 4
};


typedef struct tris_s
{
	int texture;
#ifdef MULTITEXTURE
	int bumpmap;
#endif

	unsigned int cnum_triangles;
	unsigned int cnum_alpha_triangles;

	unsigned int num_texcoors;
	vec2_t *texcoors;

	unsigned int num_texcoors2;
	vec2_t *texcoors2;

	// Arrays of triangle indices sorted by texture
	unsigned int num_triangles;
	unsigned int *triangles; // ABCABCABC...

	// Arrays of alpha triangle indices sorted by texture
	unsigned int num_alpha_triangles;
	unsigned int *alpha_triangles; // ABCABCABC...

} tris_t;


typedef struct rect_s
{
	int texture;
#ifdef MULTITEXTURE
	int bumpmap;
#endif

	unsigned int cnum_quads;
	unsigned int cnum_alpha_quads;

	unsigned int num_texcoors;
	vec2_t *texcoors;

	unsigned int num_texcoors2;
	vec2_t *texcoors2;

	// Arrays of rectangle indices sorted by texture
	unsigned int num_quads;
	unsigned int *quads; // ABCDABCDABCD...

	// Arrays of alpha rectangle indices sorted by texture
	unsigned int num_alpha_quads;
	unsigned int *alpha_quads; // ABCDABCDABCD...

} rect_t;


class OpenGLMesh
{
 public:

	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	OpenGLMesh();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Constructs an object of OpenGLMesh
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~OpenGLMesh();
	/*------------------------------------------------------
	 * Pre  : OpenGLMesh object is allocated
	 * Post : Deconstructs an object of OpenGLMesh
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////

	void drawAlpha();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Render alpha polygons
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void drawSolid();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Render solid polygons
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	void allocateColors(unsigned int n);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void allocateNormals(unsigned int n);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void allocateRectangles(unsigned int n);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void allocateTriangles(unsigned int n);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void allocateVertices(unsigned int n);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void bufferColorArray(unsigned int colorCount, vec_t *colors,
								 unsigned int colorWidth);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.20:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void bufferNormalArray(unsigned int normalCount, vec_t *normals);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.20:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void bufferTriangles(unsigned int count,
								unsigned int *indices, vec_t *texCoords,
								int *textures, unsigned int *flags);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.26:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void bufferVertexArray(unsigned int vertexCount, vec_t *vertices);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.20:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setColor(unsigned int index, float r, float g, float b, float a);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setColor(unsigned int index, float rgba[4]);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setNormal(unsigned int index, float i, float j, float k);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setVertex(unsigned int index, float x, float y, float z);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/

#ifdef NOT_IMPLEMENTED
	void sortFacesByTexture();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.26:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void addFace(int textureIndex, int textureIndexB, unsigned int flags,
					 unsigned int vertexIndexCount, vec_t *vertexIndices);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.26:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void addTexTiledFace(int textureIndex, int textureIndexB,
								unsigned int flags, unsigned int indexCount,
								vec_t *vertexIndices, vec_t *texcoords);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.26:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void bufferTexcoords(unsigned int texcoordCount, vec_t *texcoords);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.26:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void duplicateArraysForTexTiledTexcoords();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.26:
	 * Mongoose - Created
	 ------------------------------------------------------*/
#endif


	unsigned int mFlags;

	OpenGLMeshMode mMode;

	unsigned int mNumVertices;
	vec3_t *mVertices;  // <XYZ>

	unsigned int mNumNormals;
	vec3_t *mNormals;   // <IJK>

	unsigned int mNumColors;
	vec4_t *mColors;    // <RGBA>

	unsigned int mNumTris;
	tris_t *mTris;

	unsigned int mNumQuads;
	rect_t *mQuads;


	unsigned int mTriangleCount;
	int *mTriangleTextures;
	unsigned int *mTriangleIndices;
	unsigned int *mTriangleFlags;
	vec_t *mTriangleTexCoordArray;

	vec_t *mVertexArray;
	vec_t *mNormalArray;
	vec_t *mColorArray;

 private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////


	/* */
};

#endif
