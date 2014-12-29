/*!
 * \file include/Render.h
 * \brief OpenRaider Renderer class
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _RENDER_H_
#define _RENDER_H_

#include <vector>
#include <glm/vec4.hpp>

#include "Room.h"
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
    static void displayUI();

    static void clearRoomList();

    static void screenShot(const char* filenameBase);

    static void drawTexture(float x, float y, float w, float h, glm::vec4 color,
                            unsigned int texture, TextureManager::TextureStorage s);

    static void setDisplayViewFrustum(bool d) { displayViewFrustum = d; }
    static bool getDisplayViewFrustum() { return displayViewFrustum; }

  private:
    static void buildRoomList(int room);

    static RenderMode mode;
    static std::vector<Room*> roomList;

    static bool displayViewFrustum;
};

#endif

