/*!
 *
 * \file src/math/math.cpp
 * \brief Vector and Matrix math
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <cstdlib>
#include <cmath>
#include <float.h>
#include <algorithm>

#include "global.h"
#include "math/Vec3.h"
#include "math/Matrix.h"
#include "math/math.h"

bool equalEpsilon(float a, float b) {
    float epsilon = FLT_EPSILON;
    if (fabs(a - b) <= (std::max(fabs(a), fabs(b)) * epsilon))
        return true;
    return false;
}

int intersectionLinePolygon(float intersect[3],
                            float p1[3], float p2[3], float polygon[3][3]) {
    assert(polygon != NULL);

    // float normal[3], a[3], b[3];
    Vec3 a, b, normal, pA, pB;
    float d, denominator, mu;


    pA = Vec3(p1);
    pB = Vec3(p2);

    // Find normal
    a = Vec3(polygon[1]) - Vec3(polygon[0]);
    b = Vec3(polygon[2]) - Vec3(polygon[0]);
    normal = Vec3::cross(a, b);
    normal.normalize();

    // find D
    d = (normal.x * polygon[0][0] -
         normal.y * polygon[0][1] -
         normal.z * polygon[0][2]);

    // line segment parallel to plane?
    a = pB - pA;

    denominator = Vec3::dot(normal, a);

    if (denominator > 0.0)
        return 0;

    // Line segment contains intercept point?
    mu = -((d + Vec3::dot(normal, pA)) / denominator);

    if (mu < 0.0 || mu > 1.0)
        return 0;

    b = pA + (a * mu);
    intersect[0] = b.x;
    intersect[1] = b.y;
    intersect[2] = b.z;

    // See if the intercept is bound by polygon by winding number
    // assume convex polygons here for sure
    double theta = Vec3::dot(b - Vec3(polygon[0]), normal); // b = intersect

    if (theta >= 90.0) // Yeah I know
        return 0;

    return 1;
}

float distance(const float a[3], const float b[3]) {
    return sqrtf(((b[0] - a[0]) * (b[0] - a[0])) +
                 ((b[1] - a[1]) * (b[1] - a[1])) +
                 ((b[2] - a[2]) * (b[2] - a[2])));
}

void midpoint(const float a[3], const float b[3], float mid[3]) {
    mid[0] = (a[0] + b[0]) / 2.0f;
    mid[1] = (a[1] + b[1]) / 2.0f;
    mid[2] = (a[2] + b[2]) / 2.0f;
}

float randomNum(float from, float to) {
    return from + ((to - from) * rand() / (RAND_MAX + 1.0f));
}

