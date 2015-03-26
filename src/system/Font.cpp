/*!
 * \file src/system/Font.cpp
 * \brief Font Interface
 *
 * \author xythobuz
 */

#include <vector>

#include "global.h"
#include "Log.h"
#include "utils/strings.h"
#include "system/FontImGui.h"
#include "system/FontTRLE.h"
#include "system/FontTTF.h"
#include "system/Shader.h"
#include "system/Font.h"

bool Font::isInit = false;
std::string Font::fontName;
bool Font::showFontBox = false;

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
                    glm::vec4 color, std::string s) {
    if (stringEndsWith(fontName, ".pc")) {
        FontTRLE::drawText(x, y, scale, color, s);
    } else if (stringEndsWith(fontName, ".ttf")) {
        FontTTF::drawText(x, y, scale, color, s);
    } else {
        FontImGui::drawText(x, y, scale, color, s);
    }

    if (showFontBox) {
        unsigned int w = widthText(scale, s);
        unsigned int h = heightText(scale, w + 100, s); // Should always be one line
        drawFontBox(x, y, w, h);
    }
}

void Font::drawTextWrapped(unsigned int x, unsigned int y, float scale,
                           glm::vec4 color, unsigned int maxWidth, std::string s) {
    if (stringEndsWith(fontName, ".pc")) {
        FontTRLE::drawTextWrapped(x, y, scale, color, maxWidth, s);
    } else if (stringEndsWith(fontName, ".ttf")) {
        FontTTF::drawTextWrapped(x, y, scale, color, maxWidth, s);
    } else {
        FontImGui::drawTextWrapped(x, y, scale, color, maxWidth, s);
    }

    if (showFontBox) {
        unsigned int w = widthText(scale, s);
        if (w > maxWidth)
            w = maxWidth;
        unsigned int h = heightText(scale, w, s); // Should always be one line
        drawFontBox(x, y, w, h);
    }
}

void Font::drawTextCentered(unsigned int x, unsigned int y, float scale,
                            glm::vec4 color, unsigned int width, std::string s) {
    drawText(x + ((width / 2) - (widthText(scale, s) / 2)), y, scale, color, s);
}

void Font::drawFontBox(unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
    static ShaderBuffer vert, uv;
    std::vector<glm::vec2> vertices, uvs;

    vertices.emplace_back(x, y);
    vertices.emplace_back(x + w, y);
    vertices.emplace_back(x + w, y + h);
    vertices.emplace_back(x, y + h);
    vertices.emplace_back(x, y);

    uvs.emplace_back(0.0f, 0.0f);
    uvs.emplace_back(1.0f, 0.0f);
    uvs.emplace_back(1.0f, 1.0f);
    uvs.emplace_back(0.0f, 1.0f);
    uvs.emplace_back(0.0f, 0.0f);

    vert.bufferData(vertices);
    uv.bufferData(uvs);

    Shader::drawGL(vert, uv, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), TEXTURE_WHITE,
                   TextureStorage::SYSTEM, gl::GL_LINE_STRIP);
}

