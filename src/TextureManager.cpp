/*!
 * \file src/TextureManager.cpp
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
#include "Console.h"
#include "OpenRaider.h"
#include "utils/pcx.h"
#include "utils/pixel.h"
#include "utils/strings.h"
#include "utils/tga.h"
#include "TextureManager.h"

#ifdef USING_PNG
#include "utils/png.h"
#endif

TextureManager::TextureManager() {
    mFlags = 0;
}

TextureManager::~TextureManager() {
    reset();
}

void TextureManager::reset() {
    while (mTextureIds.size() > 0) {
        unsigned int id = mTextureIds.at(mTextureIds.size() - 1);
        glDeleteTextures(1, &id);
        mTextureIds.pop_back();
    }
}

int TextureManager::initialize() {
    unsigned char *image = generateColorTexture(WHITE, 32, 32, 32);
    loadBufferSlot(image, 32, 32, RGBA, 32, mTextureIds.size());
    delete [] image;

    //! \fixme Temporary
    char *filename = bufferString("%s/tr2/TITLE.PCX", getOpenRaider().mPakDir);
    if (loadPCX(filename) < 0) {
        delete [] filename;
        //! \fixme Error Checking. Return negative error code, check in calling place too
        filename = bufferString("%s/%s", getOpenRaider().mDataDir, "splash.tga");
        loadTGA(filename);
    }
    delete [] filename;

    return (mTextureIds.size() == 0) ? -1 : 0;
}

void TextureManager::setFlag(TextureFlag flag) {
    mFlags |= flag;
}

void TextureManager::clearFlag(TextureFlag flag) {
    mFlags &= ~flag;
}

int TextureManager::getTextureCount() {
    return mTextureIds.size();
}

void TextureManager::disableMultiTexture() {
    mFlags &= ~fUseMultiTexture;

#ifdef MULTITEXTURE
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE0_ARB);
#endif
}

void TextureManager::useMultiTexture(float aU, float aV, float bU, float bV) {
    if (!(mFlags & fUseMultiTexture))
        return;

#ifdef MULTITEXTURE
    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, aU, aV);
    glMultiTexCoord2fARB(GL_TEXTURE1_ARB, bU, bV);
#endif
}

void TextureManager::bindMultiTexture(int texture0, int texture1) {
    assert(texture0 >= 0);
    assert(texture1 >= 0);
    assert((unsigned int)texture0 < mTextureIds.size());
    assert((unsigned int)texture1 < mTextureIds.size());

    mFlags |= fUseMultiTexture;

#ifdef MULTITEXTURE
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mTextureIds.at(texture0));

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mTextureIds.at(texture1));
#endif
}

int TextureManager::loadBufferSlot(unsigned char *image,
        unsigned int width, unsigned int height,
        ColorMode mode, unsigned int bpp,
        unsigned int slot) {
    assert(image != NULL);
    assert(width > 0);
    assert(height > 0);
    assert((mode == GREYSCALE) || (mode == RGB)
            || (mode == BGR) || (mode == ARGB)
            || (mode == RGBA) || (mode ==  BGRA));
    assert((bpp == 8) || (bpp == 24) || (bpp == 32));

    while (mTextureIds.size() <= slot) {
        unsigned int id;
        glGenTextures(1, &id);
        mTextureIds.push_back(id);
    }

    unsigned int glcMode;
    switch (mode) {
        case GREYSCALE:
            glcMode = GL_LUMINANCE;
            break;

        case BGR:
            bgr2rgb24(image, width, height);
            glcMode = GL_RGB;
            break;

        case RGB:
            glcMode = GL_RGB;
            break;

        case ARGB:
            argb2rgba32(image, width, height);
            glcMode = GL_RGBA;
            break;

        case BGRA:
            bgra2rgba32(image, width, height);
            glcMode = GL_RGBA;
            break;

        case RGBA:
            glcMode = GL_RGBA;
            break;
    }

    glColor3ubv(WHITE);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glBindTexture(GL_TEXTURE_2D, mTextureIds[slot]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, bpp / 8, width, height, 0, glcMode, GL_UNSIGNED_BYTE, image);

    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    return slot;
}

void TextureManager::bindTextureId(unsigned int n) {
    assert(n < mTextureIds.size());

    glEnable(GL_TEXTURE_2D);
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBindTexture(GL_TEXTURE_2D, mTextureIds.at(n));
}

int TextureManager::loadImage(const char *filename) {
    if (stringEndsWith(filename, ".pcx") || stringEndsWith(filename, ".PCX")) {
        return loadPCX(filename);
    } else if (stringEndsWith(filename, ".png") || stringEndsWith(filename, ".PNG")) {
        return loadPNG(filename);
    } else if (stringEndsWith(filename, ".tga") || stringEndsWith(filename, ".TGA")) {
        return loadTGA(filename);
    } else {
        getConsole().print("No known image file type? (%s)", filename);
    }

    return -1;
}

int TextureManager::loadPCX(const char *filename) {
    assert(filename != NULL);
    assert(filename[0] != '\0');

    unsigned char *image;
    unsigned int w, h, bpp;
    ColorMode c;
    int id = -1;
    int error = pcxLoad(filename, &image, &w, &h, &c, &bpp);

    if (error == 0) {
        unsigned char *image2 = scaleBuffer(image, &w, &h, bpp);
        if (image2) {
            delete [] image;
            image = image2;
        }
        id = loadBufferSlot(image, w, h, c, bpp, mTextureIds.size());
        delete [] image;
    }

    return id;
}

int TextureManager::loadPNG(const char *filename) {
#ifdef USING_PNG
    assert(filename != NULL);
    assert(filename[0] != '\0');

    if (pngCheck(filename) != 0) {
        return -1;
    }

    unsigned char *image;
    unsigned int w, h, bpp;
    ColorMode c;
    int id = -1;
    int error = pngLoad(filename, &image, &w, &h, &c, &bpp);

    if (error == 0) {
        unsigned char *image2 = scaleBuffer(image, &w, &h, bpp);
        if (image2) {
            delete [] image;
            image = image2;
        }
        id = loadBufferSlot(image, w, h, c, bpp, mTextureIds.size());
        delete [] image;
    }

    return id;
#else
    getConsole().print("No PNG support available (%s)", filename);
    return -1;
#endif
}

int TextureManager::loadTGA(const char *filename) {
    assert(filename != NULL);
    assert(filename[0] != '\0');

    unsigned char *image;
    unsigned int w, h;
    char type;
    int id = -1;

    if (!tgaCheck(filename)) {
        tgaLoad(filename, &image, &w, &h, &type);

        unsigned char *image2 = scaleBuffer(image, &w, &h, (type == 2) ? 32 : 24);
        if (image2) {
            delete [] image;
            image = image2;
        }
        if (image) {
            id = loadBufferSlot(image, w, h,
                    (type == 2) ? RGBA : RGB,
                    (type == 2) ? 32 : 24,
                    mTextureIds.size());
            delete [] image;
        }
    }

    return id;
}

