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
float Camera::thetaX = 0.0f;
float Camera::thetaY = 0.0f;
float Camera::rotationDeltaX = 0.75f;
float Camera::rotationDeltaY = 0.75f;

void Camera::reset() {
    pos = glm::vec3(0.0f, 0.0f, 0.0f);
    thetaX = 0.0f;
    thetaY = 0.0f;
}

void Camera::handleAction(ActionEvents action, bool isFinished) {
    if (isFinished)
        return;

    const static float step = 256.0f;

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

    if (action == forwardAction) {
        pos += dir * step;
    } else if (action == backwardAction) {
        pos -= dir * step;
    } else if (action == leftAction) {
        pos += right * step;
    } else if (action == rightAction) {
        pos -= right * step;
    } else if (action == jumpAction) {
        pos += up * step;
    } else if (action == crouchAction) {
        pos -= up * step;
    } else if (action == useAction) {
    } else if (action == holsterAction) {
    } else if (action == walkAction) {
    }
}

void Camera::handleMouseMotion(int x, int y) {
    while (x > 0) {
        thetaX += rotationDeltaX;
        x--;
    }
    while (x < 0) {
        thetaX -= rotationDeltaX;
        x++;
    }
    while (y > 0) {
        if (thetaY > -(glm::pi<float>() / 2.0f)) {
            thetaY -= rotationDeltaY;
        }
        y--;
    }
    while (y < 0) {
        if (thetaY < (glm::pi<float>() / 2.0f)) {
            thetaY += rotationDeltaY;
        }
        y++;
    }

    while (thetaX > (glm::pi<float>() * 2.0f))
        thetaX -= glm::pi<float>() * 2.0f;

    while (thetaX < -(glm::pi<float>() * 2.0f))
        thetaX += glm::pi<float>() * 2.0f;
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

