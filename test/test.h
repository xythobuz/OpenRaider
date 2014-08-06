/*!
 * \file test/test.h
 * \brief Common Unit Test helpers
 *
 * \author xythobuz
 */

#ifndef _TEST_H
#define _TEST_H

#include <iostream>

#define assertEqual(x, y) if (x != y) { \
    std::cout << "Assertion failed:" << std::endl; \
    std::cout << #x << " == " << #y << " (" << x << ", " << y << ")" << std::endl; \
    return 1; \
}

#endif

