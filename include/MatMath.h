/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : Freyja
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  :
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments:
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History ------------------------------------------------
 *
 * 2002.05.11:
 * Mongoose - Created
 ================================================================*/


#ifndef _MATMATH_H
#define _MATMATH_H

#define HEL_PI           3.14159265358979323846  /* pi */
#define HEL_PI_OVER_2    1.57079632679489661923  /* pi/2 */
#define HEL_2_PI         6.28318530717958647692  /* pi*2 */
#define HEL_PI_OVER_4    0.78539816339744830962  /* pi/4 */
#define HEL_PI_OVER_180  0.017453292519943295    /* pi/180 */
#define HEL_180_OVER_PI  57.295779513082323      /* 180/pi */

#define HEL_RAD_TO_DEG(x) ((x) * HEL_180_OVER_PI)
#define HEL_DEG_TO_RAD(x) ((x) * HEL_PI_OVER_180)

typedef float vec_t;
typedef float vec2_t[2];
typedef float vec3_t[3];
typedef float vec4_t[4];
typedef vec_t matrix_t[16];  /* Used as _Column_major_ in every class now! */


int helIntersectionLineAndPolygon(vec3_t intersect,
											 vec3_t p1, vec3_t p2,
											 unsigned int vertexCount, vec3_t *ploygon);
/*------------------------------------------------------
 * Pre  : Given P1 and P2 of line segment and
 *        Vertex count and ploygon with count vertices
 *
 *        Only supports triangles and coplanar quads
 *
 * Post : Returns intersect point or 0 if none
 *
 *-- History ------------------------------------------
 *
 * 2003.05.30:
 * Mongoose - Created
 ------------------------------------------------------*/

vec_t helDistToSphereFromPlane3v(vec3_t center,	vec_t radius, vec4_t plane);
/*------------------------------------------------------
 * Pre  : Given center and radius of sphere and a plane
 * Post : Returns distance from sphere to plane
 *
 *-- History ------------------------------------------
 *
 * 1999.06.14:
 * Mongoose - Created, from mtk3d
 ------------------------------------------------------*/

vec_t helDistToBboxFromPlane3v(vec3_t min, vec3_t max, vec4_t plane);
/*------------------------------------------------------
 * Pre  : Given min and max points of a bounding box
 *        and a plane
 *
 * Post : Returns distance from box to plane
 *
 *-- History ------------------------------------------
 *
 * 1999.06.14:
 * Mongoose - Created, from mtk3d
 ------------------------------------------------------*/

vec_t helDist3v(vec3_t a, vec3_t b);
/*------------------------------------------------------
 * Pre  : Given point A and B
 * Post : Returns length of line segment
 *
 *-- History ------------------------------------------
 *
 * 1999.06.14:
 * Mongoose - Created, from mtk3d
 ------------------------------------------------------*/

void helMidpoint3v(vec3_t a, vec3_t b, vec3_t mid);
/*------------------------------------------------------
 * Pre  : Given point A and B
 * Post : Returns midpoint of line segment
 *
 *-- History ------------------------------------------
 *
 * 1999.06.14:
 * Mongoose - Created, from mtk3d
 ------------------------------------------------------*/


vec_t helRandomNum(vec_t from, vec_t to);

#endif
