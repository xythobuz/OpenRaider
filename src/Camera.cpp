/*!
 * \file src/Camera.cpp
 * \brief Camera class
 *
 * \author Mongoose
 * \author xythobuz
 */

#include "global.h"
#include <cmath>
#include "math/Matrix.h"
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

    mQ.setIdentity();
}

void Camera::getTarget(float target[3]) {
    target[0] = mTarget[0];
    target[1] = mTarget[1];
    target[2] = mTarget[2];
}

void Camera::setPosition(float pos[3]) {
    mPos[0] = pos[0];
    mPos[1] = pos[1];
    mPos[2] = pos[2];
}

void Camera::update() {
    mTarget[0] = (mViewDistance * std::sin(mTheta)) + mPos[0];
    mTarget[1] = (mViewDistance * std::sin(mTheta2)) + mPos[1]; // + height_offset;
    mTarget[2] = (mViewDistance * std::cos(mTheta)) + mPos[2];
}

void Camera::rotate(float angle, float x, float y, float z) {
    Quaternion t, n;
    float look[4] = { 0.0f, 0.0f, -1.0f, 1.0f };

    t.set(angle, x, y, z);
    n = mQ * t;
    n.normalize();

    Matrix matrix(n);
    matrix.multiply4v(look, mTarget);

    for (int i = 0; i < 3; ++i)
        mTarget[i] += mPos[i];

    mQ = n;
}

void Camera::command(enum camera_command cmd) {
    switch (cmd) {
        case CAMERA_ROTATE_UP:
            if (mTheta2 < (OR_PI / 2)) {
                mTheta2 += mRotationDeltaY;
                rotate(mTheta2, 1.0f, 0.0f, 0.0f);
            }
            break;

        case CAMERA_ROTATE_DOWN:
            if (mTheta2 > -(OR_PI / 2)) {
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

