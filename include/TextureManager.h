/*!
 * \file include/TextureManager.h
 * \brief Texture registry
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _TEXTURE_MANAGER_H
#define _TEXTURE_MANAGER_H

#include <vector>

// These are loaded into TextureStorage::SYSTEM by initialize()!
#define TEXTURE_WHITE 0
#define TEXTURE_SPLASH 1

class TextureTileVertex {
  public:
    TextureTileVertex(int xc, int xp, int yc, int yp)
        : xCoordinate(xc), xPixel(xp), yCoordinate(yc), yPixel(yp) { }

    int xCoordinate, xPixel;
    int yCoordinate, yPixel;
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

/*!
 * \brief Texture registry
 */
class TextureManager {
  public:
    static int initialize();
    static int initializeSplash();
    static void shutdown();
    static void clear();

    static int numTextures(TextureStorage s = TextureStorage::GAME);

    /*!
     * \brief Bind texture to next free texture unit.
     * \param n ID of texture to bind
     * \param s Place where texture is stored
     * \returns ID of GL texture unit to which this texture is bound.
     */
    static int bindTexture(unsigned int n, TextureStorage s);

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
    static int loadBufferSlot(unsigned char* image = nullptr,
                              unsigned int width = 256, unsigned int height = 256,
                              ColorMode mode = ColorMode::RGBA, unsigned int bpp = 32,
                              TextureStorage s = TextureStorage::GAME,
                              int slot = -1, bool filter = true);

    static int loadImage(std::string filename, TextureStorage s = TextureStorage::GAME, int slot = -1);

    static void addTile(TextureTile* t);
    static int numTiles();
    static TextureTile& getTile(int index);

    static void addAnimatedTile(int index, int tile);
    static int numAnimatedTiles();
    static int getFirstTileAnimation(int index);
    static int getNextTileAnimation(int tile);

  private:
    static std::vector<unsigned int>& getIds(TextureStorage s);
    static std::vector<int>& getUnits(TextureStorage s);

    static void bindTextureId(unsigned int n, TextureStorage s, unsigned int unit);
    static int loadPCX(std::string filename, TextureStorage s, int slot);

    static std::vector<unsigned int> mTextureIdsGame;
    static std::vector<unsigned int> mTextureIdsSystem;

    static std::vector<TextureTile*> tiles;
    static std::vector<std::vector<int>> animations;

    static std::vector<int> gameUnits;
    static std::vector<int> systemUnits;
    static unsigned int nextFreeTextureUnit;
};

#endif

