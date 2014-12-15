/*!
 * \file src/Camera.cpp
 * \brief Camera class
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <glm/gtc/matrix_transform.hpp>

#include "global.h"
#include "Camera.h"

glm::vec3 Camera::pos(0.0f, 0.0f, 0.0f);
float Camera::thetaX = glm::pi<float>();
float Camera::thetaY = 0.0f;
float Camera::rotationDeltaX = 0.75f;
float Camera::rotationDeltaY = 0.75f;

void Camera::handleMouseMotion(int x, int y) {
    while (x > 0) {
        if (thetaX < (glm::pi<float>() / 2.0f)) {
            thetaX += rotationDeltaX;
        }
        x--;
    }
    while (x < 0) {
        if (thetaX > -(glm::pi<float>() / 2.0f)) {
            thetaX -= rotationDeltaX;
        }
        x++;
    }
    while (y > 0) {
        if (thetaY < (glm::pi<float>() / 2.0f)) {
            thetaY += rotationDeltaY;
        }
        y--;
    }
    while (y < 0) {
        if (thetaY > -(glm::pi<float>() / 2.0f)) {
            thetaY -= rotationDeltaY;
        }
        y++;
    }
}

glm::mat4 Camera::getViewMatrix() {
    glm::vec3 dir(
        glm::cos(thetaY) * glm::sin(thetaX),
        glm::sin(thetaY),
        glm::cos(thetaY) * glm::cos(thetaX)
    );
    glm::vec3 right(
        glm::sin(thetaX - glm::pi<float>() / 2.0f),
        0.0f,
        glm::cos(thetaX - glm::pi<float>() / 2.0f)
    );
    glm::vec3 up = glm::cross(right, dir);
    return glm::lookAt(pos, pos + dir, up);
}

