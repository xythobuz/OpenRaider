/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : OpenRaider
 * Author  : Mongoose
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : OpenRaider
 * License : No use w/o permission (C) 2001 Mongoose
 * Comments: Loads TR 1, 2, 3, and 4 maps, meshes, and textures
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History ------------------------------------------------
 *
 * 2003.05.13:
 * Mongoose - New API, maintance cost was becoming so high
 *            it was needed to sort out methods in groups
 *            like my newer source code -- of course some
 *            methods were altered or removed ( clean up )
 *
 * 2001.06.19:
 * Mongoose - New texture API for the new TR4/TR5 bump map
 *            support, also purged textile exposure
 *
 * 2001.05.21:
 * Mongoose - Added to project OpenRaider, more documentation
 *            than Freyja version I wrote ( 3d modeler )
 *
 *
 * 2000-05-13:
 * Mongoose - Added gcc and VC++ pragmas for packing
 *
 *            id style typedefs for structs
 *
 *            Heavy clean up and ported to C++
 *
 *            I saved yuri's notes as best I could and
 *            reformatted and corected as needed
 *
 * Mongoose - Created, based on:
 *                tr_view's tr2io.c by Yuri Zhivago, PhD,
 *                TR Rosetta Stone ( TombRaider pak format specs )
 ================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#include <TombRaider.h>

#ifdef ZLIB_SUPPORT
#include <zlib.h>
#else
#warning "No zlib. Won't support TR4+ files!"
#endif

#ifdef __TEST_TR5_DUMP_TGA
#include <mtk_tga.h>
#endif

#ifdef DEBUG_MEMORY
#include <memory_test.h>
#endif



TombRaider::TombRaider()
{
  _textile8 = NULL;
  _textile16 = NULL;
  _textile32 = NULL;
  _tex_special = NULL;
  _rooms = NULL;
  _floor_data = NULL;
  _animations = NULL;
  _state_changes = NULL;
  _anim_dispatches = NULL;
  _anim_commands = NULL;
  _mesh_trees = NULL;
  _frames = NULL;
  _moveables = NULL;
  _static_meshes = NULL;
  _object_textures = NULL;
  _sprite_textures = NULL;
  _sprite_sequences = NULL;
  _cameras = NULL;
  _sound_sources = NULL;
  _boxes = NULL;
  _overlaps = NULL;
  _zones = NULL;
  _animated_textures = NULL;
  _items = NULL;
  _light_map = NULL;
  _cinematic_frames = NULL;
  _demo_data = NULL;

  mRoomsTR5 = 0x0;
  mMeshes = 0x0;
  mSoundMap = 0x0;
  mSoundDetails = 0x0;
  mSampleIndices = 0x0;
  mRiffData = 0x0;
  mTR4Samples = 0x0;
  mTR4SamplesSz = 0x0;
  mRiffAlternateOffsets = 0x0;
  mCompressedLevelData = 0x0;
  moveablesTR5 = 0x0;
  animationsTR5 = 0x0;
  objectTexturesTR5 = 0x0;
  cinematicFramesTR5 = 0x0;
  flyByCamerasTR5 = 0x0;

  mNumTR4Samples = 0;
  mReset = false;
  mDebug = false;
  mRiffAlternateLoaded = false;
  mRoomVertexLightingFactor = 50.0f;
  mTexelScale = 256.0f;

  reset();
}


TombRaider::~TombRaider()
{
	reset();
}


int TombRaider::NumMoveables()
{
	return _num_moveables;
}


int TombRaider::NumRooms()
{
  return _num_rooms;
}


int TombRaider::NumAnimations()
{
	return _num_animations;
}


unsigned int TombRaider::NumFrames()
{
	return _num_frames;
}


int TombRaider::NumStaticMeshes()
{
	return _num_static_meshes;
}


int TombRaider::NumSprites()
{
	return _num_sprite_textures;
}


int TombRaider::NumSpriteSequences()
{
	return _num_sprite_sequences;
}


int TombRaider::NumItems()
{
	return _num_items;
}


int TombRaider::NumTextures()
{
	//return _num_room_textures + _num_misc_textures + _num_bump_map_textures / 2;
	return _num_textiles - _num_bump_map_textures / 2;
}


tr2_room_t *TombRaider::Room()
{
  return _rooms;
}


tr2_item_t *TombRaider::Item()
{
  return _items;
}


tr2_object_texture_t *TombRaider::ObjectTextures()
{
  return _object_textures;
}


unsigned int TombRaider::getNumBoxes()
{
	return _num_boxes;
}


tr2_box_t *TombRaider::Box()
{
	return _boxes;
}


tr2_mesh_t *TombRaider::Mesh()
{
  /*
  if (n > 0 || n > mMeshCount)
    return NULL;

  return _meshes+n;
  */

  return mMeshes;
}


int TombRaider::getNumAnimsForMoveable(int moveable_index)
{
	/***************************************************************************
    * It seems the number of animations isn't available in the moveable,
	 * so we have to calculate it:
    *  - Get the "# starting anim" for the next moveable (->N)
    *  - Substract the "# starting anim" for moveable to N
	 *
    * Doing this, we assume that the next moveable has its animations following
    * the animations of the current moveable (seems right for all tested
    * levels, but...)
	 *
    * We also have to deal with the fact that the next moveable
	 * could have "# starting anim" == -1
	 * (ie. anim handled by the engine, like the ponytail anim).
	 * If it's the case, we skip the moveable
    * and use the next moveable for our computation
	 *
	 * - Mongoose, Notes I edited from TRViewer
	 **************************************************************************/

	int start_anim;
	int next_start_anim;
	tr2_moveable_t *last_moveable = 0x0;
	tr2_moveable_t *moveable = 0x0;
	tr2_moveable_t *next_moveable = 0x0;


	if ((moveable_index >= 0 &&
		 moveable_index <= (int)_num_moveables) || _num_moveables < 1)
	{
		moveable = &_moveables[moveable_index];
	}

	if (!moveable)
	{
		return -1; // Was 0
	}

	last_moveable = &_moveables[moveable_index-1];

	start_anim = moveable->animation;

	while (moveable != last_moveable)
	{
		next_moveable = moveable + 1;
		next_start_anim = next_moveable->animation;

		if (next_start_anim != 0xFFFF)
			break;

		moveable = next_moveable++;
	}

	if (moveable == last_moveable)
	{
		next_start_anim = _num_animations;
	}

	return ((start_anim != 0xFFFF) ? next_start_anim - start_anim : 0);
}


tr2_staticmesh_t *TombRaider::StaticMesh()
{
	return _static_meshes;
}


tr2_version_type TombRaider::Engine()
{
  return mEngineVersion;
}


tr2_animation_t *TombRaider::Animation()
{
  return _animations;
}


unsigned short *TombRaider::Frame()
{
  return _frames;
}


tr2_moveable_t *TombRaider::Moveable()
{
  /*
  if (n > 0 || n > (int)_num_moveables)
    return NULL;
  */

  return _moveables;
}


tr2_meshtree_t *TombRaider::MeshTree()
{
  /*
  if (n > 0 || n > (int)_num_mesh_trees)
    return NULL;
  */

  return _mesh_trees;
}


tr2_sprite_texture_t *TombRaider::Sprite()
{
	return _sprite_textures;
}


tr2_sprite_sequence_t *TombRaider::SpriteSequence()
{
	return _sprite_sequences;
}

unsigned char *TombRaider::SpecialTexTile(int texture)
{
	unsigned char *image;
	unsigned char *ptr;


	image = NULL;

	if (texture >=0 && texture < NumSpecialTextures())
   {
		// Get base and offset into 32bit special textures/bump maps
		ptr = _tex_special;
		ptr += 256*256*4*texture;

		// Clone it as a single 256x256 @ 32bpp image
		image = new unsigned char[256*256*4];
		memcpy(image, ptr, 256*256*4);
	}

	return image;
}


int TombRaider::NumSpecialTextures()
{
	return _num_tex_special;
}


void TombRaider::Texture(int texture, unsigned char **image,
								 unsigned char **bumpmap)
{
	int bumpmap_base = _num_room_textures + _num_misc_textures;


	*image = getTexTile(texture);
	*bumpmap = NULL;

	if (_num_bump_map_textures && texture >= bumpmap_base)
	{
		*bumpmap = getTexTile(texture + _num_bump_map_textures/2);
	}
}


unsigned int *TombRaider::Palette16()
{
  return _palette16;
}


unsigned char *TombRaider::Palette8()
{
  return (unsigned char *)_palette8;
}


int TombRaider::checkMime(char *filename)
{
   FILE *f;
	unsigned int version;


	if (!filename || !filename[0])
	{
		print("checkFile", "Given filename was empty string or NULL");
		return -1;
	}

   f = fopen(filename, "rb");

   if (!f)
   {
     perror(filename);
     return -1;
   }

	/* FIXME: Endianess */
   fread(&version, sizeof(version), 1, f);
	fclose(f);

   switch (version)
   {
   case 0x00000020:
   case 0x0000002d:
   case 0xff080038:
   case 0xff180038:
   case 0xfffffff0:           // bogus
   case 0x00345254:           // "TR4\0"
		return 0;
     break;
   default:
		;
   }

	return 1;
}


