/*!
 * \file src/Room.cpp
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

#include <algorithm>
#include <assert.h>

#include "main.h"
#include "Room.h"

Light::Light(vec4_t pos, vec3_t dir, vec_t att, vec4_t color, vec_t cutoff, LightType type) {
    for (unsigned int i = 0; i < 4; i++) {
        mPos[i] = pos[i];
        mColor[i] = color[i];
        if (i < 3)
            mDir[i] = dir[i];
    }
    mAtt = att;
    mCutoff = cutoff;
    mType = type;
}

StaticModel::StaticModel(int _index, vec_t _yaw, vec3_t _pos) {
    index = _index;
    yaw = _yaw;
    for (unsigned int i = 0; i < 3; i++)
        pos[i] = _pos[i];
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

    getRender().drawModelMesh(mesh, Render::roomMesh);
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

vec_t Sector::getFloor() {
    return floor;
}

vec_t Sector::getCeiling() {
    return ceiling;
}

bool Sector::isWall() {
    return wall;
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

void Room::setFlags(unsigned int f) {
    flags = f;
}

unsigned int Room::getFlags() {
    return flags;
}

void Room::setNumXSectors(unsigned int n) {
     numXSectors = n;
}

unsigned int Room::getNumXSectors() {
    return numXSectors;
}

void Room::setNumZSectors(unsigned int n) {
    numZSectors = n;
}

unsigned int Room::getNumZSectors() {
    return numZSectors;
}

void Room::setPos(vec3_t p) {
    for (unsigned int i = 0; i < 3; i++)
        pos[i] = p[i];
}

void Room::getPos(vec3_t p) {
    for (unsigned int i = 0; i < 3; i++)
        p[i] = pos[i];
}

void Room::getBoundingBox(vec3_t box[2]) {
    for (unsigned int i = 0; i < 2; i++)
        for (unsigned int j = 0; j < 3; j++)
            box[i][j] = bbox[i][j];
}

void Room::setBoundingBox(vec3_t box[2]) {
    for (unsigned int i = 0; i < 2; i++)
        for (unsigned int j = 0; j < 3; j++)
            bbox[i][j] = box[i][j];
}

bool Room::inBox(vec_t x, vec_t y, vec_t z) {
    return ((y > bbox[0][1]) && (y < bbox[1][1]) && inBoxPlane(x, z));
}

bool Room::inBoxPlane(vec_t x, vec_t z) {
    return ((x > bbox[0][0]) && (x < bbox[1][0])
            && (z > bbox[0][2]) && (z < bbox[1][2]));
}

unsigned int Room::sizeAdjacentRooms() {
    return adjacentRooms.size();
}

int Room::getAdjacentRoom(unsigned int index) {
    assert(index < adjacentRooms.size());
    return adjacentRooms.at(index);
}

void Room::addAdjacentRoom(int r) {
    adjacentRooms.push_back(r);
}

unsigned int Room::sizePortals() {
    return portals.size();
}

Portal &Room::getPortal(unsigned int index) {
    assert(index < portals.size());
    return *portals.at(index);
}

void Room::addPortal(Portal &p) {
    portals.push_back(&p);
}

unsigned int Room::sizeSectors() {
    return sectors.size();
}

Sector &Room::getSector(unsigned int index) {
    assert(index < sectors.size());
    return *sectors.at(index);
}

void Room::addSector(Sector &s) {
    sectors.push_back(&s);
}

unsigned int Room::sizeBox() {
    return boxes.size();
}

Box &Room::getBox(unsigned int index) {
    assert(index < boxes.size());
    return *boxes.at(index);
}

void Room::addBox(Box &b) {
    boxes.push_back(&b);
}

unsigned int Room::sizeModels() {
    return models.size();
}

StaticModel &Room::getModel(unsigned int index) {
    assert(index < models.size());
    return *models.at(index);
}

void Room::addModel(StaticModel &m) {
    models.push_back(&m);
}

void Room::sortModels() {
    std::sort(models.begin(), models.end());
}

unsigned int Room::sizeLights() {
    return lights.size();
}

Light &Room::getLight(unsigned int index) {
    assert(index < lights.size());
    return *lights.at(index);
}

void Room::addLight(Light &l) {
    lights.push_back(&l);
}

unsigned int Room::sizeSprites() {
    return sprites.size();
}

Sprite &Room::getSprite(unsigned int index) {
    assert(index < sprites.size());
    return *sprites.at(index);
}

void Room::addSprite(Sprite &s) {
    sprites.push_back(&s);
}

Mesh &Room::getMesh() {
    return mesh;
}

