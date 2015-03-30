/*!
 * \file include/Render.h
 * \brief World Renderer
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _RENDER_H_
#define _RENDER_H_

#include <vector>

#include <glm/gtc/type_precision.hpp>

#include "Room.h"
#include "TextureManager.h"

enum class RenderMode {
    LoadScreen,
    Solid,
    Wireframe,
    Texture
};

class Render {
  public:

    static void clearRoomList() { roomList.clear(); }

    static void display();
    static void displayUI();

    static void screenShot(const char* filenameBase);

    static RenderMode getMode() { return mode; }
    static void setMode(RenderMode m) { mode = m; }

    static void setDisplayViewFrustum(bool d) { displayViewFrustum = d; }
    static bool getDisplayViewFrustum() { return displayViewFrustum; }

  private:
    static void buildRoomList(int room = -2, int budget = 10);

    static RenderMode mode;
    static std::vector<Room*> roomList;

    static bool displayViewFrustum;
};

#endif