int TombRaider::Load(char *filename, void (*percent)(int))
{
   FILE *f;
   int i, j, l;
   unsigned int num_mesh_data_words, num_mesh_pointers, data_size, data_offset;
   unsigned int *mesh_pointer_list;
   unsigned char *raw_mesh_data;
	bool tr5;
   long debugf;


   f = fopen(filename, "rb");

   if (!f)
   {
     perror(filename);
     return -1;
   }


   if (percent)
		(*percent)(1);

	mReset = false;

   Fread(&mPakVersion, sizeof(mPakVersion), 1, f);
   //FIXME: endian

   printDebug("Load", "mPakVersion = %u", mPakVersion);

	tr5 = false;

   switch (mPakVersion)
   {
   case 0x00000020:
     mEngineVersion = TR_VERSION_1;
     break;
   case 0x0000002d:
     mEngineVersion = TR_VERSION_2;
     break;
   case 0xff080038:
   case 0xff180038:
     mEngineVersion = TR_VERSION_3;
     break;
   case 0xfffffff0:           // bogus
   case 0x00345254:           // "TR4\0"
     mEngineVersion = TR_VERSION_4;

	  // Check to see if this is really a TR5 demo
	  l = strlen(filename);

	  // Looking for pattern "filename.trc"
	  if ((filename[l-1] == 'c' || filename[l-1] == 'C') &&
			(filename[l-2] == 'r' || filename[l-2] == 'R') &&
			(filename[l-3] == 't' || filename[l-3] == 'T'))
	  {
		  printDebug("Load", "This is really a TR5 pak");
		  mEngineVersion = TR_VERSION_5;
		  return loadTR5(f, percent);
	  }

     break;
   default:
     mEngineVersion = TR_VERSION_UNKNOWN;
   }

   if (percent)
		(*percent)(5);

   printDebug("Load", "mEngineVersion = 0x%x", mPakVersion);

   if (mEngineVersion == TR_VERSION_UNKNOWN)
     return -1;

   if (mEngineVersion == TR_VERSION_4)
   {
#ifdef ZLIB_SUPPORT
		unsigned int sz, usz; // compressed and uncompressed size
		unsigned char *compressed_data = NULL;
		int zerr;
		uLongf foo;


		// Read texture type offsets
		Fread(&_num_room_textures, 2, 1, f);
		printDebug("LoadTR4", "_num_room_textures = %u", _num_room_textures);
		Fread(&_num_misc_textures, 2, 1, f);
		printDebug("LoadTR4", "_num_misc_textures = %u", _num_misc_textures);
		Fread(&_num_bump_map_textures, 2, 1, f);
		printDebug("LoadTR4", "_num_bump_map_textures  = %u", _num_bump_map_textures);

     // Read the sizes of the 32-bit textures
     Fread(&usz, sizeof(usz), 1, f);
     Fread(&sz, sizeof(sz), 1, f);

     printDebug("Load", "TR4 32-bit textures compressed size   = %u bytes", sz);
     printDebug("Load", "TR4 32-bit textures uncompressed size = %u bytes", usz);

	  _num_textiles = usz / sizeof(tr2_textile32_t);

	  printDebug("LoadTR4", "_num_textiles = %i/%i = %i",
					 usz, sizeof(tr2_textile32_t), _num_textiles);

	  _textile32 = new tr2_textile32_t[_num_textiles];

	  // Allocate a temporary buffer for decompression
	  compressed_data = new unsigned char[sz];
	  Fread(compressed_data, sz, 1, f);

	  // Decompress the textures
	  foo = usz;
	  zerr = uncompress((unsigned char *)_textile32,
							  &foo,
							  compressed_data,
							  sz);
	  usz = foo;

	  if (percent)
		  (*percent)(6);

	  printDebug("LoadTR4", "textile decompress  [%s]",
					 (zerr == Z_OK) ? "OK" : "ERROR");

	  switch (zerr)
	  {
	  case Z_MEM_ERROR:
		  printDebug("LoadTR4", "There was not enough memory");
		  break;
	  case Z_BUF_ERROR:
		  printDebug("LoadTR4", "There was not enough room in the output buffer");
		  break;
	  case Z_DATA_ERROR:
		  printDebug("LoadTR4", "The input data was corrupted");
		  break;
	  default:
		  printDebug("LoadTR4", "textile decompress %i", zerr);
	  }

	  // Free the temporary buffer
	  delete [] compressed_data;

     // Read in the 16-bit textures, set NumTextiles
     Fread(&usz, sizeof(usz), 1, f);
     Fread(&sz, sizeof(sz), 1, f);

     printDebug("Load", "TR4 16-bit textures compressed size   = %u bytes", sz);
     printDebug("Load", "TR4 16-bit textures uncompressed size = %u bytes", usz);

     _num_textiles = usz / sizeof(tr2_textile16_t);

     printDebug("Load", "TR4 _num_textiles = %i/%i = %i",
		usz, sizeof(tr2_textile16_t), _num_textiles);

     _textile16 = new tr2_textile16_t[_num_textiles];

     // Allocate a temporary buffer for decompression
     compressed_data = new unsigned char[sz];
     Fread(compressed_data, sz, 1, f);

     // Decompress the textures
	  foo = usz;
     zerr = uncompress((unsigned char *)_textile16,
		       &foo,
		       compressed_data,
		       sz);
     usz = foo;

	  if (percent)
		  (*percent)(7);

	  //     printDebug("Load", "TR4 textile decompress  [%s]",
	  //	(zerr == Z_OK) ? "OK" : "ERROR");

     switch (zerr)
     {
     case Z_MEM_ERROR:
		  printDebug("Load", "TR4 textile decompress [ERROR]");
		  printDebug("Load", "TR4 There was not enough memory");
		  break;
     case Z_BUF_ERROR:
		  printDebug("Load", "TR4 textile decompress [ERROR]");
		  printDebug("Load", "TR4 There was not enough room in the output buffer");
		  break;
     case Z_DATA_ERROR:
		  printDebug("Load", "TR4 textile decompress [ERROR]");
		  printDebug("Load", "TR4 The input data was corrupted");
		  break;
	  case Z_OK:
		  printDebug("Load", "TR4 textile decompress [OK]");
		  break;
	  default:
		  printDebug("Load", "TR4 textile decompress %i", zerr);
     }

     // Free the temporary buffer
     delete [] compressed_data;

     // Read the sizes of the sprite textures
     Fread(&usz, sizeof(usz), 1, f);
     Fread(&sz, sizeof(sz), 1, f);

     printDebug("Load", "TR4 sprite textures compressed size   = %u bytes", sz);
     printDebug("Load", "TR4 sprite textures uncompressed size = %u bytes", usz);

	  // Load sprite/bump map/gui/etc textures also
	  _num_tex_special = usz/(256*256*4);

	  printDebug("LoadTR5", "_num_tex_special = %i/%i = %i",
					 usz, 256*256*4, _num_tex_special);

	  printDebug("LoadTR5", "Reading %ibytes of sprite textures", usz);

	  if (usz)
	  {
		  _tex_special = new unsigned char[usz];

		  // Allocate a temporary buffer for decompression
		  compressed_data = new unsigned char[sz];
		  Fread(compressed_data, sz, 1, f);

		  // Decompress the textures
		  foo = usz;
		  zerr = uncompress(_tex_special,
								  &foo,
								  compressed_data,
								  sz);
		  usz = foo;

		  printDebug("LoadTR5", "special texture decompress  [%s]",
						 (zerr == Z_OK) ? "OK" : "ERROR");

		  switch (zerr)
		  {
		  case Z_MEM_ERROR:
			  printDebug("LoadTR5", "There was not enough memory");
			  break;
		  case Z_BUF_ERROR:
			  printDebug("LoadTR5", "There was not enough room in the output buffer");
			  break;
		  case Z_DATA_ERROR:
			  printDebug("LoadTR5", "The input data was corrupted");
			  break;
		  default:
			  printDebug("LoadTR5", "textile decompress %i", zerr);
		  }

		  // Free the temporary buffer
		  delete [] compressed_data;
	  }

	  if (percent)
		  (*percent)(9);

     // Read the sizes of the level data
     Fread(&usz, sizeof(usz), 1, f);
     Fread(&sz, sizeof(sz), 1, f);

     printDebug("Load", "TR4 level data compressed size   = %u bytes", sz);
     printDebug("Load", "TR4 level data uncompressed size = %u bytes", usz);

     // Allocate a temporary buffer for decompression
     compressed_data = new unsigned char[sz];
     Fread(compressed_data, sz, 1, f);
     mCompressedLevelData = new unsigned char[usz];

     // Decompress the level data
	  foo = usz;
     zerr = uncompress(mCompressedLevelData, &foo, compressed_data, sz);

     usz = foo;

     printDebug("Load", "TR4 level data decompress  [%s]",
		(zerr == Z_OK) ? "OK" : "ERROR");

     switch (zerr)
     {
     case Z_MEM_ERROR:
       printDebug("Load", "TR4 There was not enough memory");
       break;
     case Z_BUF_ERROR:
       printDebug("Load", "TR4 There was not enough room in the output buffer");
       break;
     case Z_DATA_ERROR:
       printDebug("Load", "TR4 The input data was corrupted");
       break;
     }

     delete [] compressed_data;

     mCompressedLevelDataOffset = 0;
     mCompressedLevelSize = usz;

     // Toggle Fread mode to read from decompressed data in memory, not diskfile
	  mFreadMode = TR_FREAD_COMPRESSED;
#else
     Print("Load> ERROR: TR4 support not compiled in this build.");
     Print("Load> Try 'make tr4' next build.  Requires zlib.");
     return -1;
#endif
   }

   if (mEngineVersion == TR_VERSION_2 || mEngineVersion == TR_VERSION_3)
   {
     /* Read the 8-bit palette */
     Fread(_palette8, sizeof(tr2_colour_t), 256, f);

     /* Read 16-bit palette */
     Fread(_palette16, sizeof(_palette16), 1, f);

     printDebug("Load", "Read TR 2|3 8bit and 16bit palettes");
   }

   if (mEngineVersion != TR_VERSION_4)
   {
     /* Read the textiles */
     Fread(&_num_textiles, sizeof(_num_textiles), 1, f);

     printDebug("Load", "_num_textiles = %i", _num_textiles);

     /* 8-bit textiles come first */
     _textile8 = new tr2_textile8_t[_num_textiles];
     Fread(_textile8, sizeof(tr2_textile8_t), _num_textiles, f);

     /* 16-bit textiles come second */
     _textile16 = new tr2_textile16_t[_num_textiles];

     if (mEngineVersion != TR_VERSION_1)
     {
       //FIXME: need endian checking here
       Fread(_textile16, sizeof(tr2_textile16_t), _num_textiles, f);
       printDebug("Load", "Read in 16bit texture tiles");
     }
   }

   if (percent)
		(*percent)(10);

   /* 32-bit unknown - seems to always be 0 */
   Fread(&_unknown_t, sizeof(_unknown_t), 1, f);
   printDebug("Load", "_unknown_t = 0x%x", _unknown_t);

   /* Read raw room data */
   //FIXME: needs endian checking
   Fread(&_num_rooms, sizeof(_num_rooms), 1, f);
   printDebug("Load", "_num_rooms = %i", _num_rooms);

   data_size = _num_rooms * sizeof(tr2_room_t);
   _rooms = new tr2_room_t[_num_rooms];

   /* Extract room details */
   for (i = 0; i < _num_rooms; ++i)
   {
		if (percent)
		{
			(*percent)(11 + (int)(((float)i/(float)_num_rooms)*70.0));
		}

     /* Read RoomInfo */
     //FIXME: endian check needed
     Fread(&_rooms[i].info, sizeof(tr2_room_info_t), 1, f);

     printDebug("Load", "_rooms[%i].info =\n { x=%i, z=%i, yt=%i, yb=%i}",
					 i,
					 _rooms[i].info.x, _rooms[i].info.z,
					 _rooms[i].info.y_top, _rooms[i].info.y_bottom);

     /* Read raw data for rest of room */
     Fread(&_rooms[i].num_data_words, sizeof(_rooms[i].num_data_words), 1, f);

     printDebug("Load", "_rooms[%i].num_data_words = %u",
					 i, _rooms[i].num_data_words);

     _rooms[i].data = new unsigned char[_rooms[i].num_data_words*2];
     Fread(_rooms[i].data, 2, _rooms[i].num_data_words, f);

     /* Identify vertices */
     data_offset = 0;
     //FIXME: endian
     _rooms[i].room_data.num_vertices = *(short *)(_rooms[i].data);

     data_offset += sizeof(_rooms[0].room_data.num_vertices);
     data_size = _rooms[i].room_data.num_vertices * sizeof(tr2_vertex_room_t);

     printDebug("Load", "_rooms[%i].room_data.num_vertices = %u",
					 i, _rooms[i].room_data.num_vertices);

	  _rooms[i].room_data.vertices = 0x0;

     if (_rooms[i].room_data.num_vertices > 0)
     {
       _rooms[i].room_data.vertices =
			 new tr2_vertex_room_t[_rooms[i].room_data.num_vertices];

       if (mEngineVersion == TR_VERSION_1)
       {
			 data_size = _rooms[i].room_data.num_vertices *
				 (sizeof(tr2_vertex_room_t) - 4);

			 for (j = 0; j < _rooms[i].room_data.num_vertices; ++j)
          {
				 memcpy(&_rooms[i].room_data.vertices[j],
						  _rooms[i].data + data_offset +
						  (j * (sizeof(tr2_vertex_room_t) - 4)),
						  sizeof(tr2_vertex_room_t) - 4);

				 // ??? Adjust for what's missing?
				 _rooms[i].room_data.vertices[j].lighting2 =
					 _rooms[i].room_data.vertices[j].lighting1;
				 _rooms[i].room_data.vertices[j].attributes = 0;
			 }
       }
       else
       {
			 memcpy(_rooms[i].room_data.vertices,
					  _rooms[i].data + data_offset, data_size);
       }

       //FIXME: endian conversions for verts needed
     }

     data_offset += data_size;

     /* identify rectangles */
     //FIXME: endian conversion
     _rooms[i].room_data.num_rectangles =
       *(short *)(_rooms[i].data + data_offset);

     data_offset += sizeof(_rooms[0].room_data.num_rectangles);
     data_size = _rooms[i].room_data.num_rectangles * sizeof(tr2_quad_t);

     printDebug("Load", "_rooms[%i].room_data.num_rectangles = %i",
					 i, _rooms[i].room_data.num_rectangles);

	  _rooms[i].room_data.rectangles = 0x0;

     if (_rooms[i].room_data.num_rectangles > 0)
     {
       _rooms[i].room_data.rectangles =
			 new tr2_quad_t[_rooms[i].room_data.num_rectangles];
       memcpy(_rooms[i].room_data.rectangles,
				  _rooms[i].data + data_offset, data_size);

       if (mEngineVersion >= TR_VERSION_3)
       {
			 int j;

			 for (j = 0; j < _rooms[i].room_data.num_rectangles; ++j)
	       {
				 _rooms[i].room_data.rectangles[j].texture &= 0x7fff;
			 }
       }
       //FIXME: endian conversion
     }

     data_offset += data_size;

     /* Identify triangles */
     _rooms[i].room_data.num_triangles =
       *(short *)(_rooms[i].data + data_offset);
     //FIXME: endian

     data_offset += sizeof(_rooms[0].room_data.num_triangles);
     data_size = _rooms[i].room_data.num_triangles * sizeof(tr2_tri_t);

     printDebug("Load", "_rooms[%i].room_data.num_triangles = %i",
					 i, _rooms[i].room_data.num_triangles);

	  _rooms[i].room_data.triangles = 0x0;

     if (_rooms[i].room_data.num_triangles > 0)
     {
       _rooms[i].room_data.triangles =
			 new tr2_tri_t[_rooms[i].room_data.num_triangles];

       memcpy(_rooms[i].room_data.triangles,
				  _rooms[i].data + data_offset, data_size);

       if (mEngineVersion >= TR_VERSION_3)
       {
			 int j;

			 for (j = 0; j < _rooms[i].room_data.num_triangles; ++j)
			 {
				 _rooms[i].room_data.triangles[j].texture &= 0x7fff;
			 }
			 //FIXME: endian
       }
     }

     data_offset += data_size;

      /* Identify sprites */
      _rooms[i].room_data.num_sprites =
			*(short *)(_rooms[i].data + data_offset);
      //FIXME: endian

      data_offset += sizeof(_rooms[0].room_data.num_sprites);
      data_size = _rooms[i].room_data.num_sprites * sizeof(tr2_room_sprite_t);

		printDebug("Load", "_rooms[%i].room_data.num_sprites = %i",
					  i, _rooms[i].room_data.num_sprites);

		_rooms[i].room_data.sprites = 0x0;

      if (_rooms[i].room_data.num_sprites > 0)
      {
			_rooms[i].room_data.sprites =
				new tr2_room_sprite_t[_rooms[i].room_data.num_sprites];
			memcpy(_rooms[i].room_data.sprites,
					 _rooms[i].data + data_offset, data_size);

			if (mEngineVersion >= TR_VERSION_3)
	      {
				int j;

				for (j = 0; j < _rooms[i].room_data.num_sprites; j++)
	         {
					_rooms[i].room_data.sprites[j].texture &= 0x7fff;
				}
			}

			//FIXME: endian
      }

      /* Free the raw room data */
      delete [] _rooms[i].data;
      _rooms[i].data = NULL;

      /* Read door info */
      //FIXME: endian
      Fread(&_rooms[i].num_portals, sizeof(_rooms[0].num_portals), 1, f);

      printDebug("Load", "_rooms[%i].num_portals = %i",
		i, _rooms[i].num_portals);

		if (_rooms[i].num_portals > 0)
			_rooms[i].portals = new tr2_room_portal_t[_rooms[i].num_portals];
		else
			_rooms[i].portals = 0;

      Fread(_rooms[i].portals, sizeof(tr2_room_portal_t),
	    _rooms[i].num_portals, f);
      //FIXME: endian

      /* Read sector info */
      //FIXME: endian
      Fread(&_rooms[i].num_zsectors, sizeof(_rooms[0].num_zsectors), 1, f);
      Fread(&_rooms[i].num_xsectors, sizeof(_rooms[0].num_xsectors), 1, f);

      printDebug("Load", "_rooms[%i].num_zsectors = %i",
		i, _rooms[i].num_zsectors);
      printDebug("Load", "_rooms[%i].num_xsectors = %i",
		i, _rooms[i].num_xsectors);

		if (_rooms[i].num_zsectors > 0 && _rooms[i].num_xsectors > 0)
		{
			_rooms[i].sector_list =
			new tr2_room_sector_t[_rooms[i].num_zsectors * _rooms[i].num_xsectors];
		}
		else
		{
			_rooms[i].sector_list = 0x0;
		}

      Fread(_rooms[i].sector_list, sizeof(tr2_room_sector_t),
	    _rooms[i].num_zsectors * _rooms[i].num_xsectors, f);
      //FIXME: endian

      printDebug("Load", "Read %u room sectors",
		 _rooms[i].num_zsectors * _rooms[i].num_xsectors);

      /* Read room lighting & mode */
      if (mEngineVersion >= TR_VERSION_3)
      {
			Fread(&_rooms[i].intensity1, 4, 1, f);
			// Fake TR2 record:
			_rooms[i].light_mode = 0;
      }
      else if (mEngineVersion == TR_VERSION_1)
      {
			Fread(&_rooms[i].intensity1, 2, 1, f);
			// Is this intensity or LightMode?

			printDebug("Load", "_rooms[%i].intensity1 = %u",
						  i, _rooms[i].intensity1);

			_rooms[i].intensity2 = _rooms[i].intensity1;
			_rooms[i].light_mode = 0;
      }
      else
      {   // TR2
			Fread(&_rooms[i].intensity1, 6, 1, f);
			printDebug("Load", "TR2 _rooms[%i].intensity1 = %u",
						  i, _rooms[i].intensity1);
      }

      /* Read room lighting info */
       //FIXME: endian
      Fread(&_rooms[i].num_lights, sizeof(_rooms[i].num_lights), 1, f);

      printDebug("Load", "_rooms[%i].num_lights = %u",
		 i, _rooms[i].num_lights);

		_rooms[i].lights = 0x0;
		_rooms[i].tr4Lights = 0x0;

		// Mongoose 2002.04.03, New TR4 light struct, removed old
		//   double size for others
      if (_rooms[i].num_lights > 0)
      {
			if (mEngineVersion == TR_VERSION_1)
			{
				_rooms[i].lights = new tr2_room_light_t[_rooms[i].num_lights];

				for (j = 0; j < _rooms[i].num_lights; ++j)
				{
					Fread(&_rooms[i].lights[j].x, sizeof(_rooms[0].lights[0].x), 3,f);
					// x, y, z

					printDebug("Load", "_rooms[%i].lights[%i] = <%i %i %i>",
								  i, j,
								  _rooms[i].lights[j].x,
								  _rooms[i].lights[j].y,
								  _rooms[i].lights[j].z);

					Fread(&_rooms[i].lights[j].intensity1, sizeof(short), 1, f);
					// Intensity1

					printDebug("Load", "_rooms[%i].lights[%i].intensity1 = %u",
								  i, j,
								  _rooms[i].lights[j].intensity1);

					_rooms[i].lights[j].intensity2 = _rooms[i].lights[j].intensity1;
					Fread(&_rooms[i].lights[j].fade1, sizeof(unsigned int), 1, f);
					// Fade1

					printDebug("Load", "_rooms[%i].lights[%i].fade1 = %u",
								  i, j,
								  _rooms[i].lights[j].fade1);

					_rooms[i].lights[j].fade2 = _rooms[i].lights[j].fade1;
				}
			}
			else if (mEngineVersion == TR_VERSION_4)
	      {
				_rooms[i].tr4Lights = new tr4_room_light_t[_rooms[i].num_lights];

				Fread(_rooms[i].tr4Lights, sizeof(tr4_room_light_t),
						_rooms[i].num_lights, f);
			}
			else
	      {
				_rooms[i].lights = new tr2_room_light_t[_rooms[i].num_lights];

				Fread(_rooms[i].lights, sizeof(tr2_room_light_t),
						_rooms[i].num_lights, f);
			}
      }
      //FIXME: endian

      /* Read Static Mesh Data */
      Fread(&_rooms[i].num_static_meshes, sizeof(unsigned short), 1, f);
      //FIXME: endian

      printDebug("Load", "_rooms[%i].num_static_meshes = %u",
					  i, _rooms[i].num_static_meshes);

		_rooms[i].static_meshes = 0x0;

      if (_rooms[i].num_static_meshes > 0)
      {
         _rooms[i].static_meshes =
				new tr2_room_staticmesh_t[_rooms[i].num_static_meshes];

			if (mEngineVersion == TR_VERSION_1)
	      {
				for (j = 0; j < _rooms[i].num_static_meshes; j++)
	         {
					Fread(&_rooms[i].static_meshes[j], 18, 1, f);
					// Account for the missing .intensity2

					_rooms[i].static_meshes[j].object_id =
						_rooms[i].static_meshes[j].intensity2;

					_rooms[i].static_meshes[j].intensity2 =
						_rooms[i].static_meshes[j].intensity1;
				}
			}
         else
			{
				Fread(_rooms[i].static_meshes, sizeof(tr2_room_staticmesh_t),
						_rooms[i].num_static_meshes, f);
			}
      }
      //FIXME: endian

      Fread(&_rooms[i].alternate_room, sizeof(short), 1, f);
      //FIXME: endian

      printDebug("Load", "_rooms[%i].alternate_room = %i",
		 i, _rooms[i].alternate_room);

      Fread(&_rooms[i].flags, sizeof(short), 1, f);
      //FIXME: endian

      printDebug("Load", "_rooms[%i].flags = 0x%x",
					  i, _rooms[i].flags);

      /* Read TR3 room light colour */
      if (mEngineVersion >= TR_VERSION_3)
      {
			/* we force this to be 3 bytes
				(instead of just sizeof(room_light_colour))
				for Macs and others that can't handle odd-length structures...
			*/
			Fread(&_rooms[i].room_light_colour, 3, 1, f);
			printDebug("Load", "TR3 _rooms[%i].room_light_colour {%i %i %i}",
						  i,
						  _rooms[i].room_light_colour.r,
						  _rooms[i].room_light_colour.g,
						  _rooms[i].room_light_colour.b);
      }
    }

   /* Read floor data */
   /*
    * Really, FloorData should be a per-sector dynamic allocation;  however,
    * that requires a parser that can accurately determine where one sector's
    * FloorData ends and another's begins.  Until we have that, we'll stick to
    * this crude (but effective) method...
    */
   Fread(&_num_floor_data, sizeof(_num_floor_data), 1, f);
   printDebug("Load", "_num_floor_data = %u", _num_floor_data);

	_floor_data = 0x0;

   if (_num_floor_data > 0)
   {
     _floor_data = new unsigned short[_num_floor_data];
     Fread(_floor_data, sizeof(short), _num_floor_data, f);
     //FIXME: endian
   }

   /* Read mesh data */
   Fread(&num_mesh_data_words, sizeof(num_mesh_data_words), 1, f);
   //FIXME: endian

   printDebug("Load", "num_mesh_data_words = %u", num_mesh_data_words);

   raw_mesh_data = new unsigned char[num_mesh_data_words*2];
   Fread(raw_mesh_data, 2, num_mesh_data_words, f);
   // Endian-conversion of this data occurs in ExtractMeshes()

   printDebug("Load", "Read raw_mesh_data");

   /* Read mesh pointers */
   Fread(&num_mesh_pointers, sizeof(num_mesh_pointers), 1, f);
   //FIXME: endian

   printDebug("Load", "num_mesh_pointers = %u", num_mesh_pointers);

   mesh_pointer_list = new unsigned int[num_mesh_pointers];
   Fread(mesh_pointer_list, sizeof(unsigned int), num_mesh_pointers, f);
   //FIXME: endian

   printDebug("Load", "Read mesh_pointer_list");

   /* Extract meshes */
   extractMeshes(raw_mesh_data, num_mesh_pointers, mesh_pointer_list);
   delete [] raw_mesh_data;
   delete [] mesh_pointer_list;

   /* Read animations */
   Fread(&_num_animations, sizeof(_num_animations), 1, f);
   //FIXME: endian

   printDebug("Load", "_num_animations = %u", _num_animations);

	_animations = 0x0;

   if (_num_animations > 0)
   {
		_animations = new tr2_animation_t[_num_animations];

     if (mEngineVersion == TR_VERSION_4)
     {
		  tr4_animation_t tr4_anim;

       for (i = 0; i < (int)_num_animations; ++i)
       {
			 Fread(&tr4_anim, 40, 1, f);

			 _animations[i].frame_offset = tr4_anim.frame_offset;
			 _animations[i].frame_rate = tr4_anim.frame_rate;
			 _animations[i].frame_size = tr4_anim.frame_size;
			 _animations[i].state_id = tr4_anim.state_id;
			 _animations[i].unknown1 = tr4_anim.unknown;
			 _animations[i].unknown2 = tr4_anim.speed;
			 _animations[i].unknown3 = tr4_anim.accel_lo;
			 _animations[i].unknown4 = tr4_anim.accel_hi;
			 _animations[i].frame_start = tr4_anim.frame_start;
			 _animations[i].frame_end = tr4_anim.frame_end;
			 _animations[i].next_animation = tr4_anim.next_animation;
			 _animations[i].next_frame = tr4_anim.next_frame;
			 _animations[i].num_state_changes = tr4_anim.num_state_changes;
			 _animations[i].state_change_offset = tr4_anim.state_change_offset;
			 _animations[i].num_anim_commands = (tr4_anim.num_anim_commands > 256)
? 0 : tr4_anim.num_anim_commands;
			 _animations[i].anim_command = tr4_anim.anim_command;
       }
     }
     else
     {
         Fread(_animations, sizeof(tr2_animation_t), _num_animations, f);
     }
   }
   //FIXME: endian

   /* Read state changes */
   Fread(&_num_state_changes, sizeof(_num_state_changes), 1, f);
   //FIXME: endian


   if (percent)
		(*percent)(80);

   printDebug("Load", "_num_state_changes = %u", _num_state_changes);

   if (_num_state_changes > 0)
   {
     _state_changes = new tr2_state_change_t[_num_state_changes];

     Fread(_state_changes, sizeof(tr2_state_change_t), _num_state_changes, f);
   }
   //FIXME: endian

   /* Read AnimDispatches */
   Fread(&_num_anim_dispatches, sizeof(_num_anim_dispatches), 1, f);
   //FIXME: endian

   printDebug("Load", "_num_anim_dispatches = %u", _num_anim_dispatches);

	_anim_dispatches = 0x0;

   if (_num_anim_dispatches > 0)
   {
      _anim_dispatches = new tr2_anim_dispatch_t[_num_anim_dispatches];
      Fread(_anim_dispatches, sizeof(tr2_anim_dispatch_t),
	    _num_anim_dispatches, f);
   }
   //FIXME: endian

   /* Read anim commands */
   Fread(&_num_anim_commands, sizeof(_num_anim_commands), 1, f);
   //FIXME: endian

   printDebug("Load", "_num_anim_commands = %u", _num_anim_commands);

	_anim_commands = 0x0;

   if (_num_anim_commands > 0)
   {
     _anim_commands = new tr2_anim_command_t[_num_anim_commands];
     Fread(_anim_commands, sizeof(tr2_anim_command_t), _num_anim_commands, f);
   }
   //FIXME: endian

   /* Read MeshTrees */
   Fread(&_num_mesh_trees, sizeof(_num_mesh_trees), 1, f);
   //FIXME: endian

   printDebug("Load", "_num_mesh_trees = %u", _num_mesh_trees);

	_mesh_trees = 0x0;

   if (_num_mesh_trees > 0)
   {
     _mesh_trees = new tr2_meshtree_t[_num_mesh_trees];
     Fread(_mesh_trees, sizeof(int), _num_mesh_trees, f);
   }
   //FIXME: endian

   /* Read frames */
   Fread(&_num_frames, sizeof(_num_frames), 1, f);
   //FIXME: endian

   printDebug("Load", "_num_frames = %u", _num_frames);

	_frames = 0x0;

   if (_num_frames > 0)
   {
     _frames = new unsigned short[_num_frames];
     Fread(_frames, 2, _num_frames, f);
     //FIXME: endian

     if (mEngineVersion == TR_VERSION_1)
     {
       // re-format the frames[] to look like TR2 frames
       int num_frames;

       for (j = 0; j < (int)_num_animations; ++j)
       {
			 int fo = _animations[j].frame_offset / 2;
			 _animations[j].frame_size = (_frames[fo + 9] * 2) + 10;
       }

       for (i = 0; i < (int)_num_frames; )
       {
			 i += 9;  // point to num_frames;
			 j = i;   // get rid of (overwrite) num_frames
			 num_frames = _frames[i++];

			 while (num_frames--)
			 {
				 _frames[j++] = _frames[i + 1];   // reverse the words as we go
				 _frames[j++] = _frames[i];
				 i += 2;
			 }
       }
     }
   }

   /* Read moveables */
   Fread(&_num_moveables, sizeof(_num_moveables), 1, f);
   //FIXME: endian
   printDebug("Load", "_num_moveables = %u", _num_moveables);

	_moveables = 0x0;

   if (_num_moveables > 0)
   {
     debugf = ftell(f);
     _moveables = new tr2_moveable_t[_num_moveables];
     Fread(_moveables, 18, _num_moveables, f);
   }
   //FIXME: endian

   Fread(&_num_static_meshes, sizeof(int), 1, f);
   //FIXME: endian
   printDebug("Load", "_num_static_meshes = %u", _num_static_meshes);

   // SAFE EXIT //////////////////////////

	_static_meshes = 0x0;

   if (_num_static_meshes > 0)
   {
     _static_meshes = new tr2_staticmesh_t[_num_static_meshes];
     Fread(_static_meshes, sizeof(tr2_staticmesh_t),
	   _num_static_meshes, f);
     //FIXME: endian
   }

	_object_textures = 0x0;

   if (mEngineVersion < TR_VERSION_3)
   {
     /* Read object textures */
     Fread(&_num_object_textures, sizeof(int), 1, f);
     printDebug("Load", "_num_object_textures = %u", _num_object_textures);
     //FIXME: endian

     if (_num_object_textures > 0)
     {
       _object_textures = new tr2_object_texture_t[_num_object_textures];

       Fread(_object_textures, sizeof(tr2_object_texture_t),
	     _num_object_textures, f);
     }
     //FIXME: endian
   }


   if (percent)
		(*percent)(90);

   if (mEngineVersion == TR_VERSION_4)
   {
     unsigned char zzbuf[4];
     Fread(zzbuf, 1, 3, f); // skip "SPR"
     zzbuf[3] = 0;

     printDebug("Load", "TR4 checking if %s == SPR", zzbuf);
   }

   /* Read sprite textures */
   Fread(&_num_sprite_textures, sizeof(int), 1, f);
   //FIXME: endian
   printDebug("Load", "_num_sprite_textures = %u", _num_sprite_textures);

	_sprite_textures = 0x0;

   if (_num_sprite_textures > 0)
   {
     _sprite_textures = new tr2_sprite_texture_t[_num_sprite_textures];
     Fread(_sprite_textures, sizeof(tr2_sprite_texture_t),
	   _num_sprite_textures, f);
   }
   //FIXME: endian

   /* Read sprite texture data (?) */
   Fread(&_num_sprite_sequences, sizeof(int), 1, f);
   //FIXME: endian
   printDebug("Load", "_num_sprite_sequences = %u", _num_sprite_sequences);

	_sprite_sequences = 0x0;

   if (_num_sprite_sequences > 0)
   {
     _sprite_sequences = new tr2_sprite_sequence_t[_num_sprite_sequences];
     Fread(_sprite_sequences, sizeof(tr2_sprite_sequence_t),
	   _num_sprite_sequences, f);
   }
   //FIXME: endian

   /* Read cameras */
   Fread(&_num_cameras, sizeof(_num_cameras), 1, f);
   //FIXME: endian
   printDebug("Load", "_num_cameras = %i", _num_cameras);

	_cameras = 0x0;

   if (_num_cameras > 0)
   {
     _cameras = new tr2_camera_t[_num_cameras];
     Fread(_cameras, sizeof(tr2_camera_t), _num_cameras, f);
     //FIXME: endian
   }

	if (mEngineVersion == TR_VERSION_4)
	{
		int num_ex_cam;
		tr4_extra_camera_t *ex_cam;

		Fread(&num_ex_cam, 4, 1, f);

		printDebug("Load", "num_extra_cam = %i", num_ex_cam);

		if (num_ex_cam > 0)
		{
			ex_cam = new tr4_extra_camera_t[num_ex_cam];
			Fread(ex_cam, sizeof(tr4_extra_camera_t), num_ex_cam, f);
			delete [] ex_cam;
		}
	}

   /* Read sound effects (?) */
   Fread(&_num_sound_sources, sizeof(_num_sound_sources), 1, f);
   //FIXME: endian
   printDebug("Load", "_num_sound_sources = %i", _num_sound_sources);

	_sound_sources = 0x0;

   if (_num_sound_sources > 0)
   {
     _sound_sources =
       (tr2_sound_source_t*) new unsigned char[_num_sound_sources*40];

     if (mEngineVersion == TR_VERSION_4)
     {
		  // Mongoose 2002.04.04, was 40 sized reads before extra_camera
       Fread(_sound_sources, sizeof(tr2_sound_source_t),
				 _num_sound_sources, f);
     }
     else
     {
       Fread(_sound_sources, sizeof(tr2_sound_source_t),
	     _num_sound_sources, f);
     }
     //FIXME: endian
   }

#ifdef OBSOLETE
   if (mEngineVersion == TR_VERSION_4)
   {
     unsigned int num_ZZ;
     unsigned char  zzbuf[17];
     Fread(&num_ZZ, 1, sizeof(num_ZZ), f);

     while (num_ZZ--)
     {
       Fread(zzbuf, 1, 16, f);
       zzbuf[16] = 0;
       printDebug("Load", "TR4 zz dump '%s'", zzbuf);
     }
   }
#endif

   /* Read boxes */
   Fread(&_num_boxes, sizeof(_num_boxes), 1, f);
   //FIXME: endian
   printDebug("Load", "_num_boxes = %i", _num_boxes);

	_boxes = 0x0;

   if (_num_boxes > 0)
   {
     _boxes = new tr2_box_t[_num_boxes];

     if (mEngineVersion == TR_VERSION_1)
     {
		  struct tr1_box
		  {
			  int zmin, zmax, xmin, xmax;
			  short true_floor, overlap_index;
		  }  __attribute__ ((packed)) *tr1box;

		  tr1box = new tr1_box[_num_boxes];

		  Fread(tr1box, sizeof(struct tr1_box), _num_boxes, f);
		  //FIXME: endian

         for (j = 0; j < _num_boxes; ++j)
			{
            _boxes[j].zmin = tr1box[j].zmin / 1024;
            _boxes[j].zmax = tr1box[j].zmax / 1024;
            _boxes[j].xmin = tr1box[j].xmin / 1024;
            _boxes[j].xmax = tr1box[j].xmax / 1024;
            _boxes[j].true_floor = tr1box[j].true_floor;
            _boxes[j].overlap_index = tr1box[j].overlap_index;
			}

			delete [] tr1box;
     }
     else
     {
       Fread(_boxes, sizeof(tr2_box_t), _num_boxes, f);
     }
     //FIXME: endian
   }

   /* Read overlaps (?) */
   Fread(&_num_overlaps, sizeof(_num_overlaps), 1, f);
   //FIXME: endian
   printDebug("Load", "_num_overlaps = %i", _num_overlaps);

	_overlaps = 0x0;

   if (_num_overlaps > 0)
   {
     _overlaps = new short[_num_overlaps];
     Fread(_overlaps, 2, _num_overlaps, f);
     //FIXME: endian
   }

	_zones = 0x0;

   /* Read Zones */
   if (_num_boxes > 0)
   {
     _zones = new short[_num_boxes*10];

     if (mEngineVersion == TR_VERSION_1)
     {
       Fread(_zones, 12, _num_boxes, f);
     }
     else
     {
       Fread(_zones, 20, _num_boxes, f);
     }
     //FIXME: endian
   }

   /* Read animation textures (?) */
   Fread(&_num_animated_textures, sizeof(_num_animated_textures), 1, f);
   //FIXME: endian
   printDebug("Load", "_num_animated_textures = %i", _num_animated_textures);

	_animated_textures = 0x0;

   if (_num_animated_textures > 0)
   {
     _animated_textures = new short[_num_animated_textures];
     Fread(_animated_textures, 2, _num_animated_textures, f);
     //FIXME: endian
   }

   if (mEngineVersion >= TR_VERSION_3)
   {
      /* Read object textures */
      if (mEngineVersion == TR_VERSION_4)
      {
			unsigned char zzbuf[5];
			Fread(zzbuf, 1, 4, f); // skip "TEX"
			//!! this should be 3, but we have a bug...
			zzbuf[4] = 0;
			printDebug("Load", "TR4 checking %s == TEX", zzbuf);
      }

      Fread(&_num_object_textures, sizeof(_num_object_textures), 1, f);
      //FIXME: endian

      printDebug("Load", "_num_object_textures = %i", _num_object_textures);

		_object_textures = 0x0;

      if (_num_object_textures > 0)
      {
			// Used to be 2 * num, and I forgot why...
			_object_textures = new tr2_object_texture_t[_num_object_textures];

			//FIXME: This is fu fu fu fu fu fu
			if (mEngineVersion == TR_VERSION_4)
	      {
				int jjj, kkk;
				tr4_object_texture_t *tr4_tex;


				tr4_tex = new tr4_object_texture_t[_num_object_textures];

				Fread(tr4_tex, 38, _num_object_textures, f);

				for (jjj = 0; jjj < (int)_num_object_textures; ++jjj)
				{
					_object_textures[jjj].transparency_flags =
						tr4_tex[jjj].attribute;

					_object_textures[jjj].tile =
						(unsigned short)tr4_tex[jjj].tile & 0x7fff;

					for (kkk = 0; kkk < 4; ++kkk)
	            {
						_object_textures[jjj].vertices[kkk].xcoordinate =
							tr4_tex[jjj].vertices[kkk].xcoordinate;
						_object_textures[jjj].vertices[kkk].xpixel =
							tr4_tex[jjj].vertices[kkk].xpixel;
						_object_textures[jjj].vertices[kkk].ycoordinate =
							tr4_tex[jjj].vertices[kkk].ycoordinate;
						_object_textures[jjj].vertices[kkk].ypixel =
							tr4_tex[jjj].vertices[kkk].ypixel;
					}
				}

				delete [] tr4_tex;
			}
			else
			{
				Fread(_object_textures, sizeof(tr2_object_texture_t),
						_num_object_textures, f);
			}
      }
      //FIXME: endian
   }

   /* Read items */
   Fread(&_num_items, sizeof(_num_items), 1, f);
   //FIXME: endian
   printDebug("Load", "_num_items = %i", _num_items);

	_items = 0x0;

   if (_num_items > 0)
   {
     _items = new tr2_item_t[_num_items];

     if (mEngineVersion == TR_VERSION_1)
     {
       for (i = 0; i < _num_items; ++i)
       {
			 Fread(&_items[i], sizeof(tr2_item_t) - 2, 1, f);
			 _items[i].flags = _items[i].intensity2;
			 _items[i].intensity2 = _items[i].intensity1;
       }
     }
     else
     {
       Fread(_items, sizeof(tr2_item_t), _num_items, f);
     }
   }
   //FIXME: endian

   /* Read LightMaps */
   _light_map = new unsigned char[32 * 256];

   if (mEngineVersion != TR_VERSION_4)
   {
     Fread(_light_map, 32, 256, f);
   }

   if (mEngineVersion == TR_VERSION_1)
   {
      /* read the 8-bit palette */
      Fread(_palette8, sizeof(tr2_colour_t), 256, f);
      printDebug("Load", "Read TR 1 palette");

      // build 16-bit textiles from 8-bit
      // (no extra colours, but creates consistent .TR2 file)
      for (i = 0; i < (int)_num_textiles; ++i)
      {
         unsigned short argb;
         double colour_tmp;

         for (j = 0; j < (256 * 256); ++j)
			{
				colour_tmp = _palette8[_textile8[i].tile[j]].r & 0x3f;
				colour_tmp = colour_tmp * 31.0 / 63.0;
				argb = ((int)colour_tmp) << 10;
				colour_tmp = _palette8[_textile8[i].tile[j]].g & 0x3f;
				colour_tmp = colour_tmp * 31.0 / 63.0;
				argb |= ((int)colour_tmp) << 5;
				colour_tmp = _palette8[_textile8[i].tile[j]].b & 0x3f;
				colour_tmp = colour_tmp * 31.0 / 63.0;
				argb |= ((int)colour_tmp);
				argb &= 0x7fff;   // ???

				if (_textile8[i].tile[j] != 0)
					argb |= 0x8000;

				_textile16[i].tile[j] = argb;
			}
      }
   }

   /* Read cinematic frames */
   if (mEngineVersion == TR_VERSION_4)
   {
		unsigned int num_ai_data;


      Fread(&num_ai_data, 4, 1, f);
		printDebug("Load", "num_ai_data = %i", num_ai_data);

		tr4_ai_object_t *ai_obj = 0x0;

      if (num_ai_data > 0)
      {
			ai_obj = new tr4_ai_object_t[num_ai_data];
			Fread(ai_obj, sizeof(tr4_ai_object_t), num_ai_data, f);

			delete [] ai_obj;
      }
   }
   else
   {
		unsigned short num_cinematic_frames;

     Fread(&num_cinematic_frames, sizeof(num_cinematic_frames), 1, f);
     //FIXME: endian

	  _num_cinematic_frames = num_cinematic_frames;

     printDebug("Load", "_num_cinematic_frames = %i", _num_cinematic_frames);

	  _cinematic_frames = 0x0;

     if (_num_cinematic_frames > 0)
     {
       _cinematic_frames = new tr2_cinematic_frame_t[_num_cinematic_frames];

       Fread(_cinematic_frames, sizeof(tr2_cinematic_frame_t),
	     _num_cinematic_frames, f);
         // There may or may not be endian conversion required here - I have
         // no idea what this data is.
     }
   }

   /* Read demodata (?) */
   Fread(&_num_demo_data, sizeof(_num_demo_data), 1, f);
   //FIXME: endian
   printDebug("Load", "_num_demo_data = %i", _num_demo_data);

	_demo_data = 0x0;

   if (_num_demo_data > 0)
   {
     _demo_data = new unsigned char[_num_demo_data];
     Fread(_demo_data, 1, _num_demo_data, f);
     // There may or may not be endian conversion required here - I have
     // no idea what this data is.
   }

   /* Read SoundMap */
   mSoundMap = new short[370];

   if (mEngineVersion == TR_VERSION_1)
   {
     Fread(mSoundMap, sizeof(short), 256, f);
     //memset(_sound_map, 0, 370 * sizeof(short));   //FIXME: KLUDGE!!!
   }
   else
   {
     if (mEngineVersion == TR_VERSION_4)
     {
       Fread(mSoundMap, sizeof(short), 370, f);
     }
     else
     {
       Fread(mSoundMap, sizeof(short), 370, f);
     }
   }
   //FIXME: endian

   /* Read SoundDetails */
   Fread(&mNumSoundDetails, sizeof(mNumSoundDetails), 1, f);
   printDebug("Load", "mNumSoundDetails = %i", mNumSoundDetails);
   //FIXME: endian

	mSoundDetails = 0x0;

   if (mNumSoundDetails > 0)
   {
     mSoundDetails = new tr2_sound_details_t[mNumSoundDetails];
     Fread(mSoundDetails, sizeof(tr2_sound_details_t), mNumSoundDetails, f);
   }
   //FIXME: endian

	// Read sound sample indices
	mSampleIndices = 0x0;
	mNumSampleIndices = 0;
	mRiffDataSz = 0;
	mRiffData = 0x0;
	mNumTR4Samples = 0;
	mTR4Samples = 0x0;

   switch (mEngineVersion)
   {
	case TR_VERSION_1:
		Fread(&mRiffDataSz, 4, 1, f);
		printDebug("Load", "mRiffDataSz = %ibytes", mRiffDataSz);

		if (mRiffDataSz > 0)
		{
			mRiffData = new unsigned char[mRiffDataSz];
			Fread(mRiffData, 1, mRiffDataSz, f);
		}

		Fread(&mNumSampleIndices, 4, 1, f);
		printDebug("Load", "mNumSampleIndices = %i", mNumSampleIndices);

		if (mNumSampleIndices > 0)
		{
			mSampleIndices = new int[mNumSampleIndices];
			// FIXME: (Endian)
			Fread(mSampleIndices, 4, mNumSampleIndices, f);
		}
		break;
	case TR_VERSION_4:
		mFreadMode = TR_FREAD_NORMAL;
		// 0x46464952

		// FIXME: (Endian) Read bitu32 / u_int32_t
		Fread(&mNumTR4Samples, 4, 1, f);
		printDebug("Load", "mNumTR4Samples = %i", mNumTR4Samples);

		mRiffDataSz = 0;
		mTR4Samples = new unsigned char *[mNumTR4Samples];
		mTR4SamplesSz = new unsigned int[mNumTR4Samples];

		memset(mTR4SamplesSz, 0, mNumTR4Samples*4);

		for (i = 0; i < (int)mNumTR4Samples; ++i)
		{
			unsigned int sizeCompressed;
			unsigned int sizeUncompressed;
			unsigned char *compressedSoundSample;
			unsigned char *unCompressedSoundSample;
			int zErr;
			uLongf libzUncompressedSize;


			Fread(&sizeUncompressed, 4, 1, f);
			printDebug("Load", " sizeUncompressed = %i", sizeUncompressed);
			Fread(&sizeCompressed, 4, 1, f);
			printDebug("Load", " sizeCompressed = %i", sizeCompressed);

			compressedSoundSample = new unsigned char[sizeCompressed];
			unCompressedSoundSample = new unsigned char[sizeUncompressed];

			//printDebug("Load", "   %lubytes read from file", ftell(f));
			Fread(compressedSoundSample, sizeCompressed, 1, f);

			printDebug("Load", " %c%c%c%c should be RIFF",
					 compressedSoundSample[0],
					 compressedSoundSample[1],
					 compressedSoundSample[2],
					 compressedSoundSample[3]);

//#define NEVER_DECOMPRESS
#ifdef NEVER_DECOMPRESS
			mTR4Samples[i] = compressedSoundSample;
			mTR4SamplesSz[i] = sizeCompressed;

			delete [] unCompressedSoundSample;
#else
			// Decompress the sample
			libzUncompressedSize = sizeUncompressed;

			zErr = uncompress(unCompressedSoundSample,
									&libzUncompressedSize,
									compressedSoundSample,
									sizeCompressed);

			sizeUncompressed = libzUncompressedSize;

			switch (zErr)
			{
			case Z_MEM_ERROR:
				printDebug("Load", "   Decompress Error: not enough memory");
				break;
			case Z_BUF_ERROR:
				printDebug("Load", "   Decompress Error: output buffer too small");
				break;
			case Z_DATA_ERROR:
				printDebug("Load", "   Decompress Error: input data was corrupted");
				break;
			case Z_OK:
				printDebug("Load", "   Decompress OK");
				break;
			default:
				printDebug("Load", "   Decompress Error: decompress error #%i", zErr);
			}

			// Hhhmm... handle uncompressed RIFFs too?
			if (zErr == Z_OK)
			{
				mTR4Samples[i] = unCompressedSoundSample;
				mTR4SamplesSz[i] = sizeUncompressed;
				delete [] compressedSoundSample;
			}
			else
			{
				printDebug("Load", "   %lubytes read from file", ftell(f));
				mTR4Samples[i] = compressedSoundSample;
				mTR4SamplesSz[i] = sizeCompressed;
				delete [] unCompressedSoundSample;
			}
#endif
		}
		break;
	default:
		// FIXME: (Endian) Read bit32 / int32_t
		Fread(&mNumSampleIndices, 4, 1, f);
		printDebug("Load", "mNumSampleIndices = %i", mNumSampleIndices);

		if (mNumSampleIndices > 0)
		{
			mSampleIndices = new int[mNumSampleIndices];
			// FIXME: (Endian)
			Fread(mSampleIndices, 4, mNumSampleIndices, f);
		}
   }

#ifdef ZLIB_SUPPORT
   if (mCompressedLevelData)
   {
     printDebug("Load", "Freeing uncompressed TR4 data");
     delete [] mCompressedLevelData;
   }

	// FIXME: memory damage?
	mCompressedLevelData = NULL;
#endif

   fclose(f);

   if (percent)
		(*percent)(100);

   return 0;
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////

float TombRaider::adjustTexel(unsigned char texel, char offset)
{
   if (offset >= 0)
      texel++;
   else
      texel--;

   return ((float)texel / 255.0);
}


void TombRaider::computeRotationAngles(unsigned short **frame,
													unsigned int *frame_offset,
													unsigned int *angle_offset,
													float *x, float *y, float *z)
{
	unsigned short itmp, itmp2;
	double angle;


	itmp = (*frame)[(*frame_offset) + (*angle_offset)++];

	if (Engine() == TR_VERSION_1)
	{
		// All angles are three-axis
		angle = (itmp >> 4) & 0x03ff;
		angle *= 360.0 / 1024.0;
		*x = angle;

		itmp2 = (itmp << 6) & 0x03c0;

		// Get Z rotation
		itmp = (*frame)[(*frame_offset) + (*angle_offset)];
		++(*angle_offset);

		itmp2 |= (itmp >> 10) & 0x003f;
		angle = itmp2;
		angle *= 360.0 / 1024.0;
		*y = angle;

		angle = itmp & 0x3ff;
		angle *= 360.0 / 1024.0;
		*z = angle;
	}
	else if (itmp & 0xc000)
	{
		// TR2, TR3, TR4 - single axis of rotation
		if (Engine() == TR_VERSION_4)
		{
			angle = itmp & 0x0fff;
			angle /= 4096.0;
			angle *= 360.0;
		}
		else
		{
			angle = itmp & 0x3ff;
			angle /= 1024.0;
			angle *= 360.0;
		}

		switch (itmp & 0xc000)
		{
		case 0x4000:
			*x = angle;
			break;
		case 0x8000:
			*y = angle;
			break;
		case 0xc000:
			*z = angle;
			break;
		}
	}
	else    // TR2, TR3, TR4 - three axes
	{
		angle = (itmp >> 4) & 0x03ff;
		angle *= 360.0 / 1024.0;
		*x = angle;

		itmp2 = (itmp << 6) & 0x03c0;
		itmp = (*frame)[(*frame_offset) + (*angle_offset)++]; // get Z rotation

		itmp2 |= (itmp >> 10) & 0x003f;
		angle = itmp2;
		angle *= 360.0 / 1024.0;
		*y = angle;

		angle = itmp & 0x3ff;
		angle *= 360.0 / 1024.0;
		*z = angle;
	}
}


void TombRaider::computeUV(tr2_object_texture_vert_t *st, float *u, float *v)
{
	unsigned char x, y;


	if (!st || !u || !v)
		return;

	x = st->xpixel;
	y = st->ypixel;

	x += (char)st->xcoordinate;
	y += (char)st->ycoordinate;

	//x += ((char)st->xcoordinate >= 0) ? 1 : -1;
	//y += ((char)st->ycoordinate >= 0) ? 1 : -1;

	*u = (float)x / 255.0;
	*v = (float)y / 255.0;
}


int TombRaider::getBumpMapCount()
{
	return _num_bump_map_textures / 2;
}


void TombRaider::getColor(int index, float color[4])
{
	switch (getEngine())
	{
	case TR_VERSION_1:
		color[0] = _palette8[index].r / 64.0;
		color[1] = _palette8[index].g / 64.0;
		color[2] = _palette8[index].b / 64.0;
		//color[0] = (_palette8[index].r & 0xfd) / 64.0;
		//color[1] = (_palette8[index].g & 0xfd) / 64.0;
		//color[2] = (_palette8[index].b & 0xfd) / 64.0;
		color[3] = 1.0;
		break;
	default:
		color[0] = (float)(_palette16[index] & 0xff) / 256.0;
		color[1] = (float)((_palette16[index] >> 8) & 0xff) / 256.0;
		color[2] = (float)((_palette16[index] >> 16) & 0xff) / 256.0;
		color[3] = 1.0;
	}
}


tr2_version_type TombRaider::getEngine()
{
	return mEngineVersion;
}


void TombRaider::getMeshCollisionInfo(unsigned int meshIndex,
												  float center[3], float *radius)
{
	if ((int)meshIndex > mMeshCount)
	{
		print("getMeshCollisionInfo", "Assertion error: invalid mesh index\n");
		return;
	}

	center[0] = mMeshes[meshIndex].centre.x;
	center[1] = mMeshes[meshIndex].centre.y;
	center[2] = mMeshes[meshIndex].centre.z;
	*radius = mMeshes[meshIndex].collision_size;
}


int TombRaider::getMeshCount()
{
	return mMeshCount;
}


// FIXME: Needs refinement once the ideal format it's feeding is refined
// I should stick a HACK postfix on the method name - it's temporary
// until an array format can be crafted from a pinned down design and
// RE notes review session ( eg what about TR5? )
void TombRaider::getMeshColoredRectangle(unsigned int meshIndex,
													  unsigned int faceIndex,
													  int *index, float *color)
{
	unsigned int t1_1, t1_2, t1_3, t2_1, t2_2, t2_3;


	if ((int)meshIndex > mMeshCount)
	{
		print("getMeshColoredRectangle", "Assertion error: invalid mesh index\n");
		return;
	}

	// Make 2 triangles from one quad!
	t1_1 = 0;
	t1_2 = 1;
	t1_3 = 2;

	t2_1 = 3;
	t2_2 = 0;
	t2_3 = 2;

	index[0] = mMeshes[meshIndex].coloured_rectangles[faceIndex].vertices[t1_1];
	index[1] = mMeshes[meshIndex].coloured_rectangles[faceIndex].vertices[t1_2];
	index[2] = mMeshes[meshIndex].coloured_rectangles[faceIndex].vertices[t1_3];
	index[3] = mMeshes[meshIndex].coloured_rectangles[faceIndex].vertices[t2_1];
	index[4] = mMeshes[meshIndex].coloured_rectangles[faceIndex].vertices[t2_2];
	index[5] = mMeshes[meshIndex].coloured_rectangles[faceIndex].vertices[t2_3];

	switch (Engine())
	{
	case TR_VERSION_1:
		getColor(mMeshes[meshIndex].coloured_rectangles[faceIndex].texture & 0xff, color);
		break;
	default:
		getColor((mMeshes[meshIndex].coloured_rectangles[faceIndex].texture>>8) & 0xff, color);
	}
}


void TombRaider::getMeshColoredTriangle(unsigned int meshIndex,
													 unsigned int faceIndex,
													 int *index, float *color)
{
	if ((int)meshIndex > mMeshCount)
	{
		print("getMeshColoredTriangle", "Assertion error: invalid mesh index\n");
		return;
	}

	index[0] = mMeshes[meshIndex].coloured_triangles[faceIndex].vertices[0];
	index[1] = mMeshes[meshIndex].coloured_triangles[faceIndex].vertices[1];
	index[2] = mMeshes[meshIndex].coloured_triangles[faceIndex].vertices[2];

	switch (Engine())
	{
	case TR_VERSION_1:
		getColor(mMeshes[meshIndex].coloured_triangles[faceIndex].texture & 0xff,
					color);
		break;
	default:
		getColor((mMeshes[meshIndex].coloured_triangles[faceIndex].texture>>8) & 0xff,
					color);
	}
}


void TombRaider::getMeshTexturedRectangle(unsigned int meshIndex,
													  unsigned int faceIndex,
													  int *index, float *st, int *texture,
													  unsigned short *transparency)
{
	unsigned int t1_1, t1_2, t1_3, t2_1, t2_2, t2_3;
	static bool givenWarning = false; // hahaha... okay less spewing
	tr2_mesh_t *meshes;
	tr2_object_texture_t *object_texture;
   int t_index;
	unsigned int i, m;


	if ((int)meshIndex > mMeshCount)
	{
		print("getMeshTexturedRectangle", "Assertion error: invalid mesh index\n");
		return;
	}

	m = meshIndex; // lazy
	meshes = mMeshes; // lazy
	object_texture = _object_textures; // lazy
	i = faceIndex; // lazy

	// Make 2 triangles from one quad!
	t1_1 = 0;
	t1_2 = 1;
	t1_3 = 2;

	t2_1 = 3;
	t2_2 = 0;
	t2_3 = 2;

	t_index = meshes[m].textured_rectangles[i].texture;

	index[0] = meshes[m].textured_rectangles[i].vertices[t1_1];
	index[1] = meshes[m].textured_rectangles[i].vertices[t1_2];
	index[2] = meshes[m].textured_rectangles[i].vertices[t1_3];
	index[3] = meshes[m].textured_rectangles[i].vertices[t2_1];
	index[4] = meshes[m].textured_rectangles[i].vertices[t2_2];
	index[5] = meshes[m].textured_rectangles[i].vertices[t2_3];

	computeUV(object_texture[t_index].vertices+t1_1, (st),   (st)+1);
	computeUV(object_texture[t_index].vertices+t1_2, (st)+2, (st)+3);
	computeUV(object_texture[t_index].vertices+t1_3, (st)+4, (st)+5);
	computeUV(object_texture[t_index].vertices+t2_1, (st)+6, (st)+7);
	computeUV(object_texture[t_index].vertices+t2_2, (st)+8, (st)+9);
	computeUV(object_texture[t_index].vertices+t2_3, (st)+10, (st)+11);

	*texture = object_texture[t_index].tile;
	*transparency = object_texture[t_index].transparency_flags;

	// TR3+ alpha Textured polygons
	if (!givenWarning && *transparency == 2)
	{
		givenWarning = true;

		// FIXME: Use Material class to handle greyscale alpha intensity
		//        (partial alpha)
		print("getMeshTexturedRectangle",
				"TR3+ greyscale alpha intensity not implmented, %s:%i",
				__FILE__, __LINE__);
	}
}


void TombRaider::getMeshTexturedTriangle(unsigned int meshIndex,
													  unsigned int faceIndex,
													  int *index, float *st, int *texture,
													  unsigned short *transparency)
{
	static bool givenWarning = false; // hahaha... okay less spewing
	tr2_mesh_t *meshes;
	tr2_object_texture_t *object_texture;
   int t_index;
	unsigned int i;


	if ((int)meshIndex > mMeshCount)
	{
		print("getMeshTexturedTriangle", "Assertion error: invalid mesh index\n");
		return;
	}

	meshes = mMeshes; // lazy
	object_texture = _object_textures; // lazy
	i = faceIndex; // lazy

	t_index = meshes[meshIndex].textured_triangles[i].texture;

	index[0] = meshes[meshIndex].textured_triangles[i].vertices[0];
	index[1] = meshes[meshIndex].textured_triangles[i].vertices[1];
	index[2] = meshes[meshIndex].textured_triangles[i].vertices[2];

	computeUV(object_texture[t_index].vertices,   (st),   (st)+1);
	computeUV(object_texture[t_index].vertices+1, (st)+2, (st)+3);
	computeUV(object_texture[t_index].vertices+2, (st)+4, (st)+5);

	*texture = object_texture[t_index].tile;
	*transparency = object_texture[t_index].transparency_flags;

	// TR3+ alpha Textured polygons
	if (!givenWarning && *transparency == 2)
	{
		givenWarning = true;

		// FIXME: Use Material class to handle greyscale alpha intensity
		//        (partial alpha)
		print("getMeshTexturedTriangle",
				"TR3+ greyscale alpha intensity not implmented, %s:%i",
				__FILE__, __LINE__);
	}
}


int TombRaider::getMeshTexturedTriangleCount(unsigned int meshIndex)
{
	if ((int)meshIndex > mMeshCount)
	{
		print("getMeshTexturedTriangleCount", "Assertion error: invalid mesh index\n");
		return 0;
	}

	return ((mMeshes[meshIndex].num_textured_triangles <= 0) ? 0 :
			  mMeshes[meshIndex].num_textured_triangles);
}


int TombRaider::getMeshColoredTriangleCount(unsigned int meshIndex)
{
	if ((int)meshIndex > mMeshCount)
	{
		print("getMeshColoredTriangleCount", "Assertion error: invalid mesh index\n");
		return 0;
	}

	return ((mMeshes[meshIndex].num_coloured_triangles <= 0) ? 0 :
			  mMeshes[meshIndex].num_coloured_triangles);
}


int TombRaider::getMeshTexturedRectangleCount(unsigned int meshIndex)
{
	if ((int)meshIndex > mMeshCount)
	{
		print("getMeshTexturedRectangleCount", "Assertion error: invalid mesh index\n");
		return 0;
	}

	return ((mMeshes[meshIndex].num_textured_rectangles <= 0) ? 0 :
			  mMeshes[meshIndex].num_textured_rectangles);
}


int TombRaider::getMeshColoredRectangleCount(unsigned int meshIndex)
{
	if ((int)meshIndex > mMeshCount)
	{
		print("getMeshColoredRectangleCount", "Assertion error: invalid mesh index\n");
		return 0;
	}

	return ((mMeshes[meshIndex].num_coloured_rectangles <= 0) ? 0:
			  mMeshes[meshIndex].num_coloured_rectangles);
}


// FIXME: Perhaps making color an 8bit intensity would be a better idea
void TombRaider::getMeshVertexArrays(unsigned int meshIndex,
												 unsigned int *vertexCount, float **verts,
												 unsigned int *normalCount, float **norms,
												 unsigned int *colorCount, float **colors)
{
	unsigned int i;
	float colorValue;


	*vertexCount = 0;
	*verts = 0x0;
	*normalCount = 0;
	*norms = 0x0;
	*colorCount = 0;
	*colors = 0x0;

	if ((int)meshIndex > mMeshCount || mMeshes[meshIndex].num_vertices < 0)
		return;

	// Vertices
	*vertexCount = mMeshes[meshIndex].num_vertices;
	*verts = new float[*vertexCount * 3];

	for (i = 0; i < *vertexCount; ++i)
	{
		(*verts)[i*3]   = mMeshes[meshIndex].vertices[i].x;
		(*verts)[i*3+1] = mMeshes[meshIndex].vertices[i].y;
		(*verts)[i*3+2] = mMeshes[meshIndex].vertices[i].z;
	}


	// Normals, if any
   if (mMeshes[meshIndex].num_normals > 0 &&
		 mMeshes[meshIndex].normals &&
		 mMeshes[meshIndex].num_normals == mMeshes[meshIndex].num_vertices)
	{
		*normalCount = mMeshes[meshIndex].num_vertices;
		*norms = new float[*normalCount * 3];

      for (i = 0; i < *normalCount; ++i)
		{
			(*norms)[i*3]   = mMeshes[meshIndex].normals[i].x;
			(*norms)[i*3+1] = mMeshes[meshIndex].normals[i].y;
			(*norms)[i*3+2] = mMeshes[meshIndex].normals[i].z;
		}
	}
	// Vertex lighting/colors, if any
   else if (mMeshes[meshIndex].num_vertices > 0 &&
				mMeshes[meshIndex].mesh_lights)
   {
		*colorCount = mMeshes[meshIndex].num_vertices;

#ifdef MESH_COLORS_SHOULD_BR_RGBA
		*colors = new float[*colorCount * 4];
#else
		*colors = new float[*colorCount];
#endif

      for (i = 0; i < *colorCount; ++i)
		{
			colorValue = mMeshes[meshIndex].mesh_lights[i];

			switch (Engine())
			{
			case TR_VERSION_4:
			case TR_VERSION_3:
				colorValue /= 16384.0;
			default:
				colorValue = (1.0 - (colorValue / 8192.0));
				break;
			}

#ifdef MESH_COLORS_SHOULD_BR_RGBA
			(*colors)[i*4+0] = colorValue;
			(*colors)[i*4+1] = colorValue;
			(*colors)[i*4+2] = colorValue;
			(*colors)[i*4+3] = 1.0;
#else
			(*colors)[i] = colorValue;
#endif
      }
	}
}


int TombRaider::getRoomBox(unsigned int roomIndex, unsigned int index,
									float *xyzA, float *xyzB,
									float *xyzC, float *xyzD)
{

	if (!getRoomBoxCount(roomIndex) || index > getRoomBoxCount(roomIndex))
		return -1;

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	default: // TR1-TR5
      xyzA[0] = (unsigned short)_boxes[index].xmin * 1024.0;
      xyzA[1] = (short)_boxes[index].true_floor;
      xyzA[2] = (unsigned short)_boxes[index].zmin * 1024.0;

      xyzB[0] = (unsigned short)_boxes[index].xmax * 1024.0;
      xyzB[1] = (short)_boxes[index].true_floor;
      xyzB[2] = (unsigned short)_boxes[index].zmin * 1024.0;

      xyzC[0] = (unsigned short)_boxes[index].xmax * 1024.0;
      xyzC[1] = (short)_boxes[index].true_floor;
      xyzC[2] = (unsigned short)_boxes[index].zmax * 1024.0;

      xyzD[0] = (unsigned short)_boxes[index].xmin * 1024.0;
      xyzD[1] = (short)_boxes[index].true_floor;
      xyzD[2] = (unsigned short)_boxes[index].zmax * 1024.0;
	}

	return 0;
}


