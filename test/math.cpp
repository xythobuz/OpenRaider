/*!
 * \file test/math.cpp
 * \brief Math Unit Test
 *
 * \author xythobuz
 */

#include <iostream>

#include "global.h"
#include "math/math.h"
#include "math/Vec3.h"

#define check(x) if (!(x)) { \
    std::cout << "Test failed: " << #x << std::endl; \
    return -1; \
}

int main() {
    // math.h
    check(equalEpsilon(42.23f, 42.23f));
    check(!equalEpsilon(42.23f, 23.42f));

    // Vec3.h
    Vec3 a(42.0f, 23.0f, 66.0f);
    Vec3 b(23.0f, 66.0f, 42.0f);
    Vec3 result(-3390.0f, -246.0f, 2243.0f);
    check(Vec3::cross(a, b) == result);

    check(equalEpsilon(a * b, 5256.0f));

    return 0;
}

