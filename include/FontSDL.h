/*!
 * \file include/FontSDL.h
 * \brief SDL windowing implementation
 *
 * \author xythobuz
 */

#ifndef _FONT_SDL_H_
#define _FONT_SDL_H_

#include "SDL_ttf.h"

#include "Font.h"

/*!
 * \brief SDL windowing implementation
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

    virtual void drawText(unsigned int x, unsigned int y, float scale, const float color[4], const char *s, ...)
        __attribute__((format(printf, 6, 0)));

private:
    TTF_Font *mFont;
    unsigned int mFontTexture;

    FontString tempText;
};

#endif

