/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 * 
 * Project : OpenRaider, PSKModel viewer, libhel_modelloader
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : UTPackage
 * License : No use w/o permission (C) 2003 Mongoose
 * Comments: This class handles UT packages
 *
 *
 *           This file was generated using Mongoose's C++ 
 *           template generator script.  <stu7440@westga.edu>
 * 
 *-- History ------------------------------------------------- 
 *
 * 2003.07.11:
 * Mongoose - Created, form old PSKModel prototype
 =================================================================*/

#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "mtk_tga.h"

#include "UTPackage.h"


//#define USE_PROPERTY_STRUCT


char *get_utx_name(unsigned int version, FILE *f)
{
	unsigned int i, l;
	unsigned char sl;
	char buffer[256];
	char *s;


	if (version < 64)
	{
		for (i = 0; i < 255; ++i)
		{
			fread(buffer+i, 1, 1, f);
			buffer[i+1] = 0;

			if (buffer[i] == '\0')
			{
				break;
			}
		}

		l = i;
		s = new char[l];
		strcpy(s, buffer);
	}
	else
	{
		fread(&sl, 1, 1, f);

		s = new char[sl];
		fread(s, sl, 1, f);
	}

	return s;
}


int get_utx_array_index(FILE *f)
{
	unsigned char u8;
	short u16;
	int index, u32;


	
	fread(&u8, 1, 1, f);

	if (u8 < 128)
	{
		index = (unsigned int)u8;
	}
	else 
	{
		fseek(f, -1, SEEK_SET);
		fread(&u16, 2, 1, f);
		
		if (u16 < 16384 )
		{
			index = (unsigned int)u16;
		}
		else
		{
			fseek(f, -2, SEEK_SET);
			fread(&u32, 4, 1, f);
			index = u32 & 0x3fffffff;
		}
	}
		
	return index;
}


int get_utx_index(FILE *f)
{
	unsigned int index, data;
	int vindex;
	unsigned short shift = 6;
	unsigned char b;
	bool sign;

	fread(&b, 1, 1, f);
	sign = (b & INDEX_SIGN_BIT) != 0;
	index = b & INDEX_BYT1_MASK;

	if (b & INDEX_CONT1_BIT)
	{
		do  {
			fread(&b, 1, 1, f);
			data = b & INDEX_BYTE_MASK;
			data = ((unsigned int)data) << shift;
			index = index | data;
			shift = (unsigned short)(shift + 7);

		} while ((b & INDEX_CONT_BIT) && (shift < 32));
	}

	vindex = index;
	
	return ((sign) ? -vindex : vindex);
}


unsigned int use_utx_index(int index, unsigned int *type)
{
	if (index == 0)
	{
		*type = UTPackage::UT_NULL;
		return 0;
	}
	else if (index < 0)
	{
		*type = UTPackage::UT_IMPORT; // import entry
		//		return ((-index)-1);
		return (-index - 1);
	}

	*type = UTPackage::UT_EXPORT; // export entry
	return (index-1);
}