unsigned int TombRaider::getRoomBoxCount(unsigned int roomIndex)
{
	if (!isRoomValid(roomIndex))
		return 0;

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	default:  // TR1-TR5
		return _num_boxes;
	}

	return 0;
}


void TombRaider::getRoomInfo(unsigned int index,
									  unsigned int *flags, float pos[3],
									  float bboxMin[3], float bboxMax[3])
{
	unsigned int i, n;
	float f;


	if (!isRoomValid(index))
		return;

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	case TR_VERSION_5:
		// Flags
		*flags = mRoomsTR5[index].roomFlag; // Needs to be generic flags in class

		// Positioning
		pos[0] = mRoomsTR5[index].roomX;
		pos[1] = 0.0f;
		pos[2] = mRoomsTR5[index].roomZ;

		// Bounding box setup
		bboxMin[0] = 0.0;
		bboxMin[1] = 0.0;
		bboxMin[2] = 0.0;
		bboxMax[0] = 0.0;
		bboxMax[1] = 0.0;
		bboxMax[2] = 0.0;

		// Bounding Box setup
		for (i = 0; i < mRoomsTR5[index].numLayers; ++i)
		{
			// FIXME: check the boxes are in min, max order in TRC

			if (i == 0)
			{
				bboxMin[0] = mRoomsTR5[index].layers[i].layerBoundingBoxX1;
				bboxMin[1] = mRoomsTR5[index].layers[i].layerBoundingBoxY1;
				bboxMin[2] = mRoomsTR5[index].layers[i].layerBoundingBoxZ1;

				bboxMax[0] = mRoomsTR5[index].layers[i].layerBoundingBoxX1;
				bboxMax[1] = mRoomsTR5[index].layers[i].layerBoundingBoxY2;
				bboxMax[2] = mRoomsTR5[index].layers[i].layerBoundingBoxZ2;

				continue;
			}

			if (mRoomsTR5[index].layers[i].layerBoundingBoxX1 < bboxMin[0])
				bboxMin[0] = mRoomsTR5[index].layers[i].layerBoundingBoxX1;

			if (mRoomsTR5[index].layers[i].layerBoundingBoxY1 < bboxMin[1])
				bboxMin[1] = mRoomsTR5[index].layers[i].layerBoundingBoxY1;

			if (mRoomsTR5[index].layers[i].layerBoundingBoxZ1 < bboxMin[2])
				bboxMin[2] = mRoomsTR5[index].layers[i].layerBoundingBoxZ1;


			if (mRoomsTR5[index].layers[i].layerBoundingBoxX2 < bboxMax[0])
				bboxMax[0] = mRoomsTR5[index].layers[i].layerBoundingBoxX2;

			if (mRoomsTR5[index].layers[i].layerBoundingBoxY2 < bboxMax[1])
				bboxMax[1] = mRoomsTR5[index].layers[i].layerBoundingBoxY2;

			if (mRoomsTR5[index].layers[i].layerBoundingBoxZ2 < bboxMax[2])
				bboxMax[2] = mRoomsTR5[index].layers[i].layerBoundingBoxZ2;
		}
		break;
	case TR_VERSION_1:
	case TR_VERSION_2:
	case TR_VERSION_3:
	case TR_VERSION_4:
		// Flags
		*flags = _rooms[index].flags; // Needs to be generic flags in class

		// Positioning
		pos[0] = _rooms[index].info.x;
		pos[1] = 0.0f;
		pos[2] = _rooms[index].info.z;

		bboxMin[0] = 0.0;
		bboxMin[1] = 0.0;
		bboxMin[2] = 0.0;
		bboxMax[0] = 0.0;
		bboxMax[1] = 0.0;
		bboxMax[2] = 0.0;

		// Bounding Box setup
		n = ((_rooms[index].room_data.num_vertices < 0) ? 0 :
			  _rooms[index].room_data.num_vertices);

		for (i = 0; i < n; ++i)
		{
			if (i == 0)
			{
				bboxMin[0] = _rooms[index].room_data.vertices[i].vertex.x;
				bboxMin[1] = _rooms[index].room_data.vertices[i].vertex.y;
				bboxMin[2] = _rooms[index].room_data.vertices[i].vertex.z;

				bboxMax[0] = _rooms[index].room_data.vertices[i].vertex.x;
				bboxMax[1] = _rooms[index].room_data.vertices[i].vertex.y;
				bboxMax[2] = _rooms[index].room_data.vertices[i].vertex.z;

				continue;
			}

			f = _rooms[index].room_data.vertices[i].vertex.x;

			if (f < bboxMin[0])
				bboxMin[0] = f;
			if (f > bboxMax[0])
				bboxMax[0] = f;

			f = _rooms[index].room_data.vertices[i].vertex.y;

			if (f < bboxMin[1])
				bboxMin[1] = f;
			if (f > bboxMax[1])
				bboxMax[1] = f;

			f = _rooms[index].room_data.vertices[i].vertex.z;

			if (f < bboxMin[2])
				bboxMin[2] = f;
			if (f > bboxMax[2])
				bboxMax[2] = f;
		}

		break;
	}
}


