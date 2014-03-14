/*!
 *
 * \file include/math/math.h
 * \brief Vector and Matrix math
 *
 * \author Mongoose
 */

#include <math.h>

#ifndef _MATH_MATH_H
#define _MATH_MATH_H

#define HEL_PI           ((float)M_PI) //!< pi
#define HEL_2_PI         (HEL_PI * 2.0f) //!< pi*2
#define HEL_PI_OVER_4    (HEL_PI / 4.0f) //!< pi/4
#define HEL_PI_OVER_180  (HEL_PI / 180.0f) //!< pi/180
#define HEL_180_OVER_PI  (180.0f / HEL_PI) //!< 180/pi

#define HEL_RAD_TO_DEG(x) ((x) * HEL_180_OVER_PI) //!< Convert radians to degrees
#define HEL_DEG_TO_RAD(x) ((x) * HEL_PI_OVER_180) //!< Convert degrees to radians

typedef float vec_t;        //!< 1D Vector, aka float
typedef float vec2_t[2];    //!< 2D Vector
typedef float vec3_t[3];    //!< 3D Vector
typedef float vec4_t[4];    //!< 4D Vector
typedef vec_t matrix_t[16]; //!< Used as _Column_major_ in every class now!

/*!
 * \brief Compare two floats with an Epsilon.
 * \param a first float
 * \param b second float
 * \returns true if a and b are probably the same.
 */
bool equalEpsilon(vec_t a, vec_t b);

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

