/*!
 *
 * \file include/MatMath.h
 * \brief Vector and Matrix math
 *
 * \author Mongoose
 */

#include <math.h>

#ifndef _MATMATH_H
#define _MATMATH_H

#define HEL_PI           M_PI //!< pi
#define HEL_2_PI         (M_PI * 2.0) //!< pi*2
#define HEL_PI_OVER_4    M_PI_4 //!< pi/4
#define HEL_PI_OVER_180  (M_PI / 180.0) //!< pi/180
#define HEL_180_OVER_PI  (180.0 / M_PI) //!< 180/pi

#define HEL_RAD_TO_DEG(x) ((x) * HEL_180_OVER_PI) //!< Convert radians to degrees
#define HEL_DEG_TO_RAD(x) ((x) * HEL_PI_OVER_180) //!< Convert degrees to radians

typedef float vec_t;        //!< 1D Vector, aka float
typedef float vec2_t[2];    //!< 2D Vector
typedef float vec3_t[3];    //!< 3D Vector
typedef float vec4_t[4];    //!< 4D Vector
typedef vec_t matrix_t[16]; //!< Used as _Column_major_ in every class now!

/*!
 * \fixme Add Documentation or remove, as it is unused?
 */
vec_t helIntersectionOfAbstractSpheres(vec3_t centerA, vec_t radiusA, vec3_t centerB, vec_t radiusB);

/*!
 * \fixme Add Documentation or remove, as it is unused?
 */
int helIntersectionOfAbstractSphereAndLine(vec3_t center, vec_t radius, vec3_t posA, vec3_t posB, vec3_t intersectionA, vec3_t intersectionB);

/*!
 * \brief Calculate Intersection of a line and a polygon
 * \param intersect Where the intersection is stored, if it exists
 * \param p1 First point of line segment
 * \param p2 Second point of line segment
 * \param polygon polygon vertex array (0 to 2 are used)
 * \returns 0 if there is no intersection
 */
int helIntersectionLineAndPolygon(vec3_t intersect, vec3_t p1, vec3_t p2, vec3_t *polygon);

/*!
 * \brief Calculate the distance from a sphere to a plane
 * \param center Center of sphere
 * \param radius Radius of sphere
 * \param plane Plane
 * \returns distance
 */
vec_t helDistToSphereFromPlane3v(vec3_t center, vec_t radius, vec4_t plane);

/*!
 * \brief Calculate the distance from a box to a plane
 * \param min Minimum Point of a bounding box
 * \param max Maximum Point of a bounding box
 * \param plane Plane
 * \returns distance
 */
vec_t helDistToBboxFromPlane3v(vec3_t min, vec3_t max, vec4_t plane);

/*!
 * \brief Calculate the length of a line segment / the distance between two points
 * \param a First point
 * \param b Second point
 * \returns distance/length
 */
vec_t helDist3v(vec3_t a, vec3_t b);

/*!
 * \brief Calculates the midpoint between two points / of a line segment
 * \param a First point
 * \param b Second point
 * \param mid Midpoint will be stored here
 */
void helMidpoint3v(vec3_t a, vec3_t b, vec3_t mid);

/*!
 * \brief Calculates a pseudo-random number
 * \param from Lower bound of resulting number
 * \param to Upper bound
 * \returns random number
 */
vec_t helRandomNum(vec_t from, vec_t to);

#endif