/* Hexdump util */
void hex_dump(FILE *f, unsigned char *buffer, unsigned int size);
#define hex_dump_file(f, size) hex_dump(f, 0x0, size)
#define hex_dump_buffer(buffer, size) hex_dump(0x0, buffer, size)
void hex_dump(FILE *f, unsigned char *buffer, unsigned int size)
{
	unsigned int i, j;
	unsigned char u;
	char s[16];
	char none = 183;

	memset(s, 0, 16);

	/* Dump it out all pretty */
	if (f)
	{
		printf("[Hexdumping, Offset 0x%08lx to 0x%08lx, %u bytes]\n", 
				 ftell(f), ftell(f)+size, size);
	}
	else if (buffer)
	{
		printf("[Hexdumping buffer, %u bytes]\n", size);
	}
	else
	{
		printf("Nothing to hexdump!\n");
		return;
	}

	for (i = 0, j = 0; i < size; ++i, ++j)
	{
		if (f) // Dump from file
		{
			fread(&u, 1, 1, f); 
		}
		else  // Dump from buffer
		{
			u = buffer[i];
		}

		s[j] = u;

		/* Print ASCII */
		if (j == 16)
		{
			printf("     ");

			for (j = 0; j < 16; ++j)
			{
				printf("%s",
							((j == 0) ? "" :
							 (j%8 == 0) ? "  ": 
							 (j%4 == 0) ? " " : ""));

				if (s[j] > 32 && s[j] < 127)
					printf("%c", s[j]);
				else
					printf("%c", none);
			}

			j = 0;
			memset(s, 0, 16);
		}

		/* Print Hex */
		printf("%s%02X", 
				 ((i == 0) ? "" :
				  ((i%16 == 0) ? "\n" : 
					((i%8 == 0) ? "   ": 
					 (i%4 == 0) ? "  " : " "))), 
				 u);
	}

	if (j < 16 && j != 0)
	{
		i = 16 - j;

		for (j = 0; j < i; ++j)
		{
			printf("%s  ", 
					 ((j == 0) ? " " :
					  ((j%16 == 0) ? "\n" : 
						((j%8 == 0) ? "   ": 
						 (j%4 == 0) ? "  " : " "))));
		}

		printf("     ");
		
		for (j = 0; j < 16; ++j)
		{
			printf("%s",
					 ((j == 0) ? "" :
					  (j%8 == 0) ? "  ": 
					  (j%4 == 0) ? " " : ""));

			if (s[j] > 32 && s[j] < 127)
				printf("%c", s[j]);
			else
				printf("%c", none);
		}
	}

	printf("\n");
}


int dump_file(FILE *f, unsigned int size, char *s, ...)
{
	char buffer[1024];
	FILE *f2;
	va_list args;

	
	if (!f || size <= 0)
		return -1;

	va_start(args, s);
	vsnprintf(buffer, 1023, s, args);
	va_end(args);

	f2 = fopen(buffer, "wb");

	if (!f2)
	{
		perror(buffer);
		return -2;
	}

	while (size > 1024)
	{
		fread(buffer, 1, 1024, f);
		fwrite(buffer, 1, 1024, f2);

		if (size - 1024 < 0)
			break;

		size -= 1024;
	}

	fread(buffer, 1, size, f);
	fwrite(buffer, 1, size, f2);

	fclose(f2);
	return 0;
}


//////////////////////////////////////////////////////////////////////
// UTPackageProperty Class
//////////////////////////////////////////////////////////////////////

UTPackageProperty::UTPackageProperty()
{
	index = 0;
	nameIndex = 0;
	size = 0;
	type = 0;
	flags = 0;

	data = 0x0;
}


UTPackageProperty::~UTPackageProperty()
{
	clear();
}


void UTPackageProperty::clear()
{
	index = 0;
	nameIndex = 0;
	size = 0;
	type = 0;
	flags = 0;

	if (data)
		delete [] data;
}


