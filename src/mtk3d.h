/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*===============================================================
 * Project: Mongoose Tool Kit
 * Author : Terry 'Mongoose' Hendrix
 * Website: http://www.westga.edu/~stu7440/
 * Email  : stu7440@westga.edu
 * Object : 
 * Comment: 3d functions.
 * 
 *-- History -----------------------------------------------
 *
 * 2001.11.23:
 * Mongoose - GL compatible matrix type
 *
 * 2001.07.05:
 * Mongoose - Backport of OpenRaider mtk3d
 *
 * 1999.06.14:
 * Mongoose - Created
 ==============================================================*/

#ifndef GUARD__MONGOOSE_MTK_3D_MTK3D_H
#define GUARD__MONGOOSE_MTK_3D_MTK3D_H

#include <stdlib.h>


#define MTK_PI           3.14159265358979323846  /* pi */
#define MTK_PI_OVER_2    1.57079632679489661923  /* pi/2 */
#define MTK_2_PI         6.28318530717958647692  /* pi*2 */
#define MTK_PI_OVER_4    0.78539816339744830962  /* pi/4 */

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t matrix_t[16];


typedef struct quaternion_s
{
  float w, x, y, z;

} quaternion_t;


vec_t mtkRandNum(vec_t from, vec_t to);


vec_t mtkDist2d(vec_t x, vec_t y, vec_t x2, vec_t y2);

vec_t mtkDist3d(vec_t x,  vec_t y,  vec_t z,
					 vec_t x2, vec_t y2, vec_t z2);

//////////////////////////////////////////////////

vec_t mtkDegToRad(vec_t degrees);
vec_t mtkRadToDeg(vec_t rad);

//////////////////////////////////////////////////


void mtkVectorMatrixMult4dv(double v[4], matrix_t m, double result[4]);
void mtkVectorMatrixMult(vec3_t v, matrix_t m, vec3_t result);
void mtkVectorSubtract(vec3_t a, vec3_t b, vec3_t result);
void mtkVectorAdd(vec3_t a, vec3_t b, vec3_t result);
vec_t mtkVectorNorm(vec3_t v);
void mtkVectorNormalize(vec3_t v, vec3_t result); // v can be result
void mtkVectorCrossProduct(vec3_t a, vec3_t b, vec3_t normal);
vec_t mtkVectorDotProduct(vec3_t a, vec3_t b);

void mtkMatrixConvert3x3ToMtk(float *m3x3, matrix_t dest);
void mtkMatrixMult(matrix_t m1, matrix_t m2, matrix_t mp);
void mtkMatrixCopy(matrix_t source, matrix_t dest);
void mtkMatrixTransform(matrix_t t, vec3_t p);
void mtkMatrixIdentity(matrix_t matrix);
void mtkMatrixScale(matrix_t matrix, vec_t sx, vec_t sy, vec_t sz);
void mtkMatrixRotate(matrix_t matrix, vec_t ax, vec_t ay, vec_t az);
void mtkMatrixTranslate(matrix_t matrix, vec_t tx, vec_t ty, vec_t tz);


//////////////////////////////////////////////////


void mtkQuaternionIdentity(quaternion_t q);
void mtkQuaternionCopy(quaternion_t source, quaternion_t dest);
void mtkQuaternionSlerp(quaternion_t qI, quaternion_t qA, quaternion_t qB, 
								float time);
void mtkQuaternionFrom3x3Matrix(float *m3x3, quaternion_t q);
void mtkQuaternionFromMatrix(matrix_t m, quaternion_t q);
void mtkQuaternionToMatrix(quaternion_t q, matrix_t m);
void mtkQuaternionSet(quaternion_t q, float angle, float x, float y, float z);
void mtkQuaternionMult(quaternion_t a, quaternion_t b, quaternion_t result);
void mtkQuaternionNormalize(quaternion_t q);

#endif
