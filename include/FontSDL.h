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

    virtual void writeString(FontString &s);

private:
    TTF_Font *mFont;
    unsigned int mFontTexture;
};

#endif

