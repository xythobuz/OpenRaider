/*!
 * \file src/math/Vec3.cpp
 * \brief 3D Math vector
 *
 * \author Mongoose
 */

#include <math.h>

#include "global.h"
#include "math/Vec3.h"

Vec3::Vec3(float _x, float _y, float _z) {
    x = _x;
    y = _y;
    z = _z;
}

Vec3::Vec3(float v[3]) {
    assert(v != nullptr);
    x = v[0];
    y = v[1];
    z = v[2];
}

float Vec3::magnitude() {
    return sqrtf(x * x + y * y + z * z);
}

void Vec3::normalize() {
    float norm = magnitude();
    x /= norm;
    y /= norm;
    z /= norm;
}

Vec3 Vec3::unit() {
    float norm = magnitude();
    return Vec3(x / norm,
                y / norm,
                z / norm);
}

Vec3 Vec3::operator +(const Vec3& v) {
    return Vec3(x + v.x,
                y + v.y,
                z + v.z);
}

Vec3 Vec3::operator -(const Vec3& v) {
    return Vec3(x - v.x,
                y - v.y,
                z - v.z);
}

Vec3 Vec3::operator -() {
    return Vec3(-x,
                -y,
                -z);
}

Vec3 Vec3::operator *(float s) {
    return Vec3(s * x,
                s * y,
                s * z);
}

Vec3 Vec3::operator /(float s) {
    return Vec3(x / s,
                y / s,
                z / s);
}

float Vec3::operator *(const Vec3& v) {
    return dot(*this, v);
}

Vec3& Vec3::operator +=(const Vec3& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vec3& Vec3::operator -=(const Vec3& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

Vec3& Vec3::operator *=(float s) {
    x *= s;
    y *= s;
    z *= s;
    return *this;
}

bool Vec3::operator ==(const Vec3& v) {
    return equalEpsilon(x, v.x)
           && equalEpsilon(y, v.y)
           && equalEpsilon(z, v.z);
}

bool Vec3::operator !=(const Vec3& v) {
    return !((*this) == v);
}

float Vec3::dot(const Vec3& u, const Vec3& v) {
    return (u.x * v.x + u.y * v.y + u.z * v.z);
}

Vec3 Vec3::cross(const Vec3& u, const Vec3& v) {
    return Vec3(u.y * v.z - u.z * v.y,
                u.z * v.x - u.x * v.z,
                u.x * v.y - u.y * v.x);
}

