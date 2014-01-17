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

#ifdef __TEST_TR5_DUMP_TGA
#include <tga.h>
#endif

#ifdef DEBUG_MEMORY
#include <memory_test.h>
#endif


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

