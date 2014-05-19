/*!
 * \file src/Texture.cpp
 * \brief Texture registry
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "global.h"
#include "utils/pcx.h"
#include "utils/strings.h"
#include "utils/tga.h"
#include "Texture.h"

Texture::Texture() {
    mTextureIds = NULL;
    mFlags = 0;
    mTextureId = -1;
    mTextureId2 = -1;
    mTextureCount = 0;
    mTextureLimit = 0;
}

Texture::~Texture() {
    reset();
}

unsigned char *Texture::generateColorTexture(unsigned char rgba[4],
        unsigned int width, unsigned int height) {
    unsigned char *image;
    unsigned int i, size;

    assert(rgba != NULL);
    assert(width > 0);
    assert(height > 0);

    image = new unsigned char[height*width*4];

    for (i = 0, size = width*height; i < size; ++i) {
        /* RBGA */
        image[i*4]   = rgba[0];
        image[i*4+1] = rgba[1];
        image[i*4+2] = rgba[2];
        image[i*4+3] = rgba[3];
    }

    return image;
}

int Texture::loadColorTexture(unsigned char rgba[4],
        unsigned int width, unsigned int height) {
    unsigned char *image;
    int id;

    assert(rgba != NULL);
    assert(width > 0);
    assert(height > 0);

    image = generateColorTexture(rgba, width, height);
    id = loadBuffer(image, width, height, RGBA, 32);
    delete [] image;

    return id;
}

void Texture::setFlag(TextureFlag flag) {
    mFlags |= flag;
}

void Texture::clearFlag(TextureFlag flag) {
    mFlags &= ~flag;
}

void Texture::reset() {
    if (mTextureIds) {
        glDeleteTextures(mTextureLimit, mTextureIds);
        delete [] mTextureIds;
    }

    mTextureIds = NULL;
    mTextureCount = 0;
    mTextureLimit = 0;
}

void Texture::disableMultiTexture() {
    mFlags ^= fUseMultiTexture;
    mTextureId = -1;
    mTextureId2 = -1;

    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE0_ARB);
}

void Texture::useMultiTexture(float aU, float aV, float bU, float bV) {
    if (!(mFlags & fUseMultiTexture))
        return;

    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, aU, aV);
    glMultiTexCoord2fARB(GL_TEXTURE1_ARB, bU, bV);
}

void Texture::useMultiTexture(float u, float v) {
    if (!(mFlags & fUseMultiTexture))
        return;

    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, u, v);
    glMultiTexCoord2fARB(GL_TEXTURE1_ARB, u, v);
}

void Texture::bindMultiTexture(int texture0, int texture1) {
    assert(mTextureIds != NULL);
    assert(texture0 >= 0);
    assert((unsigned int)texture0 <= mTextureCount);
    assert(texture1 >= 0);
    assert((unsigned int)texture1 <= mTextureCount);

    mFlags |= fUseMultiTexture;
    mTextureId  = texture0;
    mTextureId2 = texture1;

    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mTextureIds[texture0]);

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mTextureIds[texture1]);
}

void Texture::setMaxTextureCount(unsigned int n) {
    assert(n > 0);

    mTextureLimit = n;

    mTextureIds = new unsigned int[n];

    glGenTextures(n, mTextureIds);
}

int Texture::getTextureCount() {
    return mTextureCount - 1;
}

int Texture::loadBuffer(unsigned char *image,
        unsigned int width, unsigned int height,
        ColorMode mode, unsigned int bpp) {
    int id;

    assert(image != NULL);
    assert(width > 0);
    assert(height > 0);
    assert((bpp == 8) || (bpp == 24) || (bpp == 32));

    id = loadBufferSlot(image, width, height, mode, bpp, mTextureCount++);

    if (id < 0)
        return id;

    return id;
}

void convertARGB32bppToRGBA32bpp(unsigned char *image,
        unsigned int w, unsigned int h) {
    unsigned int i, size = w * h;
    unsigned char swap;

    assert(image != NULL);
    assert(w > 0);
    assert(h > 0);

    for (i = 0; i < size; ++i) {
        /* 32-bit ARGB to RGBA */
        swap = image[(i * 4) + 3];
        image[(i * 4)] = image[(i * 4) + 1];
        image[(i * 4) + 1] = image[(i * 4) + 2];
        image[(i * 4) + 2] = image[(i * 4) + 3];
        image[(i * 4) + 3] = swap;
    }
}