int TombRaider::getRoomLight(unsigned int roomIndex, unsigned int index,
									  float pos[4], float color[4], float dir[3],
									  float *attenuation, float *cutoffAngle,
									  unsigned int *type, unsigned int *flags)
{
	const float dd = 10000.0; // 4095.0;
	float f;


	*flags = 0; // reset

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		return -1;
		break;
	case TR_VERSION_1:
	case TR_VERSION_2:
	case TR_VERSION_3:
		if (_rooms[roomIndex].num_lights <= 0 ||
			 (int)index > _rooms[roomIndex].num_lights)
		{
			return -1;
		}

		if (_rooms[roomIndex].lights[index].fade1 == 100730731)
		{
			f = 1.0;
		}
		else
		{
			f = _rooms[roomIndex].lights[index].fade1;
			f /= dd;
		}

		*attenuation = f;
		*flags |= tombraiderLight_useAttenuation;

		pos[0] = _rooms[roomIndex].lights[index].x;
		pos[1] = _rooms[roomIndex].lights[index].y;
		pos[2] = _rooms[roomIndex].lights[index].z;
		pos[3] = 1.0f;

		color[0] = _rooms[roomIndex].lights[index].intensity1 / 409.60;
		color[1] = color[0];
		color[2] = color[0];
		color[3] = 1.0f;

		if (Engine() == TR_VERSION_3)
		{
			color[0] = _rooms[roomIndex].room_light_colour.r;
			color[1] = _rooms[roomIndex].room_light_colour.g;
			color[2] = _rooms[roomIndex].room_light_colour.b;
		}

		*type = tombraiderLight_typeDirectional;
		break;
	case TR_VERSION_4:
		if (_rooms[roomIndex].num_lights <= 0 ||
			 (int)index > _rooms[roomIndex].num_lights)
		{
			return -1;
		}

		pos[0] = _rooms[roomIndex].tr4Lights[index].xPosition;
		pos[1] = _rooms[roomIndex].tr4Lights[index].yPosition;
		pos[2] = _rooms[roomIndex].tr4Lights[index].zPosition;
		pos[3] = 0.0f;

		color[0] = _rooms[roomIndex].tr4Lights[index].color.r;
		color[1] = _rooms[roomIndex].tr4Lights[index].color.g;
		color[2] = _rooms[roomIndex].tr4Lights[index].color.b;

		dir[0] = _rooms[roomIndex].tr4Lights[index].xDir;
		dir[1] = _rooms[roomIndex].tr4Lights[index].yDir;
		dir[2] = _rooms[roomIndex].tr4Lights[index].zDir;

		*cutoffAngle = _rooms[roomIndex].tr4Lights[index].cutoff;
		*flags |= tombraiderLight_useCutoff;

		switch (_rooms[roomIndex].tr4Lights[index].lightType)
		{
		case 1:
			*type = tombraiderLight_typeDirectional;
			break;
		case 2:
			*type = tombraiderLight_typeSpot;
			break;
		default:
			*type = tombraiderLight_typePoint;
		}
		break;
	case TR_VERSION_5:
		if (mRoomsTR5[roomIndex].numRoomLights <= 0 ||
			 (int)index > mRoomsTR5[roomIndex].numRoomLights)
		{
			return -1;
		}

		pos[0] = mRoomsTR5[roomIndex].lights[index].x;
		pos[1] = mRoomsTR5[roomIndex].lights[index].y;
		pos[2] = mRoomsTR5[roomIndex].lights[index].z;
		pos[3] = 0.0f;

		color[0] = mRoomsTR5[roomIndex].lights[index].red;
		color[1] = mRoomsTR5[roomIndex].lights[index].green;
		color[2] = mRoomsTR5[roomIndex].lights[index].blue;

		dir[0] = mRoomsTR5[roomIndex].lights[index].directionVectorX;
		dir[1] = mRoomsTR5[roomIndex].lights[index].directionVectorY;
		dir[2] = mRoomsTR5[roomIndex].lights[index].directionVectorZ;

		switch (mRoomsTR5[roomIndex].lights[index].lightType)
		{
		case 2:
			*type = tombraiderLight_typeSpot;
			break;
		case 3:
			*type = tombraiderLight_typeDirectional;
			break;
		default:
			*type = tombraiderLight_typePoint;
		}
		break;
	}

	return 0;
}


unsigned int TombRaider::getRoomLightCount(unsigned int roomIndex)
{
	if (!isRoomValid(roomIndex))
		return 0;

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	case TR_VERSION_5:
		return mRoomsTR5[roomIndex].numRoomLights;
		break;
	default:
		return _rooms[roomIndex].num_lights;
	}

	return 0;
}


int TombRaider::getRoomModel(unsigned int roomIndex, unsigned int index,
									  int *modelIndex, float pos[3], float *yaw)
{
	unsigned int i, count;


	if (!getRoomModelCount(roomIndex) || index > getRoomModelCount(roomIndex))
		return -1;

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		return -1;
		break;
	case TR_VERSION_5:
		count = NumStaticMeshes();

		for (i = 0; i < count; ++i)
		{
			if (mRoomsTR5[roomIndex].meshes[index].object_id ==
				 _static_meshes[i].object_id)
			{
				*modelIndex = _static_meshes[i].starting_mesh;

				pos[0] = mRoomsTR5[roomIndex].meshes[index].x;
				pos[1] = mRoomsTR5[roomIndex].meshes[index].y;
				pos[2] = mRoomsTR5[roomIndex].meshes[index].z;

				*yaw = ((mRoomsTR5[roomIndex].meshes[index].rotation >> 14)
						  & 0x03) * 90;
			}
		}
		break;
	default:  // TR1-TR4
		count = NumStaticMeshes();

		for (i = 0; i < count; ++i)
		{
			if (_rooms[roomIndex].static_meshes[index].object_id ==
				 _static_meshes[i].object_id)
			{
				*modelIndex = _static_meshes[i].starting_mesh;

				pos[0] = _rooms[roomIndex].static_meshes[index].x;
				pos[1] = _rooms[roomIndex].static_meshes[index].y;
				pos[2] = _rooms[roomIndex].static_meshes[index].z;

				*yaw = ((_rooms[roomIndex].static_meshes[index].rotation >> 14)
						  & 0x03) * 90;
			}
		}
	}

	return 0;
}


unsigned int TombRaider::getRoomModelCount(unsigned int roomIndex)
{
	if (!isRoomValid(roomIndex))
		return 0;

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	case TR_VERSION_5:
		return mRoomsTR5[roomIndex].numStaticMeshes;
		break;
	default:  // TR1-TR4
		return _rooms[roomIndex].num_static_meshes;
	}

	return 0;
}


int TombRaider::getRoomPortal(unsigned int roomIndex, unsigned int index,
										int *adjoiningRoom,
										float normal[3], float vertices[12])
{
	if (!getRoomPortalCount(roomIndex) || index > getRoomPortalCount(roomIndex))
		return 0;

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	case TR_VERSION_5:
		*adjoiningRoom = mRoomsTR5[roomIndex].doors[index].adjoining_room;

		normal[0] = mRoomsTR5[roomIndex].doors[index].normal.x;// / NORMAL_SCALE;
		normal[1] = mRoomsTR5[roomIndex].doors[index].normal.y;// / NORMAL_SCALE;
		normal[2] = mRoomsTR5[roomIndex].doors[index].normal.z;// / NORMAL_SCALE;

		vertices[0] = mRoomsTR5[roomIndex].doors[index].vertices[0].x;
		vertices[1] = mRoomsTR5[roomIndex].doors[index].vertices[0].y;
		vertices[2] = mRoomsTR5[roomIndex].doors[index].vertices[0].z;
		vertices[3] = mRoomsTR5[roomIndex].doors[index].vertices[1].x;
		vertices[4] = mRoomsTR5[roomIndex].doors[index].vertices[1].y;
		vertices[5] = mRoomsTR5[roomIndex].doors[index].vertices[1].z;
		vertices[6] = mRoomsTR5[roomIndex].doors[index].vertices[2].x;
		vertices[7] = mRoomsTR5[roomIndex].doors[index].vertices[2].y;
		vertices[8] = mRoomsTR5[roomIndex].doors[index].vertices[2].z;
		vertices[9] = mRoomsTR5[roomIndex].doors[index].vertices[3].x;
		vertices[10] = mRoomsTR5[roomIndex].doors[index].vertices[3].y;
		vertices[11] = mRoomsTR5[roomIndex].doors[index].vertices[3].z;
		break;
	default:  // TR1-TR4
		*adjoiningRoom = _rooms[roomIndex].portals[index].adjoining_room;

		normal[0] = _rooms[roomIndex].portals[index].normal.x;// / NORMAL_SCALE;
		normal[1] = _rooms[roomIndex].portals[index].normal.y;// / NORMAL_SCALE;
		normal[2] = _rooms[roomIndex].portals[index].normal.z;// / NORMAL_SCALE;

		vertices[0] = _rooms[roomIndex].portals[index].vertices[0].x;
		vertices[1] = _rooms[roomIndex].portals[index].vertices[0].y;
		vertices[2] = _rooms[roomIndex].portals[index].vertices[0].z;
		vertices[3] = _rooms[roomIndex].portals[index].vertices[1].x;
		vertices[4] = _rooms[roomIndex].portals[index].vertices[1].y;
		vertices[5] = _rooms[roomIndex].portals[index].vertices[1].z;
		vertices[6] = _rooms[roomIndex].portals[index].vertices[2].x;
		vertices[7] = _rooms[roomIndex].portals[index].vertices[2].y;
		vertices[8] = _rooms[roomIndex].portals[index].vertices[2].z;
		vertices[9] = _rooms[roomIndex].portals[index].vertices[3].x;
		vertices[10] = _rooms[roomIndex].portals[index].vertices[3].y;
		vertices[11] = _rooms[roomIndex].portals[index].vertices[3].z;
	}

	return 0;
}


unsigned int TombRaider::getRoomPortalCount(unsigned int roomIndex)
{
	if (!isRoomValid(roomIndex))
		return 0;

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	case TR_VERSION_5:
		return mRoomsTR5[roomIndex].numDoors;
		break;
	default:  // TR1-TR4
		return _rooms[roomIndex].num_portals;
	}

	return 0;
}


// FIXME: No TRC support
void TombRaider::getRoomRectangle(unsigned int roomIndex,
											 unsigned int rectangleIndex,
											 unsigned int *indices, float *texCoords,
											 int *texture, unsigned int *flags)
{
	int tIndex;
	unsigned int count, i, j, k;


	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	case TR_VERSION_5:
		for (i = 0, count = 0; i < mRoomsTR5[roomIndex].numLayers; ++i)
		{
			if (count + mRoomsTR5[roomIndex].layers[i].numLayerRectangles >
				 rectangleIndex)
			{
				k = rectangleIndex - count;

				*texture = *flags = 0; // FIXME

				// Setup per vertex
				for (j = 0; j < 3; ++j)
				{
					// Get vertex index { (0, a), (1, b), (2, c), (3, d) }
					indices[j] = mRoomsTR5[roomIndex].faces[i].quads[k].vertices[j];

					// FIXME
					//tIndex = mRoomsTR5[roomIndex].faces[i].vertices[j].texture & 0xff;
					//computeUV(objectTexturesTR5[tIndex].vertices + i,
					//          texCoords+i*2, texCoords+i*2+1);
				}
				break;
			}

			count += mRoomsTR5[roomIndex].layers[i].numLayerRectangles;
		}
		break;
	default:
		tIndex = _rooms[roomIndex].room_data.rectangles[rectangleIndex].texture;
		*texture = _object_textures[tIndex].tile;
		*flags = 0;

		switch (_object_textures[tIndex].transparency_flags)
		{
		case 0:
			break;
		case 2:
			*flags |= tombraiderFace_PartialAlpha;
			break;
		default:
			*flags |= tombraiderFace_Alpha;
			break;
		}

		// Setup per vertex
		for (i = 0; i < 4; ++i)
		{
			// Get vertex index {(0, a), (1, b), (2, c), (3, d)}
			indices[i] = _rooms[roomIndex].room_data.rectangles[rectangleIndex].vertices[i];

			computeUV(_object_textures[tIndex].vertices + i,
						 texCoords+i*2, texCoords+i*2+1);
		}
	}
}


unsigned int TombRaider::getRoomRectangleCount(unsigned int roomIndex)
{
	unsigned int i, count;


	if (!isRoomValid(roomIndex))
		return 0;

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	case TR_VERSION_5:
		for (i = 0, count = 0; i < mRoomsTR5[roomIndex].numLayers; ++i)
		{
			count += mRoomsTR5[roomIndex].layers[i].numLayerRectangles;
		}

		return count;
		break;
	default:
		return ((_rooms[roomIndex].room_data.num_rectangles < 0) ? 0 :
				  _rooms[roomIndex].room_data.num_rectangles);
	}

	return 0;
}

int TombRaider::getRoomSector(unsigned int roomIndex, unsigned int index,
										unsigned int *flags,
										float *ceiling, float *floor,
										int *floorDataIndex, int *boxIndex,
										int *roomBelow, int *roomAbove)
{
	unsigned int count;
	unsigned int zSectorsCount;
	unsigned int xSectorsCount;

	count = getRoomSectorCount(roomIndex, &zSectorsCount, &xSectorsCount);

	if (!count || index > count)
		return -1;

	*flags = 0;

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	case TR_VERSION_5:
		*floorDataIndex = mRoomsTR5[roomIndex].sectors[index].fd_index;
		*boxIndex = mRoomsTR5[roomIndex].sectors[index].box_index;
		*roomBelow = mRoomsTR5[roomIndex].sectors[index].room_below;
		*roomAbove = mRoomsTR5[roomIndex].sectors[index].room_above;

		if ((unsigned char)mRoomsTR5[roomIndex].sectors[index].floor == 0x81 ||
			 (unsigned char)mRoomsTR5[roomIndex].sectors[index].ceiling == 0x81)
		{
			*flags |= tombraiderSector_wall;
		}

		*floor = mRoomsTR5[roomIndex].sectors[index].floor * 256.0f;
		*ceiling = mRoomsTR5[roomIndex].sectors[index].ceiling * 256.0f;
		break;
	default:
		*floorDataIndex = _rooms[roomIndex].sector_list[index].fd_index;
		*boxIndex = _rooms[roomIndex].sector_list[index].box_index;
		*roomBelow = _rooms[roomIndex].sector_list[index].room_below;
		*roomAbove = _rooms[roomIndex].sector_list[index].room_above;

		if ((unsigned char)_rooms[roomIndex].sector_list[index].floor == 0x81 ||
			 (unsigned char)_rooms[roomIndex].sector_list[index].ceiling == 0x81)
		{
			*flags |= tombraiderSector_wall;
		}

		*floor = _rooms[roomIndex].sector_list[index].floor * 256.0f;
		*ceiling = _rooms[roomIndex].sector_list[index].ceiling * 256.0f;
	}

	if (*boxIndex == 65536)
	{
		*boxIndex = -1;
	}

	if (*roomBelow == 255)
	{
		*roomBelow = -1;
	}

	if (*roomAbove == 255)
	{
		*roomAbove = -1;
	}

	return 0;
}


unsigned int TombRaider::getRoomSectorCount(unsigned int roomIndex,
														  unsigned int *zSectorsCount,
														  unsigned int *xSectorsCount)
{
	unsigned int count;


	if (!isRoomValid(roomIndex))
		return 0;

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	case TR_VERSION_5:
		// width of sector list
		*zSectorsCount = mRoomsTR5[roomIndex].numZSectors;

		// height of sector list
		*xSectorsCount = mRoomsTR5[roomIndex].numXSectors;

		return (mRoomsTR5[roomIndex].numZSectors *
				  mRoomsTR5[roomIndex].numXSectors);
	default:
		// width of sector list
		*zSectorsCount = _rooms[roomIndex].num_zsectors;

		// height of sector list
		*xSectorsCount = _rooms[roomIndex].num_xsectors;

		count = _rooms[roomIndex].num_zsectors * _rooms[roomIndex].num_xsectors;

		return count;
	}

	return 0;
}


void TombRaider::getRoomSprite(unsigned int roomIndex, unsigned int index,
										 float scale, int *texture,
										 float *pos, float *vertices, float *texcoords)
{
	tr2_sprite_texture_t *sprite;
	tr2_vertex_t *vertex;
	int t_index, width, height, x, y;
	float width2, height2;
	unsigned int spriteCount;


	spriteCount = getRoomSpriteCount(roomIndex);

	if (spriteCount == 0 || index > spriteCount)
		return;

	if (scale == 0.0)
		scale = 10.0;

	t_index = _rooms[roomIndex].room_data.sprites[index].texture;

	vertex =
	&_rooms[roomIndex].room_data.vertices[_rooms[roomIndex].room_data.sprites[index].vertex].vertex;

	sprite = &_sprite_textures[t_index];

	// Info, offset in room added to position
	pos[0] = _rooms[roomIndex].info.x + vertex->x;
	pos[1] = vertex->y;
	pos[2] = _rooms[roomIndex].info.z + vertex->z;
	*texture = sprite->tile;

	width = sprite->width >> 8;
	height = sprite->height >> 8;
	x = sprite->x;
	y = sprite->y;
	width2 = width * scale;   // scale it up a bit (sprites are rather tiny)
	height2 = height * scale;

	// Quad
	vertices[0] = -width2 / 2.0;
	vertices[1] = 0;
	vertices[2] = 0;
	vertices[3] = -width2 / 2.0;
	vertices[4] = -height2;
	vertices[5] = 0;
	vertices[6] = width2 / 2.0;
	vertices[7] = -height2;
	vertices[8] = 0;
	vertices[9] = width2 / 2.0;
	vertices[10] = 0;
	vertices[11] = 0;

	texcoords[0] = ((float)y + height) / mTexelScale;
	texcoords[1] = (float)(x) / mTexelScale;
	texcoords[2] = (float)(y) / mTexelScale;
	texcoords[3] = (float)(x) / mTexelScale;
	texcoords[4] = (float)(y) / mTexelScale;
	texcoords[5] = ((float)x + width) / mTexelScale;
	texcoords[6] = ((float)y + height) / mTexelScale;
	texcoords[7] = ((float)x + width) / mTexelScale;
}


#ifdef OPTIONAL_ARRAY_INTERFACE
void TombRaider::getRoomSpriteArray(unsigned int index, float scale,
												unsigned int *spriteCount, int *textures,
												float *pos, float *vertices, float *texcoords)
{
	int i, t_index;
	tr2_vertex_t *vertex;
	int width, height, x, y;
	float width2, height2;


	*spriteCount = ((room[index].room_data.num_sprites < 0) ?
						 0 : room[index].room_data.num_sprites);

	if (*spriteCount == 0)
		return;

	textures = new int[*spriteCount];
	pos = new float[*spriteCount * 3];
	vertices = new float[*spriteCount * 12];
	texcoords = new float[*spriteCount * 8];

	if (scale == 0.0)
		scale = 10.0;

	for (i = 0; i < *spriteCount; ++i)
	{
		t_index = room[index].room_data.sprites[i].texture;

		vertex =
		  &room[index].room_data.vertices[room[index].room_data.sprites[i].vertex].vertex;

		sprite = &sprite_textures[t_index];

		// Info, offset in room added to position
		pos[0+i*3] = room[index].info.x + vertex->x;
		pos[1+i*3] = vertex->y;
		pos[2+i*3] = room[index].info.z + vertex->z;
		textures[i] = sprite->tile;

		width = sprite->width >> 8;
		height = sprite->height >> 8;
		x = sprite->x;
		y = sprite->y;
		width2 = width * scale;   // scale it up a bit (sprites are rather tiny)
		height2 = height * scale;

		// Quad
		vertices[0+i*12] = -width2 / 2.0;
		vertices[1+i*12] = 0;
		vertices[2+i*12] = 0;
		vertices[3+i*12] = -width2 / 2.0;
		vertices[4+i*12] = -height2;
		vertices[5+i*12] = 0;
		vertices[6+i*12] = width2 / 2.0;
		vertices[7+i*12] = -height2;
		vertices[8+i*12] = 0;
		vertices[9+i*12] = width2 / 2.0;
		vertices[10+i*12] = 0;
		vertices[11+i*12] = 0;

		texcoords[0+i*8] = ((float)y + height) / mTexelScale;
		texcoords[1+i*8] = (float)(x) / mTexelScale;
		texcoords[2+i*8] = (float)(y) / mTexelScale;
		texcoords[3+i*8] = (float)(x) / mTexelScale;
		texcoords[4+i*8] = (float)(y) / mTexelScale;
		texcoords[5+i*8] = ((float)x + width) / mTexelScale;
		texcoords[6+i*8] = ((float)y + height) / mTexelScale;
		texcoords[7+i*8] = ((float)x + width) / mTexelScale;
	}
}
#endif


unsigned int TombRaider::getRoomSpriteCount(unsigned int roomIndex)
{
	if (!isRoomValid(roomIndex))
		return 0;

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	case TR_VERSION_5:
		return 0; // No room sprites in TRC
		break;
	default:
		return ((_rooms[roomIndex].room_data.num_sprites < 0) ?
				  0 : _rooms[roomIndex].room_data.num_sprites);
	}

	return 0;
}


void TombRaider::getRoomTriangle(unsigned int roomIndex,
											unsigned int triangleIndex,
											unsigned int *indices, float *texCoords,
											int *texture, unsigned int *flags)
{
	int tIndex;
	unsigned int count, i, j, k;


	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	case TR_VERSION_5:
		for (i = 0, count = 0; i < mRoomsTR5[roomIndex].numLayers; ++i)
		{
			if (count + mRoomsTR5[roomIndex].layers[i].numLayerTriangles >
				 triangleIndex)
			{
				k = triangleIndex - count;

				*texture = *flags = 0; // FIXME

				// Setup per vertex
				for (j = 0; j < 3; ++j)
				{
					// Get vertex index {(0, a), (1, b), (2, c) }
					indices[j] = mRoomsTR5[roomIndex].faces[i].tris[k].vertices[j];

					// FIXME
					//tIndex = mRoomsTR5[roomIndex].faces[i].vertices[j].texture & 0xff;
					//computeUV(objectTexturesTR5[tIndex].vertices + i,
					//          texCoords+i*2, texCoords+i*2+1);
				}
				break;
			}

			count += mRoomsTR5[roomIndex].layers[i].numLayerTriangles;
		}
		break;
	default:
		tIndex = _rooms[roomIndex].room_data.triangles[triangleIndex].texture;
		*texture = _object_textures[tIndex].tile;
		*flags = 0;

		switch (_object_textures[tIndex].transparency_flags)
		{
		case 0:
			break;
		case 2:
			*flags |= tombraiderFace_PartialAlpha;
			break;
		default:
			*flags |= tombraiderFace_Alpha;
			break;
		}

		// Setup per vertex
		for (i = 0; i < 3; ++i)
		{
			// Get vertex index {(0, a), (1, b), (2, c) }
			indices[i] = _rooms[roomIndex].room_data.triangles[triangleIndex].vertices[i];

			computeUV(_object_textures[tIndex].vertices + i,
						 texCoords+i*2, texCoords+i*2+1);
		}
	}
}


void TombRaider::getRoomTriangles(unsigned int index, int textureOffset,
											 unsigned int *triangleCount,
											 unsigned int **indices,
											 float **texCoords, int **textures,
											 unsigned int **flags)
{
	unsigned int count, i, j;
	int texture;


	if (!isRoomValid(index))
		return;

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	case TR_VERSION_5:
		for (i = 0, count = 0; i < mRoomsTR5[index].numLayers; ++i)
		{
			count += mRoomsTR5[index].layers[i].numLayerTriangles;
		}

		// FIXME!!!
		break;
	default:
		// Generate textured triangles
		count = ((_rooms[index].room_data.num_triangles < 0) ? 0 :
					_rooms[index].room_data.num_triangles);

		*triangleCount = count;

		*indices = new unsigned int[count * 3];
		*texCoords = new float[count * 6];
		*textures = new int[count];
		*flags = new unsigned int[count];

		for (i = 0; i < count; ++i)
		{
			texture = _rooms[index].room_data.triangles[i].texture;

			// Adjust texture id using m_texOffset to map into
			// correct textures
			(*textures)[i] = _object_textures[texture].tile + textureOffset;
			(*flags)[i] = 0;

			switch (_object_textures[texture].transparency_flags)
			{
			case 0:
				break;
			case 2:
				(*flags)[i] |= tombraiderFace_PartialAlpha;
				break;
			default:
				(*flags)[i] |= tombraiderFace_Alpha;
				break;
			}

			// Setup per vertex
			for (j = 0; j < 3; ++j)
			{
				// Get vertex index {(0, a), (1, b), (2, c)}
				(*indices)[i*3+j] = _rooms[index].room_data.triangles[i].vertices[j];

				computeUV(_object_textures[texture].vertices+j,
							 (*texCoords)+i*3+j*2, (*texCoords)+i*3+j*2+1);
			}
		}
	}
}


unsigned int TombRaider::getRoomTriangleCount(unsigned int roomIndex)
{
	unsigned int i, count;


	if (!isRoomValid(roomIndex))
		return 0;

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	case TR_VERSION_5:
		for (i = 0, count = 0; i < mRoomsTR5[roomIndex].numLayers; ++i)
		{
			count += mRoomsTR5[roomIndex].layers[i].numLayerTriangles;
		}

		return count;
		break;
	default:
		return ((_rooms[roomIndex].room_data.num_triangles < 0) ? 0 :
				  _rooms[roomIndex].room_data.num_triangles);
	}

	return 0;
}


// FIXME: No TR5 support
void TombRaider::getRoomVertex(unsigned int roomIndex,unsigned int vertexIndex,
										 float *xyz, float *rgba)
{
	float color_value;
	unsigned int index = roomIndex, i = vertexIndex;
	tr2_vertex_t *vertex = 0x0;


	switch (getEngine())
	{
	case TR_VERSION_5:
		// FIXME!!!
		print("getRoomVertex", "Error: No TRC implementation is the answer");
		break;
	default:
		vertex = &_rooms[index].room_data.vertices[i].vertex;

		xyz[0] = vertex->x;
		xyz[1] = vertex->y;
		xyz[2] = vertex->z;

		switch (getEngine())
		{
		case TR_VERSION_1:
			color_value = _rooms[index].room_data.vertices[i].attributes;
			color_value = (1.1 - (color_value / 8192.0));

			rgba[0] = color_value;
			rgba[1] = color_value;
			rgba[2] = color_value;
			break;
		case TR_VERSION_3:
		case TR_VERSION_4:
			color_value = _rooms[index].room_data.vertices[i].lighting1;
			color_value /= 16384.0;

			rgba[0] = color_value + ((float)_rooms[index].room_light_colour.r /
											 mRoomVertexLightingFactor);

			rgba[1] = color_value + ((float)_rooms[index].room_light_colour.g /
											 mRoomVertexLightingFactor);

			rgba[2] = color_value + ((float)_rooms[index].room_light_colour.b /
											 mRoomVertexLightingFactor);
			break;
		default:
			color_value = _rooms[index].room_data.vertices[i].lighting1;
			color_value = (1.1 - (color_value / 8192.0));

			rgba[0] = color_value;
			rgba[1] = color_value;
			rgba[2] = color_value;
		}

		// Underwater rooms have an ambient color with a shade of blue
		if (_rooms[index].flags & 0x0001)
		{
			rgba[0] *= 0.6;
			rgba[2] *= 1.2;
		}

		// Alpha color
		rgba[3] = 1.0;
	}
}


