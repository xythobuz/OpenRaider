/*!
 * \file src/RoomData.cpp
 * \brief World Room Mesh
 *
 * \author xythobuz
 */

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "main.h"
#include "RoomData.h"

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
    model_mesh_t *mesh = getWorld().getMesh(index);

    if (!mesh)
        return;

    if (!getRender().isVisible(pos[0], pos[1], pos[2], mesh->radius))
        return;

    glPushMatrix();
    glTranslated(pos[0], pos[1], pos[2]);
    glRotated(yaw, 0, 1, 0);

    getRender().drawModelMesh(mesh);
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