int Texture::loadBufferSlot(unsigned char *image,
        unsigned int width, unsigned int height,
        ColorMode mode, unsigned int bpp,
        unsigned int slot) {
    unsigned char bytes;
    unsigned int glcMode;

    assert(mTextureIds != NULL);
    assert(slot < mTextureLimit);
    assert(image != NULL);
    assert(width > 0);
    assert(height > 0);
    assert((bpp == 8) || (bpp == 24) || (bpp == 32));

    switch (mode) {
        case GREYSCALE:
            if (bpp != 8) {
                printf("Texture::Load ERROR Unsupported GREYSCALE, %i bpp\n", bpp);
                return -1;
            }
            bytes = 1;
            glcMode = GL_LUMINANCE;
            break;
        case RGB:
            if (bpp != 24) {
                printf("Texture::Load ERROR Unsupported RGB, %i bpp\n", bpp);
                return -1;
            }
            bytes = 3;
            glcMode = GL_RGB;
            break;
        case ARGB:
            if (bpp == 32) {
                convertARGB32bppToRGBA32bpp(image, width, height);
            } else {
                printf("Texture::Load ERROR Unsupported ARGB, %i bpp\n", bpp);
                return -1;
            }
            bytes = 4;
            glcMode = GL_RGBA;
            break;
        case RGBA:
            if (bpp != 32) {
                printf("Texture::Load ERROR Unsupported RGBA, %i bpp\n", bpp);
                return -1;
            }
            bytes = 4;
            glcMode = GL_RGBA;
            break;
    }

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glBindTexture(GL_TEXTURE_2D, mTextureIds[slot]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (mFlags & fUseMipmaps) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                GL_NEAREST_MIPMAP_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                GL_LINEAR_MIPMAP_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexImage2D(GL_TEXTURE_2D, 0, bytes, width, height, 0, glcMode, GL_UNSIGNED_BYTE, image);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, glcMode, width, height, 0,
                glcMode, GL_UNSIGNED_BYTE, image);
    }

    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    return slot;
}

void Texture::bindTextureId(unsigned int n) {
    assert(mTextureIds != NULL);
    assert(n <= mTextureCount);

    mTextureId = n;

    glEnable(GL_TEXTURE_2D);
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBindTexture(GL_TEXTURE_2D, mTextureIds[n]);
}

int Texture::loadTGA(const char *filename) {
#ifdef PCX_PROOF_OF_CONCEPT
    unsigned char *image;
    unsigned int w, h;
    int id = -1;
    int error = pcxLoad(filename, &image, &w, &h);
    if (error == 0) {
        id = loadBuffer(image, w, h, RGBA, 32);
        delete [] image;
    }
    return id;
#else
    FILE *f;
    unsigned char *image = NULL;
    unsigned char *image2 = NULL;
    unsigned int w, h;
    char type;
    int id = -1;

    assert(filename != NULL);
    assert(filename[0] != '\0');

    f = fopen(filename, "rb");

    if (!f) {
        perror("Couldn't load file");
    } else if (!tgaCheck(f)) {
        tgaLoad(f, &image, &w, &h, &type);

        type += 2;

        image2 = scaleBuffer(image, w, h, (type == 4) ? 4 : 3);

        if (image2) {
            image = image2;
            w = h = 256;
        }

        if (image) {
            id = loadBuffer(image, w, h,
                    (type == 4) ? RGBA : RGB,
                    (type == 4) ? 32 : 24);

            delete [] image;
        }

        fclose(f);
    }

    if (id == -1) {
        printf("Texture::loadTGA> ERROR: Failed to load '%s'\n", filename);
    }

    return id;
#endif
}

int Texture::nextPower(int seed) {
    int i = 1;
    for (; i < seed; i *= 2);
    return i;
}

