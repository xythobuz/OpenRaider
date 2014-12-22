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

#include "RoomData.h"

class Camera {
  public:

    static void reset();

    static void handleAction(ActionEvents action, bool isFinished);
    static void handleMouseMotion(int x, int y);
    static bool update();

    static void setPosition(glm::vec3 p) { pos = p; }
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

    static bool boxInFrustum(BoundingBox b);
    static void displayFrustum(glm::mat4 MVP);

  private:
    static glm::vec3 pos;
    static glm::vec2 rot;
    static glm::vec3 lastPos;
    static glm::vec2 lastRot;
    static glm::vec2 lastSize;
    static glm::mat4 projection;
    static glm::mat4 view;
    static float rotationDeltaX, rotationDeltaY;
    static bool updateViewFrustum;
};

#endif