int UTPackageProperty::load(utx_header_t *header, FILE *f)
{
	bool isArray;
	unsigned char info;//, u8;
	unsigned short u16;
	unsigned int u32;
	int idx;


	/* Property */
	index = get_utx_index(f);
	nameIndex = ((type == UTPackage::UT_NULL) ? 0 :
					 (type == UTPackage::UT_IMPORT) ? 
					 header->importTable[index].objName :
					 header->exportTable[index].objName);

	if (index >= 0)
		nameIndex = index;

	if (nameIndex > header->nameCount)
	{
		printf("index = %i nameIndex = %i ERROR Out of bounds\n",
				 index, nameIndex);
		return -2;
	}

	printf("\n{ index = %i, '%s'\n", nameIndex, 
			 ((nameIndex >= 0) ? header->nameTable[nameIndex].objName : 
			  "Bad Index"));

	if (nameIndex < 0 ||
		 strcmp("None", header->nameTable[nameIndex].objName) == 0)
	{
		return -1;
	}

	/* Read info byte */
	fread(&info, 1, 1, f);
	
	isArray = (info & PROPERTY_ARRAY_MASK) != 0;
	type = (info & PROPERTY_TYPE_MASK);
	
	switch (info & PROPERTY_SIZE_MASK)
	{
	case 0x00:
		size = 1;
		break;
	case 0x10:
		size = 2;
		break;
	case 0x20:
		size = 4;
		break;
	case 0x30:  
		size = 12;
		break;
	case 0x40:  
		size = 16;
		break;
	case 0x50:  
		fread(&info, 1, 1, f);
		size = info;
		break;
	case 0x60:
		fread(&u16, 2, 1, f);
		size = u16;
		break;
	case 0x70:
		fread(&size, 4, 1, f);
		break;
	}
	
	printf(" size %u, type 0x%x, flags 0x%x\n", 
			 size, type, flags);

	idx = 1;

	switch (type)
	{
	case UNP_ByteProperty:
		if (isArray)
		{
			idx = get_utx_array_index(f);
		}
		
		data = new unsigned char[1*idx];
		fread(data, 1, idx, f);
		printf(" %i byte 0x%02x", idx, data[0]);
		break;
	case UNP_IntProperty:
		if (isArray)
		{
			idx = get_utx_array_index(f);
		}
		
		data = new unsigned char[4*idx];
		fread(data, 4, idx, f);
		printf(" %i int  %i", idx, *((int *)data));
		break;
	case UNP_BoolProperty:
		printf(" bool 0x%x\n", isArray);
		data = new unsigned char[1];
		data[0] = isArray;
		break;
	case UNP_FloatProperty:
		if (isArray)
		{
			idx = get_utx_array_index(f);
		}

		fread(&u32, 4, 1, f);
		printf(" float %f", *((float *)&u32));
		break;
	case UNP_ObjectProperty:
	case UNP_NameProperty: //  SetObjectProperty( aUf );  
		if (isArray)
		{
			idx = get_utx_array_index(f);
		}

		if (isArray)
		{
			idx = get_utx_index(f);
		}
		
		hex_dump_buffer((unsigned char *)&idx, 4);     
		break;
	case UNP_StrProperty:
		printf(" String:\n"); 
		hex_dump_file(f, size);     
		break;
	case UNP_StringProperty:
		printf(" ASCII String:\n");   
		hex_dump_file(f, size);     
		break;
	case UNP_StructProperty:
		printf(" Struct:\n"); 

#ifdef USE_PROPERTY_STRUCT
		idx = get_utx_index(f);

		if (isArray)
		{
			get_utx_array_index(f);
		}

		if (idx > (int)header->nameCount || idx < 0)
		{
			printf(" idx = %i\n", idx);
			hex_dump_file(f, size);
		}
		else if (strcmp("Color", header->nameTable[idx].objName) == 0)
		{
			//fread(&u32, 4, 1, f);
			hex_dump_file(f, 4);  /* RGBA */
		}
		else if (strcmp("Vector", header->nameTable[idx].objName) == 0)
		{
			/* XYZ */
			fread(&u32, 4, 1, f);
			printf(" float %f", *((float *)&u32));
			fread(&u32, 4, 1, f);
			printf(" float %f", *((float *)&u32));
			fread(&u32, 4, 1, f);
			printf(" float %f", *((float *)&u32));
		}
		else if (strcmp("Rotator", header->nameTable[idx].objName) == 0)
		{
			/* Pitch, Roll, Yaw */
			fread(&u32, 4, 1, f);
			printf(" float %f", *((float *)&u32));
			fread(&u32, 4, 1, f);
			printf(" float %f", *((float *)&u32));
			fread(&u32, 4, 1, f);
			printf(" float %f", *((float *)&u32));
		}
		else if (strcmp("Scale", header->nameTable[idx].objName) == 0)
		{
			/* XYZ, sheer rate, sheer axis */
			fread(&u32, 4, 1, f);
			printf(" float %f", *((float *)&u32));
			fread(&u32, 4, 1, f);
			printf(" float %f", *((float *)&u32));
			fread(&u32, 4, 1, f);
			printf(" float %f", *((float *)&u32));
			fread(&u32, 4, 1, f);
			printf(" %i\n", u32);
			fread(&u8, 1, 1, f);
			printf(" %i\n", u8);
		}
		else if (strcmp("PointRegion", header->nameTable[idx].objName) == 0)
		{
			get_utx_index(f);
			fread(&u32, 4, 1, f);
			fread(&u8, 1, 1, f);
		}
		else
		{
			printf("UNKOWN Struct\n");
			hex_dump_file(f, size);
		}
#else
		hex_dump_file(f, size);
#endif
		break;
	case UNP_ArrayProperty:  //SetArrayProperty( aUf, aUn, aUi, aUe );
		printf(" ** Array Property!\n"); 
		hex_dump_file(f, size);
		break;
	case UNP_ClassProperty:
	case UNP_VectorProperty:
	case UNP_RotatorProperty:
	case UNP_FixedArrayProperty: //    SetUnknownProperty( aUn,  aUf )
		printf(" ** Unknown/Unsupported Property!\n");

		//data = new unsigned char[size];
		//fread(data, 1, size, f);
		hex_dump_file(f, size);
		break;
	case UNP_MapProperty:
		printf(" ** Map Property!\n");
		hex_dump_file(f, size);
		break;
	}

	printf(" }\n");

	return 0;	
}


