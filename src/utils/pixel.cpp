/*!
 * \file src/utils/pixel.cpp
 * \brief Pixel buffer utilities
 *
 * \author xythobuz
 */

#include "global.h"
#include "utils/pixel.h"

unsigned char* generateColorTexture(glm::vec4 rgba, unsigned int width,
                                    unsigned int height, unsigned int bpp) {
    orAssert(width > 0);
    orAssert(height > 0);
    orAssert((bpp == 24) || (bpp == 32));

    unsigned char* image = new unsigned char[height * width * (bpp / 8)];
    for (unsigned int i = 0; i < (width * height); i++) {
        image[i * (bpp / 8)] = static_cast<unsigned char>(rgba.r * 255);
        image[(i * (bpp / 8)) + 1] = static_cast<unsigned char>(rgba.g * 255);
        image[(i * (bpp / 8)) + 2] = static_cast<unsigned char>(rgba.b * 255);
        if (bpp == 32) {
            image[(i * (bpp / 8)) + 3] = static_cast<unsigned char>(rgba.a * 255);
        }
    }
    return image;
}

void argb2rgba32(unsigned char* image, unsigned int w, unsigned int h) {
    orAssert(image != nullptr);
    orAssert(w > 0);
    orAssert(h > 0);

    for (unsigned int i = 0; i < (w * h); ++i) {
        // 32-bit ARGB to RGBA
        unsigned char swap = image[i * 4];
        image[i * 4] = image[(i * 4) + 1];
        image[(i * 4) + 1] = image[(i * 4) + 2];
        image[(i * 4) + 2] = image[(i * 4) + 3];
        image[(i * 4) + 3] = swap;
    }
}

unsigned char* argb16to32(unsigned char* image, unsigned int w, unsigned int h) {
    orAssert(image != nullptr);
    orAssert(w > 0);
    orAssert(h > 0);

    unsigned char* img = new unsigned char[w * h * 4];
    for (unsigned int i = 0; i < (w * h); ++i) {
        // arrr.rrgg gggb.bbbb shift to 5bit
        img[i * 4] = (image[(i * 2) + 1] & 0x80) ? 0xFF : 0; // A
        img[(i * 4) + 1] = (image[(i * 2) + 1] & 0x7C) >> 2; // R
        img[(i * 4) + 2] = static_cast<unsigned char>((image[(i * 2) + 1] & 0x03) << 3);
        img[(i * 4) + 2] |= (image[i * 2] & 0xE0) >> 5; // G
        img[(i * 4) + 3] = image[i * 2] & 0x1F; // B

        img[(i * 4) + 1] <<= 3; // R
        img[(i * 4) + 2] <<= 3; // G
        img[(i * 4) + 3] <<= 3; // B
    }
    return img;
}

unsigned char* grayscale2rgba(unsigned char* image, unsigned int w, unsigned int h) {
    orAssert(image != nullptr);
    orAssert(w > 0);
    orAssert(h > 0);

    unsigned char* img = new unsigned char[w * h * 4];
    for (unsigned int i = 0; i < (w * h); i++) {
        img[i * 4] = image[i];
        img[(i * 4) + 1] = image[i];
        img[(i * 4) + 2] = image[i];
        img[(i * 4) + 3] = (image[i] == 0) ? 0 : 255;
    }
    return img;
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
    orAssert(image != nullptr);
    orAssert(width > 0);
    orAssert(height > 0);
    orAssert((bpp % 8) == 0);

    unsigned int components = bpp / 8;
    unsigned int original_height = height;
    unsigned int original_width = width;

    NEXT_POWER(height);
    NEXT_POWER(width);

    // Check to see if scaling is needed
    if (height == original_height && width == original_width)
        return nullptr;

    *w = width;
    *h = height;

    unsigned char* timage = new unsigned char[height * width * components];
    float* tempin = new float[original_width * original_height * components];
    float* tempout = new float[width * height * components];

    // Copy user data to float format.
    for (unsigned int i = 0; i < original_height * original_width * components; ++i) {
        tempin[i] = image[i];
    }

    // Determine which filter to use by checking ratios.
    float sx;
    if (width > 1) {
        sx = (original_width - 1.0f) / (width - 1.0f);
    } else {
        sx = original_width - 1.0f;
    }

    float sy;
    if (height > 1) {
        sy = (original_height - 1.0f) / (height - 1.0f);
    } else {
        sy = original_height - 1.0f;
    }

    if (sx < 1.0 && sy < 1.0) { // Magnify both width and height: use weighted sample of 4 pixels
        for (unsigned int i = 0; i < height; ++i) {
            unsigned int i0 = static_cast<unsigned int>(i * sy);
            unsigned int i1 = i0 + 1;

            if (i1 >= original_height) {
                i1 = original_height - 1;
            }

            float alpha = i * sy - i0;

            for (unsigned int j = 0; j < width; ++j) {
                unsigned int j0 = static_cast<unsigned int>(j * sx);
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
            unsigned int i0 = static_cast<unsigned int>(i * sy);
            unsigned int i1 = i0 + 1;

            if (i1 >= original_height) {
                i1 = original_height - 1;
            }

            for (unsigned int j = 0; j < width; ++j) {
                unsigned int j0 = static_cast<unsigned int>(j * sx);
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
        timage[i] = static_cast<unsigned char>(tempout[i]);
    }

    // Delete our temp buffers.
    delete [] tempin;
    delete [] tempout;

    return timage;
}

