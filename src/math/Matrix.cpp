/*!
 * \file src/math/Matrix.cpp
 * \brief 3D Matrix
 *
 * \author Mongoose
 */

#include <stdio.h>
#include <math.h>

#include "global.h"
#include "math/Matrix.h"

Matrix::Matrix() {
    setIdentity();
}

Matrix::Matrix(float m[16]) {
    setMatrix(m);
}

Matrix::Matrix(Quaternion& q) {
    float m[16];
    q.getMatrix(m);
    setMatrix(m);
}

bool Matrix::getInvert(float out[16]) {
    float m[16];

#ifdef COLUMN_ORDER
    getMatrix(m);
#else
    getTransposeMatrix(m);
#endif

    /* Mongoose: This code was from a Jeff Lander tutorial which was based
       on MESA GL's InvertMatrix */

    /* NB. OpenGL Matrices are COLUMN major. */
#define SWAP_ROWS(a, b) { float *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) (m)[(c)*4+(r)]

    float wtmp[4][8];
    float m0, m1, m2, m3, s;
    float* r0, *r1, *r2, *r3;

    r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

    r0[0] = MAT(m, 0, 0), r0[1] = MAT(m, 0, 1),
                                  r0[2] = MAT(m, 0, 2), r0[3] = MAT(m, 0, 3),
                                          r0[4] = 1.0f, r0[5] = r0[6] = r0[7] = 0.0f,

                                                  r1[0] = MAT(m, 1, 0), r1[1] = MAT(m, 1, 1),
                                                          r1[2] = MAT(m, 1, 2), r1[3] = MAT(m, 1, 3),
                                                                  r1[5] = 1.0f, r1[4] = r1[6] = r1[7] = 0.0f,

                                                                          r2[0] = MAT(m, 2, 0), r2[1] = MAT(m, 2, 1),
                                                                                  r2[2] = MAT(m, 2, 2), r2[3] = MAT(m, 2, 3),
                                                                                          r2[6] = 1.0f, r2[4] = r2[5] = r2[7] = 0.0f,

                                                                                                  r3[0] = MAT(m, 3, 0), r3[1] = MAT(m, 3, 1),
                                                                                                          r3[2] = MAT(m, 3, 2), r3[3] = MAT(m, 3, 3),
                                                                                                                  r3[7] = 1.0f, r3[4] = r3[5] = r3[6] = 0.0f;

    /* choose pivot - or die */
    if (fabs(r3[0]) > fabs(r2[0])) SWAP_ROWS(r3, r2);
    if (fabs(r2[0]) > fabs(r1[0])) SWAP_ROWS(r2, r1);
    if (fabs(r1[0]) > fabs(r0[0])) SWAP_ROWS(r1, r0);
    if (0.0f == r0[0])  return false;

    /* eliminate first variable     */
    m1 = r1[0] / r0[0]; m2 = r2[0] / r0[0]; m3 = r3[0] / r0[0];
    s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
    s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
    s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
    s = r0[4];
    if (s != 0.0f) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
    s = r0[5];
    if (s != 0.0f) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
    s = r0[6];
    if (s != 0.0f) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
    s = r0[7];
    if (s != 0.0f) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

    /* choose pivot - or die */
    if (fabs(r3[1]) > fabs(r2[1])) SWAP_ROWS(r3, r2);
    if (fabs(r2[1]) > fabs(r1[1])) SWAP_ROWS(r2, r1);
    if (0.0f == r1[1])  return false;

    /* eliminate second variable */
    m2 = r2[1] / r1[1]; m3 = r3[1] / r1[1];
    r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
    r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
    s = r1[4];
    if (0.0f != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
    s = r1[5];
    if (0.0f != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
    s = r1[6];
    if (0.0f != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
    s = r1[7];
    if (0.0f != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

    /* choose pivot - or die */
    if (fabs(r3[2]) > fabs(r2[2])) SWAP_ROWS(r3, r2);
    if (0.0f == r2[2])  return false;

    /* eliminate third variable */
    m3 = r3[2] / r2[2];
    r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
                                  r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6],
                                           r3[7] -= m3 * r2[7];

    /* last check */
    if (0.0f == r3[3]) return false;

    s = 1.0f / r3[3];            /* now back substitute row 3 */
    r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

    m2 = r2[3];                 /* now back substitute row 2 */
    s  = 1.0f / r2[2];
    r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
            r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
    m1 = r1[3];
    r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
                                  r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
    m0 = r0[3];
    r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
                                  r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

    m1 = r1[2];                 /* now back substitute row 1 */
    s  = 1.0f / r1[1];
    r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
            r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
    m0 = r0[2];
    r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
                                  r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

    m0 = r0[1];                 /* now back substitute row 0 */
    s  = 1.0f / r0[0];
    r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
            r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

    MAT(out, 0, 0) = r0[4];
    MAT(out, 0, 1) = r0[5], MAT(out, 0, 2) = r0[6];
    MAT(out, 0, 3) = r0[7], MAT(out, 1, 0) = r1[4];
    MAT(out, 1, 1) = r1[5], MAT(out, 1, 2) = r1[6];
    MAT(out, 1, 3) = r1[7], MAT(out, 2, 0) = r2[4];
    MAT(out, 2, 1) = r2[5], MAT(out, 2, 2) = r2[6];
    MAT(out, 2, 3) = r2[7], MAT(out, 3, 0) = r3[4];
    MAT(out, 3, 1) = r3[5], MAT(out, 3, 2) = r3[6];
    MAT(out, 3, 3) = r3[7];

    return true;
#undef MAT
#undef SWAP_ROWS
}

void Matrix::getMatrix(float mat[16]) {
    copy(mMatrix, mat);
}

void Matrix::getTransposeMatrix(float m[16]) {
    m[ 0] = mMatrix[0]; m[ 1] = mMatrix[4]; m[ 2] = mMatrix[ 8]; m[ 3] = mMatrix[12];
    m[ 4] = mMatrix[1]; m[ 5] = mMatrix[5]; m[ 6] = mMatrix[ 9]; m[ 7] = mMatrix[13];
    m[ 8] = mMatrix[2]; m[ 9] = mMatrix[6]; m[10] = mMatrix[10]; m[11] = mMatrix[14];
    m[12] = mMatrix[3]; m[13] = mMatrix[7]; m[14] = mMatrix[11]; m[15] = mMatrix[15];
}

Matrix Matrix::multiply(const Matrix& a, const Matrix& b) {
    Matrix c;
    multiply(a.mMatrix, b.mMatrix, c.mMatrix);
    return c;
}

Matrix Matrix::operator *(const Matrix& a) {
    return multiply(a, *this);
}

Vec3 Matrix::operator *(Vec3 v) {
    float x = v.x, y = v.y, z = v.z;

#ifdef COLUMN_ORDER
    return Vec3(mMatrix[0] * x + mMatrix[4] * y + mMatrix[ 8] * z + mMatrix[12],
                mMatrix[1] * x + mMatrix[5] * y + mMatrix[ 9] * z + mMatrix[13],
                mMatrix[2] * x + mMatrix[6] * y + mMatrix[10] * z + mMatrix[14]);
#else
    return Vec3(mMatrix[0] * x + mMatrix[1] * y + mMatrix[ 2] * z + mMatrix[ 3],
                mMatrix[4] * x + mMatrix[5] * y + mMatrix[ 6] * z + mMatrix[ 7],
                mMatrix[8] * x + mMatrix[9] * y + mMatrix[10] * z + mMatrix[11]);
#endif
}

void Matrix::multiply3v(float v[3], float result[3]) {
    float x = v[0], y = v[1], z = v[2];

    result[0] = mMatrix[0] * x + mMatrix[1] * y + mMatrix[ 2] * z + mMatrix[ 3];
    result[1] = mMatrix[4] * x + mMatrix[5] * y + mMatrix[ 6] * z + mMatrix[ 7];
    result[2] = mMatrix[8] * x + mMatrix[9] * y + mMatrix[10] * z + mMatrix[11];
}

void Matrix::multiply4v(float v[4], float result[4]) {
    float x = v[0], y = v[1], z = v[2], w = v[3];

    result[0] = mMatrix[ 0] * x + mMatrix[ 1] * y + mMatrix[ 2] * z + mMatrix[ 3] * w;
    result[1] = mMatrix[ 4] * x + mMatrix[ 5] * y + mMatrix[ 6] * z + mMatrix[ 7] * w;
    result[2] = mMatrix[ 8] * x + mMatrix[ 9] * y + mMatrix[10] * z + mMatrix[11] * w;
    result[3] = mMatrix[12] * x + mMatrix[13] * y + mMatrix[14] * z + mMatrix[15] * w;
}

void Matrix::print() {
    printf("{\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n}\n",
#ifdef COLUMN_ORDER
           mMatrix[0], mMatrix[4], mMatrix[ 8], mMatrix[12],
           mMatrix[1], mMatrix[5], mMatrix[ 9], mMatrix[13],
           mMatrix[2], mMatrix[6], mMatrix[10], mMatrix[14],
           mMatrix[3], mMatrix[7], mMatrix[11], mMatrix[15]);
#else
           mMatrix[ 0], mMatrix[ 1], mMatrix[ 2], mMatrix[ 3],
           mMatrix[ 4], mMatrix[ 5], mMatrix[ 6], mMatrix[ 7],
           mMatrix[ 8], mMatrix[ 9], mMatrix[10], mMatrix[11],
           mMatrix[12], mMatrix[13], mMatrix[14], mMatrix[15]);
#endif
}

bool Matrix::isIdentity() {
    // Hhhmm... floating point using direct comparisons
    /*
    if (mMatrix[ 0] == 1 && mMatrix[ 1] == 0 && mMatrix[ 2] == 0 &&
            mMatrix[ 3] == 0 &&    mMatrix[ 4] == 0 && mMatrix[ 5] == 1 &&
            mMatrix[ 6] == 0 && mMatrix[ 7] == 0 && mMatrix[ 8] == 0 &&
            mMatrix[ 9] == 0 && mMatrix[10] == 1 && mMatrix[11] == 0 &&
            mMatrix[12] == 0 && mMatrix[13] == 0 && mMatrix[14] == 0 &&
            mMatrix[15] == 1)
        return true;
    */
    if (equalEpsilon(mMatrix[ 0], 1.0) && equalEpsilon(mMatrix[ 1], 0.0)
        && equalEpsilon(mMatrix[ 2], 0.0) &&
        equalEpsilon(mMatrix[ 3], 0.0) && equalEpsilon(mMatrix[ 4], 0.0) && equalEpsilon(mMatrix[ 5], 1.0)
        &&
        equalEpsilon(mMatrix[ 6], 0.0) && equalEpsilon(mMatrix[ 7], 0.0) && equalEpsilon(mMatrix[ 8], 0.0)
        &&
        equalEpsilon(mMatrix[ 9], 0.0) && equalEpsilon(mMatrix[10], 1.0) && equalEpsilon(mMatrix[11], 0.0)
        &&
        equalEpsilon(mMatrix[12], 0.0) && equalEpsilon(mMatrix[13], 0.0) && equalEpsilon(mMatrix[14], 0.0)
        &&
        equalEpsilon(mMatrix[15], 1.0))
        return true;

    return false;
}

void Matrix::setMatrix(float mat[16]) {
    copy(mat, mMatrix);
}

void Matrix::setIdentity() {
    mMatrix[ 0] = 1; mMatrix[ 1] = 0; mMatrix[ 2] = 0; mMatrix[ 3] = 0;
    mMatrix[ 4] = 0; mMatrix[ 5] = 1; mMatrix[ 6] = 0; mMatrix[ 7] = 0;
    mMatrix[ 8] = 0; mMatrix[ 9] = 0; mMatrix[10] = 1; mMatrix[11] = 0;
    mMatrix[12] = 0; mMatrix[13] = 0; mMatrix[14] = 0; mMatrix[15] = 1;
}

void Matrix::scale(const float* xyz) {
    scale(xyz[0], xyz[1], xyz[2]);
}

void Matrix::scale(float sx, float sy, float sz) {
    float smatrix[16];
    float tmp[16];

    smatrix[ 0] = sx; smatrix[ 1] = 0;  smatrix[ 2] = 0;  smatrix[ 3] = 0;
    smatrix[ 4] = 0;  smatrix[ 5] = sy; smatrix[ 6] = 0;  smatrix[ 7] = 0;
    smatrix[ 8] = 0;  smatrix[ 9] = 0;  smatrix[10] = sz; smatrix[11] = 0;
    smatrix[12] = 0;  smatrix[13] = 0;  smatrix[14] = 0;  smatrix[15] = 1;

    copy(mMatrix, tmp);
    multiply(tmp, smatrix, mMatrix);
}

void Matrix::rotate(const float* xyz) {
    rotate(xyz[0], xyz[1], xyz[2]);
}

void Matrix::rotate(float ax, float ay, float az) {
    float xmat[16], ymat[16], zmat[16], tmp[16], tmp2[16];

    xmat[ 0] = 1;         xmat[ 1] = 0;         xmat[ 2] = 0;         xmat[ 3] = 0;
    xmat[ 4] = 0;         xmat[ 5] = cosf(ax);  xmat[ 6] = sinf(ax);  xmat[ 7] = 0;
    xmat[ 8] = 0;         xmat[ 9] = -sinf(ax); xmat[10] = cosf(ax);  xmat[11] = 0;
    xmat[12] = 0;         xmat[13] = 0;         xmat[14] = 0;         xmat[15] = 1;

    ymat[ 0] = cosf(ay);  ymat[ 1] = 0;         ymat[ 2] = -sinf(ay); ymat[ 3] = 0;
    ymat[ 4] = 0;         ymat[ 5] = 1;         ymat[ 6] = 0;         ymat[ 7] = 0;
    ymat[ 8] = sinf(ay);  ymat[ 9] = 0;         ymat[10] = cosf(ay);  ymat[11] = 0;
    ymat[12] = 0;         ymat[13] = 0;         ymat[14] = 0;         ymat[15] = 1;

    zmat[ 0] = cosf(az);  zmat[ 1] = sinf(az);  zmat[ 2] = 0;         zmat[ 3] = 0;
    zmat[ 4] = -sinf(az); zmat[ 5] = cosf(az);  zmat[ 6] = 0;         zmat[ 7] = 0;
    zmat[ 8] = 0;         zmat[ 9] = 0;         zmat[10] = 1;         zmat[11] = 0;
    zmat[12] = 0;         zmat[13] = 0;         zmat[14] = 0;         zmat[15] = 1;

    multiply(mMatrix, ymat, tmp);
    multiply(tmp, xmat, tmp2);
    multiply(tmp2, zmat, mMatrix);
}

void Matrix::translate(const float* xyz) {
    translate(xyz[0], xyz[1], xyz[2]);
}

void Matrix::translate(float tx, float ty, float tz) {
    float tmat[16], tmp[16];

    tmat[ 0] = 1;  tmat[ 1] = 0;  tmat[ 2] = 0;  tmat[ 3] = 0;
    tmat[ 4] = 0;  tmat[ 5] = 1;  tmat[ 6] = 0;  tmat[ 7] = 0;
    tmat[ 8] = 0;  tmat[ 9] = 0;  tmat[10] = 1;  tmat[11] = 0;
    tmat[12] = tx; tmat[13] = ty; tmat[14] = tz; tmat[15] = 1;

    copy(mMatrix, tmp);
    multiply(tmp, tmat, mMatrix);
}

void Matrix::copy(float source[16], float dest[16]) {
    for (int i = 0; i < 16; i++)
        dest[i] = source[i];
}

void Matrix::multiply(const float a[16], const float b[16], float result[16]) {
    /* Generated code for matrix mult
     * Code used:

    // char order is argument
    int i, j, k;
    if (order == 'r') {
        printf("// Row order\n");
    } else {
        printf("// Column order\n");
    }
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            if (order == 'r') {
                printf("result[%2i] = ", j+i*4);
            } else {
                printf("result[%2i] = ", j+i*4);
            }
            for (k = 0; k < 4; ++k) {
                if (order == 'r') {
                    printf("a[%2i] * b[%2i]%s",
                    k+i*4, j+k*4, (k == 3) ? ";\n" : " + ");
                } else {
                    printf("a[%2i] * b[%2i]%s",
                    i+k*4, k+j*4, (k == 3) ? ";\n" : " + ");
                }
                //sum+=(elements[i+k*4]*m.elements[k+j*4]);
            }
            //result.elements[i+j*4]=sum;
        }
        printf("\n");
    }
    printf("\n");
    printf("// Transpose\n");
    for(i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            printf("a[%2i] = b[%2i]%s",
            j+i*4, i+j*4, (j == 3) ? ";\n" : "; ");
        }
    }

     * was in test/Matrix.cpp
     */
#ifdef COLUMN_ORDER
    /* Column order */
    result[ 0] = a[ 0] * b[ 0] + a[ 4] * b[ 1] + a[ 8] * b[ 2] + a[12] * b[ 3];
    result[ 1] = a[ 0] * b[ 4] + a[ 4] * b[ 5] + a[ 8] * b[ 6] + a[12] * b[ 7];
    result[ 2] = a[ 0] * b[ 8] + a[ 4] * b[ 9] + a[ 8] * b[10] + a[12] * b[11];
    result[ 3] = a[ 0] * b[12] + a[ 4] * b[13] + a[ 8] * b[14] + a[12] * b[15];

    result[ 4] = a[ 1] * b[ 0] + a[ 5] * b[ 1] + a[ 9] * b[ 2] + a[13] * b[ 3];
    result[ 5] = a[ 1] * b[ 4] + a[ 5] * b[ 5] + a[ 9] * b[ 6] + a[13] * b[ 7];
    result[ 6] = a[ 1] * b[ 8] + a[ 5] * b[ 9] + a[ 9] * b[10] + a[13] * b[11];
    result[ 7] = a[ 1] * b[12] + a[ 5] * b[13] + a[ 9] * b[14] + a[13] * b[15];

    result[ 8] = a[ 2] * b[ 0] + a[ 6] * b[ 1] + a[10] * b[ 2] + a[14] * b[ 3];
    result[ 9] = a[ 2] * b[ 4] + a[ 6] * b[ 5] + a[10] * b[ 6] + a[14] * b[ 7];
    result[10] = a[ 2] * b[ 8] + a[ 6] * b[ 9] + a[10] * b[10] + a[14] * b[11];
    result[11] = a[ 2] * b[12] + a[ 6] * b[13] + a[10] * b[14] + a[14] * b[15];

    result[12] = a[ 3] * b[ 0] + a[ 7] * b[ 1] + a[11] * b[ 2] + a[15] * b[ 3];
    result[13] = a[ 3] * b[ 4] + a[ 7] * b[ 5] + a[11] * b[ 6] + a[15] * b[ 7];
    result[14] = a[ 3] * b[ 8] + a[ 7] * b[ 9] + a[11] * b[10] + a[15] * b[11];
    result[15] = a[ 3] * b[12] + a[ 7] * b[13] + a[11] * b[14] + a[15] * b[15];
#else
    /* Row order */
    result[ 0] = a[ 0] * b[ 0] + a[ 1] * b[ 4] + a[ 2] * b[ 8] + a[ 3] * b[12];
    result[ 1] = a[ 0] * b[ 1] + a[ 1] * b[ 5] + a[ 2] * b[ 9] + a[ 3] * b[13];
    result[ 2] = a[ 0] * b[ 2] + a[ 1] * b[ 6] + a[ 2] * b[10] + a[ 3] * b[14];
    result[ 3] = a[ 0] * b[ 3] + a[ 1] * b[ 7] + a[ 2] * b[11] + a[ 3] * b[15];

    result[ 4] = a[ 4] * b[ 0] + a[ 5] * b[ 4] + a[ 6] * b[ 8] + a[ 7] * b[12];
    result[ 5] = a[ 4] * b[ 1] + a[ 5] * b[ 5] + a[ 6] * b[ 9] + a[ 7] * b[13];
    result[ 6] = a[ 4] * b[ 2] + a[ 5] * b[ 6] + a[ 6] * b[10] + a[ 7] * b[14];
    result[ 7] = a[ 4] * b[ 3] + a[ 5] * b[ 7] + a[ 6] * b[11] + a[ 7] * b[15];

    result[ 8] = a[ 8] * b[ 0] + a[ 9] * b[ 4] + a[10] * b[ 8] + a[11] * b[12];
    result[ 9] = a[ 8] * b[ 1] + a[ 9] * b[ 5] + a[10] * b[ 9] + a[11] * b[13];
    result[10] = a[ 8] * b[ 2] + a[ 9] * b[ 6] + a[10] * b[10] + a[11] * b[14];
    result[11] = a[ 8] * b[ 3] + a[ 9] * b[ 7] + a[10] * b[11] + a[11] * b[15];

    result[12] = a[12] * b[ 0] + a[13] * b[ 4] + a[14] * b[ 8] + a[15] * b[12];
    result[13] = a[12] * b[ 1] + a[13] * b[ 5] + a[14] * b[ 9] + a[15] * b[13];
    result[14] = a[12] * b[ 2] + a[13] * b[ 6] + a[14] * b[10] + a[15] * b[14];
    result[15] = a[12] * b[ 3] + a[13] * b[ 7] + a[14] * b[11] + a[15] * b[15];
#endif
}