void TombRaider::getRoomVertexArrays(unsigned int roomIndex,
												 unsigned int *vertexCount, float **vertices,
												 unsigned int *normalCount, float **normals,
												 unsigned int *colorCount, float **colors)
{
	float color_value;
	float rgba[4];
	unsigned int i, j, k, count;
	unsigned char c;
	tr2_vertex_t *vertex = 0x0;


	count = getRoomVertexCount(roomIndex);

	*vertexCount = 0;
	*vertices = 0x0;
	*normalCount = 0;
	*normals = 0x0;
	*colorCount = 0;
	*colors = 0x0;

	if (count == 0)
		return;

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	case TR_VERSION_5:
		*vertexCount = count;
		*vertices = new float[count*3];
		*normalCount = count;
		*normals = new float[count*3];
		*colorCount = count;
		*colors = new float[count*4];

		for (i = 0, k = 0; i < mRoomsTR5[roomIndex].numLayers; ++i)
		{
			for (j = 0; j < mRoomsTR5[roomIndex].layers[i].numLayerVertices; ++j)
			{
				(*vertices)[k*3] = mRoomsTR5[roomIndex].faces[i].verts[j].x;
				(*vertices)[k*3+1] = mRoomsTR5[roomIndex].faces[i].verts[j].y;
				(*vertices)[k*3+2] = mRoomsTR5[roomIndex].faces[i].verts[j].z;

				(*normals)[k*3] = mRoomsTR5[roomIndex].faces[i].verts[j].nx;
				(*normals)[k*3+1] = mRoomsTR5[roomIndex].faces[i].verts[j].ny;
				(*normals)[k*3+2] = mRoomsTR5[roomIndex].faces[i].verts[j].nz;

				// FIXME: Ah, yeah this may be wrong
				c = ((unsigned char *)&mRoomsTR5[roomIndex].faces[i].verts[j].vColor)[1];
				(*colors)[k*4] = (float)c / 255.0f;
				c = ((unsigned char *)&mRoomsTR5[roomIndex].faces[i].verts[j].vColor)[2];
				(*colors)[k*4+1] = (float)c / 255.0f;
				c = ((unsigned char *)&mRoomsTR5[roomIndex].faces[i].verts[j].vColor)[3];
				(*colors)[k*4+2] = (float)c / 255.0f;
				c = ((unsigned char *)&mRoomsTR5[roomIndex].faces[i].verts[j].vColor)[0];
				(*colors)[k*4+3] = (float)c / 255.0f;
			}
		}
		break;
	default:
		*vertexCount = count;
		*vertices = new float[count*3];
		*normalCount = 0;  // FIXME: Do some TR1-TR4 levels support normals here?
		*normals = 0x0;
		*colorCount = count;
		*colors = new float[count*4];

		// Setup vertex coloring and vertices
		for (i = 0; i < count; ++i)
		{
			vertex = &_rooms[roomIndex].room_data.vertices[i].vertex;

			(*vertices)[i*3] = vertex->x;
			(*vertices)[i*3+1] = vertex->y;
			(*vertices)[i*3+2] = vertex->z;

			switch (getEngine())
			{
			case TR_VERSION_1:
				color_value = _rooms[roomIndex].room_data.vertices[i].attributes;
				color_value = (1.1 - (color_value / 8192.0));
				break;
			case TR_VERSION_4:
			case TR_VERSION_3:
				color_value = _rooms[roomIndex].room_data.vertices[i].lighting1;
				color_value /= 16384.0;
				break;
			default:
				color_value = _rooms[roomIndex].room_data.vertices[i].lighting1;
				color_value = (1.1 - (color_value / 8192.0));
			}

			switch (getEngine())
			{
			case TR_VERSION_4:
			case TR_VERSION_3:
				rgba[0] = (color_value +
							  ((float)_rooms[roomIndex].room_light_colour.r /
								mRoomVertexLightingFactor));

				rgba[1] = (color_value +
							  ((float)_rooms[roomIndex].room_light_colour.g /
								mRoomVertexLightingFactor));

				rgba[2] = (color_value +
							  ((float)_rooms[roomIndex].room_light_colour.b /
								mRoomVertexLightingFactor));
				break;
			default:
				rgba[0] = color_value;
				rgba[1] = color_value;
				rgba[2] = color_value;
			}

			// Underwater rooms have an ambient color with a shade of blue
			if (_rooms[roomIndex].flags & 0x0001)
			{
				rgba[0] *= 0.6;
				rgba[2] *= 1.2;
			}

			// Alpha color
			rgba[3] = 1.0;

			(*colors)[i*4] = rgba[0];
			(*colors)[i*4+1] = rgba[1];
			(*colors)[i*4+2] = rgba[2];
			(*colors)[i*4+3] = rgba[3];
		}
	}
}


unsigned int TombRaider::getRoomVertexCount(unsigned int roomIndex)
{
	unsigned int i, count;


	if (!isRoomValid(roomIndex))
		return 0;

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	case TR_VERSION_5:
		for (i = 0, count = 0; i < mRoomsTR5[roomIndex].numLayers; ++i)
		{
			count += mRoomsTR5[roomIndex].layers[i].numLayerVertices;
		}

		return count;
		break;
	default:
		return ((_rooms[roomIndex].room_data.num_vertices < 0) ? 0 :
				  _rooms[roomIndex].room_data.num_vertices);
	}

	return 0;
}


int TombRaider::getSkyModelId()
{
	int skyMesh = -1;
	//bool rot = false;
	tr2_moveable_t *moveable;
	unsigned int i, id;


	moveable = Moveable();

	switch (Engine())
	{
	case TR_VERSION_2:
		//rot = true;
		id = 254;
		break;
	case TR_VERSION_3:
		id = 355;
		break;
	case TR_VERSION_4:
		id = 459;
		break;
	default:
		return -1;
	}

	if (id > 0)
	{
		for (i = 0; (int)i < NumMoveables(); ++i)
		{
			if (moveable[i].object_id == id)
			{
				//sky_mesh = moveable[i].starting_mesh;
				skyMesh = i;
				break;
			}
		}
	}

	return skyMesh;
}


void TombRaider::getSprites()
{
#ifdef FIXME
	int i, j, k, l, x, y, s_index, width, height;
	float scale, width2, height2;
	tr2_sprite_texture_t *sprite;
	tr2_sprite_texture_t *sprite_textures;
	tr2_sprite_sequence_t *sprite_sequence;
	sprite_seq_t *r_mesh;
	tr2_item_t *item;


	item = Item();
	sprite_textures = Sprite();
	sprite_sequence = SpriteSequence();
	scale = 4.0;

	printf("Processing sprites: ");

	for (i = 0; i < NumItems() - 1; ++i)
	{
		print(false, "Processing sprites in Items: %i/%i",
				i, NumItems());

		// It's a mesh, skip it
		if (Engine() == TR_VERSION_1 && item[i].intensity1 == -1)
			continue;

		k = item[i].object_id;

		// Search the SpriteSequence list
		//  (if we didn't already decide that it's a mesh)
		for (j = 0; j < (int)NumSpriteSequences(); ++j)
		{
			if (sprite_sequence[j].object_id == k)
			{
				k = item[i].object_id;

				s_index = sprite_sequence[j].offset;

				spriteSequence = new sprite_seq_t;
				spriteSequence->spriteCount = -sprite_sequence[j].negative_length;
				spriteSequence->sprites = new sprite_t[r_mesh->num_sprites];
				sprite = spriteSequence->sprites;

				callbackAddSpriteSequence(spriteSequence);

				for (l = 0; l < r_mesh->num_sprites; ++l)
				{
					sprite = &sprite_textures[s_index];

					width = sprite->width >> 8;
					height = sprite->height >> 8;
					x = sprite->x;
					y = sprite->y;
					width2 = width * scale;
					height2 = height * scale;

					// For external use
					sprite[l].pos[0] = item[i].x;
					sprite[l].pos[1] = item[i].y;
					sprite[l].pos[2] = item[i].z;
					sprite[l].textureIndex = sprite->tile;

					sprite[l].radius = width2 / 2.0;

					sprite[l].vertex[0].pos[0] = -width2 / 2.0;
					sprite[l].vertex[1].pos[0] = -width2 / 2.0;
					sprite[l].vertex[2].pos[0] = width2 / 2.0;
					sprite[l].vertex[3].pos[0] = width2 / 2.0;

					sprite[l].vertex[0].pos[1] = 0;
					sprite[l].vertex[1].pos[1] = -height2;
					sprite[l].vertex[2].pos[1] = -height2;
					sprite[l].vertex[3].pos[1] = 0;

					sprite[l].vertex[0].pos[2] = 0;
					sprite[l].vertex[1].pos[2] = 0;
					sprite[l].vertex[2].pos[2] = 0;
					sprite[l].vertex[3].pos[2] = 0;

					sprite[l].texel[3].st[0] = (double)(x+width) / mTexelScale;
					sprite[l].texel[3].st[1] = (double)(y+height) / mTexelScale;

					sprite[l].texel[2].st[0] = (double)(x+width) / mTexelScale;
					sprite[l].texel[2].st[1] = (double)(y) / mTexelScale;

					sprite[l].texel[1].st[0] = (double)(x) / mTexelScale;
					sprite[l].texel[1].st[1] = (double)(y) / mTexelScale;

					sprite[l].texel[0].st[0] = (double)(x) / mTexelScale;
					sprite[l].texel[0].st[1] = (double)(y+height) / mTexelScale;

					printf(".");
					fflush(stdout);
				}
			}
		}
	}

	printf("\n");
#endif
}


void TombRaider::getSoundSample(unsigned int index,
										  unsigned int *bytes, unsigned char **data)
{
	unsigned char *riff;
	unsigned int offset, altIndex;

	*bytes = 0;
	*data = 0x0;

	switch (Engine())
	{
	case TR_VERSION_1:
		// FIXME: This implies higher tmp memory cost ( copy safety )
		getRiffData(bytes, &riff);

		if (riff && (int)index < mNumSampleIndices)
		{
			offset = mSampleIndices[index];

			if ((int)index < mNumSampleIndices - 1)
			{
				*bytes = mSampleIndices[index+1] - mSampleIndices[index];
			}
			else
			{
				*bytes = *bytes - mSampleIndices[index];
			}

			*data = new unsigned char[*bytes];

			memcpy(*data, riff+offset, *bytes);
		}

		if (riff)
		{
			delete [] riff;
		}
		break;
	case TR_VERSION_2:
	case TR_VERSION_3:
		if (mRiffAlternateLoaded &&
			 mRiffData && mRiffAlternateOffsets && (int)index < mNumSampleIndices)
		{
			altIndex = mSampleIndices[index];

			offset = mRiffAlternateOffsets[altIndex];

			if ((int)offset > mRiffDataSz)
			{
				print("getSoundSample", "WARNING: offset too large, may be mismatched SFX and game pak, handling...\n");
				return;
			}

			if (altIndex < mNumTR4Samples - 1)
			{
				*bytes = mRiffAlternateOffsets[altIndex+1] - offset;
			}
			else
			{
				*bytes = mRiffDataSz - offset;
			}

			*data = new unsigned char[*bytes];

			memcpy(*data, mRiffData+offset, *bytes);
		}
		break;
	case TR_VERSION_4:
	case TR_VERSION_5:
		getRiffDataTR4(index, bytes, data);
		break;
	default:
		;
	}
}


unsigned int TombRaider::getSoundSamplesCount()
{
	unsigned int count = 0;


	switch (Engine())
	{
	case TR_VERSION_1:
		count = mNumSampleIndices;
		break;
	case TR_VERSION_2:
	case TR_VERSION_3:
		if (mRiffAlternateLoaded)
		{
			count = mNumSampleIndices;
		}
		break;
	case TR_VERSION_4:
	case TR_VERSION_5:
		count = mNumTR4Samples;
		break;
	default:
		count = 0;
	}

	return count;
}


bool TombRaider::isMeshValid(int index)
{
	return !(index < 0 ||
				index > mMeshCount ||
				((mMeshes[index].num_vertices < 0 ||
			  mMeshes[index].vertices == NULL)));
}


bool TombRaider::isRoomValid(int index)
{
	// Yes, you MUST support signed indices due to legacy engines
	if (index < 0)
		return false;

	switch (getEngine())
	{
	case TR_VERSION_UNKNOWN:
		break;
	case TR_VERSION_5:
		if (index < _num_rooms &&
			 mRoomsTR5[index].roomX != 0xcdcdcd &&
			 mRoomsTR5[index].roomZ != 0xcdcdcd)
		{
			return true;
		}
		break;
	case TR_VERSION_1:
	case TR_VERSION_2:
	case TR_VERSION_3:
	case TR_VERSION_4:
		if (index < _num_rooms &&
			 _rooms[index].room_data.num_vertices > 0)
		{
			return true;
		}
	}

	return false;
}


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

int TombRaider::loadSFX(char *filename)
{
	FILE *f = fopen(filename, "rb");
	long bytes = 0;
	unsigned char *data;

	if (!f)
	{
		perror("Couldn't load SFX file");
		return -1;
	}

	fseek(f, 0L, SEEK_END);
	bytes = ftell(f);
	fseek(f, 0L, SEEK_SET);

	if (bytes > 8)
	{
		data = new unsigned char[bytes];
		fread(data, bytes, 1, f);

		mNumTR4Samples = getRiffOffsets(data, bytes,
												  &mRiffAlternateOffsets,
												  mNumSampleIndices);

		// This SFX must not come close to matching this game pak
		if ((int)mNumTR4Samples < mNumSampleIndices)
		{
			delete [] data;
			fclose(f);

			print("loadSFX", "WARNING: SFX RIFF has less than pak's RIFF count\n");
			return -2;
		}

		// If you ran out of room, then reallocate and parse agian  =(
		if (mNumSampleIndices < (int)mNumTR4Samples)
		{
			delete [] mRiffAlternateOffsets;

			mNumTR4Samples = getRiffOffsets(data, bytes,
													  &mRiffAlternateOffsets,
													  mNumTR4Samples);
		}

		mRiffDataSz = bytes;
		mRiffData = data;
		mRiffAlternateLoaded = true;
	}

	fclose(f);

	return 0;
}


void TombRaider::reset()
{
	unsigned int i;


	// Mongoose 2003.05.13, Don't let some jackass reset over and over
	if (mReset)
	{
		return;
	}

  // FIXME: Palettes aren't the same size anymore
  //memset(_palette8, 0, 256);
  //memset(_palette16, 0, 256);

  if (_anim_dispatches)
    delete [] _anim_dispatches;

  if (_anim_commands)
    delete [] _anim_commands;

  if (_mesh_trees)
    delete [] _mesh_trees;

  if (_frames)
    delete [] _frames;

  if (_moveables)
    delete [] _moveables;

  if (_static_meshes)
    delete [] _static_meshes;

  if (_object_textures)
    delete [] _object_textures;

  if (_sprite_textures)
    delete [] _sprite_textures;

  if (_sprite_sequences)
    delete [] _sprite_sequences;

  if (_cameras)
    delete [] _cameras;

  if (_sound_sources)
    delete [] _sound_sources;

  if (_boxes)
    delete [] _boxes;

  if (_overlaps)
    delete [] _overlaps;

  if (_zones)
    delete [] _zones;

  if (_animated_textures)
    delete [] _animated_textures;

  if (_items)
    delete [] _items;

  if (_light_map)
    delete [] _light_map;

  if (_cinematic_frames)
    delete [] _cinematic_frames;

  if (_demo_data)
    delete [] _demo_data;

  if (mRiffAlternateOffsets)
	  delete [] mRiffAlternateOffsets;

  if (mSoundMap)
    delete [] mSoundMap;

  if (mSoundDetails)
    delete [] mSoundDetails;

  if (mSampleIndices)
    delete [] mSampleIndices;

  if (mRiffData)
	  delete [] mRiffData;

  if (mTR4Samples)
  {
	  for (i = 0; i < mNumTR4Samples; ++i)
	  {
		  if (mTR4SamplesSz[i])
			  delete [] mTR4Samples[i];
	  }

	  delete [] mTR4Samples;
  }

  if (mTR4SamplesSz)
	  delete [] mTR4SamplesSz;

  if (_rooms)
  {
	  for (i = 0; i < _num_rooms; ++i)
	  {
		  if (_rooms[i].room_data.num_vertices > 0)
			  delete [] _rooms[i].room_data.vertices;

		  if (_rooms[i].room_data.num_rectangles > 0)
			  delete [] _rooms[i].room_data.rectangles;

		  if (_rooms[i].room_data.num_triangles > 0)
			  delete [] _rooms[i].room_data.triangles;

		  if (_rooms[i].room_data.num_sprites > 0)
			  delete [] _rooms[i].room_data.sprites;

		  if (_rooms[i].num_portals > 0)
			  delete []_rooms[i].portals;

		  if (_rooms[i].sector_list)
			  delete [] _rooms[i].sector_list;

		  if (_rooms[i].lights)
			  delete [] _rooms[i].lights;

		  if (_rooms[i].tr4Lights)
			  delete [] _rooms[i].tr4Lights;

		  if (_rooms[i].static_meshes)
			  delete [] _rooms[i].static_meshes;
	  }

	  delete [] _rooms;
  }

  if (_floor_data)
  {
    delete [] _floor_data;
  }

  if (mMeshes)
  {
	  for (i = 0; (int)i < mMeshCount; ++i)
	  {
		  if (mMeshes[i].vertices)
			 delete [] mMeshes[i].vertices;

		  if (mMeshes[i].mesh_lights)
			  delete [] mMeshes[i].mesh_lights;

		  if (mMeshes[i].normals)
			  delete [] mMeshes[i].normals;

		  if (mMeshes[i].textured_rectangles)
			  delete [] mMeshes[i].textured_rectangles;

		  if (mMeshes[i].textured_triangles)
			  delete [] mMeshes[i].textured_triangles;

		  if (mMeshes[i].coloured_rectangles)
			  delete [] mMeshes[i].coloured_rectangles;

		  if (mMeshes[i].coloured_triangles)
			  delete [] mMeshes[i].coloured_triangles;
	  }

    delete [] mMeshes;
  }

  if (_animations)
    delete [] _animations;

  if (_state_changes)
	  delete _state_changes;
	  //delete [] _state_changes;

  numMoveablesTR5 = 0;
  if (moveablesTR5)
	  delete [] moveablesTR5;

  numAnimationsTR5 = 0;
  if (animationsTR5)
	  delete [] animationsTR5;

  numObjectTexturesTR5 = 0;
  if (objectTexturesTR5)
	  delete [] objectTexturesTR5;

  numCinematicFramesTR5 = 0;
  if (cinematicFramesTR5)
	  delete [] cinematicFramesTR5;

  numFlyByCamerasTR5 = 0;
  if (flyByCamerasTR5)
	  delete [] flyByCamerasTR5;

  // Texture use
  if (_tex_special)
    delete [] _tex_special;

  if (_textile8)
    delete [] _textile8;

  if (_textile16)
    delete [] _textile16;

  if (_textile32)
    delete [] _textile32;

  // FIXME: Damaged memory causes delete to not set pointer to NULL?
  _tex_special = NULL;
  _textile8 = NULL;
  _textile16 = NULL;
  _textile32 = NULL;

  // Compressed level use
  if (mCompressedLevelData)
	  delete [] mCompressedLevelData;

  mCompressedLevelDataOffset  = 0;
  mCompressedLevelSize = 0;
  mFreadMode = TR_FREAD_NORMAL;

  // Clear out vars
  mNumTR4Samples = 0;
  mPakVersion = 0;
  mEngineVersion = TR_VERSION_UNKNOWN;
  mNumSampleIndices = 0;
  mNumSoundDetails = 0;
  mRiffAlternateLoaded = false;
  _num_floor_data = 0;
  _num_textiles = 0;
  _num_tex_special = 0;
  _num_room_textures = 0;
  _num_misc_textures = 0;
  _num_bump_map_textures = 0;
  _unknown_t = 0;
  _num_rooms = 0;
  _num_anim_dispatches = 0;
  mMeshCount = 0;
  _num_state_changes = 0;
  _num_animations = 0;
  _num_anim_commands = 0;
  _num_mesh_trees = 0;
  _num_frames = 0;
  _num_moveables = 0;
  _num_demo_data = 0;
  _num_cinematic_frames = 0;
  _num_items = 0;
  _num_animated_textures = 0;
  _num_cameras = 0;
  _num_sound_sources = 0;
  _num_boxes = 0;
  _num_static_meshes = 0;
  _num_object_textures = 0;
  _num_sprite_textures = 0;
  _num_sprite_sequences = 0;
  _num_overlaps = 0;

  mReset = true;
}


void TombRaider::setDebug(bool toggle)
{
	mDebug = toggle;
}


void TombRaider::setRoomVertexLightingFactor(float f)
{
	mRoomVertexLightingFactor = f;
}

void TombRaider::setTexelScalingFactor(float f)
{
	mTexelScale = f;
}

////////////////////////////////////////////////////////////
// Private Accessors
////////////////////////////////////////////////////////////

void TombRaider::extractMeshes(unsigned char *mesh_data,
										 unsigned int num_mesh_pointers,
										 unsigned int *mesh_pointers)
{
	unsigned int size, i;
	unsigned char *mesh_pointer;
	int negative_size;


	/* Alloc space for mesh */
	mMeshCount = num_mesh_pointers;

	printDebug("ExtractMeshes", "mMeshCount = %u", mMeshCount);

	mMeshes = new tr2_mesh_t[mMeshCount];

	printDebug("ExtractMeshes", "num_mesh_pointers = %u", num_mesh_pointers);

	for (i = 0; i < num_mesh_pointers; ++i)
	{
		/* Get mesh start */
		mesh_pointer = &mesh_data[mesh_pointers[i]];

		/* Get Centre + Unknowns */
		memcpy(&mMeshes[i].centre.x, mesh_pointer, 10);
		//FIXME: endian

		// depending on the interpretation of the unknowns that follow the Centre
		// element, more endian conversion may be necessary
		mesh_pointer += 10;

		/* Get number of vertices */
		memcpy(&mMeshes[i].num_vertices, mesh_pointer, 2);
		//FIXME: endian

		printDebug("ExtractMeshes", "mMeshes[%i].num_vertices = %u",
					  i, mMeshes[i].num_vertices);

		mesh_pointer += sizeof(unsigned short);
		mMeshes[i].num_vertices = (short)abs(mMeshes[i].num_vertices);

		/* Get vertex list */
		size = sizeof(tr2_vertex_t) * mMeshes[i].num_vertices;
		mMeshes[i].vertices = 0x0;

		if (mMeshes[i].num_vertices > 0)
			mMeshes[i].vertices = new tr2_vertex_t[mMeshes[i].num_vertices];

		memcpy(mMeshes[i].vertices, mesh_pointer, size);
		//FIXME: endian

		mesh_pointer += size;

		/* Get number of normals */
		memcpy(&mMeshes[i].num_normals, mesh_pointer, sizeof(unsigned short));
		//FIXME: endian

		mesh_pointer += sizeof(unsigned short);
		negative_size = (mMeshes[i].num_normals < 0);
		mMeshes[i].num_normals = (short)abs(mMeshes[i].num_normals);

		printDebug("ExtractMeshes", "negative_size = %u", negative_size);

		mMeshes[i].mesh_lights = 0x0;
		mMeshes[i].normals = 0x0;

		/* Get normal list */
		if (negative_size)
		{
			negative_size = 0;

			size = mMeshes[i].num_normals * sizeof(unsigned short);

			mMeshes[i].mesh_lights = 0x0;

			if (mMeshes[i].num_normals > 0)
				mMeshes[i].mesh_lights = new short[mMeshes[i].num_normals];

			memcpy(mMeshes[i].mesh_lights, mesh_pointer, size);
		}
		else
		{
			size = sizeof(tr2_vertex_t) * mMeshes[i].num_normals;

			mMeshes[i].normals = 0x0;

			if (mMeshes[i].num_normals > 0)
				mMeshes[i].normals = new tr2_vertex_t[mMeshes[i].num_normals];

			memcpy(mMeshes[i].normals, mesh_pointer, size);
		}
		//FIXME: endian

		mesh_pointer += size;

		/* Get number of textured rectangles */
		memcpy(&mMeshes[i].num_textured_rectangles,
				 mesh_pointer, sizeof(unsigned short));
		//FIXME: endian

		mesh_pointer += sizeof(unsigned short);
		mMeshes[i].num_textured_rectangles =
      (short)abs(mMeshes[i].num_textured_rectangles);

		size = sizeof(tr2_quad_t) * mMeshes[i].num_textured_rectangles;

		mMeshes[i].textured_rectangles = 0x0;

		if (mMeshes[i].num_textured_rectangles > 0)
			mMeshes[i].textured_rectangles =
			new tr2_quad_t[mMeshes[i].num_textured_rectangles];

		printDebug("ExtractMeshes", "mMeshes[%i].num_textured_rectangles = %u",
					  i, mMeshes[i].num_textured_rectangles);

		/* Get list of textured rectangles */
		if (mMeshes[i].num_textured_rectangles > 0)
		{
			if (mEngineVersion == TR_VERSION_4)
			{
				int j;

				for (j = 0; j < mMeshes[i].num_textured_rectangles; ++j)
				{
					memcpy(&mMeshes[i].textured_rectangles[j],
							 mesh_pointer, sizeof(tr2_quad_t));
					mesh_pointer += sizeof(tr2_quad_t) + sizeof(unsigned short);
				}
			}
			else
			{
				memcpy(mMeshes[i].textured_rectangles, mesh_pointer, size);
			}
			//FIXME: endian

			if (mEngineVersion != TR_VERSION_4)
				mesh_pointer += size;
    }

		/* Get number of textured triangles */
		memcpy(&mMeshes[i].num_textured_triangles,
				 mesh_pointer, sizeof(unsigned short));
		//FIXME: endian

		mesh_pointer += sizeof(unsigned short);
		mMeshes[i].num_textured_triangles =
      (short)abs(mMeshes[i].num_textured_triangles);

		size = sizeof(tr2_tri_t) * mMeshes[i].num_textured_triangles;

		//if (mEngineVersion == TR_VERSION_4)
		//	 size += 2 * mMeshes[i].num_textured_triangles;

		mMeshes[i].textured_triangles = 0x0;

		if (mMeshes[i].num_textured_triangles > 0)
		{
			mMeshes[i].textured_triangles =
			new tr2_tri_t[mMeshes[i].num_textured_triangles];
		}

		printDebug("ExtractMeshes", " mMeshes[%i].num_textured_triangles = %u",
					  i, mMeshes[i].num_textured_triangles);

		/* Get list of textured triangles */
		if (mMeshes[i].num_textured_triangles > 0)
		{
			if (mEngineVersion == TR_VERSION_4)
			{
				int j;

				for (j = 0; j < mMeshes[i].num_textured_triangles; ++j)
				{
					memcpy(&mMeshes[i].textured_triangles[j],
							 mesh_pointer, sizeof(tr2_tri_t));
					mesh_pointer += sizeof(tr2_tri_t) + sizeof(unsigned short);
				}
			}
			else
			{
				memcpy(mMeshes[i].textured_triangles, mesh_pointer, size);
			}
			//FIXME: endian

			if (mEngineVersion != TR_VERSION_4)
				mesh_pointer += size;
		}

		if (mEngineVersion == TR_VERSION_4)
		{
			mMeshes[i].num_coloured_rectangles = 0;
			mMeshes[i].num_coloured_triangles = 0;
			mMeshes[i].coloured_rectangles = 0x0;
			mMeshes[i].coloured_triangles = 0x0;

			// Mongoose 2002.04.04, FIXME is this right?
			mesh_pointer += 2;
			continue;
		}

		/* Get number of coloured rectangles */
		memcpy(&mMeshes[i].num_coloured_rectangles, mesh_pointer,
				 sizeof(unsigned short));
		//FIXME: endian
		mesh_pointer += sizeof(unsigned short);
		mMeshes[i].num_coloured_rectangles =
      (short)abs(mMeshes[i].num_coloured_rectangles);

		mMeshes[i].coloured_rectangles = 0x0;

		size = sizeof(tr2_quad_t) * mMeshes[i].num_coloured_rectangles;

		if (mMeshes[i].num_coloured_rectangles > 0)
			mMeshes[i].coloured_rectangles =
			new tr2_quad_t[mMeshes[i].num_coloured_rectangles];

		printDebug("ExtractMeshes", "mMeshes[%i].num_coloured_rectangles = %u",
					  i, mMeshes[i].num_coloured_rectangles);

		/* Get list of coloured rectangles */
		if (mMeshes[i].num_coloured_rectangles > 0)
		{
			memcpy(mMeshes[i].coloured_rectangles, mesh_pointer, size);
			//FIXME: endian
			mesh_pointer += size;
		}

		/* Get number of coloured triangles */
		memcpy(&mMeshes[i].num_coloured_triangles, mesh_pointer,
				 sizeof(unsigned short));
		//FIXME: endian

		mesh_pointer += sizeof(unsigned short);
		mMeshes[i].num_coloured_triangles =
      (short)abs(mMeshes[i].num_coloured_triangles);

		size = sizeof(tr2_tri_t) * mMeshes[i].num_coloured_triangles;

		mMeshes[i].coloured_triangles = 0x0;

		if (mMeshes[i].num_coloured_triangles > 0)
			mMeshes[i].coloured_triangles =
			new tr2_tri_t[mMeshes[i].num_coloured_triangles];

		printDebug("ExtractMeshes", "mMeshes[%i].num_coloured_triangles = %u",
					  i, mMeshes[i].num_coloured_triangles);

		/* Get list of coloured triangles */
		if (mMeshes[i].num_coloured_triangles > 0)
		{
			memcpy(mMeshes[i].coloured_triangles, mesh_pointer, size);
			//FIXME: endian

			mesh_pointer += size;
		}
	}
}


