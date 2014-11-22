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
#include "Log.h"
#include "RunTime.h"
#include "utils/pcx.h"
#include "utils/pixel.h"
#include "utils/strings.h"
#include "utils/tga.h"
#include "TextureManager.h"

#ifdef USING_PNG
#include "utils/png.h"
#endif

std::vector<unsigned int>& TextureManager::getIds(TextureStorage s) {
    if (s == TextureStorage::GAME)
        return mTextureIdsGame;
    else
        return mTextureIdsSystem;
}

TextureManager::~TextureManager() {
    while (mTextureIdsSystem.size() > 0) {
        unsigned int id = mTextureIdsSystem.at(mTextureIdsSystem.size() - 1);
        glDeleteTextures(1, &id);
        mTextureIdsSystem.pop_back();
    }

    while (mTextureIdsGame.size() > 0) {
        unsigned int id = mTextureIdsGame.at(mTextureIdsGame.size() - 1);
        glDeleteTextures(1, &id);
        mTextureIdsGame.pop_back();
    }
}

int TextureManager::initialize() {
    unsigned char* image = generateColorTexture(WHITE, 32, 32, 32);
    int res = loadBufferSlot(image, 32, 32, RGBA, 32, TextureStorage::SYSTEM, TEXTURE_WHITE);
    delete [] image;
    if (res < 0) {
        return -1;
    }

    //! \fixme Temporary?
    std::string filename = getRunTime().getPakDir() + "/tr2/TITLE.PCX";
    if (loadPCX(filename.c_str(), TextureStorage::SYSTEM, TEXTURE_SPLASH) < 0) {
        filename = getRunTime().getDataDir() + "/splash.tga";
        if (loadTGA(filename.c_str(), TextureStorage::SYSTEM, TEXTURE_SPLASH) < 0) {
            return -2;
        }
    }

    return 0;
}

int TextureManager::loadBufferSlot(unsigned char* image,
                                   unsigned int width, unsigned int height,
                                   ColorMode mode, unsigned int bpp,
                                   TextureStorage s, int slot, bool filter) {
    assert(image != NULL);
    assert(width > 0);
    assert(height > 0);
    assert((mode == GREYSCALE) || (mode == RGB)
           || (mode == BGR) || (mode == ARGB)
           || (mode == RGBA) || (mode ==  BGRA));
    assert((bpp == 8) || (bpp == 24) || (bpp == 32));

    if (slot == -1)
        slot = getIds(s).size();

    while (getIds(s).size() <= slot) {
        unsigned int id;
        glGenTextures(1, &id);
        getIds(s).push_back(id);
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
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, getIds(s).at(slot));

    if (filter) {
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, bpp / 8, width, height, 0, glcMode, GL_UNSIGNED_BYTE, image);

    return slot;
}

void TextureManager::bindTextureId(unsigned int n, TextureStorage s) {
    assert(n < getIds(s).size());

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBindTexture(GL_TEXTURE_2D, getIds(s).at(n));
}

int TextureManager::loadImage(const char* filename, TextureStorage s, int slot) {
    if (stringEndsWith(filename, ".pcx") || stringEndsWith(filename, ".PCX")) {
        return loadPCX(filename, s, slot);
    } else if (stringEndsWith(filename, ".png") || stringEndsWith(filename, ".PNG")) {
        return loadPNG(filename, s, slot);
    } else if (stringEndsWith(filename, ".tga") || stringEndsWith(filename, ".TGA")) {
        return loadTGA(filename, s, slot);
    } else {
        getLog() << "No known image file type? (" << filename << ")" << Log::endl;
    }

    return -1;
}

int TextureManager::loadPCX(const char* filename, TextureStorage s, int slot) {
    assert(filename != NULL);
    assert(filename[0] != '\0');

    unsigned char* image;
    unsigned int w, h, bpp;
    ColorMode c;
    int id = -1;
    int error = pcxLoad(filename, &image, &w, &h, &c, &bpp);

    if (error == 0) {
        unsigned char* image2 = scaleBuffer(image, &w, &h, bpp);
        if (image2) {
            delete [] image;
            image = image2;
        }
        id = loadBufferSlot(image, w, h, c, bpp, s, slot);
        delete [] image;
    }

    return id;
}

int TextureManager::loadPNG(const char* filename, TextureStorage s, int slot) {
#ifdef USING_PNG
    assert(filename != NULL);
    assert(filename[0] != '\0');

    if (pngCheck(filename) != 0) {
        return -1;
    }

    unsigned char* image;
    unsigned int w, h, bpp;
    ColorMode c;
    int id = -1;
    int error = pngLoad(filename, &image, &w, &h, &c, &bpp);

    if (error == 0) {
        unsigned char* image2 = scaleBuffer(image, &w, &h, bpp);
        if (image2) {
            delete [] image;
            image = image2;
        }
        id = loadBufferSlot(image, w, h, c, bpp, s, slot);
        delete [] image;
    }

    return id;
#else
    getLog() << "No PNG support available (" << filename << ")" << Log::endl;
    return -1;
#endif
}

int TextureManager::loadTGA(const char* filename, TextureStorage s, int slot) {
    assert(filename != NULL);
    assert(filename[0] != '\0');

    unsigned char* image;
    unsigned int w, h;
    char type;
    int id = -1;

    if (!tgaCheck(filename)) {
        tgaLoad(filename, &image, &w, &h, &type);

        unsigned char* image2 = scaleBuffer(image, &w, &h, (type == 2) ? 32 : 24);
        if (image2) {
            delete [] image;
            image = image2;
        }
        if (image) {
            id = loadBufferSlot(image, w, h,
                                (type == 2) ? RGBA : RGB,
                                (type == 2) ? 32 : 24,
                                s, slot);
            delete [] image;
        }
    }

    return id;
}

