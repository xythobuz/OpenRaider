/*!
 * \file include/ViewVolume.h
 * \brief Viewing Volume for culling use
 *
 * Thanks Mark Morley for the article I used to get several algorithms.
 *
 * \author Mongoose
 */

#include <math.h>

#include "global.h"
#include "ViewVolume.h"

ViewVolume::ViewVolume() {
    mFrustum[0][0] = mFrustum[0][1] = mFrustum[0][2] = mFrustum[0][3] = 0.0f;
    mFrustum[1][0] = mFrustum[1][1] = mFrustum[1][2] = mFrustum[1][3] = 0.0f;
    mFrustum[2][0] = mFrustum[2][1] = mFrustum[2][2] = mFrustum[2][3] = 0.0f;
    mFrustum[3][0] = mFrustum[3][1] = mFrustum[3][2] = mFrustum[3][3] = 0.0f;
    mFrustum[4][0] = mFrustum[4][1] = mFrustum[4][2] = mFrustum[4][3] = 0.0f;
    mFrustum[5][0] = mFrustum[5][1] = mFrustum[5][2] = mFrustum[5][3] = 0.0f;
}

bool ViewVolume::isPointInFrustum(float x, float y, float z) {
    for (unsigned int p = 0; p < 6; ++p) {
        if (mFrustum[p][0] * x + mFrustum[p][1] * y + mFrustum[p][2] * z +
            mFrustum[p][3] <= 0) {
            return false;
        }
    }
    return true;
}

bool ViewVolume::isSphereInFrustum(float x, float y, float z, float radius) {
    for (unsigned int p = 0; p < 6; ++p) {
        float d = mFrustum[p][0] * x + mFrustum[p][1] * y + mFrustum[p][2] * z + mFrustum[p][3];
        if (d <= -radius)
            return false;
    }
    return true;
}

bool ViewVolume::isBboxInFrustum(float min[3], float max[3]) {
    for (unsigned int p = 0; p < 6; ++p) {
        if (mFrustum[p][0] * min[0] +
            mFrustum[p][1] * min[1] +
            mFrustum[p][2] * min[2] + mFrustum[p][3] > 0)
            continue;

        if (mFrustum[p][0] * max[0] +
            mFrustum[p][1] * max[1] +
            mFrustum[p][2] * max[2] + mFrustum[p][3] > 0)
            continue;

        if (mFrustum[p][0] * min[0] +
            mFrustum[p][1] * max[1] +
            mFrustum[p][2] * max[2] + mFrustum[p][3] > 0)
            continue;

        if (mFrustum[p][0] * min[0] +
            mFrustum[p][1] * min[1] +
            mFrustum[p][2] * max[2] + mFrustum[p][3] > 0)
            continue;

        if (mFrustum[p][0] * min[0] +
            mFrustum[p][1] * max[1] +
            mFrustum[p][2] * min[2] + mFrustum[p][3] > 0)
            continue;

        if (mFrustum[p][0] * max[0] +
            mFrustum[p][1] * min[1] +
            mFrustum[p][2] * min[2] + mFrustum[p][3] > 0)
            continue;

        if (mFrustum[p][0] * max[0] +
            mFrustum[p][1] * max[1] +
            mFrustum[p][2] * min[2] + mFrustum[p][3] > 0)
            continue;

        if (mFrustum[p][0] * max[0] +
            mFrustum[p][1] * min[1] +
            mFrustum[p][2] * max[2] + mFrustum[p][3] > 0)
            continue;

        return false;
    }
    return true;
}

float ViewVolume::getDistToSphereFromNear(float x, float y, float z, float radius) {
    float d = 0.0;
    for (unsigned int p = 0; p < 6; ++p) {
        d = mFrustum[p][0] * x + mFrustum[p][1] * y + mFrustum[p][2] * z + mFrustum[p][3];
        if (d <= -radius)
            return 0;
    }
    return d + radius;
}

float ViewVolume::getDistToBboxFromNear(const float min[3], const float max[3]) {
    float center[3];
    float d, radius;

    midpoint(min, max, center);

    // 5 should be near plane
    d = (mFrustum[5][0] * center[0] +
         mFrustum[5][1] * center[1] +
         mFrustum[5][2] * center[2] +
         mFrustum[5][3]);

    radius = distance(max, center);

    if (d <= -radius)
        return 0;

    return d + radius;
}

void ViewVolume::getFrustum(float frustum[6][4]) {
    for (unsigned int p = 0; p < 6; ++p) {
        for (unsigned int i = 0; i < 4; ++i) {
            frustum[p][i] = mFrustum[p][i];
        }
    }
}

void ViewVolume::getPlane(ViewVolumeSide p, float plane[4]) {
    for (unsigned int i = 0; i < 4; ++i) {
        plane[i] =  mFrustum[p][i];
    }
}

