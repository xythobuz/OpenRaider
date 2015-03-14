/*!
 * \file src/TextureManager.cpp
 * \brief Texture Registry
 *
 * \author Mongoose
 * \author xythobuz
 */

#include "imgui/imgui.h"
#include "stb/stb_image.h"

#include "global.h"
#include "Game.h"
#include "Log.h"
#include "RunTime.h"
#include "World.h"
#include "utils/Folder.h"
#include "utils/pcx.h"
#include "utils/pixel.h"
#include "utils/random.h"
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

#define COLOR_PALETTE_SIZE 256

std::vector<unsigned int> TextureManager::mTextureIdsGame;
std::vector<unsigned int> TextureManager::mTextureIdsSystem;
std::vector<TextureTile*> TextureManager::tiles;
std::vector<std::vector<int>> TextureManager::animations;
std::vector<int> TextureManager::gameUnits;
std::vector<int> TextureManager::systemUnits;
unsigned int TextureManager::nextFreeTextureUnit = 0;
std::vector<BufferManager> TextureManager::gameBuffers;
std::vector<BufferManager> TextureManager::systemBuffers;
std::array<glm::vec4, 256> TextureManager::colorPalette;
std::vector<std::tuple<unsigned char*, unsigned int, unsigned int>> TextureManager::indexedTextures;

int TextureManager::initialize() {
    assertEqual(mTextureIdsGame.size(), 0);
    assertEqual(mTextureIdsSystem.size(), 0);

    while (mTextureIdsSystem.size() < 2) {
        unsigned int id;
        glGenTextures(1, &id);
        mTextureIdsSystem.push_back(id);
    }

    unsigned char* image = generateColorTexture(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 32, 32, 32);
    int res = loadBufferSlot(image, 32, 32, ColorMode::RGBA, 32, TextureStorage::SYSTEM, TEXTURE_WHITE,
                             false);
    delete [] image;
    if (res < 0) {
        return -1;
    }

    return 0;
}

int TextureManager::initializeSplash() {
    Folder f(RunTime::getPakDir());
    std::vector<File> files;
    f.findRecursiveFilesEndingWith(files, ".pcx");
    f.findRecursiveFilesEndingWith(files, ".bmp");
    f.findRecursiveFilesEndingWith(files, ".png");
    f.findRecursiveFilesEndingWith(files, ".tga");
    f.findRecursiveFilesEndingWith(files, ".jpg");
    if (files.size() == 0) {
        if (loadImage(RunTime::getDataDir() + "/splash.tga", TextureStorage::SYSTEM, TEXTURE_SPLASH) < 0) {
            return -2;
        }
    } else {
        int i = randomInteger(files.size() - 1);
        if (loadImage(files.at(i).getPath(), TextureStorage::SYSTEM, TEXTURE_SPLASH) < 0) {
            if (loadImage(RunTime::getDataDir() + "/splash.tga", TextureStorage::SYSTEM, TEXTURE_SPLASH) < 0) {
                return -3;
            }
        }
    }

    return 0;
}

void TextureManager::shutdown() {
    while (mTextureIdsSystem.size() > 0) {
        unsigned int id = mTextureIdsSystem.at(mTextureIdsSystem.size() - 1);
        glDeleteTextures(1, &id);
        mTextureIdsSystem.pop_back();
    }

    gameBuffers.clear();
    systemBuffers.clear();

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

    animations.clear();

    gameUnits.clear();
    systemUnits.clear();
    nextFreeTextureUnit = 0;

    indexedTextures.clear();
}

int TextureManager::loadBufferSlot(unsigned char* image,
                                   unsigned int width, unsigned int height,
                                   ColorMode mode, unsigned int bpp,
                                   TextureStorage s, int slot, bool filter) {
    assertGreaterThan(width, 0);
    assertGreaterThan(height, 0);
    assert((mode == ColorMode::RGB)
           || (mode == ColorMode::BGR)
           || (mode == ColorMode::ARGB)
           || (mode == ColorMode::RGBA)
           || (mode ==  ColorMode::BGRA));
    assert((bpp == 8) || (bpp == 24) || (bpp == 32));

    if (slot < 0)
        slot = getIds(s).size();

    while (getIds(s).size() <= slot) {
        unsigned int id;
        glGenTextures(1, &id);
        getIds(s).push_back(id);
    }

    GLenum glcMode;
    switch (mode) {
        case ColorMode::BGR:
            glcMode = GL_BGR;
            break;

        case ColorMode::RGB:
            glcMode = GL_RGB;
            break;

        case ColorMode::ARGB:
            if (image != nullptr)
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
    bindTexture(slot, s);
    glTexImage2D(GL_TEXTURE_2D, 0, GLint(GL_RGBA), width, height, 0, glcMode, GL_UNSIGNED_BYTE, image);

    if (filter) {
        // Trilinear filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLint(GL_REPEAT));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLint(GL_REPEAT));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLint(GL_LINEAR));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLint(GL_LINEAR_MIPMAP_LINEAR));
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLint(GL_NEAREST));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLint(GL_NEAREST));
    }

    return slot;
}

