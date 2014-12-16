/*!
 * \file src/RoomData.cpp
 * \brief World Room Mesh
 *
 * \author xythobuz
 */

#include "global.h"
#include "SkeletalModel.h"
#include "World.h"
#include "RoomData.h"

BoundingBox::BoundingBox(glm::vec3 min, glm::vec3 max) : a(min), b(max) { }

bool BoundingBox::inBox(float x, float y, float z) {
    return ((y > a.y) && (y < b.y) && inBoxPlane(x, z));
}

bool BoundingBox::inBoxPlane(float x, float z) {
    return ((x > a.x) && (x < b.x)
            && (z > a.z) && (z < b.z));
}

void BoundingBox::display(bool points, const unsigned char c1[4], const unsigned char c2[4]) {
    /*
    // Bind before entering now
    //glBindTexture(GL_TEXTURE_2D, 1);
    glPointSize(4.0);
    //glLineWidth(1.25);

    //! \fixme Need to make custom color key for this
    glColor3ubv(c1);

    glBegin(GL_POINTS);
    glVertex3f(b[0], b[1], b[2]);
    glVertex3f(a[0], a[1], a[2]);

    if (points) {
        glVertex3f(b[0], a[1], b[2]);
        glVertex3f(a[0], b[1], b[2]);
        glVertex3f(b[0], b[1], a[2]);
        glVertex3f(a[0], a[1], b[2]);
        glVertex3f(a[0], b[1], a[2]);
        glVertex3f(b[0], a[1], a[2]);
    }

    glEnd();

    glColor3ubv(c2);

    glBegin(GL_LINES);
    // max, top quad
    glVertex3f(b[0], b[1], b[2]);
    glVertex3f(b[0], a[1], b[2]);

    glVertex3f(b[0], b[1], b[2]);
    glVertex3f(a[0], b[1], b[2]);

    glVertex3f(b[0], b[1], b[2]);
    glVertex3f(b[0], b[1], a[2]);

    // max-min, vertical quads
    glVertex3f(a[0], b[1], b[2]);
    glVertex3f(a[0], b[1], a[2]);

    glVertex3f(b[0], a[1], b[2]);
    glVertex3f(b[0], a[1], a[2]);

    glVertex3f(b[0], a[1], b[2]);
    glVertex3f(a[0], a[1], b[2]);

    // min-max, vertical quads
    glVertex3f(b[0], b[1], a[2]);
    glVertex3f(b[0], a[1], a[2]);

    glVertex3f(b[0], b[1], a[2]);
    glVertex3f(a[0], b[1], a[2]);

    glVertex3f(a[0], b[1], b[2]);
    glVertex3f(a[0], a[1], b[2]);


    // min, bottom quad
    glVertex3f(a[0], a[1], a[2]);
    glVertex3f(a[0], b[1], a[2]);

    glVertex3f(a[0], a[1], a[2]);
    glVertex3f(b[0], a[1], a[2]);

    glVertex3f(a[0], a[1], a[2]);
    glVertex3f(a[0], a[1], b[2]);
    glEnd();

    glPointSize(1.0);
    //glLineWidth(1.0);
    */
}

// ----------------------------------------------------------------------------

void Light::getPos(float p[4]) {
    p[0] = pos[0];
    p[1] = pos[1];
    p[2] = pos[2];
    p[3] = pos[3];
}

void Light::getDir(float d[3]) {
    d[0] = dir[0];
    d[1] = dir[1];
    d[2] = dir[2];
}

float Light::getAtt() {
    return att;
}

void Light::getColor(float c[4]) {
    c[0] = color[0];
    c[1] = color[1];
    c[2] = color[2];
    c[3] = color[3];
}

float Light::getCutoff() {
    return cutoff;
}

Light::LightType Light::getType() {
    return type;
}

// ----------------------------------------------------------------------------

void StaticModel::display() {
    StaticMesh& mesh = getWorld().getStaticMesh(index);

    //if (!getRender().isVisible(pos[0], pos[1], pos[2], mesh.getRadius()))
    //    return;

    /*
    glPushMatrix();
    glTranslated(pos[0], pos[1], pos[2]);
    glRotated(yaw, 0, 1, 0);
    mesh.display();
    glPopMatrix();
    */
}

// ----------------------------------------------------------------------------

Portal::Portal(glm::vec3 vert[4], float norm[3], int adj) {
    for (unsigned int i = 0; i < 4; i++) {
        for (unsigned int j = 0; j < 3; j++) {
            vertices[i][j] = vert[i][j];
        }

        if (i < 3) {
            normal[i] = norm[i];
        }
    }

    adjoiningRoom = adj;
}

void Portal::getVertices(float vert[4][3]) {
    for (unsigned int i = 0; i < 4; i++) {
        for (unsigned int j = 0; j < 3; j++) {
            vert[i][j] = vertices[i][j];
        }
    }
}

int Portal::getAdjoiningRoom() {
    return adjoiningRoom;
}

// ----------------------------------------------------------------------------

float Sector::getFloor() {
    return floor;
}

float Sector::getCeiling() {
    return ceiling;
}

bool Sector::isWall() {
    return wall;
}

