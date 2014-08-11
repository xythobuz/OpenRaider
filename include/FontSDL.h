/*!
 * \file include/FontSDL.h
 * \brief SDL Font implementation
 *
 * \author xythobuz
 */

#ifndef _FONT_SDL_H_
#define _FONT_SDL_H_

#include "SDL_ttf.h"

#include "Font.h"

/*!
 * \brief SDL Font implementation
 */
class FontSDL : public Font {
public:

    /*!
     * \brief Constructs an object of FontSDL
     */
    FontSDL();

    /*!
     * \brief Deconstructs an object of FontSDL
     */
    virtual ~FontSDL();

    virtual int initialize();

    virtual unsigned int widthText(float scale, std::string s);

    virtual void drawText(unsigned int x, unsigned int y, float scale,
            const unsigned char color[4], std::string s);

    virtual unsigned int heightText(float scale, unsigned int maxWidth, std::string s);

    virtual void drawTextWrapped(unsigned int x, unsigned int y, float scale,
            const unsigned char color[4], unsigned int maxWidth, std::string s);

private:
    TTF_Font *mFont;
    unsigned int mFontTexture;
};

#endif

