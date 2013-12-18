/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : Freyja
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : PSKModel
 * License : No use w/o permission (C) 2003 Mongoose
 * Comments: Unreal Tournament skeletal model
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History -------------------------------------------------
 *
 * 2003.07.12:
 * Mongoose - UT Package prototype code moved into 'UTPackage'
 *            API changes to structs to clearify meaning
 *
 * 2003.06.12:
 * Mongoose - PSA keyframes fixed, special thanks to:
 *            Steven Fuller and Forest Hale for help picking
 *            apart those tough little 8 bytes   =)
 *
 * 2003.06.11:
 * Mongoose - Broke up rendering and model loading into seperate
 *            classes ( Closely coupled )
 *
 *            PSA experimental loader
 *
 * 2003.06.10:
 * Mongoose - Skeletal format fixed, should load perfectly
 *
 * 2003.01.20:
 * Mongoose - Fixed triangle rendering thanks to debugging by
 *            Steven Fuller, who found the tris -> UV -> vert
 *            connection
 *
 *            Finished up basic rendering, lots of
 *            small fixes/features
 *
 * 2003.01.06:
 * Mongoose - Created
 =================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#ifdef USING_OPENGL
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#endif

#include "UTPackage.h"
#include "PSKModel.h"


#define INTERACTIVE_BONE_RENDER


void multiply_matrix(float *a, float *b, float *result)
{
	/* Column order */
	result[ 0] = a[ 0] * b[ 0] + a[ 4] * b[ 1] + a[ 8] * b[ 2] + a[12] * b[ 3];
	result[ 1] = a[ 0] * b[ 4] + a[ 4] * b[ 5] + a[ 8] * b[ 6] + a[12] * b[ 7];
	result[ 2] = a[ 0] * b[ 8] + a[ 4] * b[ 9] + a[ 8] * b[10] + a[12] * b[11];
	result[ 3] = a[ 0] * b[12] + a[ 4] * b[13] + a[ 8] * b[14] + a[12] * b[15];

	result[ 4] = a[ 1] * b[ 0] + a[ 5] * b[ 1] + a[ 9] * b[ 2] + a[13] * b[ 3];
	result[ 5] = a[ 1] * b[ 4] + a[ 5] * b[ 5] + a[ 9] * b[ 6] + a[13] * b[ 7];
	result[ 6] = a[ 1] * b[ 8] + a[ 5] * b[ 9] + a[ 9] * b[10] + a[13] * b[11];
	result[ 7] = a[ 1] * b[12] + a[ 5] * b[13] + a[ 9] * b[14] + a[13] * b[15];

	result[ 8] = a[ 2] * b[ 0] + a[ 6] * b[ 1] + a[10] * b[ 2] + a[14] * b[ 3];
	result[ 9] = a[ 2] * b[ 4] + a[ 6] * b[ 5] + a[10] * b[ 6] + a[14] * b[ 7];
	result[10] = a[ 2] * b[ 8] + a[ 6] * b[ 9] + a[10] * b[10] + a[14] * b[11];
	result[11] = a[ 2] * b[12] + a[ 6] * b[13] + a[10] * b[14] + a[14] * b[15];

	result[12] = a[ 3] * b[ 0] + a[ 7] * b[ 1] + a[11] * b[ 2] + a[15] * b[ 3];
	result[13] = a[ 3] * b[ 4] + a[ 7] * b[ 5] + a[11] * b[ 6] + a[15] * b[ 7];
	result[14] = a[ 3] * b[ 8] + a[ 7] * b[ 9] + a[11] * b[10] + a[15] * b[11];
	result[15] = a[ 3] * b[12] + a[ 7] * b[13] + a[11] * b[14] + a[15] * b[15];
}


void copy_matrix(float *src, float *dest)
{
	unsigned int i;

	for (i = 0; i < 16; ++i)
		dest[i] = src[i];
}


void translate_matrix(float tx, float ty, float tz, float *m)
{
	unsigned int i;
   float tmat[16];
	float tmp[16];


   tmat[ 0]=1;  tmat[ 1]=0;  tmat[ 2]=0;  tmat[ 3]=0;
   tmat[ 4]=0;  tmat[ 5]=1;  tmat[ 6]=0;  tmat[ 7]=0;
   tmat[ 8]=0;  tmat[ 9]=0;  tmat[10]=1;  tmat[11]=0;
   tmat[12]=tx; tmat[13]=ty; tmat[14]=tz; tmat[15]=1;

	//copy_matrix(m, tmp);

	for (i = 0; i < 16; ++i)
		tmp[i] = m[i];

	multiply_matrix(tmp, tmat, m);
}


/* <M> is input matrix
	<Out> is output matrix ( M inverted )
	both are float[16] column major matrices */
