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

struct RoomRenderList {
    RoomRenderList(Room* r, glm::vec2 pos, glm::vec2 size)
        : room(r), portalPos(pos), portalSize(size) { }

    Room* room;
    glm::vec2 portalPos, portalSize;
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
    static void buildRoomList(glm::mat4 VP, int room = -2,
                              glm::vec2 min = glm::vec2(-1.0f, -1.0f),
                              glm::vec2 max = glm::vec2(1.0f, 1.0f));

    static RenderMode mode;
    static std::vector<RoomRenderList> roomList;

    static bool displayViewFrustum;
    static bool displayVisibilityCheck;
};

#endif

