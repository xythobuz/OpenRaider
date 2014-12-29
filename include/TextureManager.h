/*!
 * \file include/TextureManager.h
 * \brief Texture registry
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _TEXTURE_MANAGER_H
#define _TEXTURE_MANAGER_H

#include <cstdint>
#include <vector>
#include <glm/vec2.hpp>

// These are loaded into TextureStorage::SYSTEM by initialize()!
#define TEXTURE_WHITE 0
#define TEXTURE_SPLASH 1

class TextureTileVertex {
  public:
    TextureTileVertex(uint8_t xc, uint8_t xp, uint8_t yc, uint8_t yp)
        : xCoordinate(xc), xPixel(xp), yCoordinate(yc), yPixel(yp) { }

    uint8_t xCoordinate, xPixel;
    uint8_t yCoordinate, yPixel;
};

class TextureTile {
  public:
    TextureTile(unsigned int a, unsigned int t) : attribute(a), texture(t) { }
    void add(TextureTileVertex t) { vertices.push_back(t); }

    unsigned int getTexture() { return texture; }
    glm::vec2 getUV(unsigned int i);

  private:
    unsigned int attribute;
    unsigned int texture;
    std::vector<TextureTileVertex> vertices;
};

/*!
 * \brief Texture registry
 */
class TextureManager {
  public:

    enum class ColorMode {
        RGB,
        RGBA,
        ARGB,
        BGR,
        BGRA
    };

    enum class TextureStorage {
        GAME,
        SYSTEM
    };

    ~TextureManager();

    int initialize();
    int initializeSplash();

    void clear();

    int numTextures(TextureStorage s = TextureStorage::GAME);

    /*!
     * \brief Binds the texture for use in GL
     * \param n valid texture index
     * \param s Which TextureStorage should be accessed
     * \param unit Which GL texture unit should be used
     */
    void bindTextureId(unsigned int n, TextureStorage s = TextureStorage::GAME, unsigned int unit = 0);

    /*!
     * \brief Loads Buffer as texture
     * \param image pixmap matching other params
     * \param width width of image
     * \param height height of image
     * \param mode mode of image
     * \param bpp bits per pixel of image
     * \param s Which TextureStorage should be accessed
     * \param slot slot (ID) of image
     * \param filter if the texture should be mipmap filtered
     * \returns texture ID or < 0 on error
     */
    int loadBufferSlot(unsigned char* image = nullptr,
                       unsigned int width = 256, unsigned int height = 256,
                       ColorMode mode = ColorMode::RGBA, unsigned int bpp = 32,
                       TextureStorage s = TextureStorage::GAME,
                       int slot = -1, bool filter = true);

    int loadImage(const char* filename, TextureStorage s = TextureStorage::GAME, int slot = -1);

    void addTile(TextureTile* t);
    int numTiles();
    TextureTile& getTile(int index);

    void addAnimatedTile(int index, int tile);
    int numAnimatedTiles();
    int getFirstTileAnimation(int index);
    int getNextTileAnimation(int tile);

  private:
    std::vector<unsigned int>& getIds(TextureStorage s);
    int loadPCX(const char* filename, TextureStorage s, int slot);

    std::vector<unsigned int> mTextureIdsGame;
    std::vector<unsigned int> mTextureIdsSystem;

    std::vector<TextureTile*> tiles;
    std::vector<std::vector<int>> animations;
};

TextureManager& getTextureManager();

#endif

