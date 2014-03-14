/*!
 * \file src/math/Vector3d.cpp
 * \brief 3D Math vector
 *
 * \author Mongoose
 */

#include <math.h>

#include "math/Vector3d.h"

Vector3d::Vector3d() {
    mVec[0] = mVec[1] = mVec[2] = 0.0f;
}

Vector3d::Vector3d(vec3_t v) {
    mVec[0] = v[0];
    mVec[1] = v[1];
    mVec[2] = v[2];
}

Vector3d::Vector3d(vec_t x, vec_t y, vec_t z) {
    mVec[0] = x;
    mVec[1] = y;
    mVec[2] = z;
}

Vector3d::Vector3d(const Vector3d &v) {
    mVec[0] = v.mVec[0];
    mVec[1] = v.mVec[1];
    mVec[2] = v.mVec[2];
}

vec_t Vector3d::dot(const Vector3d &u, const Vector3d &v) {
    return (u.mVec[0]*v.mVec[0] + u.mVec[1]*v.mVec[1] + u.mVec[2]*v.mVec[2]);
}

Vector3d Vector3d::cross(const Vector3d &u, const Vector3d &v) {
    return Vector3d(u.mVec[1] * v.mVec[2] - u.mVec[2] * v.mVec[1],
            u.mVec[2] * v.mVec[0] - u.mVec[0] * v.mVec[2],
            u.mVec[0] * v.mVec[1] - u.mVec[1] * v.mVec[0]);
}

vec_t Vector3d::magnitude() {
    return sqrtf(mVec[0]*mVec[0] + mVec[1]*mVec[1] + mVec[2]*mVec[2]);
}

Vector3d Vector3d::unit() {
    vec_t norm = magnitude();

    return Vector3d(mVec[0] / norm,
            mVec[1] / norm,
            mVec[2] / norm);
}

Vector3d Vector3d::zeroVector() {
    return Vector3d(0, 0, 0);
}

Vector3d Vector3d::operator +(const Vector3d &v) {
    return Vector3d(mVec[0] + v.mVec[0],
            mVec[1] + v.mVec[1],
            mVec[2] + v.mVec[2]);
}

Vector3d Vector3d::operator -(const Vector3d &v) {
    return Vector3d(mVec[0] - v.mVec[0],
            mVec[1] - v.mVec[1],
            mVec[2] - v.mVec[2]);
}

Vector3d Vector3d::operator -() {
    return Vector3d(-mVec[0],
            -mVec[1],
            -mVec[2]);
}

Vector3d Vector3d::operator *(vec_t s) {
    return Vector3d(s * mVec[0],
            s * mVec[1],
            s * mVec[2]);
}

Vector3d Vector3d::operator /(vec_t s) {
    return Vector3d(mVec[0] / s,
            mVec[1] / s,
            mVec[2] / s);
}

vec_t Vector3d::operator *(const Vector3d &v) {
    return dot(*this, v);
}

void Vector3d::normalize() {
    vec_t norm = magnitude();

    mVec[0] /= norm;
    mVec[1] /= norm;
    mVec[2] /= norm;
}

void Vector3d::zero() {
    mVec[0] = 0;
    mVec[1] = 0;
    mVec[2] = 0;
}

Vector3d &Vector3d::operator =(const Vector3d &v) {
    mVec[0] = v.mVec[0];
    mVec[1] = v.mVec[1];
    mVec[2] = v.mVec[2];
    return *this;
}

Vector3d &Vector3d::operator +=(const Vector3d &v) {
    mVec[0] += v.mVec[0];
    mVec[1] += v.mVec[1];
    mVec[2] += v.mVec[2];
    return *this;
}

Vector3d &Vector3d::operator -=(const Vector3d &v) {
    mVec[0] -= v.mVec[0];
    mVec[1] -= v.mVec[1];
    mVec[2] -= v.mVec[2];
    return *this;
}

Vector3d &Vector3d::operator *=(vec_t s) {
    mVec[0] *= s;
    mVec[1] *= s;
    mVec[2] *= s;
    return *this;
}