int TombRaider::Fread(void *buffer, size_t size, size_t count, FILE *f)
{
#ifdef ZLIB_SUPPORT
   int num_read;


   if (mFreadMode == TR_FREAD_COMPRESSED)
   {
     num_read = count;
     num_read *= size;

     if ((mCompressedLevelDataOffset + num_read) <= mCompressedLevelSize)
     {
       memcpy(buffer, &mCompressedLevelData[mCompressedLevelDataOffset],
				  num_read);

       mCompressedLevelDataOffset += num_read;

       return count;
     }
     else
     {
       print("Fread(%p, %d, %d, %p)", "ERROR: Returned %d bytes too far",
				 buffer, size, count, f, num_read);
		 reset();
       exit(2);
     }
   }
#endif
	unsigned int offset = ftell(f);

   if (fread(buffer, size, count, f) != count)
   {
      print("Fread", "ERROR: Failed fread. Should Abort. @ 0x%x", offset);
      reset();
      exit(2);
      return -1;
   }

   return count;
}


void TombRaider::getRiffData(unsigned int *bytes, unsigned char **data)
{
	*bytes = 0;
	*data = 0x0;

	if (mRiffDataSz)
	{
		*bytes = mRiffDataSz;
		*data = new unsigned char[mRiffDataSz];

		memcpy(*data, mRiffData, mRiffDataSz);
	}
}


void TombRaider::getRiffDataTR4(unsigned int index,
										  unsigned int *bytes, unsigned char **data)
{
	*bytes = 0;
	*data = 0x0;

	if (index < mNumTR4Samples)
	{
		*bytes = mTR4SamplesSz[index];
		*data = new unsigned char[*bytes];

		memcpy(*data, mTR4Samples[index], *bytes);
	}
}


int TombRaider::getRiffOffsets(unsigned char *riffData,
										 unsigned int riffDataBytes,
										 unsigned int **offsets,
										 unsigned int numOffsets)
{
	unsigned int i, j, riffCount, state;

	*offsets = new unsigned int[numOffsets];

	for (i = 0, riffCount = 0, state = 4; i < riffDataBytes; ++i)
	{
		switch (riffData[i])
		{
		case 'R':
			if (state == 4)
			{
				j = i; // tmp offset guess
				state = 0;
				continue;
			}
			break;
		case 'I':
			if (state == 0)
			{
				state = 1;
				continue;
			}
			break;
		case 'F':
			if (state == 1)
			{
				state = 2;
				continue;
			}
			else if (state == 2)
			{
				state = 3;

				// Found RIFF header, but we can only report
				//  riffCount offsets ( buffer size limits )
				if (riffCount < numOffsets)
				{
					(*offsets)[riffCount] = j;
				}

				++riffCount;
				continue;
			}
			break;
		}

		state = 4;
	}

	return riffCount;
}


unsigned char *TombRaider::getTexTile(int texture)
{
	unsigned char *image;
	unsigned int color;
	int j, k, index, offset;
	int xmin, xmax, ymin, ymax, x, y;


	image = NULL;

	if (texture >=0 && texture < (int)_num_textiles)
   {
		image = new unsigned char[256*256*4];
		memset(image, 0, 256*256*4);

		if (_textile32)
		{
			// Convert 32bit BGRA image format to 32bit RGBA
			for (j = 0; j < 256; j++)
			{
				for (k = 0; k < 256; k++)
				{
					index = (j * 256) + k;
					color = _textile32[texture].tile[index];
					index = (j * 1024) + (k * 4);

					image[index + 2] = *((unsigned char *)(&color));
					image[index + 1] = *((unsigned char *)(&color)+1);
					image[index + 0] = *((unsigned char *)(&color)+2);
					image[index + 3] = *((unsigned char *)(&color)+3);
				}
			}
		}
		else
		{
			// Convert 16bit ARGB image format to 32bit RGBA
			for (j = 0; j < 256; j++)
			{
				for (k = 0; k < 256; k++)
				{
					index = (j * 256) + k;
					offset = _textile16[texture].tile[index];
					index = (j * 1024) + (k * 4);

					image[index + 0] = ((offset >> 10) & 0x1f) * 8;
					image[index + 1] = ((offset >> 5) & 0x1f) * 8;
					image[index + 2] = (offset & 0x1f) * 8;
					image[index + 3] = (offset & 0x8000) ? 0xFF : 0;
				}
			}
		}

		switch (Engine())
		{
		case TR_VERSION_4:
		case TR_VERSION_3: // Account for alpha flags
			for (j = 0; j < (int)_num_object_textures; j++)
			{
				// FIXME: This kind of works for lighting - but messes up lara
#ifdef FIXME
				if (_object_textures[j].tile == texture &&
					 _object_textures[j].transparency_flags == 1)
				{
					xmin = 999;
					xmax = 0;
					ymin = 999;
					ymax = 0;
					y = 4;

					// Account for triangles
					if (_object_textures[j].vertices[3].xpixel == 0 &&
						 _object_textures[j].vertices[3].ypixel == 0)
						y = 3;

					for (k = 0; k < y; k++)
					{
						if (_object_textures[j].vertices[k].xpixel > xmax)
							xmax = _object_textures[j].vertices[k].xpixel;

						if (_object_textures[j].vertices[k].xpixel < xmin)
							xmin = _object_textures[j].vertices[k].xpixel;

						if (_object_textures[j].vertices[k].ypixel > ymax)
							ymax = _object_textures[j].vertices[k].ypixel;

						if (_object_textures[j].vertices[k].ypixel < ymin)
							ymin = _object_textures[j].vertices[k].ypixel;
					}

					for (x = xmin; x <= xmax; x++)
					{
						for (y = ymin; y <= ymax; y++)
						{
							index = (y * 256) + x;
							offset = _textile16[texture].tile[index];
							index = (y * 1024) + (x * 4);

							image[index + 0] = ((offset >> 10) & 0x1f) * 8;
							image[index + 1] = ((offset >> 5) & 0x1f) * 8;
							image[index + 2] = (offset & 0x1f) * 8;

							// Set transparency to full
							if (offset & 0x8000)
							{
								image[index + 3] = 0x00;
							}
						}
					}
				}
				else
#endif
					if (_object_textures[j].tile == texture &&
						 _object_textures[j].transparency_flags == 2)
				{
					xmin = 999;
					xmax = 0;
					ymin = 999;
					ymax = 0;
					y = 4;

					// Account for triangles
					if (_object_textures[j].vertices[3].xpixel == 0 &&
						 _object_textures[j].vertices[3].ypixel == 0)
						y = 3;

					for (k = 0; k < y; k++)
					{
						if (_object_textures[j].vertices[k].xpixel > xmax)
							xmax = _object_textures[j].vertices[k].xpixel;

						if (_object_textures[j].vertices[k].xpixel < xmin)
							xmin = _object_textures[j].vertices[k].xpixel;

						if (_object_textures[j].vertices[k].ypixel > ymax)
							ymax = _object_textures[j].vertices[k].ypixel;

						if (_object_textures[j].vertices[k].ypixel < ymin)
							ymin = _object_textures[j].vertices[k].ypixel;
					}

					for (x = xmin; x <= xmax; x++)
					{
						for (y = ymin; y <= ymax; y++)
						{
							if (_textile32)
							{
								index = (y * 256) + x;
								color = _textile32[texture].tile[index];
								index = (y * 1024) + (x * 4);

								image[index + 2] = *((unsigned char *)(&color));
								image[index + 1] = *((unsigned char *)(&color)+1);
								image[index + 0] = *((unsigned char *)(&color)+2);
								image[index + 3] = *((unsigned char *)(&color)+3);

								k = image[index] + image[index + 1] + image[index + 2];

								// Set transparency based upon intensity
								image[index + 3] =  (k / 3);
							}
							else
							{
								index = (y * 256) + x;
								offset = _textile16[texture].tile[index];
								index = (y * 1024) + (x * 4);

								image[index + 0] = ((offset >> 10) & 0x1f) * 8;
								image[index + 1] = ((offset >> 5) & 0x1f) * 8;
								image[index + 2] = (offset & 0x1f) * 8;
								image[index + 3] = (offset & 0x8000) ? 0xFF : 0;

								k = image[index] + image[index + 1] + image[index + 2];

								// Set transparency based upon intensity
								if (offset & 0x8000)
									image[index + 3] = (k / 3);
								else
									image[index + 3] = 0;
							}
						}
					}
				}
			}
			break;
		default:
			;
		}
	}

	return image;
}


