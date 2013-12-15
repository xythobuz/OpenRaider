/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*===============================================================
 * Project: Mongoose Tool Kit
 * Author : Terry 'Mongoose' Hendrix
 * Website: http://www.westga.edu/~stu7440/
 * Email  : stu7440@westga.edu
 * Object : 
 * Comment: 3d math util functions.
 *
 *         FIXME: OpenGL matrices are col major OOPS!  
 *                some are using col some aren't
 * 
 *-- History -----------------------------------------------
 *
 * 2001.11.23:
 * Mongoose - Move to OpenGL compatible matrices
 *
 * 2000-03-25:
 * Mongoose - Recreated from GooseEgg prototyping
 *
 * 1999-06-14:
 * Mongoose - Created
 ==============================================================*/

#include <math.h>
#include <stdio.h>

#include "mtk3d.h"


/*
double mtk3d_norm3d(double u[3])
{
  return(sqrt(u[0]*u[0]+u[1]*u[1]+u[2]*u[2]));
}

double mtk3d_norm2d(double u[2])
{
  return(sqrt(u[0]*u[0]+u[1]*u[1]));
}
*/

vec_t mtkRandNum(vec_t from, vec_t to)
{
	return from + (to*rand()/(RAND_MAX+1.0));
}


// Heh, this is scary... try to avoid many indexing computations
void mtkVectorMatrixMult4dv(double v[4], matrix_t m, double result[4])
{
#ifdef OBSOLETE
	int i, j;

	for (i = 0; i < 4; i++)
	{
		result[i] = 0;
		
		for (j = 0; j < 4; j++)
		{
			result[i] += m[i][j] * v[j];
		}
	}
#else
	/////////////////////////////////////////////
	// 0,0 - 0;   0,1 - 1;   0,2 - 2;   0,3 - 3
	// 1,0 - 4;   1,1 - 5;   1,2 - 6;   1,3 - 7
	// 2,0 - 8;   2,1 - 9;   2,2 - 10;  2,3 - 11
	// 3,0 - 12;  3,1 - 13;  3,2 - 14;  3,3 - 15
	/////////////////////////////////////////////


	// Col major?
	//result[0] = m[ 0] * v[0] + m[ 4] * v[1] + m[ 8] * v[2] + m[12] * v[3];
	//result[1] = m[ 1] * v[0] + m[ 5] * v[1] + m[ 9] * v[2] + m[13] * v[3];
	//result[2] = m[ 2] * v[0] + m[ 6] * v[1] + m[10] * v[2] + m[14] * v[3];

   result[0] = m[ 0] * v[0] + m[ 1] * v[1] + m[ 2] * v[2] + m[ 3] * v[3];
	result[1] = m[ 4] * v[0] + m[ 5] * v[1] + m[ 6] * v[2] + m[ 7] * v[3];
	result[2] = m[ 8] * v[0] + m[ 9] * v[1] + m[10] * v[2] + m[11] * v[3];
	result[3] = m[12] * v[0] + m[13] * v[1] + m[14] * v[2] + m[15] * v[3];
#endif
}


