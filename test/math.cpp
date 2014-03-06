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

vec_t testDegrees[] = { 0.0f, 30.0f, 45.0f, 60.0f, 90.0f, 360.0f, -100.0f };
vec_t testRads[] = { 0.0f, 0.5236f, 0.7854f, 1.0472f, 1.5708f, 6.2832f, -1.7453f };
vec_t testBounds[][2] = {
    { 0.0f, 1.0f },
    { -5.0f, 5.0f },
    { 1.0f, 1.0f },
};

TEST constants() {
    ASSERTm("Pi not correct!", equalEpsilon(HEL_PI, (vec_t)M_PI));
    ASSERTm("2Pi not correct!", equalEpsilon(HEL_2_PI, ((vec_t)M_PI * 2.0f)));
    ASSERTm("Pi/4 not correct!", equalEpsilon(HEL_PI_OVER_4, ((vec_t)M_PI / 4.0f)));
    ASSERTm("Pi/180 not correct!", equalEpsilon(HEL_PI_OVER_180, ((vec_t)M_PI / 180.0f)));
    ASSERTm("180/Pi not correct!", equalEpsilon(HEL_180_OVER_PI, (180.0f / (vec_t)M_PI)));
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
    vec_t hand = (deg * (vec_t)M_PI / 180.0f);
    if (!equalEpsilon(conv, hand)) {
        printf("Degree to Radian conversion failed: %f != %f\n", conv, hand);
        FAIL();
    } else {
        PASS();
    }
}

TEST conversionToDeg(vec_t rad) {
    vec_t conv = HEL_RAD_TO_DEG(rad);
    vec_t hand = (rad * 180.0f / (vec_t)M_PI);
    if (!equalEpsilon(conv, hand)) {
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
    for (unsigned int i = 0; i < (sizeof(testDegrees) / sizeof(testDegrees[0])); i++) {
        RUN_TESTp(conversionToRad, testDegrees[i]);
    }
    for (unsigned int i = 0; i < (sizeof(testRads) / sizeof(testRads[0])); i++) {
        RUN_TESTp(conversionToDeg, testRads[i]);
    }
    for (unsigned int i = 0; i < sizeof(testBounds) / sizeof(testBounds[0]); i++) {
        RUN_TESTp(random, testBounds[i]);
    }
}

GREATEST_MAIN_DEFS();

int main(int argc, char *argv[]) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(mathSuite);
    GREATEST_MAIN_END();
}

