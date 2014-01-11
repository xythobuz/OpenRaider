/*!
 * \file test/MatMath.cpp
 * \brief Mat Math Unit Test
 *
 * \todo Also test the geometric methods (intersection, distance, midpoint)
 *
 * \author xythobuz
 */
#include <stdio.h>
#include <math.h>
#include <MatMath.h>
#include "greatest.h"

vec_t testDegrees[] = { 0.0, 30.0, 45.0, 60.0, 90.0, 360.0, -100.0 };
vec_t testRads[] = { 0.0, 0.5236, 0.7854, 1.0472, 1.5708, 6.2832, -1.7453 };
vec_t testBounds[][2] = {
    { 0.0, 1.0 },
    { -5.0, 5.0 },
    { 1.0, 1.0 },
};

TEST constants() {
    ASSERT_EQm("Pi not correct!", HEL_PI, M_PI);
    ASSERT_EQm("2Pi not correct!", HEL_2_PI, (M_PI * 2.0));
    ASSERT_EQm("Pi/4 not correct!", HEL_PI_OVER_4, (M_PI / 4.0));
    ASSERT_EQm("Pi/180 not correct!", HEL_PI_OVER_180, (M_PI / 180.0));
    ASSERT_EQm("180/Pi not correct!", HEL_180_OVER_PI, (180.0 / M_PI));
    PASS();
}

TEST types() {
    ASSERT_EQ(sizeof(vec2_t), (2 * sizeof(vec_t)));
    ASSERT_EQ(sizeof(vec3_t), (3 * sizeof(vec_t)));
    ASSERT_EQ(sizeof(vec4_t), (4 * sizeof(vec_t)));
    ASSERT_EQ(sizeof(matrix_t), (16 * sizeof(vec_t)));
    PASS();
}

TEST conversionToRad(vec_t deg) {
    vec_t conv = HEL_DEG_TO_RAD(deg);
    vec_t hand = (deg * M_PI / 180.0);
    if (conv != hand) {
        printf("Degree to Radian conversion failed: %f != %f\n", conv, hand);
        FAIL();
    } else {
        PASS();
    }
}

TEST conversionToDeg(vec_t rad) {
    vec_t conv = HEL_RAD_TO_DEG(rad);
    vec_t hand = (rad * 180.0 / M_PI);
    if (conv != hand) {
        printf("Radian to Degree conversion failed: %f != %f\n", conv, hand);
        FAIL();
    } else {
        PASS();
    }
}

#define TESTNUMS 100
TEST random(vec_t bound[2]) {
    for (int i = 0; i < TESTNUMS; i++) {
        vec_t number = helRandomNum(bound[0], bound[1]);
        ASSERT(number >= bound[0]);
        ASSERT(number <= bound[1]);
    }
    PASS();
}

SUITE(mathSuite) {
    RUN_TEST(constants);
    RUN_TEST(types);
    for (int i = 0; i < (sizeof(testDegrees) / sizeof(testDegrees[0])); i++) {
        RUN_TESTp(conversionToRad, testDegrees[i]);
    }
    for (int i = 0; i < (sizeof(testRads) / sizeof(testRads[0])); i++) {
        RUN_TESTp(conversionToDeg, testRads[i]);
    }
    for (int i = 0; i < sizeof(testBounds) / sizeof(testBounds[0]); i++) {
        RUN_TESTp(random, testBounds[i]);
    }
}

GREATEST_MAIN_DEFS();

int main(int argc, char *argv[]) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(mathSuite);
    GREATEST_MAIN_END();
}