bool invert_matrix(float *m, float *out)
{
	/* Mongoose: This code was from a Jeff Lander tutorial which was based
		on MESA GL's InvertMatrix */

	/* NB. OpenGL Matrices are COLUMN major. */
#define SWAP_ROWS(a, b) { float *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) (m)[(c)*4+(r)]

	float wtmp[4][8];
	float m0, m1, m2, m3, s;
	float *r0, *r1, *r2, *r3;

	r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

	r0[0] = MAT(m,0,0), r0[1] = MAT(m,0,1),
	r0[2] = MAT(m,0,2), r0[3] = MAT(m,0,3),
	r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,

	r1[0] = MAT(m,1,0), r1[1] = MAT(m,1,1),
	r1[2] = MAT(m,1,2), r1[3] = MAT(m,1,3),
	r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,

	r2[0] = MAT(m,2,0), r2[1] = MAT(m,2,1),
	r2[2] = MAT(m,2,2), r2[3] = MAT(m,2,3),
	r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,

	r3[0] = MAT(m,3,0), r3[1] = MAT(m,3,1),
	r3[2] = MAT(m,3,2), r3[3] = MAT(m,3,3),
	r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

	/* choose pivot - or die */
	if (fabs(r3[0])>fabs(r2[0])) SWAP_ROWS(r3, r2);
	if (fabs(r2[0])>fabs(r1[0])) SWAP_ROWS(r2, r1);
	if (fabs(r1[0])>fabs(r0[0])) SWAP_ROWS(r1, r0);
	if (0.0 == r0[0])  return false;

	/* eliminate first variable     */
	m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
	s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
	s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
	s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
	s = r0[4];
	if (s != 0.0) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r0[5];
	if (s != 0.0) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r0[6];
	if (s != 0.0) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r0[7];
	if (s != 0.0) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

	/* choose pivot - or die */
	if (fabs(r3[1])>fabs(r2[1])) SWAP_ROWS(r3, r2);
	if (fabs(r2[1])>fabs(r1[1])) SWAP_ROWS(r2, r1);
	if (0.0 == r1[1])  return false;

	/* eliminate second variable */
	m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
	r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
	r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
	s = r1[4]; if (0.0 != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r1[5]; if (0.0 != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r1[6]; if (0.0 != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r1[7]; if (0.0 != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

	/* choose pivot - or die */
	if (fabs(r3[2])>fabs(r2[2])) SWAP_ROWS(r3, r2);
	if (0.0 == r2[2])  return false;

	/* eliminate third variable */
	m3 = r3[2]/r2[2];
	r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
	r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6],
	r3[7] -= m3 * r2[7];

	/* last check */
	if (0.0 == r3[3]) return false;

	s = 1.0/r3[3];              /* now back substitute row 3 */
	r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

	m2 = r2[3];                 /* now back substitute row 2 */
	s  = 1.0/r2[2];
	r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
	r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
	m1 = r1[3];
	r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
	r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
	m0 = r0[3];
	r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
	r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

	m1 = r1[2];                 /* now back substitute row 1 */
	s  = 1.0/r1[1];
	r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
	r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
	m0 = r0[2];
	r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
	r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

	m0 = r0[1];                 /* now back substitute row 0 */
	s  = 1.0/r0[0];
	r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
	r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

	MAT(out,0,0) = r0[4];
	MAT(out,0,1) = r0[5], MAT(out,0,2) = r0[6];
	MAT(out,0,3) = r0[7], MAT(out,1,0) = r1[4];
	MAT(out,1,1) = r1[5], MAT(out,1,2) = r1[6];
	MAT(out,1,3) = r1[7], MAT(out,2,0) = r2[4];
	MAT(out,2,1) = r2[5], MAT(out,2,2) = r2[6];
	MAT(out,2,3) = r2[7], MAT(out,3,0) = r3[4];
	MAT(out,3,1) = r3[5], MAT(out,3,2) = r3[6];
	MAT(out,3,3) = r3[7];

	return true;
#undef MAT
#undef SWAP_ROWS
}


void quaternion_to_matrix(float mW, float mX, float mY, float mZ, float *m)
{
	m[ 0] = 1.0f - 2.0f * (mY*mY + mZ*mZ);
	m[ 1] = 2.0f * (mX*mY - mW*mZ);
	m[ 2] = 2.0f * (mX*mZ + mW*mY);
	m[ 3] = 0.0f;

	m[ 4] = 2.0f * (mX*mY + mW*mZ);
	m[ 5] = 1.0f - 2.0f * (mX*mX + mZ*mZ);
	m[ 6] = 2.0f * (mY*mZ - mW*mX);
	m[ 7] = 0.0f;

	m[ 8] = 2.0f * (mX*mZ - mW*mY);
	m[ 9] = 2.0f * (mY*mZ + mW*mX);
	m[10] = 1.0 - 2.0f * (mX*mX + mY*mY);
	m[11] = 0.0f;

	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
}


void quaternion_to_axis_angles(float qw, float qx, float qy, float qz,
										 float *theta, float *x, float *y, float *z)
{
	*theta = (float)acos(qw) * 2.0;

	double scale = (
						 //sqrt(qx*qx + qy*qy + qz*qz);
						 (float)sin(*theta / 2.0));

	*x = qx / scale;
	*y = qy / scale;
	*z = qz / scale;
}


void quaternion_to_euler_angles(float qw, float qx, float qy, float qz,
										  float *x, float *y, float *z)
{
	double qw2 = qw*qw;
	double qx2 = qx*qx;
	double qy2 = qy*qy;
	double qz2 = qz*qz;

	*x = atan(2.0 * (qx*qy+qz*qw)/(qx2 - qy2 + qz2 + qw2)); // heading
	*y = atan(2.0 * (qy*qz+qx*qw)/(-qx2 - qy2 + qz2 + qw2));  // bank
	*z = asin(-2.0 * (qx*qz-qy*qw)); // attitude
}


////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

PSKModel::PSKModel()
{
	mFlags = 0;

	mNumFrames = 0;
	mNumVertices = 0;
	mNumFaces = 0;
	mNumVTXWs = 0;
	mNumMaterials = 0;
	mNumBones = 0;
	mNumWeights = 0;

	mVertices = 0x0;
	mWeights = 0x0;
	mVTXWs = 0x0;
	mFaces = 0x0;
	mMaterials = 0x0;
	mBones = 0x0;
}


PSKModel::~PSKModel()
{
	if (mVertices)
		delete [] mVertices;

	if (mWeights)
		delete [] mWeights;

	if (mVTXWs)
		delete [] mVTXWs;

	if (mFaces)
		delete [] mFaces;

	if (mMaterials)
		delete [] mMaterials;

	if (mBones)
		delete [] mBones;
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////

void PSKModel::printSkeletion(bool printNames)
{
	unsigned int i, j;

	printf("Skeletion layout...\n");

	for (i = 0; i < mNumBones; ++i)
	{
		printf("%3i. ", i);
		j = i;

		while (j > 0)
		{
			if (printNames)
			{
				printf(" -> %s ", mBones[j].name);
			}
			else
			{
				printf(" -> %u ", j);
			}

			j = mBones[j].parentIndex;
		}

		printf("\n");
	}

	printf("\n");
}


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

int PSKModel::load(char *filename)
{
	FILE *f;
	char buffer[128];
	unsigned int i, frame;
	float r;


	f = fopen(filename, "rb");

	if (!f)
	{
		perror("PSKModel::load> ");
		return -1;
	}

	/* ACTRHEAD... */
	fread(buffer, 1, 28, f);
	buffer[8] = 0;

	if (strncmp("ACTRHEAD", buffer, 8) != 0)
	{
		printf("Looking for 'ACTRHEAD' found '%s'\n", buffer);
		printf("This isn't a valid or known PSK model.\n");
		return -2;
	}
	else
	{
		printf("Unreal Tourament (c) EpicGames Skeletal Model Viewer\n");
		printf("Pre ALPHA release\n");
		printf("Give feedback on correctness and bugs\n");
		printf("by mongoose@icculus.org\n");
	}

	fread(&mNumFrames, 1, 4, f);
	printf(" NumFrames =\t %u\n", mNumFrames); // used to expand NumVerts?

	printf(" FORCING FRAMES TO 1 FOR TESTING NOW.\n");
	mNumFrames = 1;

	for (frame = 0; frame < mNumFrames; ++frame)
	{
		printf(" ===========================\n");
		printf(" Reading Frame[%03u]\n", frame);
		printf(" ===========================\n");

		/* PNTS0000... */
		fread(buffer, 1, 28, f);
		buffer[8] = 0;

		if (frame == 0 && strncmp("PNTS0000", buffer, 8) != 0)
		{
			printf("Expected PNTS0000, found '%s'\n", buffer);
			printf("Test model Vertex offset 0x003c, at 0x%lx?\n", ftell(f));
			printf("This isn't a known PNTS0000\n");
			return -2;
		}

		fread(&mNumVertices, 1, 4, f);
		printf(" NumVertices =\t %u\n", mNumVertices);
		mVertices = new float[mNumVertices*3];

		for (i = 0; i < mNumVertices*3; i+= 3)
		{
			fread(&r, 1, 4, f);
			mVertices[i] = r;
			fread(&r, 1, 4, f);
			mVertices[i+1] = r;
			fread(&r, 1, 4, f);
			mVertices[i+2] = r;

			if (mFlags & fDebugPointLoad)
			{
				printf("\t%5u (%.3f, %.3f, %.3f)\n",
						 i/3, mVertices[i], mVertices[i+1], mVertices[i+2]);
			}
		}


		/* VTXW0000... */
		fread(buffer, 1, 28, f);
		buffer[8] = 0;

		if (frame == 0 && strncmp("VTXW0000", buffer, 8) != 0)
		{
			printf("Expected VTXW0000, found '%s'\n", buffer);
			printf("Test model UV offset 0x2554, at 0x%lx?\n", ftell(f));
			printf("This isn't a known VTXW0000\n");
			return -2;
		}

		fread(&mNumVTXWs, 1, 4, f);
		printf(" NumVTXWs =\t %u\n", mNumVTXWs);

		mVTXWs = new psk_vtxw_t[mNumVTXWs];

		// <relnev> (uint vertex in lowest 16bits and something in upper 16, float u, float v, uint flags?)

		for (i = 0; i < mNumVTXWs; ++i)
		{
			// Vertex
			fread(&mVTXWs[i].vertex, 2, 1, f);

			// ??
			fread(&mVTXWs[i].unknown1, 2, 1, f);

			// UV
			fread(mVTXWs[i].uv+0, 4, 1, f);
			fread(mVTXWs[i].uv+1, 4, 1, f);

			// Used to compute weight index?
			fread(&mVTXWs[i].unknown2, 4, 1, f);

			if (mFlags & fDebugUVLoad)
			{
				printf("\t%5u vertex = %u, unknown1 = 0x%x, UV = (%.3f, %.3f), unknown2 = 0x%x\n",
						 i,
						 mVTXWs[i].vertex, mVTXWs[i].unknown1,
						 mVTXWs[i].uv[0], mVTXWs[i].uv[1],
						 mVTXWs[i].unknown2);
			}
		}


		/* FACE0000... */
		fread(buffer, 1, 28, f);
		buffer[8] = 0;

		if (frame == 0 && strncmp("FACE0000", buffer, 8) != 0)
		{
			printf("Expected FACE0000, found '%s'\n", buffer);
			printf("Test model FACE offset 0x7074, at 0x%lx?\n", ftell(f));
			printf("This isn't a known value for FACE\n");
			return -2;
		}

		fread(&mNumFaces, 1, 4, f);
		printf(" NumFaces =\t %u\n", mNumFaces);

		mFaces = new psk_face_t[mNumFaces];

		for (i = 0; i < mNumFaces; ++i)
		{
			// Vertex indices
			fread(&mFaces[i].x, 1, 2, f);
			fread(&mFaces[i].y, 1, 2, f);
			fread(&mFaces[i].z, 1, 2, f);

			// Material
			fread(&mFaces[i].material, 1, 1, f);

			// Bitflags
			fread(&mFaces[i].flags, 1, 4, f);

			// ??
			fread(&mFaces[i].unknown, 1, 1, f);

			if (mFlags & fDebugFaceLoad)
			{
				printf("%5u (%u, %u, %u), material = %u, flags = %u, unknown = 0x%x\n",
						 i,
						 mFaces[i].x, mFaces[i].y, mFaces[i].z,
						 mFaces[i].material, mFaces[i].flags, mFaces[i].unknown);

			}
		}


		/* MATT0000... */
		fread(buffer, 1, 28, f);
		buffer[8] = 0;

		if (frame == 0 && strncmp("MATT0000", buffer, 8) != 0)
		{
			printf("Expected MATT0000, found '%s'\n", buffer);
			printf("Test model Material offset 0xb750, at 0x%lx?\n", ftell(f));
			printf("This isn't a known value for MATT0000\n");
			return -2;
		}

		fread(&mNumMaterials, 1, 4, f);
		printf(" NumMaterials =\t %u\n", mNumMaterials);
		mMaterials = new psk_material_t[mNumMaterials];

		for (i = 0; i < mNumMaterials; ++i)
		{
			fread(&mMaterials[i].name, 1, 64, f);
			fread(&mMaterials[i].unknown1, 1, 4, f);
			fread(&mMaterials[i].unknown2, 1, 4, f);
			fread(&mMaterials[i].unknown3, 1, 4, f);
			fread(&mMaterials[i].unknown4, 1, 4, f);
			fread(&mMaterials[i].unknown5, 1, 4, f);
			fread(&mMaterials[i].unknown6, 1, 4, f);

			if (mFlags & fDebugMattLoad)
			{
				printf("\n# File offset 0x%lx\n", ftell(f) - 88);
				printf("mMaterial[%u].name = '%s'\n", i, mMaterials[i].name);
				printf("mMaterial[%u].unknown1 = 0x%x\n",
						 i, mMaterials[i].unknown1);
				printf("mMaterial[%u].unknown2 = 0x%x\n",
						 i, mMaterials[i].unknown2);
				printf("mMaterial[%u].unknown3 = 0x%x\n",
						 i, mMaterials[i].unknown3);
				printf("mMaterial[%u].unknown4 = 0x%x\n",
						 i, mMaterials[i].unknown4);
				printf("mMaterial[%u].unknown5 = 0x%x\n",
						 i, mMaterials[i].unknown5);
				printf("mMaterial[%u].unknown6 = 0x%x\n",
						 i, mMaterials[i].unknown6);
				printf("UVWs counting??\n");
			}
		}
	}


	/* REFSKELT... */
	fread(buffer, 1, 28, f);
	buffer[8] = 0;

	if (strncmp("REFSKELT", buffer, 8) != 0)
	{
		printf("Expected REFSKELT, found '%s'\n", buffer);
		printf("Test model Material offset 0xB820, at 0x%lx?\n", ftell(f));
		printf("This isn't a known value for REFSKELT\n");
		return -2;
	}

	fread(&mNumBones, 1, 4, f);
	printf(" NumBones =\t %u\n", mNumBones);

	mBones = new psk_bone_t[mNumBones];

	for (i = 0; i < mNumBones; ++i)
	{
		fread(buffer, 1, 64, f);
		strncpy(mBones[i].name, buffer, 64);
		fread(&mBones[i].flags, 1, 4, f);
		fread(&mBones[i].numChildren, 1, 4, f);
		fread(&mBones[i].parentIndex, 1, 4, f);

		fread(&r, 1, 4, f);
		mBones[i].restDir[0] = r;
		fread(&r, 1, 4, f);
		mBones[i].restDir[1] = r;
		fread(&r, 1, 4, f);
		mBones[i].restDir[2] = r;
		fread(&r, 1, 4, f);
		mBones[i].restDir[3] = r;

		fread(&r, 1, 4, f);
		mBones[i].restLoc[0] = r;
		fread(&r, 1, 4, f);
		mBones[i].restLoc[1] = r;
		fread(&r, 1, 4, f);
		mBones[i].restLoc[2] = r;

		fread(mBones[i].unknown+0, 1, 4, f);
		fread(mBones[i].unknown+1, 1, 4, f);
		fread(mBones[i].unknown+2, 1, 4, f);
		fread(mBones[i].unknown+3, 1, 4, f);

		if (mFlags & fDebugBoneLoad)
		{
			unsigned int offset = ftell(f) - 110;
			printf("\n# File offset 0x%x + 110bytes\n", offset);

			printf("Bone[%03u] = { '%s'\n", i, buffer);
			printf("\tParent: %u, Children: %u, flags: 0x%80x\n",
					 mBones[i].parentIndex, mBones[i].numChildren,
					 mBones[i].flags);
			printf("\tRest Loc: (%.3f, %.3f, %.3f)\n",
					 mBones[i].restLoc[0], mBones[i].restLoc[1],
					 mBones[i].restLoc[2]);
			printf("\tRest Dir XYZ: (%.3f, %.3f, %.3f), W: %.3f?\n",
					 mBones[i].restDir[0], mBones[i].restDir[1],
					 mBones[i].restDir[2], mBones[i].restDir[3]);

			printf("\tUnknown: 0x%x, 0x%x, 0x%x, Flags? 0x%x\n",
					 mBones[i].unknown[0], mBones[i].unknown[1],
					 mBones[i].unknown[2], mBones[i].unknown[3]);
			printf("}\n");
		}
	}

	if (mFlags & fDebugBoneLoad)
	{
		printSkeletion(false);
	}

	/* RAWWEIGHTS... */
	fread(buffer, 1, 28, f);
	buffer[11] = 0;

	if (strncmp("RAWWEIGHTS", buffer, 10) != 0)
	{
		printf("Expected RAWWEIGHTS, found '%s'\n", buffer);
		printf("Test model Weight offset 0xcb00, at 0x%lx?\n", ftell(f));
		printf("This isn't a known value for RAWWEIGHTS\n");
		return -2;
	}

	fread(&mNumWeights, 1, 4, f);
	printf(" NumWeights =\t %u\n", mNumWeights);

	mWeights = new psk_weight_t[mNumWeights];

	for (i = 0; i < mNumWeights; ++i)
	{
		fread(&mWeights[i].weight, 4, 1, f);
		fread(&mWeights[i].vertexIndex, 4, 1, f);
		fread(&mWeights[i].boneIndex, 4, 1, f);

		if (mFlags & fDebugWeightLoad)
		{
			printf("\tWeight[%u] { w = %f, vertexIndex = %u,  boneIndex = %u }\n", i,
					 mWeights[i].weight,
					 mWeights[i].vertexIndex, mWeights[i].boneIndex);
		}
	}

	fclose(f);

	return 0;
}


////////////////////////////////////////////////////////////
// Private Accessors
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Private Mutators
////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// PSAAnimation Class
//////////////////////////////////////////////////////////////////////

PSAAnimation::PSAAnimation()
{
	mNumFrames = 0;
	mNumBones = 0;
	mNumAnimInfos = 0;
	mNumKeyFrames = 0;

	mBones = 0x0;
	mAnimInfos = 0x0;
	mKeyFrames = 0x0;
}


PSAAnimation::~PSAAnimation()
{
	if (mBones)
		delete [] mBones;

	if (mAnimInfos)
		delete [] mAnimInfos;

	if (mKeyFrames)
		delete [] mKeyFrames;
}


int PSAAnimation::load(char *filename)
{
	FILE *f = fopen(filename, "rb");
	char s[64];
	unsigned int i, u;


	if (!f)
	{
		printf("PSAAnimation::load() Error: \n");
		perror(filename);
		return -1;
	}

	/* Is this a PSA file? */
	fread(s, 28, 1, f);
	s[8] = 0;
	if (strncmp("ANIMHEAD", s, 8) != 0)
	{
		printf("Expected ANIMHEAD, found '%s'\n", s);
		printf("File offset: 0x%lx?\n", ftell(f));
		printf("This isn't a known value.\n");
		return -2;
	}

	fread(&mNumFrames, 4, 1, f);
	printf("numFrames = %u\n", mNumFrames);

	/* Vertex blending frames? */


	/* Check for new section */
	fread(s, 28, 1, f);
	s[9] = 0;
	if (strncmp("BONENAMES", s, 9) != 0)
	{
		printf("Expected BONENAMES, found '%s'\n", s);
		printf("File offset: 0x%lx?\n", ftell(f));
		printf("This isn't a known value.\n");
		return -2;
	}

	fread(&mNumBones, 4, 1, f);
	printf("numBones = %u\n", mNumBones);

	mBones = new psk_bone_t[mNumBones];

	for (i = 0; i < mNumBones; ++i)
	{
		fread(mBones[i].name, 1, 64, f);
		fread(&mBones[i].flags, 1, 4, f);
		fread(&mBones[i].numChildren, 1, 4, f);
		fread(&mBones[i].parentIndex, 1, 4, f);

		fread(&mBones[i].restDir[0], 1, 4, f);
		fread(&mBones[i].restDir[1], 1, 4, f);
		fread(&mBones[i].restDir[2], 1, 4, f);
		fread(&mBones[i].restDir[3], 1, 4, f);

		fread(&mBones[i].restLoc[0], 1, 4, f);
		fread(&mBones[i].restLoc[1], 1, 4, f);
		fread(&mBones[i].restLoc[2], 1, 4, f);

		fread(mBones[i].unknown+0, 1, 4, f);
		fread(mBones[i].unknown+1, 1, 4, f);
		fread(mBones[i].unknown+2, 1, 4, f);
		fread(mBones[i].unknown+3, 1, 4, f);

		if (mFlags &fDebugBones)
		{
			printf("Bone[%03u] = { '%s'\n", i, mBones[i].name);
			printf("\tParent: %u, Children: %u, Flags: 0x%04x\n",
					 mBones[i].parentIndex, mBones[i].numChildren,
					 mBones[i].flags);
			printf("\tRest Loc: (%.3f, %.3f, %.3f)\n",
					 mBones[i].restLoc[0], mBones[i].restLoc[1],
					 mBones[i].restLoc[2]);
			printf("\tRest Dir XYZ: (%.3f, %.3f, %.3f), W: %.3f\n",
					 mBones[i].restDir[0], mBones[i].restDir[1],
					 mBones[i].restDir[2], mBones[i].restDir[3]);
			printf("\?: 0x%x, 0x%x, 0x%x, Flags? 0x%08x\n",
					 mBones[i].unknown[0], mBones[i].unknown[1],
					 mBones[i].unknown[2], mBones[i].unknown[3]);
			printf("? %f\n", *((float *)&mBones[i].unknown[3]));
			printf("}\n");
		}
	}

	/* Check for new section */
	fread(s, 28, 1, f);
	s[8] = 0;
	if (strncmp("ANIMINFO", s, 8) != 0)
	{
		printf("Expected ANIMINFO, found '%s'\n", s);
		printf("File offset: 0x%lx?\n", ftell(f));
		printf("This isn't a known value.\n");
		return -2;
	}

	fread(&mNumAnimInfos, 4, 1, f);
	printf("numAnimInfos = %u\n", mNumAnimInfos);

	mAnimInfos = new psa_anim_info_t[mNumAnimInfos];

	for (i = 0; i < mNumAnimInfos; ++i)
	{
		fread(mAnimInfos[i].name, 64, 1, f);
		fread(mAnimInfos[i].name2, 64, 1, f);
		fread(&mAnimInfos[i].numBones, 4, 1, f);
		fread(&mAnimInfos[i].rootId, 4, 1, f);
		fread(&mAnimInfos[i].key[0], 4, 1, f);
		fread(&mAnimInfos[i].key[1], 4, 1, f);
		fread(&mAnimInfos[i].keyf, 4, 1, f);
		fread(&mAnimInfos[i].time[0], 4, 1, f);
		fread(&mAnimInfos[i].time[1], 4, 1, f);
		fread(&mAnimInfos[i].unknown[0], 4, 1, f);
		fread(&mAnimInfos[i].unknown[1], 4, 1, f);
		fread(&mAnimInfos[i].unknown[2], 4, 1, f);

		if (mFlags &fDebugAnimInfos)
		{
			printf("\n");
			printf("AnimInfo[%u] = { name: '%s', name2: '%s'\n", i,
					 mAnimInfos[i].name, mAnimInfos[i].name2);
			printf(" numBones = %u, rootId = %u\n",
					 mAnimInfos[i].numBones, mAnimInfos[i].rootId);
			printf(" key: %u %u %f\n",
					 mAnimInfos[i].key[0], mAnimInfos[i].key[1],
					 mAnimInfos[i].keyf);
			printf(" time: %f %f\n",
					 mAnimInfos[i].time[0], mAnimInfos[i].time[1]);
			printf(" unknowns: %f %f %f\n",
					 mAnimInfos[i].unknown[0], mAnimInfos[i].unknown[1],
					 mAnimInfos[i].unknown[2]);
			printf("}\n");
		}
	}

	printf("COMPUTE: psa.numKeyframes / info.numBones = frames per bone\n");
	printf("\n");

	/* Check for new section */
	fread(s, 28, 1, f);
	s[8] = 0;
	if (strncmp("ANIMKEYS", s, 8) != 0)
	{
		printf("Expected ANIMKEYS, found '%s'\n", s);
		printf("File offset: 0x%lx?\n", ftell(f));
		printf("This isn't a known value.\n");
		return -2;
	}

	fread(&mNumKeyFrames, 4, 1, f);
	printf("numKeyFrames = %u\n", mNumKeyFrames);

	mKeyFrames = new psa_key_frame_t[mNumKeyFrames];

	for (i = 0; i < mNumKeyFrames; ++i)
	{
		// tx ty tz qx qy qz qw s
		fread(&mKeyFrames[i].trans[0], 4, 1, f);
		fread(&mKeyFrames[i].trans[1], 4, 1, f);
		fread(&mKeyFrames[i].trans[2], 4, 1, f);
		fread(&mKeyFrames[i].dir[0], 4, 1, f);
		fread(&mKeyFrames[i].dir[1], 4, 1, f);
		fread(&mKeyFrames[i].dir[2], 4, 1, f);
		fread(&mKeyFrames[i].dir[3], 4, 1, f);
		fread(&mKeyFrames[i].scale, 4, 1, f);

		if (mFlags &fDebugKeyFrames)
		{
			printf("keyFrame[%u] = {\n", i);
			printf("Trans XYZ: ( %f %f %f )\n",
					 mKeyFrames[i].trans[0], mKeyFrames[i].trans[1],
					 mKeyFrames[i].trans[2]);
			printf("Dir XYZ: ( %f %f %f ) %f\n",
					 mKeyFrames[i].dir[0], mKeyFrames[i].dir[1],
					 mKeyFrames[i].dir[2], mKeyFrames[i].dir[3]);
			printf("scale = %f\n", mKeyFrames[i].scale);
			printf("}\n\n");
		}
	}

	/* Check for partial read */
	u = ftell(f);
	fseek(f, 0, SEEK_END);
	i = ftell(f);

	if (i - u > 0)
	{
		printf("ERROR: Didn't read all of file!\n");
		printf("Skipping %u bytes at end of file\n", i - u);
	}

	fclose(f);

	return 0;
}


//////////////////////////////////////////////////////////////////////
// PSKModelRenderer Class
//////////////////////////////////////////////////////////////////////

PSKModelRenderer::PSKModelRenderer()
{
	mBoneRotationCache = 0x0;
	mVertexTransformCache = 0x0;
	mInvertedMatrices = 0x0;
	mWorldMatrices = 0x0;
	mModel = 0x0;
	mAnimation = 0x0;

	mNumMatrices = 0;
}


PSKModelRenderer::~PSKModelRenderer()
{
	unsigned int i;


	if (mBoneRotationCache)
		delete [] mBoneRotationCache;

	if (mVertexTransformCache)
		delete [] mVertexTransformCache;

	if (mInvertedMatrices)
	{
		for (i = 0; i < mNumMatrices; ++i)
			delete [] mInvertedMatrices[i];

		delete [] mInvertedMatrices;
	}

	if (mWorldMatrices)
	{
		for (i = 0; i < mNumMatrices; ++i)
			delete [] mWorldMatrices[i];

		delete [] mWorldMatrices;
	}
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////

void PSKModelRenderer::render()
{
#ifdef USING_OPENGL
	unsigned int i;


	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);

	glPushMatrix();
	glRotatef(-90.0f,  1, 0, 0);
	//glTranslatef(0, 0, 2);

	if (mFlags & fRenderBones)
	{
		renderBone(0);
	}

	if (mFlags & fRenderPoints)
	{
		glBegin(GL_POINTS);

		for (i = 0; i < mNumVertices*3; i+= 3)
		{
			glVertex3fv(mVertexTransformCache+i);
		}

		glEnd();
	}

	if (mFlags & fRenderTexture)
	{
		glEnable(GL_TEXTURE_2D);
	}

	if (mFlags & fRenderFaces)
	{
		unsigned int index[3];
		int lastTexture = 999999;


		for (i = 0; i < mNumFaces; ++i)
		{
			if (mFlags & fRenderTexture)
			{
				if (lastTexture != mFaces[i].material)
				{
					glBindTexture(GL_TEXTURE_2D, mTextures[mFaces[i].material]);
					lastTexture = mFaces[i].material;
				}
			}

			glBegin(GL_TRIANGLES);

			index[0] = mVTXWs[mFaces[i].x].vertex*3;
			index[1] = mVTXWs[mFaces[i].y].vertex*3;
			index[2] = mVTXWs[mFaces[i].z].vertex*3;

			if (mFlags & fDebugFaceRender)
			{
				printf("face[%u] = \n", i);
				printf("\t (%u = (%.3f..), %u = (%.3f...), %u = (%.3f..))\n",
						 index[0], mVertexTransformCache[index[0]],
						 index[1], mVertexTransformCache[index[1]],
						 index[2], mVertexTransformCache[index[2]]);

				printf("material = %u\n", mFaces[i].material);

				printf("UVs (%.3f, %.3f)\n",
						 mVTXWs[mFaces[i].z].uv[0], mVTXWs[mFaces[i].z].uv[1]);
			}

			if (mFlags & fRenderTexture)
			{
				glTexCoord2fv(mVTXWs[mFaces[i].x].uv);
				glVertex3fv(mVertexTransformCache+index[0]);
				glTexCoord2fv(mVTXWs[mFaces[i].y].uv);
				glVertex3fv(mVertexTransformCache+index[1]);
				glTexCoord2fv(mVTXWs[mFaces[i].z].uv);
				glVertex3fv(mVertexTransformCache+index[2]);
			}
			else
			{
				((i % 2) ?
				 glColor3f(0.0f, 0.0f, (((float)i)/(float)mNumFaces)*.5+.25) :
				 glColor3f(0.0f, (((float)i)/(float)mNumFaces)*.5+.25, 0.0f));

				glVertex3fv(mVertexTransformCache+index[0]);
				glVertex3fv(mVertexTransformCache+index[1]);
				glVertex3fv(mVertexTransformCache+index[2]);
			}

			glEnd();
		}

		if (mFlags & fDebugFaceRender)
		{
			printf("Turning fDebugFaceRender off\n");
			mFlags ^= fDebugFaceRender;
		}
	}

	glPopMatrix();
#endif
}


#ifdef INTERACTIVE_BONE_RENDER
/* HACK */
float gBoneRotHackX = 0;
float gBoneRotHackY = 0;
float gBoneRotHackZ = 0;
unsigned int gBoneId = 0;
#endif

void PSKModelRenderer::renderBone(unsigned int id)
{
#ifdef USING_OPENGL
	unsigned int i;
	float pos[3] = {0, 0, 0};
	float posB[3] = {0, 0, 0};
	unsigned int f;



#ifdef INTERACTIVE_BONE_RENDER
	/* Hack */
	glPushMatrix();

	if (gBoneId == id)
	{
		glRotatef(gBoneRotHackZ,  0, 0, 1);
		glRotatef(gBoneRotHackY,  0, 1, 0);
		glRotatef(gBoneRotHackX,  1, 0, 0);
	}
#endif

	glPushMatrix();

	if (id == 0)
	{
		glPointSize(8.0);
#ifdef FUDGE_BONE_RENDER
		glRotatef(180, 0, 0, 1);
		glPushMatrix();
#endif
	}

	/* Handle PSA animation use */
	if (mAnimationFrame > 0 &&
		 mAnimation && mAnimation->mBones == mBones)
	{
		f = id + (mAnimationFrame*mAnimation->mNumBones);

		posB[0] = -mAnimation->mKeyFrames[f].trans[0];
		posB[1] = -mAnimation->mKeyFrames[f].trans[1];
		posB[2] = -mAnimation->mKeyFrames[f].trans[2];

		glTranslatef(mAnimation->mKeyFrames[f].trans[0],
						 mAnimation->mKeyFrames[f].trans[1],
						 mAnimation->mKeyFrames[f].trans[2]);
	}
	else
	{
		posB[0] = -mBones[id].restLoc[0];
		posB[1] = -mBones[id].restLoc[1];
		posB[2] = -mBones[id].restLoc[2];

		glTranslatef(mBones[id].restLoc[0],
						 mBones[id].restLoc[1],
						 mBones[id].restLoc[2]);
	}

	/* Draw a visible 'bone',
		Gradient light (parent) to dark (child) */
	glLineWidth(3.5);
	glBegin(GL_LINES);
	glColor3f(0.33, 0.33, 0.33);
	glVertex3fv(pos);
	glColor3f(1.0, 1.0, 1.0);
	glVertex3fv(posB);
	glEnd();


	if (mFlags & fConvertEuler)
	{
		// euler angles
		glRotatef(mBoneRotationCache[id*4],   1, 0, 0);
		glRotatef(mBoneRotationCache[id*4+1], 0, 1, 0);
		glRotatef(mBoneRotationCache[id*4+2], 0, 0, 1);
	}
	else
	{
		// axis angles
		glRotatef(mBoneRotationCache[id*4],
					 mBoneRotationCache[id*4+1],
					 mBoneRotationCache[id*4+2],
					 mBoneRotationCache[id*4+3]);
	}

	glLineWidth(1.5);

	/* Draw axis list to show bone orientation */
	glBegin(GL_LINES);

	/* X axis */
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-8.0f, 0.0f, 0.0f);
	glVertex3f(8.0f, 0.0f, 0.0f);

	/* X direction */
	glVertex3f(8.0f, 0.0f, 0.0f);
	glVertex3f(7.0f, 1.0f, 0.0f);
	glVertex3f(8.0f, 0.0f, 0.0f);
	glVertex3f(7.0f, -1.0f, 0.0f);

	/* Y axis */
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -8.0f, 0.0f);
	glVertex3f(0.0f, 8.0f, 0.0f);

	/* Y direction */
	glVertex3f(0.0f, 8.0f, 0.0f);
	glVertex3f(0.0f, 7.0f, 1.0f);
	glVertex3f(0.0f, 8.0f, 0.0f);
	glVertex3f(0.0f, 7.0f, -1.0f);

	/* Z axis */
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -8.0f);
	glVertex3f(0.0f, 0.0f, 8.0f);

	/* Z direction */
	glVertex3f(0.0f, 0.0f, 8.0f);
	glVertex3f(0.0f, 1.0f, 7.0f);
	glVertex3f(0.0f, 0.0f, 8.0f);
	glVertex3f(0.0f, -1.0f, 7.0f);

	glEnd();

	/* Draw childless bone pivots darker */
	if (mBones[id].numChildren > 0)
	{
		glColor3f(1.0, 1.0, 1.0);
	}
	else
	{
		glColor3f(0.5, 0.5, 0.5);
	}

	glBegin(GL_POINTS);
	glVertex3fv(pos);
	glEnd();

	if (mBones[id].numChildren > 0)
	{
		for (i = 0; i < mNumBones; ++i)
		{
			if (i == mBones[i].parentIndex)
				continue;

			if (mBones[i].parentIndex == id)
			{
				renderBone(i);
			}
		}
	}

	glPopMatrix();

#ifdef INTERACTIVE_BONE_RENDER
	glPopMatrix();
#endif

	if (id == 0)
	{
#ifdef FUDGE_BONE_RENDER
		glPopMatrix();
#endif
		glColor3f(1.0, 1.0, 1.0);
		glLineWidth(1.0);
		glPointSize(1.0);
	}
#endif
}


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

void PSKModelRenderer::convertBoneAngles()
{
	unsigned int i;


	if (!mBoneRotationCache || !mModel)
	{
		printf("convertBoneAngles()> Error: No PSKModel loaded?\n");
		return;
	}

	/* Use PSK bones */
	mBones = mModel->mBones;

	printf("Converting quaternions to [%s]\n",
			 (mFlags & fConvertEuler) ? "Eular angles" : "Axis angles");

	for (i = 0; i < mNumBones; ++i)
	{
		if (mFlags & fConvertEuler)
		{
			// eular angles
			quaternion_to_euler_angles(mBones[i].restDir[0],
												mBones[i].restDir[1],
												mBones[i].restDir[2],
												mBones[i].restDir[3],
												&mBoneRotationCache[i*4],    // x
												&mBoneRotationCache[i*4+1],  // y
												&mBoneRotationCache[i*4+2]); // z

			// Convert radians to degrees
			mBoneRotationCache[i*4] *= 57.295779513082323;
			mBoneRotationCache[i*4+1] *= 57.295779513082323;
			mBoneRotationCache[i*4+2] *= 57.295779513082323;
		}
		else
		{
			// axis angles
			quaternion_to_axis_angles(mBones[i].restDir[3], // qw is dir[3]?
											  mBones[i].restDir[0], // qx
											  mBones[i].restDir[1], // qy
											  mBones[i].restDir[2], // qz
											  &mBoneRotationCache[i*4],  // theta
											  &mBoneRotationCache[i*4+1],  // x
											  &mBoneRotationCache[i*4+2],  // y
											  &mBoneRotationCache[i*4+3]); // z

			// Convert radians to degrees, negated to account for UT coords
			mBoneRotationCache[i*4] *= -57.295779513082323;
		}
	}
}


void PSKModelRenderer::convertBoneAnglesPSA(unsigned int frame)
{
	unsigned int i, b, start, end;
	float w, x, y, z, w2, x2, y2, z2;
	float qw, qx, qy, qz;


	if (!mBoneRotationCache || !mAnimation)
	{
		printf("convertBoneAnglesPSA()> Error: No PSA Animation loaded?\n");
		return;
	}

	/* Use PSA bones */
	mBones = mAnimation->mBones;

	/* Check here for frame out of bounds */
	if (frame > mAnimation->mNumKeyFrames / mAnimation->mNumBones)
	{
		printf("convertBoneAnglesPSA()> Error: Frame out of bounds\n");
		return;
	}

	printf("Converting quaternions to [%s] for PSA frame [%u]\n",
			 (mFlags & fConvertEuler) ? "Eular angles" : "Axis angles", frame);

	/* Update frame tracker */
	mAnimationFrame = frame;

	/* Compute start and end bones from list */
	start = (mAnimationFrame*mAnimation->mNumBones);
	end = start + mAnimation->mNumBones;

	for (i = start, b = 0; i < end; ++i, ++b)
	{
		w = mBones[b].restDir[3];
		x = mBones[b].restDir[0];
		y = mBones[b].restDir[1];
		z = mBones[b].restDir[2];

		if (mAnimationFrame > 0)
		{
			w2 = mAnimation->mKeyFrames[i].dir[3];
			x2 = mAnimation->mKeyFrames[i].dir[0];
			y2 = mAnimation->mKeyFrames[i].dir[1];
			z2 = mAnimation->mKeyFrames[i].dir[2];

			//qw = w * w2 - x * x2 - y * y2 - z * z2;
			//qx = w * x2 + x * w2 + y * z2 - z * y2;
			//qy = w * y2 + y * w2 + z * x2 - x * z2;
			//qz = w * z2 + z * w2 + x * y2 - y * x2;

			qw = w2;  qx = x2;  qy = y2;  qz = z2;
		}
		else
		{
			qw = w;  qx = x;  qy = y;  qz = z;
		}

		if (mFlags & fConvertEuler)
		{
			/* FIXME: No support for frames */

			// eular angles
			quaternion_to_euler_angles(qw, qx, qy, qz,
												&mBoneRotationCache[b*4],    // x
												&mBoneRotationCache[b*4+1],  // y
												&mBoneRotationCache[b*4+2]); // z

			// Convert radians to degrees
			mBoneRotationCache[b*4] *= 57.295779513082323;
			mBoneRotationCache[b*4+1] *= 57.295779513082323;
			mBoneRotationCache[b*4+2] *= 57.295779513082323;
		}
		else
		{
			// axis angles
			quaternion_to_axis_angles(qw, qx, qy, qz,
											  &mBoneRotationCache[b*4],    // theta
											  &mBoneRotationCache[b*4+1],  // x
											  &mBoneRotationCache[b*4+2],  // y
											  &mBoneRotationCache[b*4+3]); // z

			// Convert radians to degrees, negated to account for UT coords
			mBoneRotationCache[b*4] *= -57.295779513082323;
		}
	}

	/* Setup matrices for mesh deformation use */
	setupWorldMatrices(frame);

	/* Reset vertices and then deform them */
	copyVertices();
	transformVertices(); // FIXME: pass frame time later
}


void PSKModelRenderer::copyVertices()
{
	unsigned int i;


	if (!mVertexTransformCache || !mModel)
	{
		printf("copyVertices()> Error: No PSKModel loaded?\n");
		return;
	}

	for (i = 0; i < mModel->mNumVertices; ++i)
	{
		mVertexTransformCache[i*3]   = mModel->mVertices[i*3];   // 0
		mVertexTransformCache[i*3+1] = mModel->mVertices[i*3+1]; // 2
		mVertexTransformCache[i*3+2] = mModel->mVertices[i*3+2]; // 1
	}
}


void PSKModelRenderer::setAnimation(PSAAnimation *anim)
{
	if (!anim || !mModel || anim->mNumBones != mModel->mNumBones)
	{
		printf("Error: Unable to load PSA Animation, [%s]\n",
				 (!mModel) ? "No PSK model loaded" :
				 (!anim) ? "Null PSA given" :
				 "PSA animation doesn't match loaded PSK model");
		return;
	}

	mAnimation = anim;

	/* Bones are forced to same size to match model now,
		and there is no need to default to PSA skeletion now*/

	/* Wow, now I look smart able to switch out bones on the fly */
	//mBones = anim->mBones;
	//
	/* Setup bones */
	//if (mBoneRotationCache)
	//	delete [] mBoneRotationCache;
	//
	//mBoneRotationCache = new float[anim->mNumBones*4];

	/* No use loading PSA frame 0 anymore by default */
	//convertBoneAnglesPSA(0);
}


void PSKModelRenderer::setModel(PSKModel *model)
{
	unsigned int i;


	if (!model)
	{
		return;
	}

	mModel = model;

	/* Setup cheesy unneeded attributes to simulate inhertence */
	mNumFrames = model->mNumFrames;
	mNumVertices = model->mNumVertices;
	mNumFaces = model->mNumFaces;
	mNumVTXWs = model->mNumVTXWs;
	mNumMaterials = model->mNumMaterials;
	mNumBones = model->mNumBones;
	mNumWeights = model->mNumWeights;
	mVTXWs = model->mVTXWs;
	mFaces = model->mFaces;
	mMaterials = model->mMaterials;
	mBones = model->mBones;
	mWeights = model->mWeights;

	/* Setup vertices */
	if (mVertexTransformCache)
		delete [] mVertexTransformCache;

	mVertexTransformCache = new float[model->mNumVertices*3];
	copyVertices();

	/* Setup bones */
	if (mBoneRotationCache)
		delete [] mBoneRotationCache;

	mBoneRotationCache = new float[model->mNumBones*4];
	convertBoneAngles();

	/* Setup rest / transform matrices */
	mNumMatrices = model->mNumBones;

	mInvertedMatrices = new float*[mNumMatrices];

	for (i = 0; i < mNumMatrices; ++i)
		mInvertedMatrices[i] = new float[16];

	mWorldMatrices = new float*[mNumMatrices];

	for (i = 0; i < mNumMatrices; ++i)
		mWorldMatrices[i] = new float[16];

	/* Setup matrices for mesh deformation use */
	setupRestMatrices(0);
}


void PSKModelRenderer::setupRestMatrices(unsigned int id)
{
#ifdef USING_OPENGL
	unsigned int i;
	float theta, x, y, z;
	float m[16];


	if (!mModel || !mModel->mBones)
		return;

	glPushMatrix();

	glTranslatef(mModel->mBones[id].restLoc[0],
					 mModel->mBones[id].restLoc[1],
					 mModel->mBones[id].restLoc[2]);

	/* Get the rotation in axis angles */
	quaternion_to_axis_angles(mModel->mBones[id].restDir[0], // qw
									  mModel->mBones[id].restDir[1], // qx
									  mModel->mBones[id].restDir[2], // qy
									  mModel->mBones[id].restDir[3], // qz
									  &theta, &x, &y, &z);

	/* Convert radians to degrees */
	theta *= -57.295779513082323;

	/* OpenGL axis angles rotation */
	glRotatef(theta, x, y, z);

	/* Store this bones tranform matrix inverted to
		get bone rest position of model vertices */
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	invert_matrix(m, mInvertedMatrices[id]);

	if (mBones[id].numChildren > 0)
	{
		for (i = 0; i < mNumBones; ++i)
		{
			if (i == mBones[i].parentIndex)
				continue;

			if (mBones[i].parentIndex == id)
			{
				setupRestMatrices(i);
			}
		}
	}

	glPopMatrix();
#endif
}


/* Currently doesn't do what is says it does heh */
void PSKModelRenderer::setupWorldMatrices(unsigned int id)
{
#ifdef USING_OPENGL
	unsigned int i, f;


	glPushMatrix();

	/* Handle PSA animation use */
	if (mAnimationFrame > 0 &&
		 mAnimation && mAnimation->mBones == mBones)
	{
		f = id + (mAnimationFrame*mAnimation->mNumBones);

		glTranslatef(mAnimation->mKeyFrames[f].trans[0],
						 mAnimation->mKeyFrames[f].trans[1],
						 mAnimation->mKeyFrames[f].trans[2]);
	}
	else
	{
		glTranslatef(mBones[id].restLoc[0],
						 mBones[id].restLoc[1],
						 mBones[id].restLoc[2]);
	}

	if (mFlags & fConvertEuler)
	{
		// euler angles
		glRotatef(mBoneRotationCache[id*4],   1, 0, 0);
		glRotatef(mBoneRotationCache[id*4+1], 0, 1, 0);
		glRotatef(mBoneRotationCache[id*4+2], 0, 0, 1);
	}
	else
	{
		// axis angles
		glRotatef(mBoneRotationCache[id*4],
					 mBoneRotationCache[id*4+1],
					 mBoneRotationCache[id*4+2],
					 mBoneRotationCache[id*4+3]);
	}

	/* Store world transform matrix */
	glGetFloatv(GL_MODELVIEW_MATRIX, mWorldMatrices[id]);

	if (mBones[id].numChildren > 0)
	{
		for (i = 0; i < mNumBones; ++i)
		{
			if (i == mBones[i].parentIndex)
				continue;

			if (mBones[i].parentIndex == id)
			{
				setupWorldMatrices(i);
			}
		}
	}

	glPopMatrix();
#elif FIXME_SOON
	unsigned int i, j;
	float x, y, z;
	float m[16], m2[16], m3[16];


	/* Currently inline transforms all vertices to produce
		one deformed mesh keyframe */
	if (id == 0)
		copyVertices();

	/* Rest skeleton matrices */
   m[ 0]=1;  m[ 1]=0;  m[ 2]=0;  m[ 3]=0;
   m[ 4]=0;  m[ 5]=1;  m[ 6]=0;  m[ 7]=0;
   m[ 8]=0;  m[ 9]=0;  m[10]=1;  m[11]=0;
   m[12]=mModel->mBones[id].restLoc[0]; m[13]=mModel->mBones[id].restLoc[1]; m[14]=mModel->mBones[id].restLoc[2]; m[15]=1;

	quaternion_to_matrix(mModel->mBones[id].restDir[3], // qw
								mModel->mBones[id].restDir[0], // qx
								mModel->mBones[id].restDir[1], // qy
								mModel->mBones[id].restDir[2], // qz
								m2);

	multiply_matrix(m, m2, m3);

	if (id == 0)
	{
		invert_matrix(m3, mInvertedMatrices[id]);
	}
	else
	{
		invert_matrix(m3, m2);
		multiply_matrix(mInvertedMatrices[mModel->mBones[id].parent],
							 m2, mInvertedMatrices[id]);
	}

	/* Transformed skeleton matrices */
   m[ 0]=1;  m[ 1]=0;  m[ 2]=0;  m[ 3]=0;
   m[ 4]=0;  m[ 5]=1;  m[ 6]=0;  m[ 7]=0;
   m[ 8]=0;  m[ 9]=0;  m[10]=1;  m[11]=0;
   m[12]=mAnimation->mBones[id].restLoc[0]; m[13]=mAnimation->mBones[id].restLoc[1]; m[14]=mAnimation->mBones[id].restLoc[2]; m[15]=1;

	quaternion_to_matrix(mAnimation->mBones[id].restDir[3], // qw
								mAnimation->mBones[id].restDir[0], // qx
								mAnimation->mBones[id].restDir[1], // qy
								mAnimation->mBones[id].restDir[2], // qz
								m2);

	multiply_matrix(m, m2, m3);

	/* Transform vertices */
	//multiply_matrix(mInvertedMatrices[id], m3, m);

	if (id == 0)
	{
		multiply_matrix(mInvertedMatrices[id], m3, m);
	}
	else
	{
		multiply_matrix(mInvertedMatrices[id], m3, m2);
		multiply_matrix(mInvertedMatrices[mModel->mBones[id].parent],
							 m2, m);
	}
	//copy_matrix(m, mInvertedMatrices[id]);

	for (i = 0; i < mModel->mNumWeights; ++i)
	{
		if (mModel->mWeights[i].boneIndex == id)
		{
			j = mModel->mWeights[i].vertexIndex;

			x = mVertexTransformCache[j*3+0] * mModel->mWeights[i].weight;
			y = mVertexTransformCache[j*3+1] * mModel->mWeights[i].weight;
			z = mVertexTransformCache[j*3+2] * mModel->mWeights[i].weight;

			mVertexTransformCache[j*3+0] = m[0]*x + m[1]*y + m[ 2]*z + m[ 3];
			mVertexTransformCache[j*3+1] = m[4]*x + m[5]*y + m[ 6]*z + m[ 7];
			mVertexTransformCache[j*3+2] = m[8]*x + m[9]*y + m[10]*z + m[11];
		}
	}

	/* Transform by rest of model */
	for (i = 0; i < mAnimation->mNumBones; ++i)
	{
		if (i == mAnimation->mBones[i].parent)
			continue;

		if (mAnimation->mBones[i].parent == id)
		{
			setupMatrixTransforms(i);
		}
	}
#endif
}


void PSKModelRenderer::transformVertices()
{
	unsigned int id, i, j;
	float x, y, z;
	float m[16];

	for (id = 0; id < mModel->mNumBones; ++id)
	{
		multiply_matrix(mWorldMatrices[id], mInvertedMatrices[id], m);
		//copy_matrix(mWorldMatrices[id], m);

		for (i = 0; i < mModel->mNumWeights; ++i)
		{
			if (mModel->mWeights[i].boneIndex == id)
			{
				j = mModel->mWeights[i].vertexIndex;

				x = mVertexTransformCache[j*3+0] * mModel->mWeights[i].weight;
				y = mVertexTransformCache[j*3+1] * mModel->mWeights[i].weight;
				z = mVertexTransformCache[j*3+2] * mModel->mWeights[i].weight;

				mVertexTransformCache[j*3+0] = m[0]*x + m[4]*y + m[ 8]*z + m[12];
				mVertexTransformCache[j*3+1] = m[1]*x + m[5]*y + m[ 9]*z + m[13];
				mVertexTransformCache[j*3+2] = m[2]*x + m[6]*y + m[10]*z + m[14];
			}
		}
	}
}


////////////////////////////////////////////////////////////
// Unit Test code
////////////////////////////////////////////////////////////

#ifdef UNIT_TEST_PSKMODEL

PSKModel gModel;
PSAAnimation gAnim;

#ifdef USING_OPENGL

PSKModelRenderer gModelRenderer;

#   include <SDL/SDL.h>
#   include <Texture.h>

Texture gTexture;

char gTitle[128];
char gMessage[256];
float gYaw = 0.0f;
bool gYawOn = true;
float gPitch = -20.0f;
float gScale = 1.75f;
float gLightPos[] = { 0.0, 512.0, 1024.0, 0.0 };
unsigned int gWidth = 640, gHeight = 460;
bool gWireframe = false;


unsigned int getTicks();
void event_resize(unsigned int width, unsigned int height);


void renderScene()
{
	static float lastTime = 0.0f;
	const float size = 500.0f, step = 50.0f;
	float x, y, time;


	gluLookAt(0.0, 0.0, -256.0,
				 0.0, 8.0, 0.0,
				 0.0, 1.0, 0.0);

	glDisable(GL_TEXTURE_2D);

	time = getTicks() * 0.1f;

	if (time - lastTime > 5.0f)
	{
		lastTime = time;

		if (gYawOn)
			++gYaw;
	}

#ifdef DRAW_ACTUAL_ORIGIN_AXIS_WITH_ROTATION
	glPushMatrix();
	glRotatef(yaw, 0, 1, 0);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 10.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 10.0f);
	glEnd();
	glPopMatrix();
#endif

	// Draw light symbol
	glPushMatrix();
	glTranslatef(gLightPos[0], gLightPos[1], gLightPos[2]);

	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 10.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 10.0f);
	glEnd();
	glPopMatrix();

	glTranslatef(0.0f, -128.0f, 512.0f);
	glRotatef(gPitch, 1, 0, 0);
	glRotatef(gYaw,   0, 1, 0);

	// Draw transformed origin axis
	const float axisLength = 30.0f;
	const float axisLength2 = 26.25f;
	const float axisTip = 3.75f;
	glBegin(GL_LINES);
	/* X axis */
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-axisLength, 0.0f, 0.0f);
	glVertex3f(axisLength, 0.0f, 0.0f);
	/* X direction */
	glVertex3f(axisLength, 0.0f, 0.0f);
	glVertex3f(axisLength2, axisTip, 0.0f);
	glVertex3f(axisLength, 0.0f, 0.0f);
	glVertex3f(axisLength2, -axisTip, 0.0f);
	/* Y axis */
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -axisLength, 0.0f);
	glVertex3f(0.0f, axisLength, 0.0f);
	/* Y direction */
	glVertex3f(0.0f, axisLength, 0.0f);
	glVertex3f(0.0f, axisLength2, axisTip);
	glVertex3f(0.0f, axisLength, 0.0f);
	glVertex3f(0.0f, axisLength2, -axisTip);
	/* Z axis */
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -axisLength);
	glVertex3f(0.0f, 0.0f, axisLength);
	/* Z direction */
	glVertex3f(0.0f, 0.0f, axisLength);
	glVertex3f(0.0f, axisTip, axisLength2);
	glVertex3f(0.0f, 0.0f, axisLength);
	glVertex3f(0.0f, -axisTip, axisLength2);
	glEnd();


	// Draw grid
	glPushMatrix();
	glScalef(2.0f, 2.0f, 2.0f);
	glColor3f(0.4f, 0.4f, 0.6f);

	for (x = -size; x < size; x += step)
	{
		glBegin(GL_LINE_LOOP);
		for (y = -size; y < size; y += step)
		{
			glVertex3f(x + step, 0.0f, y);
			glVertex3f(x, 0.0f, y);
			glVertex3f(x, 0.0f, y + step);
			glVertex3f(x + step, 0.0f, y + step);
		}
		glEnd();
	}
	glPopMatrix();

	// Draw model
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix();
	glTranslatef(0.0f, 8.0f, 0.0f);
	glScalef(gScale, gScale, gScale);
	gModelRenderer.render();
	glPopMatrix();

