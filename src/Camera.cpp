/*!
 * \file src/Camera.cpp
 * \brief Camera class
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "global.h"
#include "RunTime.h"
#include "system/Window.h"
#include "Camera.h"

#define NEAR 0
#define FAR 1
#define TOP 2
#define BOTTOM 3
#define LEFT 4
#define RIGHT 5

#define NTL 0
#define NBL 1
#define NBR 2
#define NTR 3
#define FTL 4
#define FBL 5
#define FBR 6
#define FTR 7

const static float fov = 45.0f;
const static float nearDist = 0.1f;
const static float farDist = 75000.0f;
const static float maxSpeed = 2048.0f;
const static float controllerViewFactor = 384.0f;
const static float controllerDeadZone = 0.1f;

const static glm::vec3 rightUnit(1.0f, 0.0f, 0.0f);
const static glm::vec3 upUnit(0.0f, 1.0f, 0.0f);
const static glm::vec3 dirUnit(0.0f, 0.0f, -1.0f);

glm::vec3 Camera::pos(0.0f, 0.0f, 0.0f);
glm::quat Camera::quaternion(glm::vec3(0.0f, 0.0f, 0.0f));
glm::vec3 Camera::posSpeed(0.0f, 0.0f, 0.0f);
glm::vec2 Camera::rotSpeed(0.0f, 0.0f);
glm::vec2 Camera::lastSize(0.0f, 0.0f);
glm::mat4 Camera::projection(1.0f);
glm::mat4 Camera::view(1.0f);
float Camera::rotationDeltaX = 0.75f;
float Camera::rotationDeltaY = 0.75f;
bool Camera::updateViewFrustum = true;

void Camera::reset() {
    pos = glm::vec3(0.0f, 0.0f, 0.0f);
    quaternion = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
    posSpeed = glm::vec3(0.0f, 0.0f, 0.0f);
    rotSpeed = glm::vec2(0.0f, 0.0f);
    lastSize = glm::vec2(0.0f, 0.0f);
    projection = glm::mat4(1.0f);
    view = glm::mat4(1.0f);
}

void Camera::handleAction(ActionEvents action, bool isFinished) {
    float factor = 1.0f;
    if (isFinished)
        factor = -1.0f;

    if (action == forwardAction) {
        posSpeed += dirUnit * maxSpeed * factor;
    } else if (action == backwardAction) {
        posSpeed -= dirUnit * maxSpeed * factor;
    } else if (action == leftAction) {
        posSpeed += rightUnit * maxSpeed * factor;
    } else if (action == rightAction) {
        posSpeed -= rightUnit * maxSpeed * factor;
    } else if (action == jumpAction) {
        posSpeed += upUnit * maxSpeed * factor;
    } else if (action == crouchAction) {
        posSpeed -= upUnit * maxSpeed * factor;
    }
}

void Camera::handleMouseMotion(int x, int y) {
    if (x != 0)
        quaternion = glm::quat(upUnit * rotationDeltaX * float(x)) * quaternion;

    if (y != 0)
        quaternion = glm::quat(quaternion * -rightUnit * rotationDeltaY * float(y)) * quaternion;
}

void Camera::handleControllerAxis(float value, KeyboardButton axis) {
    if (glm::epsilonEqual(value, 0.0f, controllerDeadZone))
        value = 0.0f;

    if (axis == leftXAxis) {
        posSpeed.x = -maxSpeed * value;
    } else if (axis == leftYAxis) {
        posSpeed.z = maxSpeed * value;
    } else if (axis == rightXAxis) {
        rotSpeed.x = controllerViewFactor * value;
    } else if (axis == rightYAxis) {
        rotSpeed.y = controllerViewFactor * value;
    }
}

void Camera::handleControllerButton(KeyboardButton button, bool released) {
    if (button == aButton) {
        handleAction(jumpAction, released);
    } else if (button == bButton) {
        handleAction(crouchAction, released);
    } else if (button == padUp) {
        handleAction(forwardAction, released);
    } else if (button == padDown) {
        handleAction(backwardAction, released);
    } else if (button == padLeft) {
        handleAction(leftAction, released);
    } else if (button == padRight) {
        handleAction(rightAction, released);
    }
}

bool Camera::update() {
    glm::vec2 size(getWindow().getWidth(), getWindow().getHeight());

    if (lastSize != size) {
        //! \fixme TODO instead of mirroring the Y axis in the shader, scale with -1 here
        projection = glm::perspective(fov, size.x / size.y, nearDist, farDist);
        lastSize = size;
    }

    float dT = getRunTime().getLastFrameTime();
    pos += quaternion * posSpeed * dT;

    if (glm::epsilonNotEqual(rotSpeed.x, 0.0f, controllerDeadZone))
        quaternion = glm::quat(upUnit * rotationDeltaX * rotSpeed.x * dT) * quaternion;

    if (glm::epsilonNotEqual(rotSpeed.y, 0.0f, controllerDeadZone))
        quaternion = glm::quat(quaternion * -rightUnit * rotationDeltaY * rotSpeed.y * dT) * quaternion;

    glm::mat4 translate = glm::translate(glm::mat4(1.0f), pos);
    glm::mat4 rotate = glm::toMat4(quaternion);
    view = glm::inverse(translate * rotate);

    if (updateViewFrustum)
        calculateFrustumPlanes();

    return updateViewFrustum;
}

glm::vec2 Camera::getRotation() {
    glm::vec3 euler = glm::eulerAngles(quaternion);
    return glm::vec2(euler.y, euler.x);
}

// ----------------------------------------------------------------------------

class FrustumPlane {
  public:
    FrustumPlane() : normal(glm::vec3(0.0f, 0.0f, 0.0f)), d(0.0f) { }
    void set(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
        normal = glm::normalize(glm::cross(v3 - v2, v1 - v2));
        d = -glm::dot(normal, v2);
    }
    float distance(glm::vec3 p) {
        return d + glm::dot(normal, p);
    }
  private:
    glm::vec3 normal;
    float d;
};

// ----------------------------------------------------------------------------

static FrustumPlane planes[6];
static glm::vec3 frustumColors[6] = {
    glm::vec3(1.0f, 0.0f, 0.0f), // NEAR, red
    glm::vec3(0.0f, 1.0f, 0.0f), // FAR, green
    glm::vec3(0.0f, 0.0f, 1.0f), // TOP, blue
    glm::vec3(1.0f, 1.0f, 0.0f), // BOTTOM, yellow
    glm::vec3(0.0f, 1.0f, 1.0f), // LEFT, light-blue
    glm::vec3(1.0f, 0.0f, 1.0f)  // RIGHT, pink
};
static glm::vec3 frustumVertices[8];

void Camera::calculateFrustumPlanes() {
    glm::mat4 combo = projection * view;

    // Calculate frustum corners to display them
    glm::mat4 inverse = glm::inverse(combo);
    frustumVertices[NTL] = glm::vec3(1.0f,  1.0f, 0.0f);
    frustumVertices[NTR] = glm::vec3(-1.0f,  1.0f, 0.0f);
    frustumVertices[NBL] = glm::vec3(1.0f, -1.0f, 0.0f);
    frustumVertices[NBR] = glm::vec3(-1.0f, -1.0f, 0.0f);
    frustumVertices[FTL] = glm::vec3(1.0f,  1.0f, 1.0f);
    frustumVertices[FTR] = glm::vec3(-1.0f,  1.0f, 1.0f);
    frustumVertices[FBL] = glm::vec3(1.0f, -1.0f, 1.0f);
    frustumVertices[FBR] = glm::vec3(-1.0f, -1.0f, 1.0f);
    for (int i = 0; i < 8; i++) {
        glm::vec4 t = inverse * glm::vec4(frustumVertices[i], 1.0f);
        frustumVertices[i] = glm::vec3(t) / t.w;
        frustumVertices[i].y *= -1.0f;
    }

    // Set planes used for frustum culling
    planes[TOP].set(frustumVertices[NTR], frustumVertices[NTL], frustumVertices[FTL]);
    planes[BOTTOM].set(frustumVertices[NBL], frustumVertices[NBR], frustumVertices[FBR]);
    planes[LEFT].set(frustumVertices[NTL], frustumVertices[NBL], frustumVertices[FBL]);
    planes[RIGHT].set(frustumVertices[NBR], frustumVertices[NTR], frustumVertices[FBR]);
    planes[NEAR].set(frustumVertices[NTL], frustumVertices[NTR], frustumVertices[NBR]);
    planes[FAR].set(frustumVertices[FTR], frustumVertices[FTL], frustumVertices[FBL]);
}

bool Camera::boxInFrustum(BoundingBox b) {
    for (int i = 0; i < 6; i++) {
        int out = 0, in = 0;
        for (int c = 0; (c < 8) && ((in == 0) || (out == 0)); c++) {
            if (planes[i].distance(b.getCorner(c)) < 0)
                out++;
            else
                in++;
        }

        if (in == 0)
            return false;
    }

    return true;
}

void Camera::displayFrustum(glm::mat4 MVP) {
    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> cols;
    std::vector<unsigned short> inds;

    // Near
    verts.push_back(frustumVertices[NTL]);
    verts.push_back(frustumVertices[NTR]);
    verts.push_back(frustumVertices[NBR]);
    verts.push_back(frustumVertices[NBL]);

    // Far
    verts.push_back(frustumVertices[FTR]);
    verts.push_back(frustumVertices[FTL]);
    verts.push_back(frustumVertices[FBL]);
    verts.push_back(frustumVertices[FBR]);

    // Top
    verts.push_back(frustumVertices[NTR]);
    verts.push_back(frustumVertices[NTL]);
    verts.push_back(frustumVertices[FTL]);
    verts.push_back(frustumVertices[FTR]);

    // Bottom
    verts.push_back(frustumVertices[NBL]);
    verts.push_back(frustumVertices[NBR]);
    verts.push_back(frustumVertices[FBR]);
    verts.push_back(frustumVertices[FBL]);

    // Left
    verts.push_back(frustumVertices[NTL]);
    verts.push_back(frustumVertices[NBL]);
    verts.push_back(frustumVertices[FBL]);
    verts.push_back(frustumVertices[FTL]);

    // Right
    verts.push_back(frustumVertices[NBR]);
    verts.push_back(frustumVertices[NTR]);
    verts.push_back(frustumVertices[FTR]);
    verts.push_back(frustumVertices[FBR]);

    for (int i = 0; i < 6; i++) {
        cols.push_back(frustumColors[i]);
        cols.push_back(frustumColors[i]);
        cols.push_back(frustumColors[i]);
        cols.push_back(frustumColors[i]);

        inds.push_back(4 * i);
        inds.push_back((4 * i) + 1);
        inds.push_back((4 * i) + 2);
        inds.push_back((4 * i) + 3);
        inds.push_back((4 * i) + 2);
        inds.push_back(4 * i);
    }

    Window::drawGL(verts, cols, inds, MVP);
}

