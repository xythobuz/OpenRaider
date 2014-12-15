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

// These are loaded into TextureStorage::SYSTEM by initialize()!
#define TEXTURE_WHITE 0
#define TEXTURE_SPLASH 1

class TextureTileVertex {
  public:
    TextureTileVertex(uint8_t xc, uint8_t xp, uint8_t yc, uint8_t yp);

    uint8_t xCoordinate, xPixel;
    uint8_t yCoordinate, yPixel;
};

class TextureTile {
  public:
    TextureTile(uint16_t a, uint16_t t) : attribute(a), texture(t) { }
    ~TextureTile();

    void add(TextureTileVertex* t);

    void displayTriangle(float a[3], float b[3], float c[3]);
    void displayRectangle(float a[3], float b[3], float c[3], float d[3]);

  private:
    uint16_t attribute;
    uint16_t texture;
    std::vector<TextureTileVertex*> vertices;
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

    int loadTGA(const char* filename, TextureStorage s, int slot);
    int loadPCX(const char* filename, TextureStorage s, int slot);
    int loadPNG(const char* filename, TextureStorage s, int slot);

    std::vector<unsigned int> mTextureIdsGame;
    std::vector<unsigned int> mTextureIdsSystem;

    std::vector<TextureTile*> tiles;
    std::vector<std::vector<int>> animations;
};

TextureManager& getTextureManager();

#endif

