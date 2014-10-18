/*!
 * \file src/utils/pixel.cpp
 * \brief Pixel buffer utilities
 *
 * \author xythobuz
 */

#include "global.h"
#include "utils/pixel.h"

unsigned char* generateColorTexture(const unsigned char* rgba, unsigned int width,
                                    unsigned int height, unsigned int bpp) {
    assert(rgba != NULL);
    assert(width > 0);
    assert(height > 0);
    assert((bpp % 8) == 0);

    unsigned char* image = new unsigned char[height * width * (bpp / 8)];
    for (unsigned int i = 0; i < (width * height); i++) {
        for (unsigned int a = 0; a < (bpp / 8); a++) {
            image[(i * (bpp / 8)) + a] = rgba[a];
        }
    }
    return image;
}

void bgr2rgb24(unsigned char* image, unsigned int w, unsigned int h) {
    assert(image != nullptr);
    assert(w > 0);
    assert(h > 0);

    for (unsigned int i = 0; i < (w * h); ++i) {
        /* 24-bit BGR to RGB */
        unsigned char swap = image[(i * 3) + 2];
        image[(i * 3) + 2] = image[(i * 3)];
        image[(i * 3)] = swap;
    }
}

void bgra2rgba32(unsigned char* image, unsigned int w, unsigned int h) {
    assert(image != nullptr);
    assert(w > 0);
    assert(h > 0);

    for (unsigned int i = 0; i < (w * h); ++i) {
        /* 32-bit BGRA to RGBA */
        unsigned char swap = image[(i * 4) + 2];
        image[(i * 4) + 2] = image[(i * 4)];
        image[(i * 4)] = swap;
    }
}

void argb2rgba32(unsigned char* image, unsigned int w, unsigned int h) {
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

#define NEXT_POWER(x) do {        \
    unsigned int i;               \
    for (i = 1; i < (x); i *= 2); \
    (x) = i;                      \
} while (false);

// This code based off on gluScaleImage()
unsigned char* scaleBuffer(unsigned char* image, unsigned int* w, unsigned int* h,
                           unsigned int bpp) {
    unsigned int width = *w;
    unsigned int height = *h;
    assert(image != NULL);
    assert(width > 0);
    assert(height > 0);
    assert((bpp % 8) == 0);

    unsigned int components = bpp / 8;
    unsigned int original_height = height;
    unsigned int original_width = width;

    NEXT_POWER(height);
    NEXT_POWER(width);

    // Check to see if scaling is needed
    if (height == original_height && width == original_width)
        return NULL;

    *w = width;
    *h = height;

    unsigned char* timage = new unsigned char[height * width * components];
    float* tempin = new float[original_width * original_height * components];
    float* tempout = new float[width * height * components];

    // Copy user data to float format.
    for (unsigned int i = 0; i < original_height * original_width * components; ++i) {
        tempin[i] = (float)image[i];
    }

    // Determine which filter to use by checking ratios.
    float sx;
    if (width > 1) {
        sx = (float)(original_width - 1) / (float)(width - 1);
    } else {
        sx = (float)(original_width - 1);
    }

    float sy;
    if (height > 1) {
        sy = (float)(original_height - 1) / (float)(height - 1);
    } else {
        sy = (float)(original_height - 1);
    }

    if (sx < 1.0 && sy < 1.0) { // Magnify both width and height: use weighted sample of 4 pixels
        for (unsigned int i = 0; i < height; ++i) {
            unsigned int i0 = (unsigned int)(i * sy);
            unsigned int i1 = i0 + 1;

            if (i1 >= original_height) {
                i1 = original_height - 1;
            }

            float alpha = i * sy - i0;

            for (unsigned int j = 0; j < width; ++j) {
                unsigned int j0 = (unsigned int)(j * sx);
                unsigned int j1 = j0 + 1;

                if (j1 >= original_width) {
                    j1 = original_width - 1;
                }

                float beta = j * sx - j0;

                // Compute weighted average of pixels in rect (i0,j0)-(i1,j1)
                float* src00 = tempin + (i0 * original_width + j0) * components;
                float* src01 = tempin + (i0 * original_width + j1) * components;
                float* src10 = tempin + (i1 * original_width + j0) * components;
                float* src11 = tempin + (i1 * original_width + j1) * components;

                float* dst = tempout + (i * width + j) * components;

                for (unsigned int k = 0; k < components; ++k) {
                    float s1 = *src00++ * (1.0f - beta) + *src01++ * beta;
                    float s2 = *src10++ * (1.0f - beta) + *src11++ * beta;
                    *dst++ = s1 * (1.0f - alpha) + s2 * alpha;
                }
            }
        }
    } else { // Shrink width and/or height: use an unweighted box filter
        for (unsigned int i = 0; i < height; ++i) {
            unsigned int i0 = (unsigned int)(i * sy);
            unsigned int i1 = i0 + 1;

            if (i1 >= original_height) {
                i1 = original_height - 1;
            }

            for (unsigned int j = 0; j < width; ++j) {
                unsigned int j0 = (unsigned int)(j * sx);
                unsigned int j1 = j0 + 1;

                if (j1 >= original_width) {
                    j1 = original_width - 1;
                }

                float* dst = tempout + (i * width + j) * components;

                // Compute average of pixels in the rectangle (i0,j0)-(i1,j1)
                for (unsigned int k = 0; k < components; ++k) {
                    float sum = 0.0;

                    for (unsigned int ii = i0; ii <= i1; ++ii) {
                        for (unsigned int jj = j0; jj <= j1; ++jj) {
                            sum += *(tempin + (ii * original_width + jj)
                                     * components + k);
                        }
                    }

                    sum /= (j1 - j0 + 1) * (i1 - i0 + 1);
                    *dst++ = sum;
                }
            }
        }
    }

    // Copy to our results.
    for (unsigned int i = 0; i < height * width * components; ++i) {
        timage[i] = (unsigned char)tempout[i];
    }

    // Delete our temp buffers.
    delete [] tempin;
    delete [] tempout;

    return timage;
}

