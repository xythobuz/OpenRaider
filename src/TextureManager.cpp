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

#include "stb/stb_image.h"

#include "global.h"
#include "Log.h"
#include "RunTime.h"
#include "utils/pcx.h"
#include "utils/pixel.h"
#include "utils/strings.h"
#include "TextureManager.h"

glm::vec2 TextureTile::getUV(unsigned int i) {
    glm::vec2 uv(vertices.at(i).xPixel,
                 vertices.at(i).yPixel);

    /*! \fixme
     * This is my somewhat hacky approach to fixing
     * the bad texture-bleeding problems everywhere.
     * That's better, but makes the seams between
     * each sector much more visible!
     */

    if (vertices.at(i).xCoordinate == 1) {
        uv.x += 0.375f;
    }

    if (vertices.at(i).yCoordinate == 1) {
        uv.y += 0.375f;
    }

    return uv / 256.0f;
}

// ----------------------------------------------------------------------------

TextureManager::~TextureManager() {
    while (mTextureIdsSystem.size() > 0) {
        unsigned int id = mTextureIdsSystem.at(mTextureIdsSystem.size() - 1);
        glDeleteTextures(1, &id);
        mTextureIdsSystem.pop_back();
    }

    clear();
}

void TextureManager::clear() {
    while (mTextureIdsGame.size() > 0) {
        unsigned int id = mTextureIdsGame.at(mTextureIdsGame.size() - 1);
        glDeleteTextures(1, &id);
        mTextureIdsGame.pop_back();
    }

    while (!tiles.empty()) {
        delete tiles.at(tiles.size() - 1);
        tiles.pop_back();
    }

    while (!animations.empty())
        animations.pop_back();
}

void TextureManager::addTile(TextureTile* t) {
    tiles.push_back(t);
}

int TextureManager::numTiles() {
    return tiles.size();
}

TextureTile& TextureManager::getTile(int index) {
    assert(index >= 0);
    assert(index < tiles.size());
    return *tiles.at(index);
}

void TextureManager::addAnimatedTile(int index, int tile) {
    while (index >= animations.size())
        animations.push_back(std::vector<int>());

    animations.at(index).push_back(tile);
}

int TextureManager::numAnimatedTiles() {
    return animations.size();
}

int TextureManager::getFirstTileAnimation(int index) {
    assert(index < animations.size());
    assert(animations.at(index).size() > 0);
    return animations.at(index).at(0);
}

int TextureManager::getNextTileAnimation(int tile) {
    for (int a = 0; a < animations.size(); a++) {
        for (int i = 0; i < animations.at(a).size(); i++) {
            if (animations.at(a).at(i) == tile) {
                if (i < (animations.at(a).size() - 1))
                    return animations.at(a).at(i + 1);
                else
                    return animations.at(a).at(0);
            }
        }
    }
    return -1;
}

std::vector<unsigned int>& TextureManager::getIds(TextureStorage s) {
    if (s == TextureStorage::GAME)
        return mTextureIdsGame;
    else
        return mTextureIdsSystem;
}

int TextureManager::initialize() {
    assert(mTextureIdsGame.size() == 0);
    assert(mTextureIdsSystem.size() == 0);

    while (mTextureIdsSystem.size() < 2) {
        unsigned int id;
        glGenTextures(1, &id);
        mTextureIdsSystem.push_back(id);
    }

    return 0;
}

int TextureManager::initializeSplash() {
    unsigned char* image = generateColorTexture(WHITE, 32, 32, 32);
    int res = loadBufferSlot(image, 32, 32, ColorMode::RGBA, 32, TextureStorage::SYSTEM, TEXTURE_WHITE,
                             false);
    delete [] image;
    if (res < 0) {
        return -1;
    }

    //! \fixme Temporary?
    std::string filename = RunTime::getPakDir() + "/tr2/TITLE.PCX";
    if (loadImage(filename.c_str(), TextureStorage::SYSTEM, TEXTURE_SPLASH) < 0) {
        filename = RunTime::getDataDir() + "/splash.tga";
        if (loadImage(filename.c_str(), TextureStorage::SYSTEM, TEXTURE_SPLASH) < 0) {
            return -2;
        }
    }

    return 0;
}

int TextureManager::loadBufferSlot(unsigned char* image,
                                   unsigned int width, unsigned int height,
                                   ColorMode mode, unsigned int bpp,
                                   TextureStorage s, int slot, bool filter) {
    assert(width > 0);
    assert(height > 0);
    assert((mode == ColorMode::RGB)
           || (mode == ColorMode::BGR)
           || (mode == ColorMode::ARGB)
           || (mode == ColorMode::RGBA)
           || (mode ==  ColorMode::BGRA));
    assert((bpp == 8) || (bpp == 24) || (bpp == 32));

    if (slot == -1)
        slot = getIds(s).size();

    while (getIds(s).size() <= slot) {
        unsigned int id;
        glGenTextures(1, &id);
        getIds(s).push_back(id);
    }

    if (image == nullptr)
        return slot;

    unsigned int glcMode;
    switch (mode) {
        case ColorMode::BGR:
            glcMode = GL_BGR;
            break;

        case ColorMode::RGB:
            glcMode = GL_RGB;
            break;

        case ColorMode::ARGB:
            argb2rgba32(image, width, height);
            glcMode = GL_RGBA;
            break;

        case ColorMode::BGRA:
            glcMode = GL_BGRA;
            break;

        case ColorMode::RGBA:
            glcMode = GL_RGBA;
            break;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, getIds(s).at(slot));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, glcMode, GL_UNSIGNED_BYTE, image);

    if (filter) {
        // Trilinear filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    return slot;
}

int TextureManager::numTextures(TextureStorage s) {
    return getIds(s).size();
}

void TextureManager::bindTextureId(unsigned int n, TextureStorage s, unsigned int unit) {
    assert(n < getIds(s).size());
    assert(unit < 80); //! \todo Query GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, getIds(s).at(n));
}

int TextureManager::loadImage(const char* filename, TextureStorage s, int slot) {
    //! \todo case insensitive compare

    if (stringEndsWith(filename, ".pcx") || stringEndsWith(filename, ".PCX")) {
        return loadPCX(filename, s, slot);
    } else {
        int x, y, n;
        unsigned char* data = stbi_load(filename, &x, &y, &n, 0);
        if (data) {
            if ((n < 3) || (n > 4)) {
                getLog() << "Image \"" << filename << "\" has unsupported format ("
                         << n << ")!" << Log::endl;
                stbi_image_free(data);
                return -2;
            }
            int id = loadBufferSlot(data, x, y, (n == 3) ? ColorMode::RGB : ColorMode::RGBA,
                                    (n == 3) ? 24 : 32, s, slot);
            stbi_image_free(data);
            return id;
        } else {
            getLog() << "Can't load image \"" << filename << "\"!" << Log::endl;
            return -1;
        }
    }
}

int TextureManager::loadPCX(const char* filename, TextureStorage s, int slot) {
    assert(filename != nullptr);
    assert(filename[0] != '\0');

    int error = pcxCheck(filename);
    if (!error) {
        unsigned char* image;
        unsigned int w, h, bpp;
        ColorMode c;

        error = pcxLoad(filename, &image, &w, &h, &c, &bpp);
        if (!error) {
            unsigned char* image2 = scaleBuffer(image, &w, &h, bpp);
            if (image2) {
                delete [] image;
                image = image2;
            }
            int id = loadBufferSlot(image, w, h, c, bpp, s, slot);
            delete [] image;
            return id;
        }

        return -5;
    }

    return -4;
}

