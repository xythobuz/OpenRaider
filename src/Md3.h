/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*==========================================================================
 * 
 * Project : Freyja
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://gooseegg.sourceforge.net
 * Email   : stu7440@westga.edu
 * Object  : Md3
 * Comments: This is the MD3 model class.
 *
 *           See file COPYING for license details.
 *
 *           Quake (c) id Software 1995 - 2000
 *
 *           MD3 file format specs provided by Mental Vortex,
 *           and that site says the specs may not be 100% correct
 *           so - since this class is desgined using their specs...
 *
 *-- Test Defines -----------------------------------------------------
 *           
 * UNIT_TEST_MD3      - Builds Md3 class as a console unit test program 
 * UNIT_TEST_MD3_LV2  - Does more debugging when used with unit test
 *
 *-- History ---------------------------------------------------------- 
 *
 * 2002.10.19:
 * Mongoose - Interface update, Animation support starts
 *
 * 2000.10.07:
 * Mongoose - Interface update
 *
 * 2000.06.03:
 * Mongoose - Created, MD3 specs provided by Mental Vortex
 ==========================================================================*/


#ifndef __FREYJA_MONGOOSE_MD3_H
#define __FREYJA_MONGOOSE_MD3_H


#define MD3_IDALIASHEADER  0x33504449 // "IDP3"
#define MD3_ALIAS_VERSION  15
#define MD3_MAX_FRAMES     1024

#ifndef vec3_t
typedef float vec3_t[3];
#endif

#ifndef mat3_t
typedef float mat3_t[3][3];
#endif


typedef struct md3_tag_s
{
  char name[64];    /* Name of 'tag' as it's usually 
							  called in the md3 files try to 
							  see it as a sub-mesh/seperate 
							  mesh-part. 
							  
							  Sometimes this 64 string may 
							  contain some garbage, but 
							  i've been told this is because
							  some tools leave garbage in
							  those strings, but they ARE
							  strings... */

  vec3_t center;    /* Relative position of tag */
 
  mat3_t rotation;  /* The direction the tag is facing 
							  relative to the rest of the model */

} md3_tag_t;


typedef struct md3_boneframe_s
{
  float  mins[3];
  float  maxs[3];
  float  center[3];
  float  scale;
  char   creator[16]; 

} md3_bone_t;


typedef struct md3_skin_s
{
  char name[68];       /* Name of skin used by mesh
								  65 chars, 
								  32 bit aligned == 68 chars  */

  int index;           /* For shader use (skin is a shader) */

} md3_skin_t;


typedef struct md3_tri_index_s
{
  int  triangle[3];    /* Vertex 1, 2, 3 of triangle */

} md3_tri_index_t;


typedef struct md3_texel_s
{
  float st[2];         /* Texel (s, t) */

} md3_texel_t;


typedef struct md3_vertex_s
{
  signed short pos[3];       /* Vertex X/Y/Z coordinate */

  unsigned char st[2];       /* Enviromental mapping 
										  texture coordinates 
										  (Acutally encoded normal?) */
	
	float norm[3];            /* Decoded normal from above decoded */

} md3_vertex_t;


typedef struct md3_mesh_s
{
  // Start Mesh Header /////////////

  char id[4];          /* Mesh alias id, must be IDP3  */

  char name[68];       /* Name of mesh  
								  65 chars,
								  32 bit aligned == 68 chars */
  int flags;


  int  num_frames;     /* Number of VertexMorph frames 
								  in mesh */

  int num_shaders;     /* Number of shaders in mesh*/

  int  num_skins;      /* Number of skins in mesh, 
								  for backwards compatibility with
								  incorrect/obsolete spec  */

  int  num_vertices;   /* Number of vertices */
 
  int  num_triangles;  /* Number of triangles */
 
  int  tris_offset;    /* Starting position of 
								  Triangle data, relative 
								  to start of Mesh_Header  */

  int  header_size;    /* Size of header  */
  
  int  texel_offset;   /* Starting position of 
								  texvector data, relative 
								  to start of Mesh_Header */
 
  int  vertex_offset;  /* Starting position of 
								  vertex data,relative 
								  to start of Mesh_Header  */

  int  mesh_size;      /* Size of mesh  */

  // End Mesh Header ///////////////


  md3_skin_t *skin;       /* Skins */

  md3_tri_index_t *tris;  /* Triangles */

  md3_texel_t *texel;     /* Texels */
  
  md3_vertex_t *vertex;   /* Vertices */

} md3_mesh_t;


class Md3
{
public:

