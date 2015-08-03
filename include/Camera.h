/*!
 * \file include/Camera.h
 * \brief Camera, View Frustum
 *
 * \author xythobuz
 */

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/gtc/type_precision.hpp>

#include "RoomData.h"

class Camera {
  public:
    static void reset();
    static bool update();
    static void setSize(glm::i32vec2 s);

    static void handleAction(ActionEvents action, bool isFinished);
    static void handleMouseMotion(int x, int y);
    static void handleControllerAxis(float value, KeyboardButton axis);

    static void setPosition(glm::vec3 p) { pos = p; dirty = true; }
    static glm::vec3 getPosition() { return pos; }

    static glm::vec2 getRotation() { return rot; }
    static glm::mat4 getProjectionMatrix() { return projection; }
    static glm::mat4 getViewMatrix() { return view; }

    static void setSensitivityX(float sens) { rotationDeltaX = sens; }
    static float getSensitivityX() { return rotationDeltaX; }

    static void setSensitivityY(float sens) { rotationDeltaY = sens; }
    static float getSensitivityY() { return rotationDeltaY; }

    static void setUpdateViewFrustum(bool u) { updateViewFrustum = u; }
    static bool getUpdateViewFrustum() { return updateViewFrustum; }

    static void setRoom(int r) { if (room != r) dirty = true; room = r; }
    static int getRoom() { return room; }

    static void setKeepInRoom(bool k) { keepInRoom = k; }
    static bool getKeepInRoom() { return keepInRoom; }

    static bool boxInFrustum(BoundingBox b);
    static void displayFrustum(glm::mat4 MVP);

    static const float fov;
    static const float nearDist;
    static const float farDist;

  private:
    static void calculateFrustumPlanes();

    static glm::vec3 pos;
    static glm::vec2 rot;
    static glm::vec3 posSpeed;
    static glm::vec2 rotSpeed;
    static glm::mat4 projection;
    static glm::mat4 view;
    static float rotationDeltaX, rotationDeltaY;
    static bool updateViewFrustum, dirty, movingFaster;
    static bool keepInRoom;
    static int room;
};

#endif

