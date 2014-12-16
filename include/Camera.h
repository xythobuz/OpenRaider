/*!
 * \file include/Camera.h
 * \brief Camera class
 *
 * \author xythobuz
 */

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera {
  public:

    static void reset();

    static void handleAction(ActionEvents action, bool isFinished);
    static void handleMouseMotion(int x, int y);
    static glm::mat4 getViewMatrix();

    static float getRadianPitch() { return thetaX; }
    static float getRadianYaw() { return thetaY; }
    static void setRadianPitch(float x) { thetaX = x; }

    static void setPosition(glm::vec3 p) { pos = p; }
    static glm::vec3 getPosition() { return pos; }

    static void setSensitivityX(float sens) { rotationDeltaX = sens; }
    static float getSensitivityX() { return rotationDeltaX; }

    static void setSensitivityY(float sens) { rotationDeltaY = sens; }
    static float getSensitivityY() { return rotationDeltaY; }

  private:
    static glm::vec3 pos;
    static float thetaX, thetaY;
    static float rotationDeltaX, rotationDeltaY;
};

#endif