// FIXME: Move these data about to make full use in the class  ;)
int TombRaider::loadTR5(FILE *f, void (*percent)(int))
{
	unsigned int level_data_sz, riffOffset, seperator0;
	unsigned int portalOffset, nextRoomOffset, thisRoomOffset;
	int i, j, k;
	u_int16_t us;

	u_int32_t numMeshData, numMeshPointers, u;
	u_int32_t *meshPointers;
	u_int8_t *meshData;
	char check[32];

   if (percent)
		(*percent)(5);

   printDebug("Load", "mEngineVersion = 0x%x", mPakVersion);

   if (mEngineVersion != TR_VERSION_5)
     return -1;

#ifdef ZLIB_SUPPORT
	unsigned int sz, usz; // compressed and uncompressed size
	unsigned char *compressed_data = NULL;
	int zerr;
	uLongf foo;


	// Read texture type offsets
	Fread(&_num_room_textures, 2, 1, f);
	printDebug("LoadTR5", "_num_room_textures = %u", _num_room_textures);
	Fread(&_num_misc_textures, 2, 1, f);
	printDebug("LoadTR5", "_num_misc_textures = %u", _num_misc_textures);
	Fread(&_num_bump_map_textures, 2, 1, f);
	printDebug("LoadTR5", "_num_bump_map_textures  = %u", _num_bump_map_textures);


	// Read the sizes of the 32-bit textures
	Fread(&usz, sizeof(usz), 1, f);
	Fread(&sz, sizeof(sz), 1, f);

	printDebug("LoadTR5", "32-bit textures compressed size   = %u bytes", sz);
	printDebug("LoadTR5", "32-bit textures uncompressed size = %u bytes", usz);

	_num_textiles = usz / sizeof(tr2_textile32_t);

	printDebug("LoadTR5", "_num_textiles = %i/%i = %i",
				  usz, sizeof(tr2_textile32_t), _num_textiles);

	_textile32 = new tr2_textile32_t[_num_textiles];

	// Allocate a temporary buffer for decompression
	compressed_data = new unsigned char[sz];
	Fread(compressed_data, sz, 1, f);

	// Decompress the textures
	foo = usz;
	zerr = uncompress((unsigned char *)_textile32,
							&foo,
							compressed_data,
							sz);
	usz = foo;

	printDebug("LoadTR5", "textile decompress  [%s]",
				  (zerr == Z_OK) ? "OK" : "ERROR");

	switch (zerr)
	{
	case Z_MEM_ERROR:
		printDebug("LoadTR5", "There was not enough memory");
		break;
	case Z_BUF_ERROR:
		printDebug("LoadTR5", "There was not enough room in the output buffer");
		break;
	case Z_DATA_ERROR:
		printDebug("LoadTR5", "The input data was corrupted");
		break;
	default:
		printDebug("LoadTR5", "textile decompress %i", zerr);
	}

	// Free the temporary buffer
	delete [] compressed_data;

	// Read in the 16-bit textures, set NumTextiles
	Fread(&usz, sizeof(usz), 1, f);
	Fread(&sz, sizeof(sz), 1, f);

	printDebug("LoadTR5", "16-bit textures compressed size   = %u bytes", sz);
	printDebug("LoadTR5", "16-bit textures uncompressed size = %u bytes", usz);

	_num_textiles = usz / sizeof(tr2_textile16_t);

	printDebug("LoadTR5", "_num_textiles = %i/%i = %i",
				  usz, sizeof(tr2_textile16_t), _num_textiles);

	_textile16 = new tr2_textile16_t[_num_textiles];

	// Allocate a temporary buffer for decompression
	compressed_data = new unsigned char[sz];
	Fread(compressed_data, sz, 1, f);

	// Decompress the textures
	foo = usz;
	zerr = uncompress((unsigned char *)_textile16,
							&foo,
							compressed_data,
							sz);
	usz = foo;

   if (percent)
		(*percent)(7);

	printDebug("LoadTR5", "textile decompress  [%s]",
				  (zerr == Z_OK) ? "OK" : "ERROR");

	switch (zerr)
	{
	case Z_MEM_ERROR:
		printDebug("LoadTR5", "There was not enough memory");
		break;
	case Z_BUF_ERROR:
		printDebug("LoadTR5", "There was not enough room in the output buffer");
		break;
	case Z_DATA_ERROR:
		printDebug("LoadTR5", "The input data was corrupted");
		break;
	default:
		printDebug("LoadTR5", "textile decompress %i", zerr);
	}

	// Free the temporary buffer
	delete [] compressed_data;

	// Read the sizes of the sprite textures
	Fread(&usz, sizeof(usz), 1, f);
	Fread(&sz, sizeof(sz), 1, f);

	printDebug("LoadTR5", "sprite textures compressed size   = %u bytes", sz);
	printDebug("LoadTR5", "sprite textures uncompressed size = %u bytes", usz);

	// Load sprite/bump map/gui/etc textures also
	_num_tex_special = usz/(256*256*4);

	printDebug("LoadTR5", "_num_tex_special = %i/%i = %i",
				  usz, 256*256*4, _num_tex_special);

	printDebug("LoadTR5", "Reading %ibytes of sprite textures", usz);

	if (usz)
	{
		_tex_special = new unsigned char[usz];

		// Allocate a temporary buffer for decompression
		compressed_data = new unsigned char[sz];
		Fread(compressed_data, sz, 1, f);

		// Decompress the textures
		foo = usz;
		zerr = uncompress(_tex_special,
								&foo,
								compressed_data,
								sz);
		usz = foo;

		printDebug("LoadTR5", "special texture decompress  [%s]",
					  (zerr == Z_OK) ? "OK" : "ERROR");

		switch (zerr)
		{
		case Z_MEM_ERROR:
			printDebug("LoadTR5", "There was not enough memory");
			break;
		case Z_BUF_ERROR:
			printDebug("LoadTR5", "There was not enough room in the output buffer");
			break;
		case Z_DATA_ERROR:
			printDebug("LoadTR5", "The input data was corrupted");
			break;
		default:
			printDebug("LoadTR5", "textile decompress %i", zerr);
		}

		// Free the temporary buffer
		delete [] compressed_data;
	}
#else
	Print("LoadTR5> ERROR: TR5 support not compiled in this build.");
	Print("LoadTR5> Requires zlib-devel.");
	return -1;
#endif

   if (percent)
		(*percent)(10);

	// Mongoose 2002.01.08, Michiel has discovered the
	//   first 4 bytes here are 2 bitu16 flags for Lara type and weather
	u_int16_t laraType, weather;

	Fread(&laraType, 2, 1, f);
	printDebug("LoadTR5", "laraType = 0x%x", laraType);
	Fread(&weather, 2, 1, f);
	printDebug("LoadTR5", "weather = 0x%x", weather);

	printDebug("LoadTR5", "skipping 28bytes of unknowns");
	Fread(&seperator0, 4, 1, f);
	printDebug("LoadTR5", "0x%x", seperator0);
	Fread(&seperator0, 4, 1, f);
	printDebug("LoadTR5", "0x%x", seperator0);
	Fread(&seperator0, 4, 1, f);
	printDebug("LoadTR5", "0x%x", seperator0);
	Fread(&seperator0, 4, 1, f);
	printDebug("LoadTR5", "0x%x", seperator0);
	Fread(&seperator0, 4, 1, f);
	printDebug("LoadTR5", "0x%x", seperator0);
	Fread(&seperator0, 4, 1, f);
	printDebug("LoadTR5", "0x%x", seperator0);
	Fread(&seperator0, 4, 1, f);
	printDebug("LoadTR5", "0x%x", seperator0);

	Fread(&level_data_sz, 4, 1, f);
	printDebug("LoadTR5", "Level data size = %u", level_data_sz);

	Fread(&riffOffset, 4, 1, f);
	printDebug("LoadTR5", "Same as last, also offset to RIFFs = %u",
				  riffOffset);

	Fread(&seperator0, 4, 1, f);
	printDebug("LoadTR5", "Seperator, always 0x00? = %u", seperator0);

	Fread(&_num_rooms, 4, 1, f);
	printDebug("LoadTR5", "_num_rooms = %u", _num_rooms);

	mRoomsTR5 = new tr5_room_t[_num_rooms];

	for (i = 0; i < _num_rooms; ++i)
	{
		if (percent)
			(*percent)(11 + (int)(((float)i/(float)_num_rooms)*90.0));

		thisRoomOffset = ftell(f);

		Fread(&mRoomsTR5[i].checkXELA, 4, 1, f);
		printDebug("LoadTR5", "room[%i].checkXELA (0x414c4558)? = 0x%x",
					  i, mRoomsTR5[i].checkXELA);

		if (mRoomsTR5[i].checkXELA != 0x414c4558)
		{
			print("LoadTR5", "Error #1 room[%i].checkXELA (0x414c4558) != 0x%x",
					i, mRoomsTR5[i].checkXELA);
			return -3;
		}

		Fread(&mRoomsTR5[i].roomDataSize, 4, 1, f);
		printDebug("LoadTR5", "offset to next room = %u",
					  mRoomsTR5[i].roomDataSize);

		nextRoomOffset = ftell(f) + mRoomsTR5[i].roomDataSize;

		Fread(&mRoomsTR5[i].seperator, 4, 1, f);
		printDebug("LoadTR5", "room[%i].seperator, CDCDCDCD = %X?",
					  i, mRoomsTR5[i].seperator);

		if (mRoomsTR5[i].seperator != 0xcdcdcdcd)
		{
			print("LoadTR5", "Error #2 room[%i].seperator, CDCDCDCD != 0x%X",
					i, mRoomsTR5[i].seperator);
			return -3;
		}

		// Start 60byte struct /////////////
		printDebug("LoadTR5", "60byte struct {");

		// Often start of "XELA" +216 + ublock1 = FD end,
		// but have seen 0xffffffff (-1). Better using next data
		// and compute FD size the old way of X*Z*8
		Fread(&mRoomsTR5[i].endSDOffset, 4, 1, f);
		printDebug("LoadTR5", "%u + 216 + XELA start = FD end",
					  mRoomsTR5[i].endSDOffset);

		// Start of "XELA" + 216 + ublock2 = FD start
		Fread(&mRoomsTR5[i].startSDOffset, 4, 1, f);
		printDebug("LoadTR5", "%u + 216 + XELA start = FD start",
					  mRoomsTR5[i].startSDOffset);

		Fread(&mRoomsTR5[i].seperator2, 4, 1, f);
		printDebug("LoadTR5", "seperator2 = %u", mRoomsTR5[i].seperator2);

		if (mRoomsTR5[i].seperator2 != 0xcdcdcdcd &&
			 mRoomsTR5[i].seperator2 != 0x00000000)
		{
			print("LoadTR5", "Error #3 CDCDCDCD | 0x0 != 0x%x",
					mRoomsTR5[i].seperator2);
			return -3;
		}

		// Possibly start of "XELA" + 216 + ublock4 = end portals
		Fread(&mRoomsTR5[i].endPortalOffset, 4, 1, f);
		printDebug("LoadTR5", "%u + 216 + XELA start = end portals",
					  mRoomsTR5[i].endPortalOffset);

		Fread(&mRoomsTR5[i].x, 4, 1, f);
		printDebug("LoadTR5", "room.x = %u", mRoomsTR5[i].x);
		Fread(&mRoomsTR5[i].seperator3, 4, 1, f);
		printDebug("LoadTR5", "0x00000000 = %u ?", mRoomsTR5[i].seperator3);
		Fread(&mRoomsTR5[i].z, 4, 1, f);
		printDebug("LoadTR5", "room.z = %u", mRoomsTR5[i].z);
		Fread(&mRoomsTR5[i].yBottom, 4, 1, f);
		printDebug("LoadTR5", "room.y_bottom = %u", mRoomsTR5[i].yBottom);
		Fread(&mRoomsTR5[i].yTop, 4, 1, f);
		printDebug("LoadTR5", "room.y_top = %u", mRoomsTR5[i].yTop);

		Fread(&mRoomsTR5[i].numZSectors, 2, 1, f);
		printDebug("LoadTR5", "num_z_sectors = %i", mRoomsTR5[i].numZSectors);
		Fread(&mRoomsTR5[i].numXSectors, 2, 1, f);
		printDebug("LoadTR5", "num_x_sectors = %i", mRoomsTR5[i].numXSectors);

		Fread(&mRoomsTR5[i].roomAmbientColor, 4, 1, f);
		printDebug("LoadTR5", "room_ambient_color = 0x%x",
					  mRoomsTR5[i].roomAmbientColor);

		Fread(&mRoomsTR5[i].numRoomLights, 2, 1, f);
		printDebug("LoadTR5", "num_lights = %i", mRoomsTR5[i].numRoomLights);
		Fread(&mRoomsTR5[i].numStaticMeshes, 2, 1, f);
		printDebug("LoadTR5", "num_static_meshes = %i",
					  mRoomsTR5[i].numStaticMeshes);

		Fread(&mRoomsTR5[i].unknownR1, 2, 1, f);
		printDebug("LoadTR5", "unknown 0x0001? = 0x%x", mRoomsTR5[i].unknownR1);
		Fread(&mRoomsTR5[i].unknownR2, 2, 1, f);
		printDebug("LoadTR5", "unknown 0x0000? = 0x%x", mRoomsTR5[i].unknownR2);
		Fread(&mRoomsTR5[i].filler, 4, 1, f);
		printDebug("LoadTR5", "Always 0x7fff? = 0x%x", mRoomsTR5[i].filler);
		Fread(&mRoomsTR5[i].filler2, 4, 1, f);
		printDebug("LoadTR5", "Always 0x7fff? = 0x%x", mRoomsTR5[i].filler2);

		printDebug("LoadTR5", "}");
		// End 60byte structure /////////////////

		Fread(&mRoomsTR5[i].seperator4, 4, 1, f);
		printDebug("LoadTR5", "seperator4 CDCDCDCD = 0x%x",
					  mRoomsTR5[i].seperator4);

		if (mRoomsTR5[i].seperator4 != 0xcdcdcdcd)
		{
			print("LoadTR5", "Error #5 CDCDCDCD != 0x%x",
					mRoomsTR5[i].seperator4);
			return -3;
		}

		Fread(&mRoomsTR5[i].seperator5, 4, 1, f);
		printDebug("LoadTR5", "seperator5 CDCDCDCD = 0x%x",
					  mRoomsTR5[i].seperator5);

		if (mRoomsTR5[i].seperator5 != 0xcdcdcdcd)
		{
			print("LoadTR5", "Error #6 CDCDCDCD != 0x%x",
					mRoomsTR5[i].seperator5);
			return -3;
		}

		// Start 20byte structure ///////////////
		printDebug("LoadTR5", "20byte struct {");
		Fread(mRoomsTR5[i].seperator6, 6, 1, f);
		printDebug("LoadTR5", "6 bytes 0xFF = 0x%x%x%x%x%x",
					  mRoomsTR5[i].seperator6[0], mRoomsTR5[i].seperator6[1],
					  mRoomsTR5[i].seperator6[2], mRoomsTR5[i].seperator6[3],
					  mRoomsTR5[i].seperator6[4], mRoomsTR5[i].seperator6[5]);
		Fread(&mRoomsTR5[i].roomFlag, 2, 1, f);
		printDebug("LoadTR5", "room_flag = %i", mRoomsTR5[i].roomFlag);
		Fread(&mRoomsTR5[i].unknownR5, 2, 1, f);
		printDebug("LoadTR5", "unknown = %i", mRoomsTR5[i].unknownR5);
		Fread(mRoomsTR5[i].seperator7, 10, 1, f);
		printDebug("LoadTR5", "10 bytes 0x00 = 0x%x%x%x%x%x%x%x%x%x%x",
					  mRoomsTR5[i].seperator7[0], mRoomsTR5[i].seperator7[1],
					  mRoomsTR5[i].seperator7[2], mRoomsTR5[i].seperator7[3],
					  mRoomsTR5[i].seperator7[4], mRoomsTR5[i].seperator7[5],
					  mRoomsTR5[i].seperator7[6], mRoomsTR5[i].seperator7[7],
					  mRoomsTR5[i].seperator7[8], mRoomsTR5[i].seperator7[9]);
		printDebug("LoadTR5", "}");
		// End 20byte structure /////////////////

		Fread(&mRoomsTR5[i].seperator8, 4, 1, f);
		printDebug("LoadTR5", "seperator8 CDCDCDCD = 0x%x",
					  mRoomsTR5[i].seperator8);

		if (mRoomsTR5[i].seperator8 != 0xcdcdcdcd)
		{
			print("LoadTR5", "Error #9 CDCDCDCD != 0x%x",
					mRoomsTR5[i].seperator8);
			return -3;
		}

		printDebug("LoadTR5", "16byte struct {");
		Fread(&mRoomsTR5[i].unknownR6, 4, 1, f);
		printDebug("LoadTR5", "unknownR6 = %i", mRoomsTR5[i].unknownR6);
		Fread(&mRoomsTR5[i].roomX, 4, 1, f);
		printDebug("LoadTR5", "roomX = %i", mRoomsTR5[i].roomX);

		Fread(&mRoomsTR5[i].seperator9, 4, 1, f);
		printDebug("LoadTR5", "seperator9 CDCDCDCD | 0x0 = 0x%x",
					  mRoomsTR5[i].seperator9);

		if (mRoomsTR5[i].seperator9 != 0xcdcdcdcd &&
			 mRoomsTR5[i].seperator9 != 0x0)
		{
			print("LoadTR5", "Error #10 CDCDCDCD | 0x0 != 0x%x",
					mRoomsTR5[i].seperator9);
			return -3;
		}

		Fread(&mRoomsTR5[i].roomZ, 4, 1, f);
		printDebug("LoadTR5", "roomZ = %i", mRoomsTR5[i].roomZ);
		printDebug("LoadTR5", "}");


		Fread(&mRoomsTR5[i].seperator10, 4, 1, f);
		printDebug("LoadTR5", "seperator10 CDCDCDCD = 0x%x",
					  mRoomsTR5[i].seperator10);

		if (mRoomsTR5[i].seperator10 != 0xcdcdcdcd)
		{
			print("LoadTR5", "Error #11 CDCDCDCD != 0x%x",
					mRoomsTR5[i].seperator10);
			return -3;
		}

		Fread(&mRoomsTR5[i].seperator11, 4, 1, f);
		printDebug("LoadTR5", "seperator11 CDCDCDCD = 0x%x",
					  mRoomsTR5[i].seperator11);

		if (mRoomsTR5[i].seperator11 != 0xcdcdcdcd)
		{
			print("LoadTR5", "Error #12 CDCDCDCD != 0x%x",
					mRoomsTR5[i].seperator11);
			return -3;
		}

		Fread(&mRoomsTR5[i].seperator12, 4, 1, f);
		printDebug("LoadTR5", "seperator12 CDCDCDCD = 0x%x",
					  mRoomsTR5[i].seperator12);

		if (mRoomsTR5[i].seperator12 != 0xcdcdcdcd)
		{
			print("LoadTR5", "Error #13 CDCDCDCD != 0x%x",
					mRoomsTR5[i].seperator12);
			return -3;
		}

		Fread(&mRoomsTR5[i].seperator13, 4, 1, f);
		printDebug("LoadTR5", "seperator13 CDCDCDCD = 0x%x",
					  mRoomsTR5[i].seperator13);

		if (mRoomsTR5[i].seperator13 != 0xcdcdcdcd)
		{
			print("LoadTR5", "Error #14 CDCDCDCD | 0x0 != 0x%x",
					mRoomsTR5[i].seperator13);
			return -3;
		}

		Fread(&mRoomsTR5[i].seperator14, 4, 1, f);
		printDebug("LoadTR5", "seperator14 CDCDCDCD = 0x%x",
					  mRoomsTR5[i].seperator14);

		if (mRoomsTR5[i].seperator14 != 0xcdcdcdcd &&
			 mRoomsTR5[i].seperator14 != 0x00000000)
		{
			print("LoadTR5", "Error #15 CDCDCDCD | 0x0 != 0x%x",
					mRoomsTR5[i].seperator14);
			return -3;
		}

		Fread(&mRoomsTR5[i].seperator15, 4, 1, f);
		printDebug("LoadTR5", "seperator15 CDCDCDCD = 0x%x",
					  mRoomsTR5[i].seperator15);

		if (mRoomsTR5[i].seperator15 != 0xcdcdcdcd)
		{
			print("LoadTR5", "Error #16 CDCDCDCD != 0x%x",
					mRoomsTR5[i].seperator15);
			return -3;
		}


		// 56byte struct /////////////
		printDebug("LoadTR5", "56byte struct {");
		Fread(&mRoomsTR5[i].numRoomTriangles, 4, 1, f);
		printDebug("LoadTR5", "num_triangles = %u",
					  mRoomsTR5[i].numRoomTriangles);
		Fread(&mRoomsTR5[i].numRoomRectangles, 4, 1, f);
		printDebug("LoadTR5", "num_rectangles = %u",
					  mRoomsTR5[i].numRoomRectangles);
		Fread(&mRoomsTR5[i].seperator16, 4, 1, f);
		printDebug("LoadTR5", "seperator16, 0x00? = 0x%x",
					  mRoomsTR5[i].seperator16);

		// Num lights * 88bytes
		Fread(&mRoomsTR5[i].lightSize, 4, 1, f);
		printDebug("LoadTR5", "light_size = %u", mRoomsTR5[i].lightSize);
		Fread(&mRoomsTR5[i].numTotalRoomLights, 4, 1, f);
		printDebug("LoadTR5", "num_lights = %u",
					  mRoomsTR5[i].numTotalRoomLights);

		Fread(&mRoomsTR5[i].unknownR7, 4, 1, f); // was num_unknown_36byte structs to read
		printDebug("LoadTR5", "unknownR7 structs = %u", mRoomsTR5[i].unknownR7);

		Fread(&mRoomsTR5[i].unknownR8, 4, 1, f);
		printDebug("LoadTR5", "unknownR8 = 0x%x", mRoomsTR5[i].unknownR8);

		Fread(&mRoomsTR5[i].yBottom, 4, 1, f);
		printDebug("LoadTR5", "lyBottom = 0x%x", mRoomsTR5[i].lyBottom);

		Fread(&mRoomsTR5[i].numLayers, 4, 1, f);
		printDebug("LoadTR5", "num_layers = %u", mRoomsTR5[i].numLayers);

		Fread(&mRoomsTR5[i].layerOffset, 4, 1, f);
		printDebug("LoadTR5", "layerOffset = 0x%x", mRoomsTR5[i].layerOffset);
		Fread(&mRoomsTR5[i].verticesOffset, 4, 1, f);
		printDebug("LoadTR5", "verticesOffset = 0x%x", mRoomsTR5[i].verticesOffset);
		Fread(&mRoomsTR5[i].polyOffset, 4, 1, f);
		printDebug("LoadTR5", "polyOffset = 0x%x", mRoomsTR5[i].polyOffset);
		Fread(&mRoomsTR5[i].polyOffset2, 4, 1, f);
		printDebug("LoadTR5", "polyOffset2 = 0x%x", mRoomsTR5[i].polyOffset2);
		Fread(&mRoomsTR5[i].verticesSize, 4, 1, f);
		printDebug("LoadTR5", "verticesSize = 0x%x", mRoomsTR5[i].verticesSize);

		printDebug("LoadTR5", "}");
		//////////////////////////////

		Fread(&mRoomsTR5[i].seperator17, 4, 1, f);
		printDebug("LoadTR5", "seperator17 CDCDCDCD = 0x%x",
					  mRoomsTR5[i].seperator17);

		if (mRoomsTR5[i].seperator17 != 0xcdcdcdcd)
		{
			print("LoadTR5", "Error #18 CDCDCDCD != 0x%x",
					mRoomsTR5[i].seperator17);
			return -3;
		}

		Fread(&mRoomsTR5[i].seperator18, 4, 1, f);
		printDebug("LoadTR5", "seperator18 CDCDCDCD = 0x%x",
					  mRoomsTR5[i].seperator18);

		if (mRoomsTR5[i].seperator18 != 0xcdcdcdcd)
		{
			print("LoadTR5", "Error #19 CDCDCDCD != 0x%x",
					mRoomsTR5[i].seperator18);
			return -3;
		}

		Fread(&mRoomsTR5[i].seperator19, 4, 1, f);
		printDebug("LoadTR5", "seperator19 CDCDCDCD = 0x%x",
					  mRoomsTR5[i].seperator19);

		if (mRoomsTR5[i].seperator19 != 0xcdcdcdcd)
		{
			print("LoadTR5", "Error #20 CDCDCDCD != 0x%x",
					mRoomsTR5[i].seperator19);
			return -3;
		}

		Fread(&mRoomsTR5[i].seperator20, 4, 1, f);
		printDebug("LoadTR5", "seperator20 CDCDCDCD = 0x%x",
					  mRoomsTR5[i].seperator20);

		if (mRoomsTR5[i].seperator20 != 0xcdcdcdcd)
		{
			print("LoadTR5", "Error #21 CDCDCDCD != 0x%x",
					mRoomsTR5[i].seperator20);
			return -3;
		}

		// Lights
		printDebug("LoadTR5", "Reading %u lights @ 88bytes each",
					  mRoomsTR5[i].numRoomLights);

		if (mRoomsTR5[i].numRoomLights)
		{
			mRoomsTR5[i].lights = new tr5_light_t[mRoomsTR5[i].numRoomLights];
		}

		for (j = 0; j < (int)mRoomsTR5[i].numRoomLights; ++j)
		{
			Fread(&mRoomsTR5[i].lights[j], 88, 1, f);

			//Fread(&mRoomsTR5[i].lights[j].x, 4, 1, f);
			printDebug("LoadTR5", "light[%i].x? = %f", j,
						  mRoomsTR5[i].lights[j].x);

			//Fread(&mRoomsTR5[i].lights[j].y, 4, 1, f);
			printDebug("LoadTR5", "light[%i].y? = %f", j,
						  mRoomsTR5[i].lights[j].y);

			//Fread(&mRoomsTR5[i].lights[j].z, 4, 1, f);
			printDebug("LoadTR5", "light[%i].z? = %f", j,
						  mRoomsTR5[i].lights[j].z);

			//Fread(&mRoomsTR5[i].lights[j].red, 4, 1, f);
			printDebug("LoadTR5", "light[%i].r? = %f",
						  j, mRoomsTR5[i].lights[j].red);

			//Fread(&mRoomsTR5[i].lights[j].green, 4, 1, f);
			printDebug("LoadTR5", "light[%i].g? = %f",
						  j, mRoomsTR5[i].lights[j].green);

			//Fread(&mRoomsTR5[i].lights[j].blue, 4, 1, f);
			printDebug("LoadTR5", "light[%i].b? = %f",
						  j, mRoomsTR5[i].lights[j].blue);

			// 24bytes from start of light
			//Fread(&mRoomsTR5[i].lights[j].seperator, 4, 1, f);
			printDebug("LoadTR5", "CDCDCDCD for some maps? = 0x%8x\t\t[%s]",
						  mRoomsTR5[i].lights[j].seperator,
						  (mRoomsTR5[i].lights[j].seperator == 0xcdcdcdcd)
						  ? "OK" : "ERROR");

			//Fread(&mRoomsTR5[i].lights[j].input, 4, 1, f);
			printDebug("LoadTR5", "light[%i].input = %f",
						  j, mRoomsTR5[i].lights[j].input);

			//Fread(&mRoomsTR5[i].lights[j].output, 4, 1, f);
			printDebug("LoadTR5", "light[%i].output = %f",
						  j, mRoomsTR5[i].lights[j].output);

			//Fread(&mRoomsTR5[i].lights[j].range, 4, 1, f);
			printDebug("LoadTR5", "light[%i].range = %f",
						  j, mRoomsTR5[i].lights[j].range);

			//Fread(&mRoomsTR5[i].lights[j].directionVectorX, 4, 1, f);
			printDebug("LoadTR5", "light[%i].directionVectorX = %f",
						  j, mRoomsTR5[i].lights[j].directionVectorX);
			//Fread(&mRoomsTR5[i].lights[j].directionVectorY, 4, 1, f);
			printDebug("LoadTR5", "light[%i].directionVectorY = %f",
						  j, mRoomsTR5[i].lights[j].directionVectorY);
			//Fread(&mRoomsTR5[i].lights[j].directionVectorZ, 4, 1, f);
			printDebug("LoadTR5", "light[%i].directionVectorZ = %f",
						  j, mRoomsTR5[i].lights[j].directionVectorZ);

			//Fread(&mRoomsTR5[i].lights[j].x2, 4, 1, f);
			printDebug("LoadTR5", "light[%i].x2 = %u",
						  j, mRoomsTR5[i].lights[j].x2);
			//Fread(&mRoomsTR5[i].lights[j].y2, 4, 1, f);
			printDebug("LoadTR5", "light[%i].y2 = %u",
						  j, mRoomsTR5[i].lights[j].y2);
			//Fread(&mRoomsTR5[i].lights[j].z2, 4, 1, f);
			printDebug("LoadTR5", "light[%i].z2 = %u",
						  j, mRoomsTR5[i].lights[j].z2);


			//Fread(&mRoomsTR5[i].lights[j].directionVectorX2, 4, 1, f);
			printDebug("LoadTR5", "light[%i].directionVectorX2 = %u",
						  j, mRoomsTR5[i].lights[j].directionVectorX2);
			//Fread(&mRoomsTR5[i].lights[j].directionVectorY2, 4, 1, f);
			printDebug("LoadTR5", "light[%i].directionVectorY2 = %u",
						  j, mRoomsTR5[i].lights[j].directionVectorY2);
			//Fread(&mRoomsTR5[i].lights[j].directionVectorZ2, 4, 1, f);
			printDebug("LoadTR5", "light[%i].directionVectorZ2 = %u",
						  j, mRoomsTR5[i].lights[j].directionVectorZ2);

			//Fread(&mRoomsTR5[i].lights[j].lightType, 1, 1, f);
			printDebug("LoadTR5", "light[%i].d3d_flag = 0x%x (%s)",
						  j, mRoomsTR5[i].lights[j].lightType,
						  (mRoomsTR5[i].lights[j].lightType == 1) ? "Point" :
						  (mRoomsTR5[i].lights[j].lightType == 2) ? "Spot" :
						  (mRoomsTR5[i].lights[j].lightType == 3) ? "Directional" : "Unknown");

			//Fread(&mRoomsTR5[i].lights[j].seperator2, 3, 1, f);
			printDebug("LoadTR5", "CDCDCD = %c%c%c",
						  mRoomsTR5[i].lights[j].seperator2[0],
						  mRoomsTR5[i].lights[j].seperator2[1],
						  mRoomsTR5[i].lights[j].seperator2[2]);
		}

		int numSectors = mRoomsTR5[i].numXSectors * mRoomsTR5[i].numZSectors;

		if (numSectors)
		{
			mRoomsTR5[i].sectors = new tr2_room_sector_t[numSectors];
		}

		// Sectors
		printDebug("LoadTR5", "Reading %u sectors @ 8bytes each",
					  numSectors);

		for (j = 0; j < numSectors; ++j)
		{
			Fread(&mRoomsTR5[i].sectors[j].fd_index, 2, 1, f);
			printDebug("LoadTR5", "sector[%i].fd_index = %u", j,
						  mRoomsTR5[i].sectors[j].fd_index);
			Fread(&mRoomsTR5[i].sectors[j].box_index, 2, 1, f);
			printDebug("LoadTR5", "sector[%i].box_index = %u", j,
						  mRoomsTR5[i].sectors[j].box_index);
			Fread(&mRoomsTR5[i].sectors[j].room_below, 1, 1, f);
			printDebug("LoadTR5", "sector[%i].room_below = %u", j,
						  mRoomsTR5[i].sectors[j].room_below);
			Fread(&mRoomsTR5[i].sectors[j].floor, 1, 1, f);
			printDebug("LoadTR5", "sector[%i].floor = %i", j,
						  (*(char*)(&mRoomsTR5[i].sectors[j].floor)));
			Fread(&mRoomsTR5[i].sectors[j].room_above, 1, 1, f);
			printDebug("LoadTR5", "sector[%i].room_above = %u", j,
						  mRoomsTR5[i].sectors[j].room_above);
			Fread(&mRoomsTR5[i].sectors[j].ceiling, 1, 1, f);
			printDebug("LoadTR5", "sector[%i].ceiling = %i", j,
						  (*(char*)(&mRoomsTR5[i].sectors[j].ceiling)));
		}

		portalOffset = (thisRoomOffset + mRoomsTR5[i].startSDOffset + 216 +
							 numSectors * 8);

		u = ftell(f);

		if (u != portalOffset)
		{
			printDebug("LoadTR5", "*** Skipping %i bytes to start of portals ***",
						  portalOffset - u);
			fseek(f, portalOffset, SEEK_SET);
		}

		// Portals //////////////////////
		Fread(&mRoomsTR5[i].numDoors, 2, 1, f);
		printDebug("LoadTR5", "room[%i].tr5_num_portals = %u",
					  i, mRoomsTR5[i].numDoors);

		printDebug("LoadTR5", "Reading %u portals @ 32bytes each",
					  mRoomsTR5[i].numDoors);

		if (mRoomsTR5[i].numDoors)
		{
			mRoomsTR5[i].doors = new tr2_room_portal_t[mRoomsTR5[i].numDoors];
		}

		for (j = 0; j < (int)mRoomsTR5[i].numDoors; ++j)
		{
			Fread(&mRoomsTR5[i].doors[j].adjoining_room, 2, 1, f);
			printDebug("LoadTR5", "room[%i].portal[%i].adjoining_room = %u",
						  i, j, mRoomsTR5[i].doors[j].adjoining_room);

			Fread(&mRoomsTR5[i].doors[j].normal.x, 2, 1, f);
			Fread(&mRoomsTR5[i].doors[j].normal.y, 2, 1, f);
			Fread(&mRoomsTR5[i].doors[j].normal.z, 2, 1, f);
			printDebug("LoadTR5", "portal[%i].normal = ( %i, %i, %i )", j,
						  mRoomsTR5[i].doors[j].normal.x,
						  mRoomsTR5[i].doors[j].normal.y,
						  mRoomsTR5[i].doors[j].normal.z);

			for (k = 0; k < 4; ++k)
			{
				Fread(&mRoomsTR5[i].doors[j].vertices[k].x, 2, 1, f);
				Fread(&mRoomsTR5[i].doors[j].vertices[k].y, 2, 1, f);
				Fread(&mRoomsTR5[i].doors[j].vertices[k].z, 2, 1, f);
				printDebug("LoadTR5", "portal[%i].vertices[%i] = ( %i, %i, %i )",
							  j, k,
							  mRoomsTR5[i].doors[j].vertices[k].x,
							  mRoomsTR5[i].doors[j].vertices[k].y,
							  mRoomsTR5[i].doors[j].vertices[k].z);
			}
		}

		Fread(&mRoomsTR5[i].seperator21, 2, 1, f);
		printDebug("LoadTR5", "seperator21, CDCD = 0x%x",
					  mRoomsTR5[i].seperator21);

		if (mRoomsTR5[i].seperator21 != 0xcdcd)
		{
			print("LoadTR5", "Error #22 CDCD != 0x%x",
					mRoomsTR5[i].seperator21);
			return -3;
		}

		portalOffset = (thisRoomOffset + mRoomsTR5[i].endPortalOffset + 216);

		u = ftell(f);

		if (u != portalOffset)
		{
			printDebug("LoadTR5", "*** Skipping %i bytes to end of portals ***",
						  portalOffset - u);
			fseek(f, portalOffset, SEEK_SET);
		}

		if (mRoomsTR5[i].numStaticMeshes)
		{
			mRoomsTR5[i].meshes = new tr2_room_staticmesh_t[mRoomsTR5[i].numStaticMeshes];
		}

		// Static meshes
		for (j = 0; j < (int)mRoomsTR5[i].numStaticMeshes; ++j)
		{
			Fread(&mRoomsTR5[i].meshes[j].x, 4, 1, f);
			printDebug("LoadTR5", "static_mesh[%i].x = %i", j,
						  mRoomsTR5[i].meshes[j].x);
			Fread(&mRoomsTR5[i].meshes[j].y, 4, 1, f);
			printDebug("LoadTR5", "static_mesh[%i].y = %i", j,
						  mRoomsTR5[i].meshes[j].y);
			Fread(&mRoomsTR5[i].meshes[j].z, 4, 1, f);
			printDebug("LoadTR5", "static_mesh[%i].z = %i", j,
						  mRoomsTR5[i].meshes[j].z);

			Fread(&mRoomsTR5[i].meshes[j].rotation, 2, 1, f);
			printDebug("LoadTR5", "static_mesh[%i].rotation = %i", j,
						  mRoomsTR5[i].meshes[j].rotation);

			Fread(&mRoomsTR5[i].meshes[j].intensity1, 2, 1, f);
			printDebug("LoadTR5", "static_mesh[%i].intensity1 = %i", j,
						  mRoomsTR5[i].meshes[j].intensity1);
			Fread(&mRoomsTR5[i].meshes[j].intensity2, 2, 1, f);
			printDebug("LoadTR5", "static_mesh[%i].intensity2 = %i", j,
						  mRoomsTR5[i].meshes[j].intensity2);

			Fread(&mRoomsTR5[i].meshes[j].object_id, 2, 1, f);
			printDebug("LoadTR5", "static_mesh[%i].object_id = %i", j,
						  mRoomsTR5[i].meshes[j].object_id);
		}

		// Layers /////////////////
		if (mRoomsTR5[i].numLayers)
		{
			mRoomsTR5[i].layers = new tr5_room_layer_t[mRoomsTR5[i].numLayers];
		}

		printDebug("LoadTR5", "Reading %i layers",
					  mRoomsTR5[i].numLayers);

		for (j = 0; j < (int)mRoomsTR5[i].numLayers; ++j)
		{
			Fread(&mRoomsTR5[i].layers[j], 56, 1, f);

			printDebug("LoadTR5", "layer[%i].num_vertices = %i", j,
						  mRoomsTR5[i].layers[j].numLayerVertices);
			printDebug("LoadTR5", "layer[%i].unknown1 = %i", j,
						  mRoomsTR5[i].layers[j].unknownL1);
			printDebug("LoadTR5", "layer[%i].num_rectangles = %i", j,
						  mRoomsTR5[i].layers[j].numLayerRectangles);
			printDebug("LoadTR5", "layer[%i].num_triangles = %i", j,
						  mRoomsTR5[i].layers[j].numLayerTriangles);
			printDebug("LoadTR5", "layer[%i].num_2side_textures? = %i", j,
						  mRoomsTR5[i].layers[j].unknownL2);
			printDebug("LoadTR5", "layer[%i].filler, 0? = %i", j,
						  mRoomsTR5[i].layers[j].filler);
			printDebug("LoadTR5", "layer[%i].filler2, 0? = %i", j,
						  mRoomsTR5[i].layers[j].filler2);
			printDebug("LoadTR5", "layer[%i].bbox[0] = {%.2f %.2f %.2f}", j,
						  mRoomsTR5[i].layers[j].layerBoundingBoxX1,
						  mRoomsTR5[i].layers[j].layerBoundingBoxX1,
						  mRoomsTR5[i].layers[j].layerBoundingBoxX1);
			printDebug("LoadTR5", "layer[%i].bbox[1] = {%.2f %.2f %.2f}", j,
						  mRoomsTR5[i].layers[j].layerBoundingBoxX2,
						  mRoomsTR5[i].layers[j].layerBoundingBoxX2,
						  mRoomsTR5[i].layers[j].layerBoundingBoxX2);
			printDebug("LoadTR5", "layer[%i].filler3, 0? = %i", j,
						  mRoomsTR5[i].layers[j].filler3);
			printDebug("LoadTR5", "layer[%i].unknown6 = %i", j,
						  mRoomsTR5[i].layers[j].unknownL6);
			printDebug("LoadTR5", "layer[%i].unknown7 = %i", j,
						  mRoomsTR5[i].layers[j].unknownL7);
			printDebug("LoadTR5", "layer[%i].unknown8 = %i", j,
						  mRoomsTR5[i].layers[j].unknownL8);
		}

		if (mRoomsTR5[i].numLayers)
		{
			mRoomsTR5[i].faces = new tr5_room_geometry_t[mRoomsTR5[i].numLayers];
		}

		for (j = 0; j < (int)mRoomsTR5[i].numLayers; ++j)
		{
			mRoomsTR5[i].faces[j].quads = 0x0;
			mRoomsTR5[i].faces[j].tris = 0x0;

			k = mRoomsTR5[i].layers[j].numLayerRectangles;

			if (k)
			{
				printDebug("LoadTR5", "Reading %i layer quads", k);
				mRoomsTR5[i].faces[j].quads = new tr5_face4_t[k];
				Fread(mRoomsTR5[i].faces[j].quads, 12, k, f);
			}

			k = mRoomsTR5[i].layers[j].numLayerTriangles;

			if (k)
			{
				printDebug("LoadTR5", "Reading %i layer tris", k);
				mRoomsTR5[i].faces[j].tris = new tr5_face3_t[k];
				Fread(mRoomsTR5[i].faces[j].tris, 10, k, f);
			}
		}

		for (j = 0; j < (int)mRoomsTR5[i].numLayers; ++j)
		{
			mRoomsTR5[i].faces[j].verts = 0x0;

			k = mRoomsTR5[i].layers[j].numLayerVertices;

			if (k)
			{
				printDebug("LoadTR5", "Reading %i layer vertices", k);
				mRoomsTR5[i].faces[j].verts = new tr5_vertex_t[k];
				Fread(mRoomsTR5[i].faces[j].verts, 28, k, f);
			}
		}

#define TR5_SKIP_TO_ROOMS
#ifdef TR5_SKIP_TO_ROOMS
		unsigned int hack = ftell(f);


		if (hack < nextRoomOffset)
		{
			printDebug("LoadTR5", "Skipping %i bytes at end of room[%i]",
						  nextRoomOffset - hack, i);
			fseek(f, nextRoomOffset, SEEK_SET);
		}
#else
		long hack = 0;

		// This peels padding off the end of TRCs like ANDREA1.TRC
		while (hack != 0xcdcd)
		{
			Fread(&hack, 2, 1, f);
			printDebug("LoadTR5", "hack[%i] = 0x%x", i, hack);
		}
#endif
	}

	Fread(&_num_floor_data, 4, 1, f);
	printDebug("LoadTR5", "_num_floor_data = %u", _num_floor_data);

	printDebug("LoadTR5", "Reading %u floorData elements", _num_floor_data);
	_floor_data = new unsigned short[_num_floor_data];
	Fread(_floor_data, 2, _num_floor_data, f);

	// Number of 16bits of mesh data to follow
	Fread(&numMeshData, 4, 1, f);
	printDebug("LoadTR5", "numMeshData = %u", numMeshData);

	meshData = new unsigned char[2*numMeshData];
	Fread(meshData, 2, numMeshData, f);

	// Use pointers array to index in meshData array for tr5_mesh_t's
	Fread(&numMeshPointers, 4, 1, f);
	printDebug("LoadTR5", "numMeshPointers = %u", numMeshPointers);
	meshPointers = new u_int32_t[numMeshPointers];
	Fread(meshPointers, 4, numMeshPointers, f);

	Fread(&numAnimationsTR5, 4, 1, f);
	printDebug("LoadTR5", "numAnimationsTR5 = %u", numAnimationsTR5);
	animationsTR5 = new tr5_animation_t[numAnimationsTR5];
	Fread(animationsTR5, 40, numAnimationsTR5, f);

	Fread(&u, 4, 1, f);
	_num_state_changes = u;
	printDebug("LoadTR5", "_num_state_changes = %u", _num_state_changes);
	_state_changes = new tr2_state_change_t[_num_state_changes];
	Fread(_state_changes, 6, _num_state_changes, f);

	Fread(&u, 4, 1, f);
	_num_anim_dispatches = u;
	printDebug("LoadTR5", "_num_anim_dispatches = %u", _num_anim_dispatches);
	_anim_dispatches = new tr2_anim_dispatch_t[_num_anim_dispatches];
	Fread(_anim_dispatches, 8, _num_anim_dispatches, f);

	Fread(&u, 4, 1, f);
	_num_anim_commands = u;
	printDebug("LoadTR5", "_num_anim_commands = %u", _num_anim_commands);
	_anim_commands = new tr2_anim_command_t[_num_anim_commands];
	Fread(_anim_commands, 2, _num_anim_commands, f);

	Fread(&u, 4, 1, f);
	_num_mesh_trees = u;
	printDebug("LoadTR5", "_num_mesh_trees = %u", _num_mesh_trees);
	_mesh_trees = new tr2_meshtree_t[_num_mesh_trees];
	Fread(_mesh_trees, 4, _num_mesh_trees, f);

	Fread(&u, 4, 1, f);
	_num_frames = u;
	printDebug("LoadTR5", "_num_frames = %u", _num_frames);
	_frames = new u_int16_t[_num_frames];
	Fread(_frames, 2, _num_frames, f);

	Fread(&numMoveablesTR5, 4, 1, f);
	printDebug("LoadTR5", "numMoveablesTR5 = %u", numMoveablesTR5);
	moveablesTR5 = new tr5_moveable_t[numMoveablesTR5];
	Fread(moveablesTR5, 20, numMoveablesTR5, f);

	Fread(&u, 4, 1, f);
	_num_static_meshes = u;
	printDebug("LoadTR5", "_num_static_meshes = %u", _num_static_meshes);
	_static_meshes = new tr2_staticmesh_t[_num_static_meshes];
	Fread(_static_meshes, 32, _num_static_meshes, f);

	Fread(check, 4, 1, f);
	printDebug("LoadTR5", "Check: SPR = '%c%c%c'?",
				  check[0], check[1], check[2]);

	if (check[0] != 'S' || check[1] != 'P' || check[2] != 'R')
	{
		print("LoadTR5", "Error: SPR != '%c%c%c'", check[0], check[1], check[2]);
		return -4;
	}

	Fread(&u, 4, 1, f);
	_num_sprite_textures = u;
	printDebug("LoadTR5", "_num_sprite_textures = %u", _num_sprite_textures);
	_sprite_textures = new tr2_sprite_texture_t[_num_sprite_textures];
	Fread(_sprite_textures, 16, _num_sprite_textures, f);

	Fread(&u, 4, 1, f);
	_num_sprite_sequences = u;
	printDebug("LoadTR5", "_num_sprite_sequences = %u", _num_sprite_sequences);
	_sprite_sequences = new tr2_sprite_sequence_t[_num_sprite_sequences];
	Fread(_sprite_sequences, 8, _num_sprite_sequences, f);

	Fread(&u, 4, 1, f);
	_num_cameras = u;
	printDebug("LoadTR5", "_num_cameras = %u", _num_cameras);

	if (_num_cameras > 0)
	{
		_cameras = new tr2_camera_t[_num_cameras];
		Fread(_cameras, 16, _num_cameras, f);
	}
	else
	{
		_cameras = 0x0;
	}

	Fread(&numFlyByCamerasTR5, 4, 1, f);
	printDebug("LoadTR5", "numFlyByCameras = %u", numFlyByCamerasTR5);

	if (numFlyByCamerasTR5 > 0)
	{
		flyByCamerasTR5 = new tr5_flyby_camera_t[numFlyByCamerasTR5];
		Fread(flyByCamerasTR5, 40, numFlyByCamerasTR5, f);
	}
	else
	{
		flyByCamerasTR5 = 0x0;
	}

	Fread(&u, 4, 1, f);
	_num_sound_sources = u;
	printDebug("LoadTR5", "_num_sound_sources = %u", _num_sound_sources);

	if (_num_sound_sources > 0)
	{
		_sound_sources = new tr2_sound_source_t[_num_sound_sources];
		Fread(_sound_sources, 16, _num_sound_sources, f);
	}
	else
	{
		_sound_sources = 0x0;
	}

	Fread(&u, 4, 1, f);
	_num_boxes = u;
	printDebug("LoadTR5", "_num_boxes = %u", _num_boxes);
	_boxes = new tr2_box_t[_num_boxes];
	Fread(_boxes, 8, _num_boxes, f);

	Fread(&u, 4, 1, f);
	_num_overlaps = u;
	printDebug("LoadTR5", "_num_overlaps = %u", _num_overlaps);
	_overlaps = new short[_num_overlaps];
	Fread(_overlaps, 2, _num_overlaps, f);

	_zones = new short[_num_boxes*10];
	Fread(_zones, 20, _num_boxes, f);

	Fread(&u, 4, 1, f);
	_num_animated_textures = u;
	printDebug("LoadTR5", "_num_animated_textures = %u", _num_animated_textures);
	_animated_textures = new short[_num_animated_textures];
	Fread(_animated_textures, 2, _num_animated_textures, f);

	Fread(check, 1, 5, f);
	printDebug("LoadTR5", "Check: TEX = '%c%c%c'?",
				  check[1], check[2], check[3]);

	// check[0] is '^D'
	if (check[1] != 'T' || check[2] != 'E' || check[3] != 'X')
	{
		print("LoadTR5", "Error: TEX != '%c%c%c' @ %lu",
				check[1], check[2], check[3], ftell(f));
		return -4;
	}

	Fread(&numObjectTexturesTR5, 4, 1, f);
	printDebug("LoadTR5", "numObjectTextures = %u", numObjectTexturesTR5);
	objectTexturesTR5 = new tr5_object_texture_t[numObjectTexturesTR5];
	Fread(objectTexturesTR5, 40, numObjectTexturesTR5, f);

	Fread(&u, 4, 1, f);
	_num_items = u;
	printDebug("LoadTR5", "_num_items = %u", _num_items);
	_items = new tr2_item_t[_num_items];
	Fread(_items, 24, _num_items, f);

	Fread(&numCinematicFramesTR5, 4, 1, f);
	printDebug("LoadTR5", "numCinematicFrames = %u", numCinematicFramesTR5);

	if (numCinematicFramesTR5 > 0)
	{
		cinematicFramesTR5 = new tr5_cinematic_frame_t[numCinematicFramesTR5];
		Fread(cinematicFramesTR5, 24, numCinematicFramesTR5, f);
	}
	else
	{
		cinematicFramesTR5 = 0x0;
	}

	Fread(&us, 2, 1, f);
	_num_demo_data = us; // Could overflow?  not sure
	printDebug("LoadTR5", "_num_demo_data = %u", _num_demo_data);

	if (_num_demo_data > 0)
	{
		_demo_data = new unsigned char[_num_demo_data];
		Fread(_demo_data, 1, _num_demo_data, f);
	}
	else
	{
		_demo_data = 0x0;
	}

	printDebug("LoadTR5", "Reading soundMap");
	mSoundMap = new short[450];
	Fread(mSoundMap, 900, 1, f);

	Fread(&u, 4, 1, f);
	mNumSoundDetails = u;
	printDebug("LoadTR5", "numSoundDetails = %u", mNumSoundDetails);
	mSoundDetails = new tr2_sound_details_t[mNumSoundDetails];
	Fread(mSoundDetails, 8, mNumSoundDetails, f);

	Fread(&u, 4, 1, f);
	mNumSampleIndices = u;
	printDebug("LoadTR5", "numSampleIndices = %u", mNumSampleIndices);
	mSampleIndicesTR5 = new unsigned int[mNumSampleIndices];
	Fread(mSampleIndicesTR5, 4, mNumSampleIndices, f);

	Fread(&u, 4, 1, f);
	printDebug("LoadTR5", "Check 0xCDCDCDCD = 0x%X?", u);

	if (u != 0xcdcdcdcd)
	{
		print("LoadTR5", "Check 0xCDCDCDCD != 0x%X @ %u", u, ftell(f));
		return -5;
	}

	// Skip over the extra short in the demo.trc, but if it's not there
	// seek back
	u = ftell(f);
	Fread(&us, 2, 1, f);

	if (us != 0xcdcd)
	{
		fseek(f, u, SEEK_SET);
	}

	// FIXME: (Endian) Read bitu32 / u_int32_t
	Fread(&mNumTR4Samples, 4, 1, f);
	printDebug("Load", "mNumTR4Samples = %i", mNumTR4Samples);

	mRiffDataSz = 0;
	mTR4Samples = new unsigned char *[mNumTR4Samples];
	mTR4SamplesSz = new unsigned int[mNumTR4Samples];

	memset(mTR4SamplesSz, 0, mNumTR4Samples*4);

	for (i = 0; i < (int)mNumTR4Samples; ++i)
	{
		unsigned int sizeCompressed;
		unsigned int sizeUncompressed;
		unsigned char *compressedSoundSample;
		unsigned char *unCompressedSoundSample;
		int zErr;
		uLongf libzUncompressedSize;


		Fread(&sizeUncompressed, 4, 1, f);
		printDebug("Load", " sizeUncompressed = %i", sizeUncompressed);
		Fread(&sizeCompressed, 4, 1, f);
		printDebug("Load", " sizeCompressed = %i", sizeCompressed);

		compressedSoundSample = new unsigned char[sizeCompressed];
		unCompressedSoundSample = new unsigned char[sizeUncompressed];

		//printDebug("Load", "   %lubytes read from file", ftell(f));
		Fread(compressedSoundSample, sizeCompressed, 1, f);

		printDebug("Load", " %c%c%c%c should be RIFF",
					  compressedSoundSample[0],
					  compressedSoundSample[1],
					  compressedSoundSample[2],
					  compressedSoundSample[3]);

		// Decompress the sample
		libzUncompressedSize = sizeUncompressed;

		zErr = uncompress(unCompressedSoundSample,
								&libzUncompressedSize,
								compressedSoundSample,
								sizeCompressed);

		sizeUncompressed = libzUncompressedSize;

		switch (zErr)
		{
		case Z_MEM_ERROR:
			printDebug("Load", "   Decompress Error: not enough memory");
			break;
		case Z_BUF_ERROR:
			printDebug("Load", "   Decompress Error: output buffer too small");
			break;
		case Z_DATA_ERROR:
			printDebug("Load", "   Decompress Error: input data was corrupted");
			break;
		case Z_OK:
			printDebug("Load", "   Decompress OK");
			break;
		default:
			printDebug("Load", "   Decompress Error: decompress error #%i", zErr);
		}

		// Hhhmm... handle uncompressed RIFFs too?
		if (zErr == Z_OK)
		{
			mTR4Samples[i] = unCompressedSoundSample;
			mTR4SamplesSz[i] = sizeUncompressed;
			delete [] compressedSoundSample;
		}
		else
		{
			printDebug("Load", "   %lubytes read from file", ftell(f));
			mTR4Samples[i] = compressedSoundSample;
			mTR4SamplesSz[i] = sizeCompressed;
			delete [] unCompressedSoundSample;
		}
	}

	fclose(f);

	return 0;
}


