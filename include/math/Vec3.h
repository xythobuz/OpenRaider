/*!
 * \file include/math/Vec3.h
 * \brief 3D Math vector
 *
 * \author xythobuz
 * \author Mongoose
 */

#ifndef _MATH_VEC3_H_
#define _MATH_VEC3_H_

#include "math/math.h"

class Vec3 {
  public:
    Vec3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f);
    Vec3(float v[3]);

    float magnitude();
    void normalize();
    Vec3 unit();

    Vec3 operator +(const Vec3& v);
    Vec3 operator -(const Vec3& v);
    Vec3 operator -();
    Vec3 operator *(float s);
    Vec3 operator /(float s);
    float operator *(const Vec3& v);

    Vec3& operator +=(const Vec3& v);
    Vec3& operator -=(const Vec3& v);
    Vec3& operator *=(float s);

    bool operator ==(const Vec3& v);
    bool operator !=(const Vec3& v);

    float x, y, z;

    // ----------------------------------------------------------

    static float dot(const Vec3& u, const Vec3& v);
    static Vec3 cross(const Vec3& u, const Vec3& v);
};

#endif

