/*!
 *
 * \file include/math/math.h
 * \brief Vector and Matrix math
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _MATH_MATH_H
#define _MATH_MATH_H

#ifndef M_PI
#define OR_PI (3.14159265358979323846f) //!< pi
#else
#define OR_PI ((float)M_PI) //!< pi
#endif

#define OR_RAD_TO_DEG(x) ((x) * (180.0f / OR_PI)) //!< Convert radians to degrees
#define OR_DEG_TO_RAD(x) ((x) * (OR_PI / 180.0f)) //!< Convert degrees to radians

/*!
 * \brief Compare two floats with an Epsilon.
 * \param a first float
 * \param b second float
 * \returns true if a and b are probably the same.
 */
bool equalEpsilon(float a, float b);

/*!
 * \brief Calculate Intersection of a line and a polygon
 * \param intersect Where the intersection is stored, if it exists
 * \param p1 First point of line segment
 * \param p2 Second point of line segment
 * \param polygon polygon vertex array (0 to 2 are used)
 * \returns 0 if there is no intersection
 */
int intersectionLinePolygon(float intersect[3], float p1[3], float p2[3], float polygon[3][3]);

/*!
 * \brief Calculate the length of a line segment / the distance between two points
 * \param a First point
 * \param b Second point
 * \returns distance/length
 */
float distance(const float a[3], const float b[3]);

/*!
 * \brief Calculates the midpoint between two points / of a line segment
 * \param a First point
 * \param b Second point
 * \param mid Midpoint will be stored here
 */
void midpoint(const float a[3], const float b[3], float mid[3]);

/*!
 * \brief Calculates a pseudo-random number
 * \param from Lower bound of resulting number
 * \param to Upper bound
 * \returns random number
 */
float randomNum(float from, float to);

#endif

