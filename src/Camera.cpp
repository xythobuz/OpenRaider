/*!
 * \file src/Camera.cpp
 * \brief Camera class
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <stdio.h>
#include <math.h>

#include "Camera.h"

Camera::Camera() {
    mViewDistance = 14.0f;
    mRotationDeltaX = 1.0f;
    mRotationDeltaY = 1.0f;
    mTheta = 0.0f;
    mTheta2 = 0.0f;

    mPos[0] = 0.0f;
    mPos[1] = 0.0f;
    mPos[2] = 0.0f;

    mTarget[0] = 0.0f;
    mTarget[1] = 0.0f;
    mTarget[2] = mViewDistance;

    mSide[0] = 1.0f;
    mSide[1] = 0.0f;
    mSide[2] = 0.0f;

    mUp[0] = 0.0f;
    mUp[1] = -1.0f; // 1.0f
    mUp[2] = 0.0f;

    mQ.setIdentity();
}

void Camera::getPosition(vec3_t pos) {
    pos[0] = mPos[0];
    pos[1] = mPos[1];
    pos[2] = mPos[2];
}

void Camera::getTarget(vec3_t target) {
    target[0] = mTarget[0];
    target[1] = mTarget[1];
    target[2] = mTarget[2];
}

vec_t Camera::getRadianYaw() {
    return mTheta;
}

vec_t Camera::getRadianPitch() {
    return mTheta2;
}

void Camera::setPosition(vec3_t pos) {
    mPos[0] = pos[0];
    mPos[1] = pos[1];
    mPos[2] = pos[2];
}

void Camera::setSensitivityX(vec_t sens) {
    mRotationDeltaX = sens;
}

void Camera::setSensitivityY(vec_t sens) {
    mRotationDeltaY = sens;
}

void Camera::update() {
    mTarget[2] = (mViewDistance * cosf(mTheta)) + mPos[2];
    mTarget[0] = (mViewDistance * sinf(mTheta)) + mPos[0];
    mTarget[1] = (mViewDistance * sinf(mTheta2)) + mPos[1]; // + height_offset;
}

void Camera::rotate(float angle, float x, float y, float z) {
    Quaternion t, n;
    Matrix matrix;
    vec_t side[4] = { 1.0f, 0.0f,  0.0f, 1.0f };
    vec_t up[4] =   { 0.0f, 1.0f,  0.0f, 1.0f };
    vec_t look[4] = { 0.0f, 0.0f, -1.0f, 1.0f };
    matrix_t m;

    t.set(angle, x, y, z);
    n = mQ * t;
    n.normalize();

    n.getMatrix(m);
    matrix.setMatrix(m);
    matrix.multiply4v(side, mSide);
    matrix.multiply4v(look, mTarget);
    matrix.multiply4v(up, mUp);

    for (int i = 0; i < 3; ++i) {
        mSide[i] += mPos[i];
        mTarget[i] += mPos[i];
        mUp[i] += mPos[i];
    }

    mQ = n;
}

void Camera::command(enum camera_command cmd) {
    switch (cmd) {
        case CAMERA_ROTATE_UP:
            if (mTheta2 < (M_PI / 2)) {
                mTheta2 += mRotationDeltaY;
                rotate(mTheta2, 1.0f, 0.0f, 0.0f);
            }
            break;

        case CAMERA_ROTATE_DOWN:
            if (mTheta2 > -(M_PI / 2)) {
                mTheta2 -= mRotationDeltaY;
                rotate(mTheta2, 1.0f, 0.0f, 0.0f);
            }
            break;

        case CAMERA_ROTATE_RIGHT:
            mTheta += mRotationDeltaX;
            rotate(mTheta, 0.0f, 1.0f, 0.0f);
            break;

        case CAMERA_ROTATE_LEFT:
            mTheta -= mRotationDeltaX;
            rotate(mTheta, 0.0f, 1.0f, 0.0f);
            break;
    }
}

