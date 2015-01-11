/*!
 * \file src/system/Font.cpp
 * \brief Font implementation
 *
 * \author xythobuz
 */

#include "global.h"
#include "Log.h"
#include "utils/strings.h"
#include "system/FontImGui.h"
#include "system/FontTRLE.h"
#include "system/FontTTF.h"
#include "system/Font.h"

bool Font::isInit = false;
std::string Font::fontName;

void Font::shutdown() {
    FontTRLE::shutdown();
    FontTTF::shutdown();
}

int Font::initialize(std::string font) {
    fontName = font;
    if (stringEndsWith(fontName, ".pc")) {
        return FontTRLE::initialize(fontName);
    } else if (stringEndsWith(fontName, ".ttf")) {
        return FontTTF::initialize(fontName);
    }

    if (font != "") {
        Log::get(LOG_ERROR) << "Unknown font file format: " << font << Log::endl;
        return -1;
    } else {
        return 0;
    }
}

unsigned int Font::widthText(float scale, std::string s) {
    if (stringEndsWith(fontName, ".pc")) {
        return FontTRLE::widthText(scale, s);
    } else if (stringEndsWith(fontName, ".ttf")) {
        return FontTTF::widthText(scale, s);
    } else {
        return FontImGui::widthText(scale, s);
    }
}

unsigned int Font::heightText(float scale, unsigned int maxWidth, std::string s) {
    if (stringEndsWith(fontName, ".pc")) {
        return FontTRLE::heightText(scale, maxWidth, s);
    } else if (stringEndsWith(fontName, ".ttf")) {
        return FontTTF::heightText(scale, maxWidth, s);
    } else {
        return FontImGui::heightText(scale, maxWidth, s);
    }
}

void Font::drawText(unsigned int x, unsigned int y, float scale,
                    const unsigned char color[4], std::string s) {
    if (stringEndsWith(fontName, ".pc")) {
        FontTRLE::drawText(x, y, scale, color, s);
    } else if (stringEndsWith(fontName, ".ttf")) {
        FontTTF::drawText(x, y, scale, color, s);
    } else {
        FontImGui::drawText(x, y, scale, color, s);
    }
}

void Font::drawTextWrapped(unsigned int x, unsigned int y, float scale,
                           const unsigned char color[4], unsigned int maxWidth, std::string s) {
    if (stringEndsWith(fontName, ".pc")) {
        FontTRLE::drawTextWrapped(x, y, scale, color, maxWidth, s);
    } else if (stringEndsWith(fontName, ".ttf")) {
        FontTTF::drawTextWrapped(x, y, scale, color, maxWidth, s);
    } else {
        FontImGui::drawTextWrapped(x, y, scale, color, maxWidth, s);
    }
}

void Font::drawTextCentered(unsigned int x, unsigned int y, float scale,
                            const unsigned char color[4], unsigned int width, std::string s) {
    drawText(x + ((width / 2) - (widthText(scale, s) / 2)), y, scale, color, s);
}

