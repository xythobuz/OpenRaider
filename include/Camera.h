/*!
 * \file include/Camera.h
 * \brief Camera class
 *
 * \author xythobuz
 */

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include "RoomData.h"

class Camera {
  public:

    static void reset();
    static bool update();

    static void handleAction(ActionEvents action, bool isFinished);
    static void handleMouseMotion(int x, int y);
    static void handleControllerAxis(float value, KeyboardButton axis);
    static void handleControllerButton(KeyboardButton button, bool released);

    static void setPosition(glm::vec3 p) { pos = p; }
    static glm::vec3 getPosition() { return pos; }

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
    static glm::vec2 lastSize;
    static glm::mat4 projection;
    static glm::mat4 view;
    static float rotationDeltaX, rotationDeltaY;
    static bool updateViewFrustum;
};

#endif