	enum Md3Flag
	{
		fDecodeNormals = 1
	};

	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	Md3();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Md3 object is constructed
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.06.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	Md3(unsigned int num_meshes, unsigned int num_bones, unsigned int num_tags);
	/*------------------------------------------------------
	 * Pre  : Md3 is being used for exporting, since
	 *        this is the only way to allocate arrays
	 *        from public access
	 *
	 * Post : Md3 object is constructed and ready to edit
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~Md3();
	/*------------------------------------------------------
	 * Pre  : Md3 object is allocated
	 * Post : Md3 object is deconstructed
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.06.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////
  
	unsigned int getNumMeshes();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Mesh count is returned
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created, Obsoletes:
	 *            2000.06.03, int NumTags();
	 ------------------------------------------------------*/

	unsigned int getNumTags();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Tag count is returned
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created, Obsoletes:
	 *            2000.06.03, int NumTags();
	 ------------------------------------------------------*/
  
	unsigned int getNumBones();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Bone count is returned
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created, Obsoletes:
	 *            2000.06.03, int NumTags();
	 ------------------------------------------------------*/

	md3_mesh_t *getMeshes();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Mesh list is returned
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created, Obsoletes:
	 *            2000.06.03, md3_mesh_t *Mesh();
	 ------------------------------------------------------*/
  
	md3_tag_t *getTags();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Tag list is returned
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created, Obsoletes:
	 *            2000.06.03, md3_tag_t *Tag();
	 ------------------------------------------------------*/
  
	md3_bone_t *getBones();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Bone list is returned
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created, Obsoletes:
	 *            2000.06.03, md3_boneframe_t *Bone();
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////
  
	void toggleFlag(Md3Flag flag);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Toggles a flag
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.01.01:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void reset();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Resets all model data
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.22:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setDebug(unsigned char level);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : level 0 = errors, 1 = warnings, 2 = debug1
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int load(char *filename);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Error code is returned, 0 no error
	 *        Md3 filename is read into memory
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.06.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int save(char *filename);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Error code is returned, 0 no error
	 *        Md3 saved as filename
	 *
	 *-- History ------------------------------------------
	 *
	 * 2000.10.06:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	// Testing members
	unsigned int *slaveTest;
	unsigned int idTest;
	int *texTest;

	// For animation rendering
	unsigned int numAnimations;
	unsigned int currentAnimation;
	unsigned int currentFrame;
	unsigned int nextFrame;
	float time;
	float lastTime;
 
private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////

	void printError(char *method, char *s, ...);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Dumps error to stderr
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void printWarning(char *method, char *s, ...);
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Dumps warning to stderr
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void printDebug(char *method, char *s, ...);
	/*------------------------------------------------------
	 * Pre  : Debugging is on
	 * Post : Dumps debug info
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void createMeshes(unsigned int num);
	/*------------------------------------------------------
	 * Pre  : Model hasn't been init yet
	 * Post : Allocates num of meshes for model 
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created, Obsoletes:
	 *            2000.10.24, void NumMeshes(int);
	 *            2000.10.24, void Mesh(md3_mesh_t *);
	 ------------------------------------------------------*/

	void createTags(unsigned int num);
	/*------------------------------------------------------
	 * Pre  : Model hasn't been init yet
	 * Post : Allocates num of tags for model 
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created, Obsoletes:
	 *            2000.10.24, void NumTags(int);
	 *            2000.10.24, void Tag(md3_tag_t *);
	 ------------------------------------------------------*/

	void createBones(unsigned int num);
	/*------------------------------------------------------
	 * Pre  : Model hasn't been init yet
	 * Post : Allocates num of bones for model 
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.19:
	 * Mongoose - Created, Obsoletes:
	 *            2000.10.24, void NumBones(int);
	 *            2000.10.24, void Bone(md3_bone_t *);
	 ------------------------------------------------------*/


	unsigned int mFlags;


  // Start Header //////////////////////////////////////////////

	int m_id;                 /* Alias id always "IDP3" */

	int m_version;            /* Version number, always 15 */

	char m_filename[68];      /* Sometimes left blank... 
										  65 chars, 32bit aligned == 
										  68 chars */
 
	int m_num_bones;          /* Number of Bone */

	int m_num_tags;           /* Number of 'tags' per Bone */

	int m_num_meshes;         /* Number of meshes/skins */ 

	int m_max_skins;          /* Maximum number of unique skins
										  used in md3 file */ 

	int m_header_length;      /* Always equal to the length of 
										  this header */ 

	int m_tag_start;          /* Starting position of 
										  tag-structures */ 

	int m_surfaces_start;     /* Starting position of 
										  geometeric data (mesh structures) */

	int m_file_size;          /* Size of file */

	// End Header ////////////////////////////////////////////////


	unsigned char m_debug;    /* Set level of debug info to stdout */

	md3_tag_t *m_tags;        /* Tags */

	md3_bone_t *m_bones;      /* Bones */

	md3_mesh_t *m_meshes;     /* Meshes */
};
#endif