int TextureManager::numTextures(TextureStorage s) {
    return getIds(s).size();
}

void TextureManager::bindTextureId(unsigned int n, TextureStorage s, unsigned int unit) {
    assertLessThan(n, getIds(s).size());
    assertLessThan(unit, 80); //! \fixme Query GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, getIds(s).at(n));
}

int TextureManager::bindTexture(unsigned int n, TextureStorage s) {
    assertLessThan(n, getIds(s).size());

    if ((n < getUnits(s).size()) && (getUnits(s).at(n) >= 0)) {
        bindTextureId(n, s, getUnits(s).at(n));
        return getUnits(s).at(n);
    } else {
        while (getUnits(s).size() <= n)
            getUnits(s).push_back(-1);
        getUnits(s).at(n) = nextFreeTextureUnit;
        bindTextureId(n, s, nextFreeTextureUnit);
        nextFreeTextureUnit++;
        return nextFreeTextureUnit - 1;
    }
}

unsigned int TextureManager::getTextureID(int n, TextureStorage s) {
    assertLessThan(n, getIds(s).size());
    return getIds(s).at(n);
}

void TextureManager::addTile(TextureTile* t) {
    tiles.push_back(t);
}

int TextureManager::numTiles() {
    return tiles.size();
}

TextureTile& TextureManager::getTile(int index) {
    assertGreaterThanEqual(index, 0);
    assertLessThan(index, tiles.size());
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
    assertLessThan(index, animations.size());
    assertGreaterThan(animations.at(index).size(), 0);
    return animations.at(index).at(0);
}

int TextureManager::getNextTileAnimation(int index, int tile) {
    assertLessThan(index, animations.size());
    for (int i = 0; i < animations.at(index).size(); i++) {
        if (animations.at(index).at(i) == tile) {
            if (i < (animations.at(index).size() - 1))
                return animations.at(index).at(i + 1);
            else
                return animations.at(index).at(0);
        }
    }
    return -1;
}

BufferManager* TextureManager::getBufferManager(int tex, TextureStorage store) {
    auto& v = (store == TextureStorage::GAME) ? gameBuffers : systemBuffers;
    while (v.size() <= (tex + 1)) {
        v.emplace_back(v.size(), store);
    }
    return &(v.at(tex));
}

void TextureManager::setPalette(int index, glm::vec4 color) {
    assertGreaterThanEqual(index, 0);
    assertLessThan(index, COLOR_PALETTE_SIZE);
    colorPalette[index] = color;
}

glm::vec4 TextureManager::getPalette(int index) {
    assertGreaterThanEqual(index, 0);
    assertLessThan(index, COLOR_PALETTE_SIZE);
    return colorPalette[index];
}

void TextureManager::addIndexedTexture(unsigned char* image, unsigned int width,
                                       unsigned int height) {
    unsigned char* img = new unsigned char[width * height];
    for (unsigned int i = 0; i < (width * height); i++)
        img[i] = image[i];
    indexedTextures.emplace_back(img, width, height);
}

void TextureManager::prepare() {
    for (int i = 0; i < indexedTextures.size(); i++) {
        auto tex = indexedTextures.at(i);
        unsigned char* img = std::get<0>(tex);
        unsigned int width = std::get<1>(tex);
        unsigned int height = std::get<2>(tex);
        unsigned char* image = new unsigned char[width * height * 4];
        for (unsigned int i = 0; i < (width * height); i++) {
            auto col = getPalette(img[i]);
            image[i * 4] = col.x * 255;
            image[(i * 4) + 1] = col.y * 255;
            image[(i * 4) + 2] = col.z * 255;
            image[(i * 4) + 3] = col.w * 255;
        }
        delete [] img;
        loadBufferSlot(image, width, height, ColorMode::RGBA, 32, TextureStorage::GAME, i, true);
    }
}