#ifdef HAVE_SDL_TTF
	if (gWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/* Render 2d text */
	glEnterMode2d(gWidth, gHeight);
	glColor3f(1.0, 1.0, 1.0);
	glPrint2d(8, 8, 0.90, gMessage);
	glExitMode2d();

	if (gWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
}


void initScene(int argc, char *argv[])
{
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);    // Q3A uses front face culling
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_LIGHTING);


	if (argc > 3)
	{
		//loadMd3(argv[1], argv[2], argv[3]);
	}
	else if (argc > 2)
	{
		//loadMd3(argv[1], argv[2], "machinegun");
	}
	else if (argc > 1)
	{
		//loadMd3(argv[1], "default", "machinegun");
	}
	else
	{
		printf("Usage:\n");
		printf("%s ModelDir [Skin] [WeaponDir]\n", argv[0]);
		exit(0);
	}

	// Texture setup
	gTexture.reset();
	gTexture.setFlag(Texture::fUseMipmaps);
	gTexture.setMaxTextureCount(64);
	printf("Loading textures: ");

	gModelRenderer.mFlags |= PSKModelRenderer::fRenderTexture;
	for (unsigned int i = 0; i < gModel.mNumMaterials; ++i)
	{
		char texture[256];
		sprintf(texture, "%s/%s.tga", argv[3], gModel.mMaterials[i].name);
		gModelRenderer.mTextures[i] = gTexture.loadTGA(texture);
	}

	printf(" done\n\n");