int load_utx(char *filename, bool hexDump, bool diskDump)
{
	FILE *f;
	unsigned int i, index, type;
	utx_header_t header;


	f = fopen(filename, "rb");

	if (!f)
	{
		perror(filename);
		return -1;
	}

	fread(&header.signature, 4, 1, f);

	/* UT package signature 0x9E2a83c1 */
	if (header.signature != 0x9E2a83c1)
	{
		printf("Not a UT package ( 0x9E2a83c1 )\n");
		return -2;
	}

	fread(&header.version, 2, 1, f);
	fread(&header.version2, 2, 1, f);
	fread(&header.flags, 4, 1, f);
	fread(&header.nameCount, 4, 1, f);
	fread(&header.nameOffset, 4, 1, f);
	fread(&header.exportCount, 4, 1, f);
	fread(&header.exportOffset, 4, 1, f);
	fread(&header.importCount, 4, 1, f);
	fread(&header.importOffset, 4, 1, f);

	printf("header {\n");
	printf("signature = 0x%x\n", header.signature);
	printf("version = %u, version2 (mode) = %u\n", 
			 header.version, header.version2);
	printf("flags = 0x%x\n", header.flags);
	printf("nameCount = %u, nameOffset = 0x%x\n", 
			 header.nameCount, header.nameOffset);
	printf("exportCount = %u, exportOffset = 0x%x\n", 
			 header.exportCount, header.exportOffset);
	printf("importCount = %u, importOffset = 0x%x\n", 
			 header.importCount, header.importOffset);

	if (header.version < 68)
	{
		header.generationCount = 0;
		header.generations = 0x0;

		fread(&header.heritageCount, 4, 1, f);
		fread(&header.heritageOffset, 4, 1, f);

		printf("heritageCount = %u, heritageOffset = 0x%x\n", 
				 header.heritageCount, header.heritageOffset);
	}
	else
	{
		header.heritageCount = 0;
		header.heritageOffset = 0;

		fread(header.guid, 16, 1, f);
		fread(&header.generationCount, 4, 1, f);

		printf("guid = 0x%x%x%x%x\n",
				 header.guid[0], header.guid[1], header.guid[2], header.guid[3]);
		printf("generationCount = %u\n", 
				 header.generationCount);

		header.generations = new utx_generation_t[header.generationCount];
		
		for (i = 0; i < header.generationCount; ++i)
		{	
			fread(&header.generations[i].exportCount, 4, 1, f);
			fread(&header.generations[i].nameCount, 4, 1, f);
			
			printf("  generation[%u] { exportCount = %u, nameCount = %u }\n", i,
					 header.generations[i].exportCount, 
					 header.generations[i].nameCount);
		}
	}

	printf("}\n");

	/* 2003.06.14, Mongoose - These tables are not sequentally read,
		you have to skip around a lot */

	/* Read Name table */
	if ((int)header.nameOffset != ftell(f))
	{
		fseek(f, header.nameOffset, SEEK_SET);
	}

	header.nameTable = new utx_name_table_t[header.nameCount];

	for (i = 0; i < header.nameCount; ++i)
	{
		header.nameTable[i].objName = get_utx_name(header.version, f);
		fread(&header.nameTable[i].objFlags, 4, 1, f);

		printf("nameTable[%u] { objName = '%s', objFlags = 0x%x }\n", i,
				 header.nameTable[i].objName, header.nameTable[i].objFlags);
	}

	/* Read export table */
	if ((int)header.exportOffset != ftell(f))
	{
		fseek(f, header.exportOffset, SEEK_SET);
	}

	header.exportTable = new utx_export_table_t[header.exportCount];

	for (i = 0; i < header.exportCount; ++i)
	{
		header.exportTable[i].objClass = get_utx_index(f);
		header.exportTable[i].objSuper = get_utx_index(f);
		fread(&header.exportTable[i].package, 4, 1, f);
		header.exportTable[i].objName = get_utx_index(f);
		fread(&header.exportTable[i].objFlags, 4, 1, f);
		header.exportTable[i].serialSize = get_utx_index(f);
		header.exportTable[i].serialOffset = get_utx_index(f);

		printf("exportTable[%u] { class %i, super %i, package %u, name %i\n",i,
				 header.exportTable[i].objClass, header.exportTable[i].objSuper,
				 header.exportTable[i].package, header.exportTable[i].objName);
		printf("   flags %u, serialSz %i bytes, serialOff %i }\n",
				 header.exportTable[i].objFlags,
				 header.exportTable[i].serialSize, 
				 header.exportTable[i].serialOffset);
	}

	/* Read import table */
	if ((int)header.importOffset != ftell(f))
	{
		fseek(f, header.importOffset, SEEK_SET);
	}

	header.importTable = new utx_import_table_t[header.importCount];

	for (i = 0; i < header.importCount; ++i)
	{
		header.importTable[i].objClass = get_utx_index(f);
		header.importTable[i].objSuper = get_utx_index(f);
		fread(&header.importTable[i].package, 4, 1, f);
		header.importTable[i].objName = get_utx_index(f);

		printf("importTable[%u] { class %i, super %i, package %u, name %i\n",i,
				 header.importTable[i].objClass, header.importTable[i].objSuper,
				 header.importTable[i].package, header.importTable[i].objName);
	}

	/* Read Heritage table */
	if (header.heritageCount > 0)
	{
		if ((int)header.heritageOffset != ftell(f))
		{
			fseek(f, header.exportOffset, SEEK_SET);
		}

		header.heritageTable = new unsigned int[header.heritageCount * 4];

		for (i = 0; i < header.heritageCount; ++i)
		{
			fread(header.heritageTable+(i*16), 16, 1, f);

			printf("heritageTable[%u] = 0x%x%x%x%x\n", i,
					 header.heritageTable[i*16], header.heritageTable[i*16+1],
					 header.heritageTable[i*16+2], header.heritageTable[i*16+3]);
		}
	}


	/* Testing the export table here to find objects in the file */
	for (i = 0; i < header.exportCount; ++i)
	{
		unsigned int sz, off;
		int nameIndex;

		index = use_utx_index(header.exportTable[i].objClass, &type);
		nameIndex = ((type == UTPackage::UT_NULL) ? -1 :
						 (type == UTPackage::UT_IMPORT) ? 
						 header.importTable[index].objName :
						 header.exportTable[index].objName);

		printf("* Export Class '%s'\n", 
				 ((nameIndex < 0) ? "Null" :
				  header.nameTable[nameIndex].objName));

		index = use_utx_index(header.exportTable[i].objName, &type);

		printf("         Name '%s'\n", 
				 ((type == UTPackage::UT_NULL) ? "Null" :
				  header.nameTable[index].objName));

		sz = use_utx_index(header.exportTable[i].serialSize, &type);
		printf("         Size %i bytes\n", sz);

		off = use_utx_index(header.exportTable[i].serialOffset, &type);
		printf("         Offset 0x%x (%u bytes)\n", off, off);

		/* Skip to the object */
		fseek(f, off, SEEK_SET);

		/* HexDump to stdout */
		if (hexDump)
		{
			hex_dump_file(f, sz);
			fseek(f, off, SEEK_SET);
		}

		/* RAW dump */
		if (diskDump)
		{
			char buf[512];
			unsigned int k, j;
			sprintf(buf, "/tmp/utpak/%s/%s.raw", 
					  header.nameTable[nameIndex].objName,
					  header.nameTable[index].objName);
			FILE *f2 = fopen(buf, "wb");


			mkdir("/tmp/utpak", S_IRWXU | S_IRWXG);
			sprintf(buf, "/tmp/utpak/%s", 
					  header.nameTable[nameIndex].objName);
			mkdir(buf, S_IRWXU | S_IRWXG);

			printf("Writing /tmp/utpak/%s/%s.raw...\n", 
					  header.nameTable[nameIndex].objName,
					 header.nameTable[index].objName);

			if (f2)
			{
				for (k = 0, j = 0; k < sz; k += j)
				{
					if (sz - k >= 512)
					{
						j = 512;
					}
					else
					{
						j = sz - k;
					}
					
					fread(buf, j, 1, f);
					fwrite(buf, j, 1, f2);
				}

				fclose(f2);
			}

			fseek(f, off, SEEK_SET);
		}

		/* Exported dump to usable external formats */
		if (diskDump && nameIndex > 0 &&
			 strcmp("Texture", header.nameTable[nameIndex].objName) == 0)
		{
			int idx;
			unsigned char u8;
			//			unsigned short u16;
			unsigned int u32, i, count, off2, w, h, byteperpixel = 4;
			unsigned char *data;
			UTPackageProperty prop;


			printf("Dumping texture...\n");

			/* Property list */
			while ((idx = prop.load(&header, f)) == 0)
			{
				if (strcmp("Format", header.nameTable[prop.nameIndex].objName) == 0)
				{
					u8 = prop.data[0];

					switch (u8)
					{
					case 0x00: /* Indexed 8bit, uncompressed mipmaps */
						printf("Indexed 8bit, uncompressed mipmaps\n");
						byteperpixel = 1;
						break;
					case 0x04: /* BGRA 32bit, uncompressed mipmaps */
						printf("BGR 24bit, uncompressed mipmaps\n");
						byteperpixel = 3;
						break;
					case 0x05: /* BGRA 32bit, uncompressed mipmaps */
						printf("BGRA 32bit, uncompressed mipmaps\n");
						byteperpixel = 4;
						break;
					default:
						printf("Unknown mipmap type\n");
						byteperpixel = 0;
					}
				}

				//prop.clear();
			}

			if (idx == -2)
			{
				printf("FATAL ERROR: Invalid property returned for class!\n");
				continue;
			}

			/* Texture class */
#ifdef OBSOLETE
			if (header.version < 63)
			{
				fread(&u8, 1, 1, f);
				count = u8;
			}
			else if (header.version < 100)
			{
				fread(&u8, 1, 1, f);
				count = u8;
				fread(&u32, 4, 1, f);
				idx = get_utx_index(f);
			}
			else if (header.version < 120)
			{
				fread(&u8, 1, 1, f);
				count = u8;
				fread(&u32, 4, 1, f);
				fread(&u32, 4, 1, f);
				idx = get_utx_index(f);
			}
			else // ut2k3
			{
				fread(&u8, 1, 1, f);
				count = u8;
				fread(&u32, 4, 1, f);
				idx = get_utx_index(f);
			}
#else
			fread(&u8, 1, 1, f);
			count = u8;
#endif

			printf("Number of MipMaps: %i\n", count);
			
			for (i = 0; i < count; ++i)
			{
				printf("MipMap[%i]\n", i);

				if (header.version >= 63)
				{
					fread(&off2, 4, 1, f);
					printf("\tMipmap header offset = %i\n", off2);
				}

				u32 = get_utx_index(f);
				u32 = use_utx_index(u32, &type) + 1;
				printf("\tMipMap pixels = %u\n", u32);
				
				if (u32 > sz)
				{
					printf("\t\t*** ERROR: Size over threshold! ***\n");
					break;
				}
				
				// Dump 'u32' bytes texture data here
				//hex_dump_file(f, u32);
				//dump_file(f, u32*4, "/tmp/utpak-%s-%i.texture", 
				//			 header.nameTable[index].objName, i);

				if (byteperpixel > 0)
				{
					data = new unsigned char[u32*byteperpixel];
					fread(data, 1, u32*byteperpixel, f);
				}

				if (header.version >= 63)
				{	
					fseek(f, off2, SEEK_SET);
				}
				
				fread(&w, 4, 1, f);
				printf("\twidth = %u\n", w);
				fread(&h, 4, 1, f);
				printf("\theight = %u\n", h);

				fread(&u8, 1, 1, f);
				printf("\tbitswidth = %u \n", u8);
				fread(&u8, 1, 1, f);
				printf("\tbitsheight = %u\n", u8);
				
		
				if (byteperpixel > 0)
				{
					printf("\tWriting /tmp/utpak/Texture/%s-%i.tga...\n", 
							 header.nameTable[index].objName,
							 i);
					mtk_image__tga_save_filename(data, w, h, byteperpixel,
														  "/tmp/utpak/Texture/%s-%i.tga", 
														  header.nameTable[index].objName,
														  i);

					delete [] data;
				}
				
				printf("\n");
				printf("\n");
			}
		}
		else if (diskDump && nameIndex > 0 &&
			 strcmp("SkeletalMesh", header.nameTable[nameIndex].objName) == 0)
		{
			unsigned short u16;
			unsigned int numWedges, i; // u32
			int idx;
			float r;
			UTPackageProperty prop;

			printf("Dumping skeletal mesh...\n");

			/* Property list */
			while ((idx = prop.load(&header, f)) == 0)
			{

			}

			if (idx == -2)
			{
				printf("FATAL ERROR: Invalid property returned for class!\n");
				continue;
			}

			/* Skeletal mesh */
			idx = get_utx_index(f);
			numWedges = use_utx_index(idx, &type) + 1;
			printf("numWedges = %i\n", numWedges);

			for (i = 0; i < numWedges; ++i)
			{
				printf("extWedges[%i] {\n", i);
				fread(&u16, 2, 1, f);
				printf("\tiVertex = %i\n", u16); 
				fread(&u16, 2, 1, f);
				printf("\tflags = 0x%04x\n", u16); 
				fread(&r, 4, 1, f);
				printf("\tu = %f\n", r);
				fread(&r, 4, 1, f);
				printf("\tv = %f\n", r);
				printf("}\n\n");
			}
		}
	}

	fclose(f);

	return 0;
}


