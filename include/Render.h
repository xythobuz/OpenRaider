/*!
 * \file include/Render.h
 * \brief OpenRaider Renderer class
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _RENDER_H_
#define _RENDER_H_

#include <glm/vec4.hpp>

#include "TextureManager.h"

enum class RenderMode {
    Disabled,
    LoadScreen,
    Solid,
    Wireframe,
    Texture
};

class Render {
  public:
    static RenderMode getMode();
    static void setMode(RenderMode m);

    static void display();

    static void screenShot(const char* filenameBase);

    static void drawTexture(float x, float y, float w, float h, glm::vec4 color,
                            unsigned int texture, TextureManager::TextureStorage s);

  private:
    static RenderMode mode;
};

#endif