#ifdef HAVE_SDL_TTF
	gTexture.loadFontTTF("data/test.ttf", 32, 126 - 32);
	bufferedPrintf(gMessage, 256, "PSKModel Unit Test");
#endif

	printf("\n");
	printf("----------------------------------\n");
	printf("ESC        - Exit\n");
	printf("F1         - Take screenshot\n");
	printf("F10        - Set window to 640x460\n");
	printf("F11        - Set window to 800x600\n");
	printf("F12        - Set window to 1024x768\n");
	printf("ALT+ENTER  - Toogle fullscreen\n");
	printf("UP/DOWN    - Adjust scene pitch\n");
	printf("RIGHT/LEFT - Adjust scene yaw\n");
	printf("-/=        - Adjust model scaling\n");
	printf(";/'        - Switch PSA frame (EXPERIMENTAL)\n");
	printf("/          - Show PSK frame\n");
	printf("----------------------------------\n");
	printf("1 - Toggle polygon rendering\n");
	printf("2 - Toggle polygon render debugging\n");
	printf("3 - Toggle textured polygons\n");
	printf("4 - Toggle points\n");
	printf("7 - Toggle scene rotation\n");
	printf("8 - Toggle alpha blending\n");
	printf("8 - Toggle OpenGL Lighting\n");
	printf("r - Reset mesh to default\n");
	printf("w - Toggle wireframe rendering\n");
