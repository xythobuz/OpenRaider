/*!
 * \file src/RoomData.cpp
 * \brief World Room Mesh
 *
 * \author xythobuz
 */

#include "global.h"
#include "Render.h"
#include "SkeletalModel.h"
#include "World.h"
#include "RoomData.h"

BoundingBox::BoundingBox() {
    a[0] = a[1] = a[2] = 0;
    b[0] = b[1] = b[2] = 0;
}

void BoundingBox::getBoundingBox(vec3_t box[2]) {
    box[0][0] = a[0];
    box[1][0] = b[0];
    box[0][1] = a[1];
    box[1][1] = b[1];
    box[0][2] = a[2];
    box[1][2] = b[2];
}

void BoundingBox::setBoundingBox(vec3_t min, vec3_t max) {
    a[0] = min[0];
    b[0] = max[0];
    a[1] = min[1];
    b[1] = max[1];
    a[2] = min[2];
    b[2] = max[2];
}

bool BoundingBox::inBox(vec_t x, vec_t y, vec_t z) {
    return ((y > a[1]) && (y < b[1]) && inBoxPlane(x, z));
}

bool BoundingBox::inBoxPlane(vec_t x, vec_t z) {
    return ((x > a[0]) && (x < b[0])
            && (z > a[2]) && (z < b[2]));
}

void BoundingBox::display(bool points, const vec4_t c1, const vec4_t c2) {
    // Bind before entering now
    //glBindTexture(GL_TEXTURE_2D, 1);
    glPointSize(4.0);
    //glLineWidth(1.25);

    //! \fixme Need to make custom color key for this
    glColor3fv(c1);

    glBegin(GL_POINTS);
    glVertex3f(b[0], b[1], b[2]);
    glVertex3f(a[0], a[1], a[2]);

    if (points)
    {
        glVertex3f(b[0], a[1], b[2]);
        glVertex3f(a[0], b[1], b[2]);
        glVertex3f(b[0], b[1], a[2]);
        glVertex3f(a[0], a[1], b[2]);
        glVertex3f(a[0], b[1], a[2]);
        glVertex3f(b[0], a[1], a[2]);
    }

    glEnd();

    glColor3fv(c2);

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
}

// ----------------------------------------------------------------------------

Light::Light(TombRaider &tr, unsigned int room, unsigned int index) {
    unsigned int lightFlags, lightType;

    tr.getRoomLight(room, index, pos, color,
            dir, &att, &cutoff, &lightType, &lightFlags);

    switch (lightType) {
        case tombraiderLight_typeDirectional:
            type = Light::typeDirectional;
            break;
        case tombraiderLight_typeSpot:
            type = Light::typeSpot;
            break;
        case tombraiderLight_typePoint:
        default:
            type = Light::typePoint;
    }

    //! \todo Light flags?
}

void Light::getPos(vec4_t p) {
    p[0] = pos[0];
    p[1] = pos[1];
    p[2] = pos[2];
    p[3] = pos[3];
}

void Light::getDir(vec3_t d) {
    d[0] = dir[0];
    d[1] = dir[1];
    d[2] = dir[2];
}

vec_t Light::getAtt() {
    return att;
}

void Light::getColor(vec4_t c) {
    c[0] = color[0];
    c[1] = color[1];
    c[2] = color[2];
    c[3] = color[3];
}

vec_t Light::getCutoff() {
    return cutoff;
}

Light::LightType Light::getType() {
    return type;
}

// ----------------------------------------------------------------------------

StaticModel::StaticModel(TombRaider &tr, unsigned int room, unsigned int i) {
    tr.getRoomModel(room, i, &index, pos, &yaw);
}

void StaticModel::display() {
    StaticMesh &mesh = getWorld().getStaticMesh(index);

    if (!getRender().isVisible(pos[0], pos[1], pos[2], mesh.getRadius()))
        return;

    glPushMatrix();
    glTranslated(pos[0], pos[1], pos[2]);
    glRotated(yaw, 0, 1, 0);
    mesh.display();
    glPopMatrix();
}

bool StaticModel::operator<(const StaticModel &other) {
    vec_t distA, distB;
    distA = getRender().mViewVolume.getDistToSphereFromNear(pos[0],
            pos[1], pos[2], 128.0f);
    distB = getRender().mViewVolume.getDistToSphereFromNear(other.pos[0],
            other.pos[1], other.pos[2], 128.0f);
    return (distA < distB);
}

// ----------------------------------------------------------------------------

Portal::Portal(TombRaider &tr, unsigned int room, unsigned int index, Matrix &transform) {
    float portalVertices[12];
    tr.getRoomPortal(room, index, &adjoiningRoom, normal, portalVertices);
    for (unsigned int j = 0; j < 4; ++j) {
        vertices[j][0] = portalVertices[j*3];
        vertices[j][1] = portalVertices[j*3+1];
        vertices[j][2] = portalVertices[j*3+2];

        // Relative coors in vis portals
        transform.multiply3v(vertices[j], vertices[j]);
    }
}

void Portal::getVertices(vec3_t vert[4]) {
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

Box::Box(TombRaider &tr, unsigned int room, unsigned int index) {
    tr.getRoomBox(room, index, a, b, c, d);
}

// ----------------------------------------------------------------------------

Sector::Sector(TombRaider &tr, unsigned int room, unsigned int index) {
    unsigned int sectorFlags;
    int floorDataIndex, boxIndex, roomBelow, roomAbove;

    tr.getRoomSector(room, index, &sectorFlags,
            &ceiling, &floor, &floorDataIndex, &boxIndex,
            &roomBelow, &roomAbove);

    wall = (sectorFlags & tombraiderSector_wall);
}

vec_t Sector::getFloor() {
    return floor;
}

vec_t Sector::getCeiling() {
    return ceiling;
}

bool Sector::isWall() {
    return wall;
}

