/*!
 * \file test/Matrix.cpp
 * \brief Matrix Unit Test
 *
 * \author Mongoose
 * \author xythobuz
 */
#include <stdio.h>
#include <math.h>
#include <strings.h>
#include <Matrix.h>
#include "greatest.h"

#ifdef COLUMN_ORDER
unsigned char i0  =  0, i1  =  4, i2  =  8; // i3  = 12
unsigned char i4  =  1, i6  =  9; // i5  =  5, i7  = 13
unsigned char i8  =  2, i9  =  6, i10 = 10; // i11 = 14
unsigned char i12 =  3, i13 =  7, i14 = 11, i15 = 15;
#else
unsigned char i0  =  0, i1  =  1, i2  =  2; // i3  =  3
unsigned char i4  =  4, i6  =  6; // i5  =  5, i7  =  7
unsigned char i8  =  8, i9  =  9, i10 = 10; // i11 = 11
unsigned char i12 = 12, i13 = 13, i14 = 14, i15 = 15;
#endif

vec_t initialValues[][4] = {
    { 45.0f, 90.0f, 180.0f, 270.0f },
    { 10.0, 20.0, 30.0, 40.0 }
};

TEST identity(vec_t init[4], int mode) {
    Matrix a, b, c;
    matrix_t m;

    // Fill A matrix with garbage
    m[ 0] = m[ 1] = m[ 2] = m[ 3] = init[0];
    m[ 4] = m[ 5] = m[ 6] = m[ 7] = init[1];
    m[ 8] = m[ 9] = m[10] = m[11] = init[2];
    m[12] = m[13] = m[14] = m[15] = init[3];
    a.setMatrix(m);

    switch (mode) {
        case 0:
            a.setIdentity();
            break;
        case 1:
            c.setIdentity();
            b.setIdentity();
            a = c * b;
            break;
        case 2:
            a.setIdentity();
            b.setIdentity();
            a = a * b;
            break;
    }
    ASSERT(a.isIdentity());
    PASS();
}

TEST translation() {
    Matrix a;
    a.setIdentity();
    a.translate(10, 20, 30);
    ASSERT(equalEpsilon(a.mMatrix[i12], 10));
    ASSERT(equalEpsilon(a.mMatrix[i13], 20));
    ASSERT(equalEpsilon(a.mMatrix[i14], 30));
    PASS();
}

TEST rotation(int axis) {
    Matrix a;
    a.setIdentity();
    vec_t rot = 90 * 0.01745329251994329f;
    a.rotate((axis == 0) ? rot : 0, (axis == 1) ? rot : 0, (axis == 2) ? rot : 0);
    if (axis == 0) {
        ASSERT(equalEpsilon(a.mMatrix[i0], 1));
        ASSERT(equalEpsilon(a.mMatrix[i15], 1));
        ASSERT(equalEpsilon(a.mMatrix[i9], -1));
        ASSERT(equalEpsilon(a.mMatrix[i6], 1));
    } else if (axis == 1) {
        ASSERT(equalEpsilon(a.mMatrix[i8], 1));
        ASSERT(equalEpsilon(a.mMatrix[i2], -1));
        ASSERT(equalEpsilon(a.mMatrix[i15], 1));
    } else if (axis == 2) {
        ASSERT(equalEpsilon(a.mMatrix[i4], -1));
        ASSERT(equalEpsilon(a.mMatrix[i15], 1));
        ASSERT(equalEpsilon(a.mMatrix[i1], 1));
        ASSERT(equalEpsilon(a.mMatrix[i10], 1));
    } else {
        FAIL();
    }
    PASS();
}

TEST precision() {
    Matrix a;
    printf("Prescision test...\n");
    a.setIdentity();
    printf(" -> Rotate (0, 0, 90 degrees)\n");
    a.rotate(0, 0, 90*0.01745329251994329f);
    printf(" -> Translate (10, 20, 30)\n");
    a.translate(10, 20, 30);
    printf(" -> scale (10, 10, 10)\n");
    a.scale(10, 10, 10);
    a.print();
    printf("\n -> scale (0.1, 0.1, 0.1)\n");
    a.scale(0.1f, 0.1f, 0.1f);
    printf(" -> Translate (-10, -20, -30)\n");
    a.translate(-10, -20, -30);
    printf(" -> Rotate (0, 0, -90 degrees)\n");
    a.rotate(0, 0, -90*0.01745329251994329f);
    a.print();
    PASS();
}

SUITE(matrixSuite) {
    for (unsigned int i = 0; i < sizeof(initialValues) / sizeof(initialValues[0]); i++) {
        for (int mode = 0; mode < 3; mode++) {
            RUN_TESTp(identity, initialValues[i], mode);
        }
    }

    //! \fixme These tests are weak and only spot check some of the matrix -- Mongoose, 2003.06.18
    RUN_TEST(translation);
    RUN_TESTp(rotation, 0);
    RUN_TESTp(rotation, 1);
    RUN_TESTp(rotation, 2);

    RUN_TEST(precision);
}

GREATEST_MAIN_DEFS();

int main(int argc, char *argv[]) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(matrixSuite);
    GREATEST_MAIN_END();
}