#ifdef INTERACTIVE_BONE_RENDER
	printf("g,h - Select bone Id\n");
	printf("z,x,c, b,n,m - Adjust bone rotatation\n");
#endif
	printf("f - Convert rotation to angles to render correctly\n");
	printf("i,k,j,l - Move light\n");
	printf("[ - Loop through lower animations\n");
	printf("] - Loop through upper animations\n");
	printf("\\ - Loop through both animations\n");
	printf("----------------------------------\n");
}


void handleKey(int key)
{
	static bool alphaBlend = false;
	static bool lighting = false;

	switch (key)
	{
	case SDLK_F1:
		gTexture.glScreenShot("PSKModel.test", gWidth, gHeight);
		bufferedPrintf(gMessage, 256, "Took screenshot...");
		break;
	case SDLK_F10:
		event_resize(640, 460);
		bufferedPrintf(gMessage, 256, "Switching to VGA resolution");
		break;
	case SDLK_F11:
		event_resize(800, 600);
		bufferedPrintf(gMessage, 256, "Switching to SVGA resolution");
		break;
	case SDLK_F12:
		event_resize(1024, 768);
		bufferedPrintf(gMessage, 256, "Switching to XGA resolution");
		break;
	case SDLK_RIGHT:
		++gYaw;
		break;
	case SDLK_LEFT:
		--gYaw;
		break;
	case SDLK_UP:
		++gPitch;
		break;
	case SDLK_DOWN:
		--gPitch;
		break;
	case 'f':
		printf("Converting bone angles...\n");
		gModelRenderer.convertBoneAngles();
		break;
#ifdef INTERACTIVE_BONE_RENDER
	case 'g':
		--gBoneId;
		printf("gBoneId = %i\n", gBoneId);
		bufferedPrintf(gMessage, 256, "gBoneId = %i\n", gBoneId);
		break;
	case 'h':
		++gBoneId;
		printf("gBoneId = %i\n", gBoneId);
		bufferedPrintf(gMessage, 256, "gBoneId = %i\n", gBoneId);
		break;
	case 'z':
		--gBoneRotHackX;
		printf("gBoneRotHackX = %f\n", gBoneRotHackX);
		bufferedPrintf(gMessage, 256, "gBoneRotHackX = %f\n", gBoneRotHackX);
		break;
	case 'x':
		--gBoneRotHackY;
		printf("gBoneRotHackY = %f\n", gBoneRotHackY);
		bufferedPrintf(gMessage, 256, "gBoneRotHackY = %f\n", gBoneRotHackY);
		break;
	case 'c':
		--gBoneRotHackZ;
		printf("gBoneRotHackZ = %f\n", gBoneRotHackZ);
		bufferedPrintf(gMessage, 256, "gBoneRotHackZ = %f\n", gBoneRotHackZ);
		break;
	case 'b':
		++gBoneRotHackX;
		printf("gBoneRotHackX = %f\n", gBoneRotHackX);
		bufferedPrintf(gMessage, 256, "gBoneRotHackX = %f\n", gBoneRotHackX);
		break;
	case 'n':
		++gBoneRotHackY;
		printf("gBoneRotHackY = %f\n", gBoneRotHackY);
		bufferedPrintf(gMessage, 256, "gBoneRotHackY = %f\n", gBoneRotHackY);
		break;
	case 'm':
		++gBoneRotHackZ;
		printf("gBoneRotHackZ = %f\n", gBoneRotHackZ);
		bufferedPrintf(gMessage, 256, "gBoneRotHackZ = %f\n", gBoneRotHackZ);
		break;
#endif
	case '-':
		if (gScale * 0.15 > 0.000001)
			gScale *= 0.15f;
		break;
	case '=':
		gScale *= 1.15f;
		break;
	case '0':
		gModelRenderer.mFlags ^= PSKModelRenderer::fRenderBones;
		break;
	case '1':
		gModelRenderer.mFlags ^= PSKModelRenderer::fRenderFaces;
		break;
	case '2':
		gModelRenderer.mFlags ^= PSKModelRenderer::fDebugFaceRender;
		break;
	case '3':
		gModelRenderer.mFlags ^= PSKModelRenderer::fRenderTexture;

		if (gModelRenderer.mFlags & PSKModelRenderer::fRenderTexture)
			glEnable(GL_TEXTURE_2D);
		else
			glDisable(GL_TEXTURE_2D);
		break;
	case '4':
		gModelRenderer.mFlags ^= PSKModelRenderer::fRenderPoints;
		break;
	case '7':
		gYawOn = !gYawOn;
		break;
	case '8':
		alphaBlend = !alphaBlend;

		if (alphaBlend)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glAlphaFunc(GL_GREATER, 0);
		}
		else
		{
			glDisable(GL_BLEND);
		}

		printf("%sabled alpha blending\n", alphaBlend  ? "En" : "Dis");
		break;
	case '9':
		lighting = !lighting;

		if (lighting)
		{
			static bool firsttime = true;


			glEnable(GL_LIGHTING);

			if (firsttime)
			{
				float specular[] = { 0.6, 0.6, 0.6, 1.0 };
				float ambient[] = { 0.64, 0.64, 0.64, 1.0 };
				float shine[] = { 50.0 };


				// Caustic fx possible with mod amb?
				glMaterialfv(GL_FRONT, GL_SPECULAR,  specular);
				glMaterialfv(GL_FRONT, GL_AMBIENT,   ambient);
				//glMaterialfv(GL_FRONT, GL_DIFFUSE,   ambient);
				glMaterialfv(GL_FRONT, GL_SHININESS, shine);
				glLightfv(GL_LIGHT0,   GL_POSITION,  gLightPos);
				glEnable(GL_LIGHT0);

				glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);

				//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
				//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, WHITE);
				//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, WHITE);
				//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, DIM_WHITE);
				//glLightf(GL_LIGHT0,  GL_SPOT_CUTOFF,    cutoff);
				//glLightfv(GL_LIGHT0, GL_POSITION,       pos);
				//glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
				//glLightfv(GL_LIGHT0, GL_DIFFUSE,        color);

				firsttime = false;
			}
		}
		else
		{
			glDisable(GL_LIGHTING);
		}
		break;
	case '/':
		gModelRenderer.convertBoneAngles();
		break;
	case '\'':
		gModelRenderer.convertBoneAnglesPSA(gModelRenderer.mAnimationFrame+1);
		break;
	case ';':
		gModelRenderer.convertBoneAnglesPSA(gModelRenderer.mAnimationFrame-1);
		break;
	case 'r':
		gModelRenderer.copyVertices();
		break;
	case 'i':
		gLightPos[1] += 16.0f;
		glLightfv(GL_LIGHT0,   GL_POSITION,  gLightPos);
		break;
	case 'k':
		gLightPos[1] -= 16.0f;
		glLightfv(GL_LIGHT0,   GL_POSITION,  gLightPos);
		break;
	case 'j':
		gLightPos[0] -= 16.0f;
		glLightfv(GL_LIGHT0,   GL_POSITION,  gLightPos);
		break;
	case 'l':
		gLightPos[0] += 16.0f;
		glLightfv(GL_LIGHT0,   GL_POSITION,  gLightPos);
		break;
	case 'w':
		gWireframe = !gWireframe;

		if (gWireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		printf("%sabled wireframe rendering\n", gWireframe ? "En" : "Dis");
		break;
	}
}