void mtkMatrixPrint(matrix_t matrix)
{
#ifdef OBSOLETE
  printf("matrix = {\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n}\n",
			matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
			matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
			matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
			matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
#else
  printf("matrix = {\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n}\n",
			matrix[ 0], matrix[ 1], matrix[ 2], matrix[ 3],
			matrix[ 4], matrix[ 5], matrix[ 6], matrix[ 7],
			matrix[ 8], matrix[ 9], matrix[10], matrix[11],
			matrix[12], matrix[13], matrix[14], matrix[15]);
#endif
}


vec_t mtkDist2d(vec_t x, vec_t y, vec_t x2, vec_t y2)
{
   return (sqrt( ((x2 - x)*(x2 - x)) + ((y2 - y)*(y2 - y)) ));
}


vec_t mtkDist3d(vec_t x, vec_t y, vec_t z,
		 vec_t x2, vec_t y2, vec_t z2)
{
   return (sqrt( ((x2 - x)*(x2 - x))+((y2 - y)*(y2 - y))+((z2 - z)*(z2 - z))));
}

vec_t mtkDegToRad(vec_t degrees)
{
   return ((degrees / 180.0) * MTK_PI); // degrees * (180.0 / MTK_PI);
}

vec_t mtkRadToDeg(vec_t rad)
{
   return ((rad / MTK_PI) * 180.0); // rad * (MTK_PI / 180.0);
}


void mtkVectorMatrixMult(vec3_t v, matrix_t m, vec3_t result)
{
   result[0] = m[ 0] * v[0] + m[ 1] * v[1] + m[ 2] * v[2] + m[ 3] * v[3];
	result[1] = m[ 4] * v[0] + m[ 5] * v[1] + m[ 6] * v[2] + m[ 7] * v[3];
	result[2] = m[ 8] * v[0] + m[ 9] * v[1] + m[10] * v[2] + m[11] * v[3];
}


// Vector from two points
void mtkVectorSubtract(vec3_t a, vec3_t b, vec3_t result)
{
	result[0] = a[0] - b[0];
	result[1] = a[1] - b[1];
	result[2] = a[2] - b[2];
}


void mtkVectorAdd(vec3_t a, vec3_t b, vec3_t result)
{
	result[0] = a[0] + b[0];
	result[1] = a[1] + b[1];
	result[2] = a[2] + b[2];
}


// Magnitude
vec_t mtkVectorNorm(vec3_t v)
{
	return (vec_t)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}


void mtkVectorNormalize(vec3_t v, vec3_t result)
{
	vec_t mag = mtkVectorNorm(v);


	result[0] = v[0] / mag;
	result[1] = v[1] / mag;
	result[2] = v[2] / mag;
}


void mtkVectorCrossProduct(vec3_t a, vec3_t b, vec3_t normal)
{
	normal[0] = a[1]*b[2] - a[2]*b[1];
	normal[0] = a[2]*b[0] - a[0]*b[2];
	normal[0] = a[0]*b[1] - a[1]*b[0];
}


vec_t mtkVectorDotProduct(vec3_t a, vec3_t b)
{
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}


// Heh, this is scary... try to avoid many indexing computations
void mtkMatrixMult(matrix_t m1, matrix_t m2, matrix_t mp)
{
#ifdef MTK3D_MULT_DIM_MATRIX
   int i,j;
	
	
   for (i = 0; i < 4; i++)
   {
		for (j = 0 ; j < 4; j++)
		{
			mp[i][j] = m1[i][0] * m2[0][j] +
			           m1[i][1] * m2[1][j] +
			           m1[i][2] * m2[2][j] +
			           m1[i][3] * m2[3][j];
		}
   }
#else
	/////////////////////////////////////////////
	// 0,0 - 0;   0,1 - 1;   0,2 - 2;   0,3 - 3
	// 1,0 - 4;   1,1 - 5;   1,2 - 6;   1,3 - 7
	// 2,0 - 8;   2,1 - 9;   2,2 - 10;  2,3 - 11
	// 3,0 - 12;  3,1 - 13;  3,2 - 14;  3,3 - 15
	/////////////////////////////////////////////

	mp[ 0] = m1[0] * m2[0] + m1[1] * m2[4] + m1[2] * m2[ 8] + m1[3] * m2[12];
	mp[ 1] = m1[0] * m2[1] + m1[1] * m2[5] + m1[2] * m2[ 9] + m1[3] * m2[13]; 
	mp[ 2] = m1[0] * m2[2] + m1[1] * m2[6] + m1[2] * m2[10] + m1[3] * m2[14]; 
	mp[ 3] = m1[0] * m2[3] + m1[1] * m2[7] + m1[2] * m2[11] + m1[3] * m2[15];

	mp[ 4] = m1[4] * m2[0] + m1[5] * m2[4] + m1[6] * m2[ 8] + m1[7] * m2[12];
	mp[ 5] = m1[4] * m2[1] + m1[5] * m2[5] + m1[6] * m2[ 9] + m1[7] * m2[13]; 
	mp[ 6] = m1[4] * m2[2] + m1[5] * m2[6] + m1[6] * m2[10] + m1[7] * m2[14]; 
	mp[ 7] = m1[4] * m2[3] + m1[5] * m2[7] + m1[6] * m2[11] + m1[7] * m2[15];

	mp[ 8] = m1[8] * m2[0] + m1[9] * m2[4] + m1[10] * m2[ 8] + m1[11] * m2[12];
	mp[ 9] = m1[8] * m2[1] + m1[9] * m2[5] + m1[10] * m2[ 9] + m1[11] * m2[13]; 
	mp[10] = m1[8] * m2[2] + m1[9] * m2[6] + m1[10] * m2[10] + m1[11] * m2[14]; 
	mp[11] = m1[8] * m2[3] + m1[9] * m2[7] + m1[10] * m2[11] + m1[11] * m2[15];

	mp[12] = m1[12] * m2[0] + m1[13] * m2[4] + m1[14] * m2[ 8] + m1[15] * m2[12];
	mp[13] = m1[12] * m2[1] + m1[13] * m2[5] + m1[14] * m2[ 9] + m1[15] * m2[13]; 
	mp[14] = m1[12] * m2[2] + m1[13] * m2[6] + m1[14] * m2[10] + m1[15] * m2[14]; 
	mp[15] = m1[12] * m2[3] + m1[13] * m2[7] + m1[14] * m2[11] + m1[15] * m2[15];
#endif
}


void mtkMatrixConvert3x3ToMtk(float *m3x3, matrix_t dest)
{
	if (!m3x3)
	{
		mtkMatrixIdentity(dest);
		return;
	}

	dest[0] = m3x3[0];   dest[1] = m3x3[1];   dest[2 ] = m3x3[2];
	dest[4] = m3x3[3];   dest[5] = m3x3[4];   dest[6 ] = m3x3[5];
	dest[8] = m3x3[6];   dest[9] = m3x3[7];   dest[10] = m3x3[8];

	// yeah, wolf
	dest[11] = dest[12] = dest[13] = dest[14] = 0.0f;
	dest[15] = 1.0f;
}


void mtkMatrixCopy(matrix_t source, matrix_t dest)
{
#ifdef OBSOLETE
   int i,j;


   for(i = 0; i < 4; i++)
   {
      for(j = 0 ; j < 4; j++)
      {
			dest[i][j] = source[i][j];
      }
   }
#else
	dest[ 0] = source[ 0];
	dest[ 1] = source[ 1];
	dest[ 2] = source[ 2];
	dest[ 3] = source[ 3];

	dest[ 4] = source[ 4];
	dest[ 5] = source[ 5];
	dest[ 6] = source[ 6];
	dest[ 7] = source[ 7];

	dest[ 8] = source[8];
	dest[ 9] = source[9];
	dest[10] = source[10];
	dest[11] = source[11];

	dest[12] = source[12];
	dest[13] = source[13];
	dest[14] = source[14];
	dest[15] = source[15];
#endif
}


void mtkMatrixTransform(matrix_t t, vec3_t p)
{
   vec_t m1[4];


   m1[0] = p[0];
   m1[1] = p[1];
   m1[2] = p[2];
   m1[3] = 1;

#ifdef OBSOLETE
   p[0] = t[0][0]*m1[0] + t[1][0]*m1[1] + t[2][0]*m1[2] + t[3][0]*m1[3];
   p[1] = t[0][1]*m1[0] + t[1][1]*m1[1] + t[2][1]*m1[2] + t[3][1]*m1[3];
   p[2] = t[0][2]*m1[0] + t[1][2]*m1[1] + t[2][2]*m1[2] + t[3][2]*m1[3];
#else
	p[0] = t[0] * m1[0] + t[4] * m1[1] + t[ 8] * m1[2] + t[12] * m1[3];
	p[1] = t[1] * m1[0] + t[5] * m1[1] + t[ 9] * m1[2] + t[13] * m1[3];
	p[2] = t[2] * m1[0] + t[6] * m1[1] + t[10] * m1[2] + t[14] * m1[3];
#endif
}


void mtkMatrixIdentity(matrix_t matrix)
{
#ifdef OBSOLETE
   matrix[0][0] = 1; matrix[0][1] = 0; matrix[0][2] = 0; matrix[0][3] = 0;
   matrix[1][0] = 0; matrix[1][1] = 1; matrix[1][2] = 0; matrix[1][3] = 0;
   matrix[2][0] = 0; matrix[2][1] = 0; matrix[2][2] = 1; matrix[2][3] = 0;
   matrix[3][0] = 0; matrix[3][1] = 0; matrix[3][2] = 0; matrix[3][3] = 1;
#else
   matrix[ 0] = 1; matrix[ 1] = 0; matrix[ 2] = 0; matrix[ 3] = 0;
   matrix[ 4] = 0; matrix[ 5] = 1; matrix[ 6] = 0; matrix[ 7] = 0;
   matrix[ 8] = 0; matrix[ 9] = 0; matrix[10] = 1; matrix[11] = 0;
   matrix[12] = 0; matrix[13] = 0; matrix[14] = 0; matrix[15] = 1;
#endif
}


// FIXME: Might want to make scale matrix, etc consts
void mtkMatrixScale(matrix_t matrix, vec_t sx, vec_t sy, vec_t sz)
{
   matrix_t tmp, smatrix;


#ifdef OBSOLETE
   smatrix[0][0] = sx;smatrix[0][1] = 0; smatrix[0][2] = 0; smatrix[0][3] = 0;
   smatrix[1][0] = 0; smatrix[1][1] = sy;smatrix[1][2] = 0; smatrix[1][3] = 0;
   smatrix[2][0] = 0; smatrix[2][1] = 0; smatrix[2][2] = sz;smatrix[2][3] = 0;
   smatrix[3][0] = 0; smatrix[3][1] = 0; smatrix[3][2] = 0; smatrix[3][3] = 1;
#else
   smatrix[ 0] = sx;smatrix[ 1] = 0; smatrix[ 2] = 0; smatrix[ 3] = 0;
   smatrix[ 4] = 0; smatrix[ 5] = sy;smatrix[ 6] = 0; smatrix[ 7] = 0;
   smatrix[ 8] = 0; smatrix[ 9] = 0; smatrix[10] = sz;smatrix[11] = 0;
   smatrix[12] = 0; smatrix[13] = 0; smatrix[14] = 0; smatrix[15] = 1;
#endif

   mtkMatrixMult(matrix, smatrix, tmp);
   mtkMatrixCopy(tmp, matrix);
}

void mtkMatrixRotate(matrix_t matrix, vec_t ax, vec_t ay, vec_t az)
{
   matrix_t xmat, ymat, zmat, tmp, tmp2;


#ifdef OBSOLETE
   xmat[0][0]=1;        xmat[0][1]=0;        xmat[0][2]=0;        xmat[0][3]=0;
   xmat[1][0]=0;        xmat[1][1]=cos(ax);  xmat[1][2]=sin(ax);  xmat[1][3]=0;
   xmat[2][0]=0;        xmat[2][1]=-sin(ax); xmat[2][2]=cos(ax);  xmat[2][3]=0;
   xmat[3][0]=0;        xmat[3][1]=0;        xmat[3][2]=0;        xmat[3][3]=1;

   ymat[0][0]=cos(ay);  ymat[0][1]=0;        ymat[0][2]=-sin(ay); ymat[0][3]=0;
   ymat[1][0]=0;        ymat[1][1]=1;        ymat[1][2]=0;        ymat[1][3]=0;
   ymat[2][0]=sin(ay);  ymat[2][1]=0;        ymat[2][2]=cos(ay);  ymat[2][3]=0;
   ymat[3][0]=0;        ymat[3][1]=0;        ymat[3][2]=0;        ymat[3][3]=1;

   zmat[0][0]=cos(az);  zmat[0][1]=sin(az);  zmat[0][2]=0;        zmat[0][3]=0;
   zmat[1][0]=-sin(az); zmat[1][1]=cos(az);  zmat[1][2]=0;        zmat[1][3]=0;
   zmat[2][0]=0;        zmat[2][1]=0;        zmat[2][2]=1;        zmat[2][3]=0;
   zmat[3][0]=0;        zmat[3][1]=0;        zmat[3][2]=0;        zmat[3][3]=1;
#else
   xmat[ 0]=1;        xmat[ 1]=0;        xmat[ 2]=0;        xmat[ 3]=0;
   xmat[ 4]=0;        xmat[ 5]=cos(ax);  xmat[ 6]=sin(ax);  xmat[ 7]=0;
   xmat[ 8]=0;        xmat[ 9]=-sin(ax); xmat[10]=cos(ax);  xmat[11]=0;
   xmat[12]=0;        xmat[13]=0;        xmat[14]=0;        xmat[15]=1;

   ymat[ 0]=cos(ay);  ymat[ 1]=0;        ymat[ 2]=-sin(ay); ymat[ 3]=0;
   ymat[ 4]=0;        ymat[ 5]=1;        ymat[ 6]=0;        ymat[ 7]=0;
   ymat[ 8]=sin(ay);  ymat[ 9]=0;        ymat[10]=cos(ay);  ymat[11]=0;
   ymat[12]=0;        ymat[13]=0;        ymat[14]=0;        ymat[15]=1;

   zmat[ 0]=cos(az);  zmat[ 1]=sin(az);  zmat[ 2]=0;        zmat[ 3]=0;
   zmat[ 4]=-sin(az); zmat[ 5]=cos(az);  zmat[ 6]=0;        zmat[ 7]=0;
   zmat[ 8]=0;        zmat[ 9]=0;        zmat[10]=1;        zmat[11]=0;
   zmat[12]=0;        zmat[13]=0;        zmat[14]=0;        zmat[15]=1;
#endif

   mtkMatrixMult(matrix, ymat, tmp);
   mtkMatrixMult(tmp, xmat, tmp2);
   mtkMatrixMult(tmp2, zmat, matrix);
}

void mtkMatrixTranslate(matrix_t matrix, vec_t tx, vec_t ty, vec_t tz)
{
   matrix_t tmat, tmp;

#ifdef OBSOLETE
   tmat[0][0]=1;  tmat[0][1]=0;  tmat[0][2]=0;  tmat[0][3]=0;
   tmat[1][0]=0;  tmat[1][1]=1;  tmat[1][2]=0;  tmat[1][3]=0;
   tmat[2][0]=0;  tmat[2][1]=0;  tmat[2][2]=1;  tmat[2][3]=0;
   tmat[3][0]=tx; tmat[3][1]=ty; tmat[3][2]=tz; tmat[3][3]=1;
#else
   tmat[ 0]=1;  tmat[ 1]=0;  tmat[ 2]=0;  tmat[ 3]=0;
   tmat[ 4]=0;  tmat[ 5]=1;  tmat[ 6]=0;  tmat[ 7]=0;
   tmat[ 8]=0;  tmat[ 9]=0;  tmat[10]=1;  tmat[11]=0;
   tmat[12]=tx; tmat[13]=ty; tmat[14]=tz; tmat[15]=1;
#endif

   mtkMatrixMult(matrix, tmat, tmp);
   mtkMatrixCopy(tmp, matrix);
}

////////////////////////////////

void mtkQuaternionCopy(quaternion_t source, quaternion_t dest)
{
  dest.x = source.x;
  dest.y = source.y;
  dest.z = source.z;
  dest.w = source.w;
}


void mtkQuaternionSlerp(quaternion_t qI, quaternion_t qA, quaternion_t qB, 
								float time)
{
	/*******************************************************************
	 * Spherical Linear Interpolation algorthim
	 *-----------------------------------------------------------------
	 *
	 * Interpolate between A and B rotations ( Find qI )
	 *
	 * qI = (((qB . qA)^ -1)^ Time) qA
	 *
	 * http://www.magic-software.com/Documentation/quat.pdf
	 *
	 * Thanks to digiben for algorithms and basis of the notes in 
	 * this func
	 *
	 *******************************************************************/

	float result, scaleA, scaleB, theta, sinTheta;


	// Don't bother if it's the same rotation, it's the same as the result
	if (&qA == &qB || 
		 qA.x == qB.x && qA.y == qB.y && qA.z == qB.z && qA.w == qB.w) 
	{
		mtkQuaternionCopy(qA, qI);
		return;
	}

	// A . B 
	result = (qA.x * qB.x) + (qA.y * qB.y) + (qA.z * qB.z) + (qA.w * qB.w);
  
	// If the dot product is less than 0, the angle is greater than 90 degrees
	if (result < 0.0f)
	{
		// Negate quaternion B and the result of the dot product
		qB.x = -qB.x;
		qB.y = -qB.y;
		qB.z = -qB.z;
		qB.w = -qB.w;
	  
		result = -result;
	}

	// Set the first and second scale for the interpolation
	scaleA = 1 - time;
	scaleB = time;

	// Next, we want to actually calculate the spherical interpolation.  Since this
	// calculation is quite computationally expensive, we want to only perform it
	// if the angle between the 2 quaternions is large enough to warrant it.  If the
	// angle is fairly small, we can actually just do a simpler linear interpolation
	// of the 2 quaternions, and skip all the complex math.  We create a "delta" value
	// of 0.1 to say that if the cosine of the angle (result of the dot product) between
	// the 2 quaternions is smaller than 0.1, then we do NOT want to perform the full on 
	// interpolation using.  This is because you won't really notice the difference.
	
	// Check if the angle between the 2 quaternions was big enough 
	// to warrant such calculations
	if (1 - result > 0.1f)
	{
		// Get the angle between the 2 quaternions, and then
		// store the sin() of that angle
		theta = (float)acos(result);
		sinTheta = (float)sin(theta);

		// Calculate the scale for qA and qB, according to
		// the angle and it's sine value
		scaleA = (float)sin((1 - time) * theta) / sinTheta;
		scaleB = (float)sin((time * theta)) / sinTheta;
	}	

	// Calculate the x, y, z and w values for the quaternion by using a special
	// form of linear interpolation for quaternions.
	qI.x = (scaleA * qA.x) + (scaleB * qB.x);
	qI.y = (scaleA * qA.y) + (scaleB * qB.y);
	qI.z = (scaleA * qA.z) + (scaleB * qB.z);
	qI.w = (scaleA * qA.w) + (scaleB * qB.w);
}


void mtkQuaternionFrom3x3Matrix(float *m3x3, quaternion_t q)
{
	matrix_t matrix;


	mtkMatrixConvert3x3ToMtk(m3x3, matrix);
	mtkQuaternionFromMatrix(matrix, q);
}


void mtkQuaternionFromMatrix(matrix_t matrix, quaternion_t q)
{
	float diagonal, scale;


	diagonal = matrix[0] + matrix[5] + matrix[10] + 1;
	scale = 0.0f;

	mtkQuaternionIdentity(q);

  // diagonal is greater than 'zero'
  if (diagonal > 0.00000001)
  {
	  scale = float(sqrt(diagonal) * 2);

	  q.x = (matrix[9] - matrix[6]) / scale;
	  q.y = (matrix[2] - matrix[8]) / scale;
	  q.z = (matrix[4] - matrix[1]) / scale;
	  q.w = 0.25f * scale;
  }
  else 
  {
	  // If the first element of the diagonal is the greatest value
	  if (matrix[0] > matrix[5] && matrix[0] > matrix[10])  
	  {	
		  // Find the scale according to the first element, and double it
		  scale  = (float)sqrt(1.0f + matrix[0] - matrix[5] - matrix[10]) * 2.0f;

		  // Calculate the x, y, x and w of the quaternion
		  q.x = 0.25f * scale;
		  q.y = (matrix[4] + matrix[1]) / scale;
		  q.z = (matrix[2] + matrix[8]) / scale;
		  q.w = (matrix[9] - matrix[6]) / scale;	
	  } 
	  // Else if the second element of the diagonal is the greatest value
	  else if (matrix[5] > matrix[10]) 
	  {
		  // Find the scale according to the second element, and double it
		  scale  = (float)sqrt(1.0f + matrix[5] - matrix[0] - matrix[10]) * 2.0f;
			
		  // Calculate the x, y, x and w of the quaternion
		  q.x = (matrix[4] + matrix[1]) / scale;
		  q.y = 0.25f * scale;
		  q.z = (matrix[9] + matrix[6]) / scale;
		  q.w = (matrix[2] - matrix[8]) / scale;
	  } 
	  // Else the third element of the diagonal is the greatest value
	  else 
	  {	
		  // Find the scale according to the third element, and double it
		  scale  = (float)sqrt(1.0f + matrix[10] - matrix[0] - matrix[5]) * 2.0f;

		  // Calculate the x, y, x and w of the quaternion
		  q.x = (matrix[2] + matrix[8]) / scale;
		  q.y = (matrix[9] + matrix[6]) / scale;
		  q.z = 0.25f * scale;
		  q.w = (matrix[4] - matrix[1]) / scale;
	  }
  }
}


void mtkQuaternionToMatrix(quaternion_t q, matrix_t m)
{
#ifdef OBSOLETE  
  float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

  
  x2 = q.x + q.x;  y2 = q.y + q.y;  z2 = q.z + q.z;
  xx = q.x * x2;   xy = q.x * y2;   xz = q.x * z2;
  yy = q.y * y2;   yz = q.y * z2;   zz = q.z * z2;
  wx = q.w * x2;   wy = q.w * y2;   wz = q.w * z2;

  m[ 0] = 1.0 - (yy + zz);
  m[ 1] = xy - wz;
  m[ 2] = xz + wy;
  m[ 3] = 0.0;
  
  m[ 4] = xy + wz;
  m[ 5] = 1.0 - (xx + zz);
  m[ 6] = yz - wx;
  m[ 7] = 0.0;
  
  m[ 8] = xz - wy;
  m[ 9] = yz + wx;
  m[10] = 1.0 - (xx + yy);
  m[11] = 0.0;
  
  m[12] = 0;
  m[13] = 0;
  m[14] = 0;
  m[15] = 1;
#else
  m[ 0] = 1.0f - 2.0f * (q.y*q.y + q.z*q.z);
  m[ 1] = 2.0f * (q.x*q.y - q.w*q.z);
  m[ 2] = 2.0f * (q.x*q.z + q.w*q.y);
  m[ 3] = 0.0f;
  
  m[ 4] = 2.0f * (q.x*q.y + q.w*q.z);
  m[ 5] = 1.0f - 2.0f * (q.x*q.x + q.z*q.z);
  m[ 6] = 2.0f * (q.y*q.z - q.w*q.x);
  m[ 7] = 0.0f;
  
  m[ 8] = 2.0f * (q.x*q.z - q.w*q.y);
  m[ 9] = 2.0f * (q.y*q.z + q.w*q.x);
  m[10] = 1.0 - 2.0f * (q.x*q.x + q.y*q.y);
  m[11] = 0.0f;
  
  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = 1.0f;
#endif
}


void mtkQuaternionIdentity(quaternion_t q)
{
  q.w = 1.0;
  q.x = 0.0;
  q.y = 0.0;
  q.z = 0.0;
}


void mtkQuaternionSet(quaternion_t q, float angle, float x, float y, float z)
{
  float temp, dist;

  // Normalize
  temp = x*x + y*y + z*z;

  dist = (float)(1.0 / sqrt(temp));

  x *= dist;
  y *= dist;
  z *= dist;

  q.x = x;
  q.y = y;
  q.z = z;

  q.w = (float)cos(angle / 2.0f);
}


void mtkQuaternionMult(quaternion_t a, quaternion_t b, quaternion_t result)
{
  result.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
  result.y = a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z;
  result.z = a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x;
  result.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;

  // Quaternion needs to be a unit quaternion
  mtkQuaternionNormalize(result);  
}


void mtkQuaternionNormalize(quaternion_t q)
{
	float dist, square;


	square = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	
	if (square > 0.00000001)
	{
		dist = (float)(1.0 / sqrt(square));
	}
	else 
	{
		dist = 1;
	}

	q.x *= dist;
	q.y *= dist;
	q.z *= dist;
	q.w *= dist; 
}