void ViewVolume::updateFrame(float proj[16], float mdl[16]) {
    setModel(mdl);
    setProjection(proj);
    updateClip();
    updateFrustum();
}

void ViewVolume::updateFrame() {
    updateClip();
    updateFrustum();
}

void ViewVolume::setModel(float mdl[16]) {
    mModel.setMatrix(mdl);
}

void ViewVolume::setProjection(float proj[16]) {
    mProjection.setMatrix(proj);
}

void ViewVolume::updateClip() {
    mClip = mProjection * mModel;
}

void ViewVolume::updateFrustum() {
    float clip[16];
    float t;

    mClip.getMatrix(clip);

    /* Extract the numbers for the RIGHT plane */
    mFrustum[0][0] = clip[ 3] - clip[ 0];
    mFrustum[0][1] = clip[ 7] - clip[ 4];
    mFrustum[0][2] = clip[11] - clip[ 8];
    mFrustum[0][3] = clip[15] - clip[12];

    /* Normalize the result */
    t = sqrtf(mFrustum[0][0] * mFrustum[0][0] +
              mFrustum[0][1] * mFrustum[0][1] +
              mFrustum[0][2] * mFrustum[0][2]);
    mFrustum[0][0] /= t;
    mFrustum[0][1] /= t;
    mFrustum[0][2] /= t;
    mFrustum[0][3] /= t;

    /* Extract the numbers for the LEFT plane */
    mFrustum[1][0] = clip[ 3] + clip[ 0];
    mFrustum[1][1] = clip[ 7] + clip[ 4];
    mFrustum[1][2] = clip[11] + clip[ 8];
    mFrustum[1][3] = clip[15] + clip[12];

    /* Normalize the result */
    t = sqrtf(mFrustum[1][0] * mFrustum[1][0] +
              mFrustum[1][1] * mFrustum[1][1] +
              mFrustum[1][2] * mFrustum[1][2]);
    mFrustum[1][0] /= t;
    mFrustum[1][1] /= t;
    mFrustum[1][2] /= t;
    mFrustum[1][3] /= t;

    /* Extract the BOTTOM plane */
    mFrustum[2][0] = clip[ 3] + clip[ 1];
    mFrustum[2][1] = clip[ 7] + clip[ 5];
    mFrustum[2][2] = clip[11] + clip[ 9];
    mFrustum[2][3] = clip[15] + clip[13];

    /* Normalize the result */
    t = sqrtf(mFrustum[2][0] * mFrustum[2][0] +
              mFrustum[2][1] * mFrustum[2][1] +
              mFrustum[2][2] * mFrustum[2][2]);
    mFrustum[2][0] /= t;
    mFrustum[2][1] /= t;
    mFrustum[2][2] /= t;
    mFrustum[2][3] /= t;

    /* Extract the TOP plane */
    mFrustum[3][0] = clip[ 3] - clip[ 1];
    mFrustum[3][1] = clip[ 7] - clip[ 5];
    mFrustum[3][2] = clip[11] - clip[ 9];
    mFrustum[3][3] = clip[15] - clip[13];

    /* Normalize the result */
    t = sqrtf(mFrustum[3][0] * mFrustum[3][0] +
              mFrustum[3][1] * mFrustum[3][1] +
              mFrustum[3][2] * mFrustum[3][2]);
    mFrustum[3][0] /= t;
    mFrustum[3][1] /= t;
    mFrustum[3][2] /= t;
    mFrustum[3][3] /= t;

    /* Extract the FAR plane */
    mFrustum[4][0] = clip[ 3] - clip[ 2];
    mFrustum[4][1] = clip[ 7] - clip[ 6];
    mFrustum[4][2] = clip[11] - clip[10];
    mFrustum[4][3] = clip[15] - clip[14];

    /* Normalize the result */
    t = sqrtf(mFrustum[4][0] * mFrustum[4][0] +
              mFrustum[4][1] * mFrustum[4][1] +
              mFrustum[4][2] * mFrustum[4][2]);
    mFrustum[4][0] /= t;
    mFrustum[4][1] /= t;
    mFrustum[4][2] /= t;
    mFrustum[4][3] /= t;

    /* Extract the NEAR plane */
    mFrustum[5][0] = clip[ 3] + clip[ 2];
    mFrustum[5][1] = clip[ 7] + clip[ 6];
    mFrustum[5][2] = clip[11] + clip[10];
    mFrustum[5][3] = clip[15] + clip[14];

    /* Normalize the result */
    t = sqrtf(mFrustum[5][0] * mFrustum[5][0] +
              mFrustum[5][1] * mFrustum[5][1] +
              mFrustum[5][2] * mFrustum[5][2]);
    mFrustum[5][0] /= t;
    mFrustum[5][1] /= t;
    mFrustum[5][2] /= t;
    mFrustum[5][3] /= t;
}