///////////////////////////////////////////////////

SDL_Surface *gSDLWindow = NULL;


void updateWindowTitle(char *newTitle)
{
	static char title[64];

	memcpy(title, newTitle, 64);
	title[63] = 0;

	SDL_WM_SetCaption(title, "PSKModel.test");
}


unsigned int getTicks()
{
	return SDL_GetTicks();
}


void swap_buffers()
{
	SDL_GL_SwapBuffers();
}


void event_resize(unsigned int width, unsigned int height)
{
	GLfloat aspect;


	gWidth = width;
	gHeight = height;

	aspect = (GLfloat)width/(GLfloat)height;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, aspect, 8.0f, 4600.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Resize window
	gSDLWindow = SDL_SetVideoMode(width, height, 16, SDL_OPENGL);
}


void event_display(int width, int height)
{
	glClearColor(0.3, 0.3, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	renderScene();

	glFlush();
	swap_buffers();
}


void shutdown_gl()
{
	SDL_Quit();
}


void init_gl(unsigned int width, unsigned int height)
{
	// Print driver support information
	printf("\n\n\t## GL Driver Info 3 ##\n");
	printf("\tVendor     : %s\n", glGetString(GL_VENDOR));
	printf("\tRenderer   : %s\n", glGetString(GL_RENDERER));
	printf("\tVersion    : %s\n", glGetString(GL_VERSION));
	printf("\tExtensions : %s\n\n\n", (char*)glGetString(GL_EXTENSIONS));

	// Setup GL
	glClearColor(0.3, 0.3, 0.5, 1.0);
	event_resize(width, height);
}


int main_gl(int argc, char *argv[])
{
  SDL_Event event;
  unsigned int mkeys, mod, key, flags;
  unsigned int width = gWidth;
  unsigned int height = gHeight;
  bool fullscreen = false;
  char *driver = 0x0;


  // Setup clean up on exit
  atexit(shutdown_gl);

  // NOTE: Removed fullscreen/driver option parser args

  // Create GL context
  SDL_Init(SDL_INIT_VIDEO);
  printf("\n@Created OpenGL Context...\n");

  if (!driver || !driver[0] || SDL_GL_LoadLibrary(driver) < 0)
  {
	  SDL_ClearError();

	  // Fallback 1
	  if (SDL_GL_LoadLibrary("libGL.so") < 0)
	  {
		  SDL_ClearError();

		  // Fallback 2
		  if (SDL_GL_LoadLibrary("libGL.so.1") < 0)
		  {
			  fprintf(stderr, "main_gl> SDL_GL_LoadLibrary failed!\n");
			  fprintf(stderr, "main_gl> Error is [%s].\n", SDL_GetError());
			  exit(1);
		  }
	  }
  }

  flags = SDL_OPENGL;

  if (fullscreen)
  {
	  flags |= SDL_FULLSCREEN;
	  SDL_ShowCursor(SDL_DISABLE);
  }

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    5);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  gSDLWindow = SDL_SetVideoMode(width, height, 16, flags);
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  // Init rendering
  init_gl(width, height);
  initScene(argc, argv);

  printf("\n@Starting event loop...\n");
  updateWindowTitle("Unreal Tournament 2003 Model test");

  for (;;)
  {
	  while (SDL_PollEvent(&event))
	  {
		  switch (event.type)
		  {
		  case SDL_QUIT:
			  exit(0);
			  break;
		  case SDL_MOUSEMOTION:
			  break;
		  case SDL_MOUSEBUTTONDOWN:
		  case SDL_MOUSEBUTTONUP:
			  break;
		  case SDL_KEYDOWN:
			  mkeys = (unsigned int)SDL_GetModState();
			  mod = 0;

			  if (mkeys & KMOD_LSHIFT)
				  mod |= KMOD_LSHIFT;

			  if (mkeys & KMOD_RSHIFT)
				  mod |= KMOD_RSHIFT;

			  if (mkeys & KMOD_LCTRL)
				  mod |= KMOD_LCTRL;

			  if (mkeys & KMOD_RCTRL)
				  mod |= KMOD_RCTRL;

			  if (mkeys & KMOD_LALT)
				  mod |= KMOD_LALT;

			  if (mkeys & KMOD_RALT)
				  mod |= KMOD_RALT;

			  key = event.key.keysym.sym;

			  switch (key)
			  {
			  case SDLK_ESCAPE: // 0x1B, 27d, ESC
				  exit(0);
				  break;
			  case SDLK_RETURN:
				  if (mod & KMOD_LALT)
				  {
					  SDL_ShowCursor(SDL_DISABLE);
					  SDL_WM_ToggleFullScreen(gSDLWindow);
				  }
			  }

			  handleKey(key);
			  break;
		  case SDL_KEYUP:
			  break;
		  case SDL_VIDEORESIZE:
			  event_resize(event.resize.w, event.resize.h);

			  width = event.resize.w;
			  height = event.resize.h;
			  event_display(width, height);
			  break;
		  }
	  }

	  event_display(width, height);
  }

  return 0;
}
#endif


