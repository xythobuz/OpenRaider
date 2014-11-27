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
    bool isTriangle();
    void display(float x, float y, float w, float h, float z);

  private:
    void displayTriangle(float x, float y, float w, float h, float z);
    void displayRectangle(float x, float y, float w, float h, float z);

    uint16_t attribute;
    uint16_t texture;
    std::vector<TextureTileVertex*> vertices;
};

/*!
 * \brief Texture registry
 */
class TextureManager {
  public:

    enum class TextureStorage {
        GAME,
        SYSTEM
    };

    ~TextureManager();

    int initialize();

    int numTextures(TextureStorage s = TextureStorage::GAME);

    /*!
     * \brief Binds the texture for use in GL
     * \param n valid texture index
     * \param s Which TextureStorage should be accessed
     */
    void bindTextureId(unsigned int n, TextureStorage s = TextureStorage::GAME);

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
    int loadBufferSlot(unsigned char* image,
                       unsigned int width, unsigned int height,
                       ColorMode mode, unsigned int bpp,
                       TextureStorage s = TextureStorage::GAME,
                       int slot = -1, bool filter = true);

    int loadImage(const char* filename, TextureStorage s = TextureStorage::GAME, int slot = -1);

    void addTile(TextureTile* t);
    int numTiles();
    TextureTile& getTile(int index);

  private:
    std::vector<unsigned int>& getIds(TextureStorage s);

    int loadTGA(const char* filename, TextureStorage s, int slot);
    int loadPCX(const char* filename, TextureStorage s, int slot);
    int loadPNG(const char* filename, TextureStorage s, int slot);

    std::vector<unsigned int> mTextureIdsGame;
    std::vector<unsigned int> mTextureIdsSystem;

    std::vector<TextureTile*> tiles;
};

TextureManager& getTextureManager();

#endif