int TextureManager::loadImage(std::string filename, TextureStorage s, int slot) {
    if (stringEndsWith(filename, ".pcx")) {
        return loadPCX(filename, s, slot);
    } else {
        int x, y, n;
        unsigned char* data = stbi_load(filename.c_str(), &x, &y, &n, 0);
        if (data) {
            if ((n < 3) || (n > 4)) {
                Log::get(LOG_ERROR) << "Image \"" << filename << "\" has unsupported format ("
                                    << n << ")!" << Log::endl;
                stbi_image_free(data);
                return -2;
            }
            int id = loadBufferSlot(data, x, y, (n == 3) ? ColorMode::RGB : ColorMode::RGBA,
                                    (n == 3) ? 24 : 32, s, slot);
            stbi_image_free(data);
            return id;
        } else {
            Log::get(LOG_ERROR) << "Can't load image \"" << filename << "\"!" << Log::endl;
            return -1;
        }
    }
}

int TextureManager::loadPCX(std::string filename, TextureStorage s, int slot) {
    int error = pcxCheck(filename.c_str());
    if (!error) {
        unsigned char* image;
        unsigned int w, h, bpp;
        ColorMode c;

        error = pcxLoad(filename.c_str(), &image, &w, &h, &c, &bpp);
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

std::vector<unsigned int>& TextureManager::getIds(TextureStorage s) {
    if (s == TextureStorage::GAME)
        return mTextureIdsGame;
    else
        return mTextureIdsSystem;
}

std::vector<int>& TextureManager::getUnits(TextureStorage s) {
    if (s == TextureStorage::GAME)
        return gameUnits;
    else
        return systemUnits;
}

void TextureManager::display() {
    if (ImGui::CollapsingHeader("Texture Viewer")) {
        static bool game = Game::isLoaded();
        static int index = 0;
        ImGui::SliderInt("##texslide", &index, 0, TextureManager::numTextures(
                             game ? TextureStorage::GAME : TextureStorage::SYSTEM) - 1);
        ImGui::SameLine();
        if (ImGui::Button("+##texplus", ImVec2(0, 0), true)) {
            if (index < (numTextures(
                             game ? TextureStorage::GAME : TextureStorage::SYSTEM) - 1))
                index++;
            else
                index = 0;
        }
        ImGui::SameLine();
        if (ImGui::Button("-##texminus", ImVec2(0, 0), true)) {
            if (index > 0)
                index--;
            else
                index = numTextures(
                            game ? TextureStorage::GAME : TextureStorage::SYSTEM) - 1;
        }

        if ((numTextures(TextureStorage::GAME) > 0)) {
            ImGui::SameLine();
            ImGui::Checkbox("Game##texgame", &game);
        } else {
            game = false;
        }

        if (index >= numTextures(game ? TextureStorage::GAME : TextureStorage::SYSTEM)) {
            index = numTextures(game ? TextureStorage::GAME : TextureStorage::SYSTEM) - 1;
            if (index < 0) {
                game = false;
                index = 0;
            }
        }

        auto bm = getBufferManager(index, game ? TextureStorage::GAME
                                   : TextureStorage::SYSTEM);
        ImGui::Image(bm, ImVec2(ImGui::GetColumnWidth() * 2 / 3, ImGui::GetColumnWidth() * 2 / 3));
    }

    if (ImGui::CollapsingHeader("Textile Viewer")) {
        if (numTiles() > 0) {
            static int index = 0;
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SliderInt("##tileslide", &index, 0, numTiles() - 1);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (ImGui::Button("+##tileplus", ImVec2(0, 0), true)) {
                if (index < (numTiles() - 1))
                    index++;
                else
                    index = 0;
            }
            ImGui::SameLine();
            if (ImGui::Button("-##tileminus", ImVec2(0, 0), true)) {
                if (index > 0)
                    index--;
                else
                    index = numTiles() - 1;
            }

            if (index >= numTiles())
                index = 0;

            auto& tile = getTile(index);
            auto bm = getBufferManager(tile.getTexture(), TextureStorage::GAME);
            ImVec2 size(ImGui::GetColumnWidth() * 2 / 3, ImGui::GetColumnWidth() * 2 / 3);
            auto uvA = tile.getUV(0);
            auto uvB = tile.getUV(2);
            ImVec2 uv1(uvA.x, uvA.y);
            ImVec2 uv2(uvB.x, uvB.y);
            ImGui::Image(bm, size, uv1, uv2);
        } else {
            ImGui::Text("No textiles are currently loaded...!");
        }
    }

    if (ImGui::CollapsingHeader("Animated Textile Viewer")) {
        if (numAnimatedTiles() > 0) {
            static int index = 0;
            static int tile = getFirstTileAnimation(index);
            if (ImGui::SliderInt("##animslide", &index, 0, numAnimatedTiles() - 1)) {
                tile = getFirstTileAnimation(index);
            }
            ImGui::SameLine();
            if (ImGui::Button("+##animplus", ImVec2(0, 0), true)) {
                if (index < (numAnimatedTiles() - 1))
                    index++;
                else
                    index = 0;
                tile = getFirstTileAnimation(index);
            }
            ImGui::SameLine();
            if (ImGui::Button("-##animminus", ImVec2(0, 0), true)) {
                if (index > 0)
                    index--;
                else
                    index = numAnimatedTiles() - 1;
                tile = getFirstTileAnimation(index);
            }

            if (index >= numAnimatedTiles()) {
                index = 0;
                tile = getFirstTileAnimation(index);
            }

            int next = getNextTileAnimation(index, tile);
            if (next == -1) {
                index = 0;
                tile = getFirstTileAnimation(index);
            }

            ImGui::SameLine();
            ImGui::Text("%d", tile);

            auto& t = getTile(tile);
            auto bm = getBufferManager(t.getTexture(), TextureStorage::GAME);
            ImVec2 size(ImGui::GetColumnWidth() * 2 / 3, ImGui::GetColumnWidth() * 2 / 3);
            auto uvA = t.getUV(0);
            auto uvB = t.getUV(2);
            ImVec2 uv1(uvA.x, uvA.y);
            ImVec2 uv2(uvB.x, uvB.y);
            ImGui::Image(bm, size, uv1, uv2);

            static int fr = 0;
            if (fr > 0) {
                fr--;
            } else {
                fr = RunTime::getFPS() / 5;
                tile = next;
            }
        } else {
            ImGui::Text("No animated textures are currently loaded...!");
        }
    }

    if (ImGui::CollapsingHeader("Sprite Sequence Viewer")) {
        if (getWorld().sizeSprite() <= 0) {
            ImGui::Text("Please load a level containing sprites!");
        } else {
            static int index = 0;
            static int sprite = 0;
            if (ImGui::SliderInt("##spriteslide", &index, 0, getWorld().sizeSpriteSequence() - 1)) {
                sprite = 0;
            }
            ImGui::SameLine();
            if (ImGui::Button("+##spriteplus", ImVec2(0, 0), true)) {
                if (index < (getWorld().sizeSpriteSequence() - 1))
                    index++;
                else
                    index = 0;
                sprite = 0;
            }
            ImGui::SameLine();
            if (ImGui::Button("-##spriteminus", ImVec2(0, 0), true)) {
                if (index > 0)
                    index--;
                else
                    index = getWorld().sizeSpriteSequence() - 1;
                sprite = 0;
            }

            if (index >= getWorld().sizeSpriteSequence()) {
                index = 0;
                sprite = 0;
            }

            if (sprite >= getWorld().getSpriteSequence(index).size()) {
                sprite = 0;
            }

            ImGui::SameLine();
            ImGui::Text("Sprite %d/%d", sprite + 1, getWorld().getSpriteSequence(index).size());

            auto& s = getWorld().getSprite(getWorld().getSpriteSequence(index).getStart() + sprite);
            auto bm = getBufferManager(s.getTexture(), TextureStorage::GAME);
            ImVec2 size(ImGui::GetColumnWidth() * 2 / 3, ImGui::GetColumnWidth() * 2 / 3);
            auto uv = s.getUVs();
            ImVec2 uv1(uv.x, uv.w);
            ImVec2 uv2(uv.z, uv.y);
            ImGui::Image(bm, size, uv1, uv2);

            static int fr = 0;
            if (fr > 0) {
                fr--;
            } else {
                fr = RunTime::getFPS() / 10;
                if (sprite < (getWorld().getSpriteSequence(index).size() - 1))
                    sprite++;
                else
                    sprite = 0;
            }

        }
    }
}

