/*!
 * \file src/Room.cpp
 * \brief World Room Mesh
 *
 * \author xythobuz
 */

#include "main.h"
#include "Room.h"

StaticModel::StaticModel(int _index, vec_t _yaw, vec3_t _pos) {
    index = _index;
    yaw = _yaw;
    for (unsigned int i = 0; i < 3; i++)
        pos[i] = _pos[i];
}

bool StaticModel::operator<(const StaticModel &other) {
    vec_t distA, distB;
    distA = getRender().mViewVolume.getDistToSphereFromNear(pos[0],
            pos[1], pos[2], 128.0f);
    distB = getRender().mViewVolume.getDistToSphereFromNear(other.pos[0],
            other.pos[1], other.pos[2], 128.0f);
    return (distA < distB);
}

Portal::Portal(vec3_t _vertices[4], vec3_t _normal, int _adjoiningRoom) {
    for (unsigned int i = 0; i < 3; i++) {
        vertices[0][i] = _vertices[0][i];
        vertices[1][i] = _vertices[1][i];
        vertices[2][i] = _vertices[2][i];
        vertices[3][i] = _vertices[3][i];
        normal[i] = _normal[i];
    }
    adjoiningRoom = _adjoiningRoom;
}

Box::Box(vec3_t _a, vec3_t _b, vec3_t _c, vec3_t _d) {
    for (unsigned int i = 0; i < 3; i++) {
        a[i] = _a[i];
        b[i] = _b[i];
        c[i] = _c[i];
        d[i] = _d[i];
    }
}

Sector::Sector(vec_t _floor, vec_t _ceiling, bool _wall) {
    floor = _floor;
    ceiling = _ceiling;
    wall = _wall;
}

Room::Room(int _id) {
    id = _id;
    flags = 0;
    numXSectors = 0;
    numZSectors = 0;
    pos[0] = pos[1] = pos[2] = 0.0f;
    bbox[0][0] = bbox[0][1] = bbox[0][2] = 0.0f;
    bbox[1][0] = bbox[1][1] = bbox[1][2] = 0.0f;
}

Room::~Room() {
    unsigned long i;

    for (i = 0; i < sprites.size(); i++)
        delete sprites[i];

    for (i = 0; i < models.size(); i++)
        delete models[i];

    for (i = 0; i < portals.size(); i++)
        delete portals[i];

    for (i = 0; i < boxes.size(); i++)
        delete boxes[i];

    for (i = 0; i < sectors.size(); i++)
        delete sectors[i];

    for (i = 0; i < lights.size(); i++) {
        delete lights[i];
    }
}