void TombRaider::print(const char *methodName, const char *s, ...)
{
    va_list args;

    va_start(args, s);
    fprintf(stderr, "TombRaider::%s> ", methodName);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
}


void TombRaider::printDebug(const char *methodName, const char *s, ...)
{
  va_list args;

  if (!mDebug)
    return;

  va_start(args, s);
  fprintf(stdout, "TombRaider::%s> ", methodName);
  vfprintf(stdout, s, args);
  fprintf(stdout, "\n");
  va_end(args);
}



////////////////////////////////////////////////////////////
// Private Mutators
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
// Unit Test code
////////////////////////////////////////////////////////////
#ifdef __TOMBRAIDER_TEST__

void dump_textures(TombRaider *tr, char *mapname)
{
#ifdef __TEST_TR5_DUMP_TGA
	int i;
	unsigned char *image;
	unsigned char *bumpmap;
	char buffer[128];
	FILE *f;


	if (!tr || !mapname)
		return;

	// Dump textures
	printf("\n\t[Texture dumping for '%s']\n", mapname);

	for (i = 0; i < tr->NumTextures(); i++)
   {
		tr->Texture(i, &image, &bumpmap);

		if (image)
		{
			snprintf(buffer, 128, "%s-%03i-texture.tga", mapname, i);

			f = fopen(buffer, "wb");

			if (f)
			{
				if (!mtk_image__tga_save(f, image, 256, 256, 4))
					printf("\tWrote texture %s\n", buffer);

				fclose(f);
			}

			snprintf(buffer, 128, "%s.lst", mapname);

			f = fopen(buffer, "a");

			if (f)
			{
				fprintf(f, "%s-%03i-texture.tga;\n", mapname, i);
				fclose(f);
			}

			delete [] image;
		}

		if (bumpmap)
		{
			snprintf(buffer, 64, "%s-%03i-bumpmap.tga", mapname, i);

			f = fopen(buffer, "wb");

			if (f)
			{
				if (!mtk_image__tga_save(f, bumpmap, 256, 256, 4))
					printf("\tWrote texture %s\n", buffer);

				fclose(f);
			}

			delete [] bumpmap;
		}
	}

	for (i = 0; i < tr->NumSpecialTextures(); i++)
   {
		image = tr->SpecialTexTile(i);

		snprintf(buffer, 128, "%s-%03i-special.tga", mapname, i);

		f = fopen(buffer, "wb");

		if (f)
		{
			if (!mtk_image__tga_save(f, image, 256, 256, 4))
				printf("\tWrote texture %s\n", buffer);

			fclose(f);
		}
		else
		{
			printf("\tFailed to write texture %s\n", buffer);
		}

		delete [] image;
	}
#else
	printf("Texture dumping not in this build\n");
#endif
}


void dump_mesh(TombRaider *tr, char *mapname, int index)
{
	tr2_object_texture_t *object_texture = NULL;
	tr2_mesh_t *meshes = NULL;
	unsigned int v, check;
   int i, triangles, rectangles, t_index;
	char buffer[128];
	float rgba[4];
	float s, t;
	char id[8];
	FILE *f;


	if (!mapname || !tr)
	{
		return;
	}

	snprintf(buffer, 128, "%s-%03i.mesh", mapname, index);

	object_texture = tr->ObjectTextures();
	meshes = tr->Mesh();


	f = fopen(buffer, "wb");

	if (!f)
	{
		perror("Failed to write mesh :");
		return;
	}

	// Setup header id and check points
	strncpy(id, "TRMESH", 7);
	id[7] = 0;
	check = 0xcdcdcdcd;


	fwrite(id, 8, 1, f);
	fwrite(&meshes[index].num_vertices, 2, 1, f);
	fwrite(&meshes[index].num_textured_triangles, 2, 1, f);
	fwrite(&meshes[index].num_textured_rectangles, 2, 1, f);
	fwrite(&meshes[index].num_coloured_triangles, 2, 1, f);
	fwrite(&meshes[index].num_coloured_rectangles, 2, 1, f);
	fwrite(&meshes[index].collision_size, 4, 1, f);


   // Textured triangles ////////////////////////
	fwrite(&check, 4, 1, f);
	triangles = meshes[index].num_textured_triangles;

	for (i = 0; triangles > 0 && i < triangles; i++)
   {
		t_index = meshes[index].textured_triangles[i].texture;

		// Store texture info
		fwrite(&object_texture[t_index].tile, 2, 1, f);
		fwrite(&object_texture[t_index].transparency_flags, 2, 1, f);

		// Store vertices
		v = meshes[index].textured_triangles[i].vertices[0];
		fwrite(&meshes[index].vertices[v].x, 2, 1, f);
		fwrite(&meshes[index].vertices[v].y, 2, 1, f);
		fwrite(&meshes[index].vertices[v].z, 2, 1, f);

		v = meshes[index].textured_triangles[i].vertices[1];
		fwrite(&meshes[index].vertices[v].x, 2, 1, f);
		fwrite(&meshes[index].vertices[v].y, 2, 1, f);
		fwrite(&meshes[index].vertices[v].z, 2, 1, f);

		v = meshes[index].textured_triangles[i].vertices[2];
		fwrite(&meshes[index].vertices[v].x, 2, 1, f);
		fwrite(&meshes[index].vertices[v].y, 2, 1, f);
		fwrite(&meshes[index].vertices[v].z, 2, 1, f);

		// Store texels
		s = tr->adjustTexel(object_texture[t_index].vertices[0].xpixel,
								  object_texture[t_index].vertices[0].xcoordinate);
		t = tr->adjustTexel(object_texture[t_index].vertices[0].ypixel,
								  object_texture[t_index].vertices[0].ycoordinate);
		fwrite(&s, 4, 1, f);
		fwrite(&t, 4, 1, f);


		s = tr->adjustTexel(object_texture[t_index].vertices[1].xpixel,
								  object_texture[t_index].vertices[1].xcoordinate);
		t = tr->adjustTexel(object_texture[t_index].vertices[1].ypixel,
								  object_texture[t_index].vertices[1].ycoordinate);
		fwrite(&s, 4, 1, f);
		fwrite(&t, 4, 1, f);

		s = tr->adjustTexel(object_texture[t_index].vertices[2].xpixel,
								  object_texture[t_index].vertices[2].xcoordinate);
		t = tr->adjustTexel(object_texture[t_index].vertices[2].ypixel,
								  object_texture[t_index].vertices[2].ycoordinate);
		fwrite(&s, 4, 1, f);
		fwrite(&t, 4, 1, f);
	}

	fwrite(&check, 4, 1, f);


   // Textured rectangles ////////////////////////
	fwrite(&check, 4, 1, f);
	rectangles = meshes[index].num_textured_rectangles;

	for (i = 0; rectangles > 0 && i < rectangles; i++)
   {
		t_index = meshes[index].textured_rectangles[i].texture;

		// Store texture info
		fwrite(&object_texture[t_index].tile, 2, 1, f);
		fwrite(&object_texture[t_index].transparency_flags, 2, 1, f);

		// Store vertices
		v = meshes[index].textured_rectangles[i].vertices[0];
		fwrite(&meshes[index].vertices[v].x, 2, 1, f);
		fwrite(&meshes[index].vertices[v].y, 2, 1, f);
		fwrite(&meshes[index].vertices[v].z, 2, 1, f);

		v = meshes[index].textured_rectangles[i].vertices[1];
		fwrite(&meshes[index].vertices[v].x, 2, 1, f);
		fwrite(&meshes[index].vertices[v].y, 2, 1, f);
		fwrite(&meshes[index].vertices[v].z, 2, 1, f);

		v = meshes[index].textured_rectangles[i].vertices[2];
		fwrite(&meshes[index].vertices[v].x, 2, 1, f);
		fwrite(&meshes[index].vertices[v].y, 2, 1, f);
		fwrite(&meshes[index].vertices[v].z, 2, 1, f);

		v = meshes[index].textured_rectangles[i].vertices[3];
		fwrite(&meshes[index].vertices[v].x, 2, 1, f);
		fwrite(&meshes[index].vertices[v].y, 2, 1, f);
		fwrite(&meshes[index].vertices[v].z, 2, 1, f);

		// Store texels
		s = tr->adjustTexel(object_texture[t_index].vertices[0].xpixel,
								  object_texture[t_index].vertices[0].xcoordinate);
		t = tr->adjustTexel(object_texture[t_index].vertices[0].ypixel,
								  object_texture[t_index].vertices[0].ycoordinate);
		fwrite(&s, 4, 1, f);
		fwrite(&t, 4, 1, f);


		s = tr->adjustTexel(object_texture[t_index].vertices[1].xpixel,
								  object_texture[t_index].vertices[1].xcoordinate);
		t = tr->adjustTexel(object_texture[t_index].vertices[1].ypixel,
								  object_texture[t_index].vertices[1].ycoordinate);
		fwrite(&s, 4, 1, f);
		fwrite(&t, 4, 1, f);

		s = tr->adjustTexel(object_texture[t_index].vertices[2].xpixel,
								  object_texture[t_index].vertices[2].xcoordinate);
		t = tr->adjustTexel(object_texture[t_index].vertices[2].ypixel,
								  object_texture[t_index].vertices[2].ycoordinate);
		fwrite(&s, 4, 1, f);
		fwrite(&t, 4, 1, f);

		s = tr->adjustTexel(object_texture[t_index].vertices[3].xpixel,
								  object_texture[t_index].vertices[3].xcoordinate);
		t = tr->adjustTexel(object_texture[t_index].vertices[3].ypixel,
								  object_texture[t_index].vertices[3].ycoordinate);
		fwrite(&s, 4, 1, f);
		fwrite(&t, 4, 1, f);
	}

	fwrite(&check, 4, 1, f);

   // Coloured triangles ////////////////////////
	fwrite(&check, 4, 1, f);
	triangles = meshes[index].num_coloured_triangles;

	for (i = 0; triangles > 0 && i < triangles; i++)
   {
		// Store vertices
		v = meshes[index].coloured_triangles[i].vertices[0];
		fwrite(&meshes[index].vertices[v].x, 2, 1, f);
		fwrite(&meshes[index].vertices[v].y, 2, 1, f);
		fwrite(&meshes[index].vertices[v].z, 2, 1, f);

		v = meshes[index].coloured_triangles[i].vertices[1];
		fwrite(&meshes[index].vertices[v].x, 2, 1, f);
		fwrite(&meshes[index].vertices[v].y, 2, 1, f);
		fwrite(&meshes[index].vertices[v].z, 2, 1, f);

		v = meshes[index].coloured_triangles[i].vertices[2];
		fwrite(&meshes[index].vertices[v].x, 2, 1, f);
		fwrite(&meshes[index].vertices[v].y, 2, 1, f);
		fwrite(&meshes[index].vertices[v].z, 2, 1, f);

		// Store color
		switch (tr->Engine())
		{
		case TR_VERSION_1:
			tr->getColor(meshes[index].coloured_triangles[i].texture
								 & 0xff, rgba);
			break;
		default:
			tr->getColor((meshes[index].coloured_triangles[i].texture>>8)
								 & 0xff, rgba);
		}

		s = rgba[0];
		t = rgba[1];
		fwrite(&s, 4, 1, f);
		fwrite(&t, 4, 1, f);

		s = rgba[2];
		t = rgba[3];
		fwrite(&s, 4, 1, f);
		fwrite(&t, 4, 1, f);
	}

	fwrite(&check, 4, 1, f);

   // Coloured rectangles ////////////////////////
	fwrite(&check, 4, 1, f);
	rectangles = meshes[index].num_coloured_rectangles;

	for (i = 0; rectangles > 0 && i < rectangles; i++)
   {
		// Store vertices
		v = meshes[index].coloured_rectangles[i].vertices[0];
		fwrite(&meshes[index].vertices[v].x, 2, 1, f);
		fwrite(&meshes[index].vertices[v].y, 2, 1, f);
		fwrite(&meshes[index].vertices[v].z, 2, 1, f);

		v = meshes[index].coloured_rectangles[i].vertices[1];
		fwrite(&meshes[index].vertices[v].x, 2, 1, f);
		fwrite(&meshes[index].vertices[v].y, 2, 1, f);
		fwrite(&meshes[index].vertices[v].z, 2, 1, f);

		v = meshes[index].coloured_rectangles[i].vertices[2];
		fwrite(&meshes[index].vertices[v].x, 2, 1, f);
		fwrite(&meshes[index].vertices[v].y, 2, 1, f);
		fwrite(&meshes[index].vertices[v].z, 2, 1, f);

		v = meshes[index].coloured_rectangles[i].vertices[3];
		fwrite(&meshes[index].vertices[v].x, 2, 1, f);
		fwrite(&meshes[index].vertices[v].y, 2, 1, f);
		fwrite(&meshes[index].vertices[v].z, 2, 1, f);

		// Store color
		switch (tr->Engine())
		{
		case TR_VERSION_1:
			tr->getColor(meshes[index].coloured_rectangles[i].texture
								 & 0xff, rgba);
			break;
		default:
			tr->getColor((meshes[index].coloured_rectangles[i].texture>>8)
								 & 0xff, rgba);
		}

		s = rgba[0];
		t = rgba[1];
		fwrite(&s, 4, 1, f);
		fwrite(&t, 4, 1, f);

		s = rgba[2];
		t = rgba[3];
		fwrite(&s, 4, 1, f);
		fwrite(&t, 4, 1, f);
	}

	fwrite(&check, 4, 1, f);
	fclose(f);

	printf(".");
	fflush(stdout);
}


void dump_lara_stuff(char *mapname, TombRaider &tr)
{
	unsigned int i, j, k, n;
	tr2_moveable_t *moveable = tr.Moveable();
	unsigned int numMoveables = tr.NumMoveables();
	char filename[64];
	unsigned char *riff;
	unsigned int riffSz, total = 0;
	FILE *f;


	snprintf(filename, 63, "%s-lara.nfo", mapname);
	f = fopen(filename, "w");

	if (!f)
	{
		perror("Failed to write lara.nfo: ");
		return;
	}

	for (i = 0; i < numMoveables; ++i)
	{
		j = n = 0;

		if (moveable[i].object_id == 0)
		{
			j = moveable[i].starting_mesh;
			n = moveable[i].num_meshes + j;

			fprintf(f, "Lara (%i)\n", i);
		}
		else if (moveable[i].object_id == 30)
		{
			j = moveable[i].starting_mesh;
			n = moveable[i].num_meshes + j;

			fprintf(f, "Lara ponytail\n");
		}
		else if (tr.Engine() == TR_VERSION_4)
		{
			switch (moveable[i].object_id)
			{
			case 8: // Joints, ( interconnecting skin/ploys )
			case 9:
				fprintf(f, "TR4 lara joints (%i)\n", i);
				j = moveable[i].starting_mesh;
				n = moveable[i].num_meshes + j;
			}
		}

		for (k = j; j < n; ++j)
		{
			fprintf(f, "\tMesh[%i] = %i\n", (j - k), j);
		}
	}

	fclose(f);

	printf("\nDumping %i audio samples: ", tr.getSoundSamplesCount());

	for (i = 0, j = 0; i < tr.getSoundSamplesCount(); ++i)
	{
		tr.getSoundSample(i, &riffSz, &riff);

		total += riffSz;

		snprintf(filename, 63, "%s-%03i.wav", mapname, j++);
		f = fopen(filename, "wb");

		if (!f)
		{
			perror("Failed to write riff.wav: ");
			continue;
		}

		fwrite(riff, 1, riffSz, f);
		fclose(f);

		delete [] riff;

		printf(".");
		fflush(stdout);
	}

	printf("\n");

	if (total)
	{
		printf("Dumped %ibytes (%.2f MB) of audio samples\n",
				 total, (float)total/1024000.0);
	}
}


void percent_callback(int p)
{
	printf("Level %i%% loaded\n", p);
}


int test_main(int argc, char *argv[])
{
  TombRaider tr;
  char mapname[128];
  int len, i, j;


  printf("[TombRaider class test]\n");

  tr.setDebug(true);

  if (argc > 2)
  {
	  // Strip for mapname //////////////////////////////
	  len = strlen(argv[2]);

	  for (i = len, j = 0; i > 0; i--, j++)
		  if (argv[2][i] == '/' || argv[2][i] == '\\')
			  break;

	  j--;

	  memset(mapname, 0, 128);

	  for (i = 0; i < len - j && i < 30; i++)
		  mapname[i] = argv[2][i + len - j];

	  ////////////////////////////////////////////////////


    if (strncmp(argv[1], "load", 4) == 0)
    {
      if (!tr.Load(argv[2], percent_callback))
		{
        printf("\nmain: Load reports success.\n");
		}
    }
    else if (strncmp(argv[1], "dump", 4) == 0)
    {
      if (!tr.Load(argv[2], percent_callback))
		{
        printf("\nmain: Load reports success.\n");

		  dump_textures(&tr, mapname);

		  printf("Mesh dumping: ");
		  for (i = 0; i < tr.getMeshCount(); i++)
		  {
			  dump_mesh(&tr, mapname, i);
		  }

		  if (argc > 3)
		  {
			  printf("\nLoading external sound SFX.\n");
			  tr.loadSFX(argv[3]);
		  }

		  dump_lara_stuff(mapname, tr);

		  printf("\n");
		}
		else
		{
			printf("\nmain: Load failed.\n");
		}
    }
    else
      printf("\n\n%s [ load | dump ] filename [sound.sfx]\n", argv[0]);
  }
  else
  {
    printf("\n\n%s [ load | dump ] filename [sound.sfx]\n", argv[0]);
  }

  return 0;
}


int main(int argc, char *argv[])
{
	test_main(argc, argv);

#ifdef DEBUG_MEMORY
  dump_memory_report();
#endif

  return 0;
}
#endif
