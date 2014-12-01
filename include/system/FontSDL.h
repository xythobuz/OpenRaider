/*!
 * \file include/FontSDL.h
 * \brief SDL Font implementation
 *
 * \author xythobuz
 */

#ifndef _FONT_SDL_H_
#define _FONT_SDL_H_

#include "SDL_ttf.h"

/*!
 * \brief SDL Font implementation
 */
class FontSDL {
  public:
    static void shutdown();

    static int initialize(std::string font);

    static unsigned int widthText(float scale, std::string s);

    static unsigned int heightText(float scale, unsigned int maxWidth, std::string s);

    static void drawText(unsigned int x, unsigned int y, float scale,
                         const unsigned char color[4], std::string s);

    static void drawTextWrapped(unsigned int x, unsigned int y, float scale,
                                const unsigned char color[4], unsigned int maxWidth, std::string s);

  private:
    static bool mFontInit;
    static TTF_Font* mFont;
    static unsigned int mFontTexture;
};

#endif

