/*!
 * \file include/Texture.h
 * \brief Texture registry
 *
 * \author Mongoose
 */

#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <stdio.h>

typedef struct {
    int x;
    int y;
    int w;
    int h;
    int minx;
    int maxx;
    int miny;
    int maxy;
    int advance;
} ttf_glyph_t;

typedef struct {
    unsigned int utf8Offset;
    unsigned int count;

/* [utf8Offset -> utf8Offset+count],
     matches indexing into glyphs[0 -> count] for texcoords, etc
    ----------------------------------------
    41     -> 126     ASCII English w/ special chars,
    0x303f -> 0x3093  Japanese hiragana kana,
    0x301a -> 0x30f6  Japanese katakana kana */

    unsigned int width;      //!< Width and height of RGBA texture
    unsigned char *texture;  //!< RGBA texture data
    ttf_glyph_t *glyphs;     //!< For typesetting and rendering use
    int fontHeight;
    int fontAscent;
    int fontDescent;
    int fontSpacing;
} ttf_texture_t;

typedef struct {
    unsigned int utf8Offset;
    unsigned int count;
    int textureId;
    int drawListBase;
} gl_font_t;

/*!
 * \brief Texture registry
 */
class Texture {
public:

    enum ColorMode {
        GREYSCALE = 1,
        RGB,
        RGBA,
        ARGB
    };

    enum TextureFlag {
        fUseMipmaps      = (1 << 0),
        fUseMultiTexture = (1 << 1),
        fUseSDL_TTF      = (1 << 2)
    };

    /*!
    * \brief Constructs an object of Texture
    */
    Texture();

   /*!
    * \brief Deconstructs an object of Texture
    */
    ~Texture();

    /*!
     * \brief Generates a texture buffer with (width * height * 4) bytes.
     * \param rgba 32bpp RGBA color to fill into buffer
     * \param width width of newly allocated buffer, power of 2, pref same as height
     * \param height height of newly allocated buffer, power of 2, pref same as width
     * \returns newly allocated texture buffer filled with specified color
     */
    static unsigned char *generateColorTexture(unsigned char rgba[4],
                                                unsigned int width,
                                                unsigned int height);

    gl_font_t *generateFont(ttf_texture_t *texture);

    /*!
     * \brief Generates a font texture with typeset info from TTF.
     *
     * Does not load the texture itself! Call loadFont() on returned ttf_texture_t
     * \param filename filename of TTF font
     * \param pointSize Point Size to generate
     * \param textureWidth width of texture, height will match it
     * \param color RGB 24bit color
     * \param utf8Offset Offset into fonts encoding chart
     * \param count number of glyphs to read from offset start
     * \param verbose dumps debug info to stdout
     * \returns font texture. Load it with loadFont()!
     */
    ttf_texture_t *generateFontTexture(const char *filename, int pointSize,
                                        unsigned int textureWidth,
                                        unsigned char color[3],
                                        unsigned int utf8Offset,
                                        unsigned int count,
                                        char verbose);

    /*!
     * \brief Get number of textures in use
     * \returns used texture count, or -1 on error (uninitialized)
     */
    int getTextureCount();

    /*!
     * \brief Dumps a screenshot to disk.
     *
     * Avoids overwriting files with same base name.
     * \param base base filename
     * \param width viewport width
     * \param height viewport height
     */
    void glScreenShot(char *base, unsigned int width, unsigned int height);

    /*!
     * \brief Sets up multitexture rendering with passed ids
     * \param texture0 first texture for multitexture
     * \param texture1 second texture for multitexture
     */
    void bindMultiTexture(int texture0, int texture1);

    /*!
     * \brief Binds the texture for use in GL
     * \param n valid texture index
     */
    void bindTextureId(unsigned int n);

    /*!
     * \brief Clears an option flag
     * \param flag flag to clear
     */
    void clearFlag(TextureFlag flag);

    void disableMultiTexture();

    /*!
     * \brief Loads SDL_TTF
     */
    void initSDL_TTF();

    /*!
     * \brief Loads Buffer as texture
     * \param image pixmap matching other params
     * \param width width of image
     * \param height height of image
     * \param mode mode of image
     * \param bpp bits per pixel of image
     * \returns texture ID or < 0 on error
     */
    int loadBuffer(unsigned char *image,
                    unsigned int width, unsigned int height,
                    ColorMode mode, unsigned int bpp);

    /*!
     * \brief Loads Buffer as texture
     * \param image pixmap matching other params
     * \param width width of image
     * \param height height of image
     * \param mode mode of image
     * \param bpp bits per pixel of image
     * \param slot slot (ID) of image
     * \returns texture ID or < 0 on error
     */
    int loadBufferSlot(unsigned char *image,
                        unsigned int width, unsigned int height,
                        ColorMode mode, unsigned int bpp,
                        unsigned int slot);

    /*!
     * \brief Generates and loads a solid color texture.
     * \param rgba color for new texture
     * \param width width of new texture
     * \param height height of new texture
     * \returns texture ID or -1 on error
     */
    int loadColorTexture(unsigned char rgba[4],
                            unsigned int width, unsigned int height);

    /*!
     * \brief Loads a TTF, generates texture image, glyph list and drawlist
     * \param filename Filename of TTF font
     * \param utf8Offset Offset into Unicode table
     * \param count number of glyphs to load
     * \returns font id if successful, or < 0 on error
     */
    int loadFontTTF(const char *filename,
                        unsigned int utf8Offset, unsigned int count);

    /*!
     * \brief Loads TGA file as texture
     * \param filename Existing TGA file
     * \returns ID of new texture or -1 on error
     */
    int loadTGA(const char *filename);

    /*!
     * \brief Resets all texture data
     */
    void reset();

    /*!
     * \brief Sets an option flag
     * \param flag flag to set
     */
    void setFlag(TextureFlag flag);

    /*!
     * \brief Sets up GL texturing.
     *
     * Must be called as first setup step!
     * \param n maximum number of textures you wish to allow
     */
    void setMaxTextureCount(unsigned int n);

    void useMultiTexture(float u, float v);

    void useMultiTexture(float aU, float aV, float bU, float bV);

    /*!
     * \brief Show a texture for a second, for debugging
     * \param textureId texture id to show
     * \returns GL texture id on success, < 0 on error
     */
    int showTextureDebug(int textureId);

    /*!
     * \brief Renders debug texture.
     *
     * Called in 2D mode from Texture::Display().
     */
    void debugTextureRender(int width, int height);

private:

    int nextPower(int seed);

    unsigned char *scaleBuffer(unsigned char *image, int width, int height,
                                int components);

    int textureDebug;
    int textureDebugFrames;

    unsigned int *mTextureIds;  //!< GL texture list
    unsigned int mTextureCount; //!< Texture counter
    unsigned int mTextureLimit; //!< The texture limit
    unsigned int mFlags;        //!< Class options
    int mTextureId;             //!< Currently bound texture id
    int mTextureId2;            //!< Multitexture Texture Id
};


// Experimental testing

void bufferedPrintf(char *string, unsigned int len, char *s, ...) __attribute__((format(printf, 3, 4)));

void glPrint3d(float x, float y, float z,
                float pitch, float yaw, float roll,
                float scale, char *string);

void glPrint2d(float x, float y, float scale, char *string);

/*!
 * \brief OpenGL ortho projection.
 *
 * Call before using glPrint2d()
 * \param width window width
 * \param height window height
 */
void glEnterMode2d(unsigned int width, unsigned int height);

/*!
 * \brief OpenGL model matrix projection.
 *
 * Call after using glPrint2d()
 */
void glExitMode2d();

#endif
