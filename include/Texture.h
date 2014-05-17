/*!
 * \file include/Texture.h
 * \brief Texture registry
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <stdio.h>

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

    /*!
     * \brief Get number of textures in use
     * \returns used texture count, or -1 on error (uninitialized)
     */
    int getTextureCount();

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

private:

    int nextPower(int seed);

    unsigned char *scaleBuffer(unsigned char *image, int width, int height,
                                int components);

    unsigned int *mTextureIds;  //!< GL texture list
    unsigned int mTextureCount; //!< Texture counter
    unsigned int mTextureLimit; //!< The texture limit
    unsigned int mFlags;        //!< Class options
    int mTextureId;             //!< Currently bound texture id
    int mTextureId2;            //!< Multitexture Texture Id
};

#endif

