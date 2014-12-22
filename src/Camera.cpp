/*!
 * \file src/Camera.cpp
 * \brief Camera class
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <glm/gtc/matrix_transform.hpp>

#include "global.h"
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

struct Plane {
    glm::vec3 normal, pos;
    float d;

    Plane(glm::vec3 n = glm::vec3(0.0f, 0.0f, 0.0f),
          glm::vec3 p = glm::vec3(0.0f, 0.0f, 0.0f)) {
        set(n, p);
    }

    void set(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
        glm::vec3 aux1 = a - b;
        glm::vec3 aux2 = c - b;
        normal = glm::normalize(glm::cross(aux2, aux1));
        pos = b;
        d = -glm::dot(normal, pos);
    }

    void set(glm::vec3 n, glm::vec3 p) {
        normal = n;
        pos = p;
        d = -glm::dot(normal, pos);
    }

    float distance(glm::vec3 p) {
        return d + glm::dot(normal, p);
    }
};

// ----------------------------------------------------------------------------

const static float fov = 45.0f;
const static float nearDist = 0.1f;
const static float farDist = 75000.0f;
const static float freeCameraStep = 256.0f;

glm::vec3 Camera::pos(0.0f, 0.0f, 0.0f);
glm::vec2 Camera::rot(0.0f, 0.0f);
glm::vec3 Camera::lastPos(1.0f, 0.0f, 0.0f);
glm::vec2 Camera::lastRot(1.0f, 0.0f);
glm::vec2 Camera::lastSize(0.0f, 0.0f);
glm::mat4 Camera::projection(1.0f);
glm::mat4 Camera::view(1.0f);
float Camera::rotationDeltaX = 0.75f;
float Camera::rotationDeltaY = 0.75f;
bool Camera::updateViewFrustum = true;
static Plane planes[6];

static glm::vec3 frustumColors[6] = {
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 0.0f, 1.0f)
};
static glm::vec3 frustumVertices[8];

void Camera::reset() {
    pos = glm::vec3(0.0f, 0.0f, 0.0f);
    rot = glm::vec2(0.0f, 0.0f);
    lastPos = glm::vec3(1.0f, 0.0f, 0.0f);
    lastRot = glm::vec2(1.0f, 0.0f);
    lastSize = glm::vec2(0.0f, 0.0f);
    projection = glm::mat4(1.0f);
    view = glm::mat4(1.0f);
}

void Camera::handleAction(ActionEvents action, bool isFinished) {
    if (isFinished)
        return;

    glm::vec3 dir(
        glm::cos(rot.y) * glm::sin(rot.x),
        glm::sin(rot.y),
        glm::cos(rot.y) * glm::cos(rot.x)
    );
    glm::vec3 right(
        glm::sin(rot.x - glm::pi<float>() / 2.0f),
        0.0f,
        glm::cos(rot.x - glm::pi<float>() / 2.0f)
    );
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    if (action == forwardAction) {
        pos += dir * freeCameraStep;
    } else if (action == backwardAction) {
        pos -= dir * freeCameraStep;
    } else if (action == leftAction) {
        pos += right * freeCameraStep;
    } else if (action == rightAction) {
        pos -= right * freeCameraStep;
    } else if (action == jumpAction) {
        pos += up * freeCameraStep;
    } else if (action == crouchAction) {
        pos -= up * freeCameraStep;
    } else if (action == useAction) {
    } else if (action == holsterAction) {
    } else if (action == walkAction) {
    }
}

void Camera::handleMouseMotion(int x, int y) {
    while (x > 0) {
        rot.x += rotationDeltaX;
        x--;
    }
    while (x < 0) {
        rot.x -= rotationDeltaX;
        x++;
    }
    while (y > 0) {
        if (rot.y > -(glm::pi<float>() / 2.0f)) {
            rot.y -= rotationDeltaY;
        }
        y--;
    }
    while (y < 0) {
        if (rot.y < (glm::pi<float>() / 2.0f)) {
            rot.y += rotationDeltaY;
        }
        y++;
    }

    while (rot.x > (glm::pi<float>() * 2.0f))
        rot.x -= glm::pi<float>() * 2.0f;

    while (rot.x < -(glm::pi<float>() * 2.0f))
        rot.x += glm::pi<float>() * 2.0f;
}

bool Camera::update() {
    glm::vec2 size(getWindow().getWidth(), getWindow().getHeight());

    if ((lastPos == pos) && (lastRot == rot) && (lastSize == size))
        return false;

    if (lastSize != size) {
        projection = glm::perspective(fov, size.x / size.y, nearDist, farDist);
        lastSize = size;
    }

    glm::vec3 dir(
        glm::cos(rot.y) * glm::sin(rot.x),
        glm::sin(rot.y),
        glm::cos(rot.y) * glm::cos(rot.x)
    );

    glm::vec3 right(
        glm::sin(rot.x - glm::pi<float>() / 2.0f),
        0.0f,
        glm::cos(rot.x - glm::pi<float>() / 2.0f)
    );

    glm::vec3 up = glm::cross(right, dir);
    view = glm::lookAt(pos, pos + dir, up);

    if (!updateViewFrustum)
        return false;

    dir = glm::normalize(dir);
    right = glm::normalize(right);
    up = glm::normalize(up);

    static float tang = glm::tan(glm::radians(fov * 0.5f));
    static float nh = nearDist * tang;
    float nw = nh * (size.x / size.y);
    static float fh = farDist * tang;
    float fw = fh * (size.x / size.y);

    glm::vec3 nearCenter = pos + dir * nearDist;
    glm::vec3 farCenter = pos + dir * farDist;

    frustumVertices[NTL] = nearCenter + up * nh - right * nw;
    frustumVertices[NTR] = nearCenter + up * nh + right * nw;
    frustumVertices[NBL] = nearCenter - up * nh - right * nw;
    frustumVertices[NBR] = nearCenter - up * nh + right * nw;
    frustumVertices[FTL] = farCenter + up * fh - right * fw;
    frustumVertices[FTR] = farCenter + up * fh + right * fw;
    frustumVertices[FBL] = farCenter - up * fh - right * fw;
    frustumVertices[FBR] = farCenter - up * fh + right * fw;

#if 1
    planes[TOP].set(frustumVertices[NTR], frustumVertices[NTL], frustumVertices[FTL]);
    planes[BOTTOM].set(frustumVertices[NBL], frustumVertices[NBR], frustumVertices[FBR]);
    planes[LEFT].set(frustumVertices[NTL], frustumVertices[NBL], frustumVertices[FBL]);
    planes[RIGHT].set(frustumVertices[NBR], frustumVertices[NTR], frustumVertices[FBR]);
    planes[NEAR].set(frustumVertices[NTL], frustumVertices[NTR], frustumVertices[NBR]);
    planes[FAR].set(frustumVertices[FTR], frustumVertices[FTL], frustumVertices[FBL]);
#else
    planes[NEAR].set(-dir, nearCenter);
    planes[FAR].set(dir, farCenter);

    glm::vec3 aux = glm::normalize((nearCenter + up * nh) - pos);
    glm::vec3 normal = glm::cross(aux, right);
    planes[TOP].set(normal, nearCenter + up * nh);

    aux = glm::normalize((nearCenter - up * nh) - pos);
    normal = glm::cross(right, aux);
    planes[BOTTOM].set(normal, nearCenter - up * nh);

    aux = glm::normalize((nearCenter - right * nw) - pos);
    normal = glm::cross(aux, up);
    planes[LEFT].set(normal, nearCenter - right * nw);

    aux = glm::normalize((nearCenter + right * nw) - pos);
    normal = glm::cross(up, aux);
    planes[RIGHT].set(normal, nearCenter + right * nw);
#endif

    lastPos = pos;
    lastRot = rot;
    return true;
}

bool Camera::boxInFrustum(BoundingBox b) {
    for (int i = 0; i < 6; i++) {
        if (planes[i].distance(b.getVertexP(planes[i].normal)) < 0)
            return false;
    }
    return true;
}

void Camera::displayFrustum(glm::mat4 MVP) {
    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> cols;
    std::vector<unsigned short> inds;

    verts.push_back(frustumVertices[NTL]);
    verts.push_back(frustumVertices[NTR]);
    verts.push_back(frustumVertices[NBR]);
    verts.push_back(frustumVertices[NBL]);

    verts.push_back(frustumVertices[FTR]);
    verts.push_back(frustumVertices[FTL]);
    verts.push_back(frustumVertices[FBL]);
    verts.push_back(frustumVertices[FBR]);

    verts.push_back(frustumVertices[NBL]);
    verts.push_back(frustumVertices[NBR]);
    verts.push_back(frustumVertices[FBR]);
    verts.push_back(frustumVertices[FBL]);

    verts.push_back(frustumVertices[NTR]);
    verts.push_back(frustumVertices[NTL]);
    verts.push_back(frustumVertices[FTL]);
    verts.push_back(frustumVertices[FTR]);

    verts.push_back(frustumVertices[NTL]);
    verts.push_back(frustumVertices[NBL]);
    verts.push_back(frustumVertices[FBL]);
    verts.push_back(frustumVertices[FTL]);

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

