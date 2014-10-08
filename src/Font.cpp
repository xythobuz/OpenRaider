/*!
 * \file src/Font.cpp
 * \brief Font implementation
 *
 * \author xythobuz
 */

#include "global.h"
#include "Log.h"
#include "utils/strings.h"
#include "Window.h"
#include "Font.h"
#include "FontTRLE.h"

#ifdef USING_SDL_FONT
#include "FontSDL.h"
#endif

bool Font::isInit = false;
std::string Font::fontName;

void Font::shutdown() {
    FontTRLE::shutdown();
#ifdef USING_SDL_FONT
    FontSDL::shutdown();
#endif
}

int Font::initialize(std::string font) {
    fontName = font;
    if (stringEndsWith(fontName, ".pc")) {
        return FontTRLE::initialize(fontName);
#ifdef USING_SDL_FONT
    } else if (stringEndsWith(fontName, ".ttf")) {
        return FontSDL::initialize(fontName);
#endif
    }

    getLog() << "Unknown font file format: " << font << Log::endl;
    return -1;
}

unsigned int Font::widthText(float scale, std::string s) {
    if (stringEndsWith(fontName, ".pc")) {
        return FontTRLE::widthText(scale, s);
#ifdef USING_SDL_FONT
    } else if (stringEndsWith(fontName, ".ttf")) {
        return FontSDL::widthText(scale, s);
#endif
    }

    return 0;
}

unsigned int Font::heightText(float scale, unsigned int maxWidth, std::string s) {
    if (stringEndsWith(fontName, ".pc")) {
        return FontTRLE::heightText(scale, maxWidth, s);
#ifdef USING_SDL_FONT
    } else if (stringEndsWith(fontName, ".ttf")) {
        return FontSDL::heightText(scale, maxWidth, s);
#endif
    }

    return 0;
}

void Font::drawText(unsigned int x, unsigned int y, float scale,
        const unsigned char color[4], std::string s) {
    if (stringEndsWith(fontName, ".pc")) {
        FontTRLE::drawText(x, y, scale, color, s);
#ifdef USING_SDL_FONT
    } else if (stringEndsWith(fontName, ".ttf")) {
        FontSDL::drawText(x, y, scale, color, s);
#endif
    }
}

void Font::drawTextWrapped(unsigned int x, unsigned int y, float scale,
        const unsigned char color[4], unsigned int maxWidth, std::string s) {
    if (stringEndsWith(fontName, ".pc")) {
        FontTRLE::drawTextWrapped(x, y, scale, color, maxWidth, s);
#ifdef USING_SDL_FONT
    } else if (stringEndsWith(fontName, ".ttf")) {
        FontSDL::drawTextWrapped(x, y, scale, color, maxWidth, s);
#endif
    }
}

void Font::drawTextCentered(unsigned int x, unsigned int y, float scale,
        const unsigned char color[4], unsigned int width, std::string s) {
    drawText(x + ((width / 2) - (widthText(scale, s) / 2)), y, scale, color, s);
}