////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

UTPackage::UTPackage()
{
}


UTPackage::~UTPackage()
{
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Private Accessors
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Private Mutators
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Unit Test code
////////////////////////////////////////////////////////////

#ifdef UNIT_TEST_UTPACKAGE
int runUTPackageUnitTest(int argc, char *argv[])
{
	UTPackage test;

	if (argc > 2)
	{
		for (int i = 0; argv[1][i] != 0; ++i)
		{
			switch (argv[1][i])
			{
			case 'd': /* Dump to file */
				load_utx(argv[2], false, true);
				break;
			case 'h': /* Hexdump to console */
				load_utx(argv[2], true, false);
				break;
			case '0':
				load_utx(argv[2], false, false);
				break;
			}
		}
	}
	else
	{
		printf("UTPackage.test [h | d | 0] <UT_Packge_Filename>\n");
		printf("h - Hexdump to console\n");
		printf("d - Dump all objects to file possible\n");
		printf("0 - Just attempt to parse\n");
	}

	return 0;
}


int main(int argc, char *argv[])
{
	printf("[UTPackage class test]\n");
	runUTPackageUnitTest(argc, argv);

	return 0;
}
#endif


#ifdef OBSOLETE
int get_utx_property(utx_header_t *header, FILE *f,
							unsigned int *rsize, unsigned int *rtype,
							unsigned char **data)
{
	unsigned char info, isArray;
	unsigned short us;
	unsigned int u, size, type, flags;
	int index, nameIndex;

	/* Property */
	index = get_utx_index(f);
	nameIndex = ((type == UTPackage::UT_NULL) ? -1 :
					 (type == UTPackage::UT_IMPORT) ? 
					 header->importTable[index].objName :
					 header->exportTable[index].objName);

	if (index < 0) index = nameIndex;

	printf("\n{ index = %i, '%s'\n", index, 
			 ((index >= 0) ? header->nameTable[index].objName : 
			  "Bad Index"));

	if (index < 0 || strcmp("None", header->nameTable[index].objName) == 0)
	{
		return -1;
	}

	/* Read info byte */
	fread(&info, 1, 1, f);
	
	isArray = (info & PROPERTY_ARRAY_MASK) != 0;
	type = (info & PROPERTY_TYPE_MASK);
	
	switch (info & PROPERTY_SIZE_MASK)
	{
	case 0x00:
		size = 1;
		break;
	case 0x10:
		size = 2;
		break;
	case 0x20:
		size = 4;
		break;
	case 0x30:  
		size = 12;
		break;
	case 0x40:  
		size = 16;
		break;
	case 0x50:  
		fread(&info, 1, 1, f);
		size = info;
		break;
	case 0x60:
		fread(&us, 2, 1, f);
		size = us;
		break;
	case 0x70:
		fread(&size, 4, 1, f);
		break;
	}
	
	printf(" size %u, type 0x%x, flags 0x%x\n", 
			 size, type, flags);
	
	*rsize = size;
	*rtype = type;

	switch (type)
	{
	case UNP_ByteProperty:
		index = 1;

		if (isArray)
		{
			index = get_utx_index(f);
		}
		
		//*data = new unsigned char[1*index];
		//fread(*data, 1, index, f);

		fread(&info, 1, 1, f);		
		printf(" byte 0x%02x", info);
		break;
	case UNP_IntProperty:
		if (isArray)
		{
			index = get_utx_index(f);
		}
		
		fread(&u, 4, 1, f);
		printf(" int  %i", *((int *)&u));
		break;
	case UNP_BoolProperty:
		printf(" bool 0x%x\n", isArray);
		break;
	case UNP_FloatProperty:
		if (isArray)
		{
			index = get_utx_index(f);
		}

		fread(&u, 4, 1, f);
		printf(" float %f", *((float *)&u));
		break;
	case UNP_ObjectProperty:
	case UNP_NameProperty: //  SetObjectProperty( aUf );  
		if (isArray)
		{
			index = get_utx_index(f);
		}

		if (isArray)
		{
				index = get_utx_index(f);
		}
		
		hex_dump_buffer((unsigned char *)&index, 4);     
		break;
	case UNP_StrProperty: //   SetStringProperty( aUf );  
		hex_dump_file(f, size);     
		break;
	case UNP_StringProperty: //SetAsciizProperty( aUf );  
		hex_dump_file(f, size);     
		break;
	case UNP_StructProperty: //SetStructProperty( aUf, aUn ); 
		hex_dump_file(f, size); 
		break;
	case UNP_ArrayProperty:  //SetArrayProperty( aUf, aUn, aUi, aUe ); 
		hex_dump_file(f, size);
		break;
	case UNP_ClassProperty:
	case UNP_VectorProperty:
	case UNP_RotatorProperty:
	case UNP_MapProperty:
	case UNP_FixedArrayProperty: //    SetUnknownProperty( aUn,  aUf )
		hex_dump_file(f, size);
		break;
	}

	printf(" }\n");

	return 0;
}
#endif
