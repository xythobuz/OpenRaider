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

/*!
 * \brief Texture registry
 */
class TextureManager {
public:

    enum ColorMode {
        GREYSCALE,
        RGB,
        RGBA,
        ARGB,
        BGR,
        BGRA
    };

    enum TextureFlag {
        fUseMultiTexture = (1 << 0),
    };

    /*!
    * \brief Constructs an object of Texture
    */
    TextureManager();

   /*!
    * \brief Deconstructs an object of Texture
    */
    ~TextureManager();

    /*!
     * \brief Resets all texture data
     */
    void reset();

    int initialize();

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
     * \param slot slot (ID) of image
     * \returns texture ID or < 0 on error
     */
    int loadBufferSlot(unsigned char *image,
                        unsigned int width, unsigned int height,
                        ColorMode mode, unsigned int bpp,
                        unsigned int slot);

    /*!
     * \brief Loads TGA file as texture
     * \param filename Existing TGA file
     * \returns ID of new texture or -1 on error
     */
    int loadTGA(const char *filename);

    int loadPCX(const char *filename);

    /*!
     * \brief Sets an option flag
     * \param flag flag to set
     */
    void setFlag(TextureFlag flag);

    void useMultiTexture(float aU, float aV, float bU, float bV);

private:
    std::vector<unsigned int> mTextureIds;
    unsigned int mFlags;
};

TextureManager &getTextureManager();

#endif

