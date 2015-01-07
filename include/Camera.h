/*!
 * \file include/Camera.h
 * \brief Camera class
 *
 * \author xythobuz
 */

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/gtc/quaternion.hpp>
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
    static void handleControllerButton(KeyboardButton button, bool released);

    //! \fixme The Y axis seems to be the source of all evil?
    static void setPosition(glm::vec3 p) { pos = glm::vec3(p.x, -p.y, p.z); }
    static glm::vec3 getPosition() { return glm::vec3(pos.x, -pos.y, pos.z); }

    static glm::vec2 getRotation();
    static glm::mat4 getProjectionMatrix() { return projection; }
    static glm::mat4 getViewMatrix() { return view; }

    static void setSensitivityX(float sens) { rotationDeltaX = sens; }
    static float getSensitivityX() { return rotationDeltaX; }

    static void setSensitivityY(float sens) { rotationDeltaY = sens; }
    static float getSensitivityY() { return rotationDeltaY; }

    static void setUpdateViewFrustum(bool u) { updateViewFrustum = u; }
    static bool getUpdateViewFrustum() { return updateViewFrustum; }

    static bool boxInFrustum(BoundingBox b);
    static void displayFrustum(glm::mat4 MVP);

  private:
    static void calculateFrustumPlanes();

    static glm::vec3 pos;
    static glm::quat quaternion;
    static glm::vec3 posSpeed;
    static glm::vec2 rotSpeed;
    static glm::mat4 projection;
    static glm::mat4 view;
    static float rotationDeltaX, rotationDeltaY;
    static bool updateViewFrustum, dirty;
};

#endif