/* This code based off on gluScaleImage()  */
unsigned char *Texture::scaleBuffer(unsigned char *image,
        int width, int height, int components) {
    int i, j, k;
    float* tempin;
    float* tempout;
    float sx, sy;
    //int components = 3;
    unsigned char *timage;
    int original_height = height;
    int original_width = width;

    assert(image != NULL);
    assert(width > 0);
    assert(height > 0);

    height = nextPower(height);
    width = nextPower(width);

    if (height > 256)
        height = 256;

    if (width > 256)
        width = 256;

    // Check to see if scaling is needed
    if (height == original_height && width == original_width)
        return NULL;

    //printf("%i\n", components);

    timage = new unsigned char[height * width * components];
    tempin = new float[original_width * original_height * components * sizeof(float)];
    tempout = new float[width * height * components * sizeof(float)];

    // Copy user data to float format.
    for (i = 0; i < original_height * original_width * components; ++i) {
        tempin[i] = (float)image[i];
    }

    // Determine which filter to use by checking ratios.
    if (width > 1) {
        sx = (float)(original_width - 1) / (float)(width - 1);
    } else {
        sx = (float)(original_width - 1);
    }

    if (height > 1) {
        sy = (float)(original_height - 1) / (float) (height - 1);
    } else {
        sy = (float)(original_height - 1);
    }

    if (sx < 1.0 && sy < 1.0) {
        /* Magnify both width and height:  use weighted sample of 4 pixels */
        int i0, i1, j0, j1;
        float alpha, beta;
        float* src00;
        float* src01;
        float* src10;
        float* src11;
        float s1, s2;
        float* dst;

        for (i = 0; i < height; ++i) {
            i0 = (int)(i * sy);
            i1 = i0 + 1;

            if (i1 >= original_height) {
                i1 = original_height - 1;
            }

            alpha = i * sy - i0;

            for (j = 0; j < width; ++j) {
                j0 = (int) (j * sx);
                j1 = j0 + 1;

                if (j1 >= original_width) {
                    j1 = original_width - 1;
                }

                beta = j * sx - j0;

                /* Compute weighted average of pixels in rect (i0,j0)-(i1,j1) */
                src00 = tempin + (i0 * original_width + j0) * components;
                src01 = tempin + (i0 * original_width + j1) * components;
                src10 = tempin + (i1 * original_width + j0) * components;
                src11 = tempin + (i1 * original_width + j1) * components;

                dst = tempout + (i * width + j) * components;

                for (k = 0; k < components; ++k) {
                    s1 = *src00++ * (1.0f - beta) + *src01++ * beta;
                    s2 = *src10++ * (1.0f - beta) + *src11++ * beta;
                    *dst++ = s1 * (1.0f - alpha) + s2 * alpha;
                }
            }
        }
    } else {
        /* Shrink width and/or height:  use an unweighted box filter */
        int i0, i1;
        int j0, j1;
        int ii, jj;
        float sum;
        float* dst;

        for (i = 0; i < height; ++i) {
            i0 = (int) (i * sy);
            i1 = i0 + 1;

            if (i1 >= original_height) {
                i1 = original_height - 1;
            }

            for (j = 0; j < width; ++j) {
                j0 = (int) (j * sx);
                j1 = j0 + 1;

                if (j1 >= original_width) {
                    j1 = original_width - 1;
                }

                dst = tempout + (i * width + j) * components;

                /* Compute average of pixels in the rectangle (i0,j0)-(i1,j1) */
                for (k = 0; k < components; ++k) {
                    sum = 0.0;

                    for (ii = i0; ii <= i1; ++ii) {
                        for (jj = j0; jj <= j1; ++jj) {
                            sum += *(tempin + (ii * original_width + jj)
                                    * components + k);
                        }
                    }

                    sum /= ( j1 - j0 + 1 ) * ( i1 - i0 + 1 );
                    *dst++ = sum;
                }
            }
        }
    }

    // Copy to our results.
    for( i = 0; i < height * width * components; ++i) {
        timage[i] = (unsigned char)tempout[i];
    }

    // Delete our temp buffers.
    delete[] tempin;
    delete[] tempout;
    delete[] image;

    return timage;
}