int runPSKModelUnitTest(int argc, char *argv[])
{
	bool gl = false;


	if (argc > 1)
	{
		for (int i = 0; argv[1][i] != 0; ++i)
		{
			switch (argv[1][i])
			{
			case '.':
				gAnim.mFlags = (PSAAnimation::fDebugBones |
									 PSAAnimation::fDebugAnimInfos |
									 PSAAnimation::fDebugKeyFrames);
				gAnim.load(argv[2]);
				break;
			case 'p':
				gAnim.mFlags = (PSAAnimation::fDebugBones |
									 PSAAnimation::fDebugAnimInfos |
									 PSAAnimation::fDebugKeyFrames);
				break;
			case 'g':
				gl = true;
				break;
			case 'w':
				gModel.mFlags |= PSKModel::fDebugWeightLoad;
				break;
			case 'b':
				gModel.mFlags |= PSKModel::fDebugBoneLoad;
				break;
			case 'v':
				gModel.mFlags |= PSKModel::fDebugPointLoad;
				break;
			case 'm':
				gModel.mFlags |= PSKModel::fDebugMattLoad;
				break;
			case 'f':
				gModel.mFlags |= PSKModel::fDebugFaceLoad;
				break;
			case 'u':
				gModel.mFlags |= PSKModel::fDebugUVLoad;
				break;
			case 'h':
				printf("PSKModel.test <options> file.psk [skindir] [file.psa]\n");
				printf("   eg 'PSKModel.test gvmfu Callisto.psk'\n");
				printf("\tl - Load model\n");
				printf("\tg - Start GL visual debugger\n");
				printf("\tv - Debug Vertex loader\n");
				printf("\tm - Debug Material loader\n");
				printf("\tf - Debug Face loader\n");
				printf("\tw - Debug Weight loader\n");
				printf("\tb - Debug Bone loader\n");
				printf("\tu - Debug UV loader\n");
				printf("\tp - Debug PSA loader\n");
				printf("\t. - PSA debugger (PSKModel.test . file.psa)\n");
				printf("\t0 - UMOD debugger (PSKModel.test 0 file.utx)\n");
				printf("\th - Help\n");
				break;
			}
		}

		if (argc > 3)
		{
			/* Load PSK model */
			gModel.load(argv[2]);

			/* Load PSK model to renderer */
			gModelRenderer.mFlags |= PSKModelRenderer::fRenderFaces;
			gModelRenderer.setModel(&gModel);

			if (argc > 4)
			{
				gAnim.load(argv[4]);
				gModelRenderer.setAnimation(&gAnim);
			}

			if (gl)
			{
#ifdef USING_OPENGL
				main_gl(argc, argv);
#else
				printf("Build not OpenGL enabled\n");
#endif
			}
		}
	}
	else
	{
		printf("'%s h' for help\n", argv[0]);
	}

	return 0;
}


int main(int argc, char *argv[])
{
	printf("[PSKModel class test]\n");
	runPSKModelUnitTest(argc, argv);

	return 0;
}
#endif
