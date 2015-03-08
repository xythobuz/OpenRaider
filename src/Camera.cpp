/*!
 * \file src/Camera.cpp
 * \brief Camera class
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <limits>

#include "imgui/imgui.h"

#include "global.h"
#include "RunTime.h"
#include "system/Shader.h"
#include "system/Sound.h"
#include "system/Window.h"
#include "Camera.h"

#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

static bool equal(float a, float b) {
    return glm::epsilonEqual(a, b, std::numeric_limits<float>::epsilon());
}

static bool equal(glm::vec2 a, float b) {
    return equal(a.x, b) && equal(a.y, b);
}

static bool equal(glm::vec3 a, float b) {
    return equal(a.x, b) && equal(a.y, b) && equal(a.z, b);
}

// ----------------------------------------------------------------------------

const static float fov = 45.0f;
const static float nearDist = 0.1f;
const static float farDist = 75000.0f;
const static float maxSpeed = 3072.0f;
const static float controllerDeadZone = 0.33f;
const static float controllerViewFactor = glm::pi<float>();
const static float rotationAngleClamp = glm::pi<float>() * 2.0f;
const static float rotationAngleVertMax = glm::pi<float>() / 2.0f;
const static float runFactor = 2.5f;

const static glm::vec3 rightUnit(1.0f, 0.0f, 0.0f);
const static glm::vec3 upUnit(0.0f, 1.0f, 0.0f);
const static glm::vec3 dirUnit(0.0f, 0.0f, -1.0f);

glm::vec3 Camera::pos(0.0f, 0.0f, 0.0f);
glm::vec2 Camera::rot(glm::pi<float>(), 0.0f);
glm::vec3 Camera::posSpeed(0.0f, 0.0f, 0.0f);
glm::vec2 Camera::rotSpeed(0.0f, 0.0f);
glm::mat4 Camera::projection(1.0f);
glm::mat4 Camera::view(1.0f);
float Camera::rotationDeltaX = 0.75f;
float Camera::rotationDeltaY = 0.75f;
bool Camera::updateViewFrustum = true;
bool Camera::dirty = true;
bool Camera::showOverlay = false;
bool Camera::movingFaster = false;
int Camera::room = -1;

void Camera::reset() {
    pos = glm::vec3(0.0f, 0.0f, 0.0f);
    rot = glm::vec2(glm::pi<float>(), 0.0f);
    posSpeed = glm::vec3(0.0f, 0.0f, 0.0f);
    rotSpeed = glm::vec2(0.0f, 0.0f);
    dirty = true;
    projection = glm::mat4(1.0f);
    view = glm::mat4(1.0f);
    room = -1;

    setSize(Window::getSize());
}

void Camera::setSize(glm::i32vec2 s) {
    //! \fixme TODO instead of mirroring the Y axis in the shader, scale with -1 here
    projection = glm::perspective(fov, float(s.x) / float(s.y), nearDist, farDist);
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
    } else if (action == walkAction) {
        movingFaster = !isFinished;
    } else {
        return;
    }

    dirty = true;
}

void Camera::handleMouseMotion(int x, int y) {
    if ((x != 0) || (y != 0))
        dirty = true;

    while (x > 0) {
        rot.x += rotationDeltaX;
        x--;
    }

    while (x < 0) {
        rot.x -= rotationDeltaX;
        x++;
    }

    while (y > 0) {
        if (rot.y > -rotationAngleVertMax) {
            rot.y -= rotationDeltaY;
        }
        y--;
    }

    while (y < 0) {
        if (rot.y < rotationAngleVertMax) {
            rot.y += rotationDeltaY;
        }
        y++;
    }
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
        rotSpeed.y = -controllerViewFactor * value;
    } else {
        return;
    }

    dirty = true;
}

bool Camera::update() {
    if ((!dirty) && equal(posSpeed, 0.0f) && equal(rotSpeed, 0.0f))
        return false;

    while (rot.x > rotationAngleClamp)
        rot.x -= rotationAngleClamp;
    while (rot.x < -rotationAngleClamp)
        rot.x += rotationAngleClamp;
    while (rot.y > rotationAngleClamp)
        rot.y -= rotationAngleClamp;
    while (rot.y < -rotationAngleClamp)
        rot.y += rotationAngleClamp;

    float dT = RunTime::getLastFrameTime();
    glm::vec2 newRot = rot + rotSpeed * dT;

    if ((newRot.y > -rotationAngleVertMax) && (newRot.y < rotationAngleVertMax))
        rot = newRot;
    else
        rotSpeed = glm::vec2(0.0f, 0.0f);

    glm::quat quatY = glm::angleAxis(rot.x, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat quatX = glm::angleAxis(rot.y, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat quaternion = quatY * quatX;

    glm::vec3 clampedSpeed;
    if (movingFaster) {
        clampedSpeed = posSpeed * runFactor;
        if (glm::length(clampedSpeed) > (maxSpeed * runFactor)) {
            clampedSpeed = glm::normalize(clampedSpeed) * maxSpeed * runFactor;
        }
    } else {
        clampedSpeed = posSpeed;
        if (glm::length(clampedSpeed) > maxSpeed) {
            clampedSpeed = glm::normalize(clampedSpeed) * maxSpeed;
        }
    }

    pos += quaternion * clampedSpeed * dT;

    glm::mat4 translate = glm::translate(glm::mat4(1.0f), pos);
    glm::mat4 rotate = glm::toMat4(quaternion);
    view = glm::inverse(translate * rotate);

    if (updateViewFrustum)
        calculateFrustumPlanes();

    glm::vec3 at(0.0f, 0.0f, -1.0f);
    glm::vec3 up(0.0f, -1.0f, 0.0f);
    Sound::listenAt(pos, quaternion * at, quaternion * up);

    dirty = false;
    return updateViewFrustum;
}

void Camera::displayUI() {
    if (!showOverlay)
        return;

    if (ImGui::Begin("Camera Look-At Overlay", &showOverlay, ImVec2(0, 0), -1.0f,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
                     | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings
                     | ImGuiWindowFlags_AlwaysAutoResize)) {
        // TODO
    }
    ImGui::End();
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

static ShaderBuffer vertexBuffer;
static ShaderBuffer colorBuffer;
static ShaderBuffer indexBuffer;
static ShaderBuffer vertexPointBuffer;
static ShaderBuffer colorPointBuffer;

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

    std::vector<glm::vec3> verts;

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

    vertexBuffer.bufferData(verts);

    verts.clear();
    std::vector<glm::vec3> cols;

    verts.push_back(getPosition());
    cols.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    vertexPointBuffer.bufferData(verts);
    colorPointBuffer.bufferData(cols);

    if (colorBuffer.getSize() == 0) {
        cols.clear();
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 4; j++) {
                cols.push_back(frustumColors[i]);
            }
        }
        colorBuffer.bufferData(cols);
    }

    if (indexBuffer.getSize() == 0) {
        std::vector<unsigned short> inds;
        for (int i = 0; i < 6; i++) {
            inds.push_back(4 * i);
            inds.push_back((4 * i) + 1);
            inds.push_back((4 * i) + 2);
            inds.push_back((4 * i) + 3);
            inds.push_back((4 * i) + 2);
            inds.push_back(4 * i);
        }
        indexBuffer.bufferData(inds);
    }
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
    Shader::set2DState(true, false);
    Shader::drawGL(vertexBuffer, colorBuffer, indexBuffer, MVP);
    Shader::drawGL(vertexPointBuffer, colorPointBuffer, MVP, GL_POINTS);
    Shader::set2DState(false, false);
}

