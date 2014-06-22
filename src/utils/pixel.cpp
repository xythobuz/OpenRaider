/*!
 * \file src/utils/pixel.cpp
 * \brief Pixel buffer utilities
 *
 * \author xythobuz
 */

#include "global.h"
#include "utils/pixel.h"

void argb2rgba32(unsigned char *image, unsigned int w, unsigned int h) {
    assert(image != nullptr);
    assert(w > 0);
    assert(h > 0);

    for (unsigned int i = 0; i < (w * h); ++i) {
        /* 32-bit ARGB to RGBA */
        unsigned char swap = image[(i * 4) + 3];
        image[(i * 4)] = image[(i * 4) + 1];
        image[(i * 4) + 1] = image[(i * 4) + 2];
        image[(i * 4) + 2] = image[(i * 4) + 3];
        image[(i * 4) + 3] = swap;
    }
}

