/*!
 * \file src/Room.cpp
 * \brief World Room Mesh
 *
 * \author xythobuz
 */

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>

#include "global.h"
#include "Game.h"
#include "Log.h"
#include "Render.h"
#include "Room.h"
#include "TextureManager.h"

void Room::display(glm::mat4 view, glm::mat4 projection) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos[0], pos[1], pos[2]));
    mesh->display(model, view, projection);

    for (auto& m : models) {
        m->display(view, projection);
    }
}

bool Room::isWall(unsigned long sector) {
    assert(sector < sectors.size());

    //! \fixme is (sector > 0) correct??
    return ((sector > 0) && sectors.at(sector)->isWall());
}

long Room::getSector(float x, float z, float* floor, float* ceiling) {
    assert(floor != nullptr);
    assert(ceiling != nullptr);

    long sector = getSector(x, z);

    if ((sector >= 0) && (sector < (long)sectors.size())) {
        *floor = sectors.at(sector)->getFloor();
        *ceiling = sectors.at(sector)->getCeiling();
    }

    return sector;
}

long Room::getSector(float x, float z) {
    long sector = (((((int)x - (int)pos[0]) / 1024) *
                    numZSectors) + (((int)z - (int)pos[2]) / 1024));

    if (sector < 0)
        return -1;

    return sector;
}

void Room::getHeightAtPosition(float x, float* y, float z) {
    long sector = getSector(x, z);
    if ((sector >= 0) && (sector < (long)sectors.size()))
        *y = sectors.at(sector)->getFloor();
}

int Room::getAdjoiningRoom(float x, float y, float z,
                           float x2, float y2, float z2) {
    float vertices[4][3];
    glm::vec3 orig(x, y, z);
    glm::vec3 dir(x2 - x, y2 - y, z2 - z);
    glm::vec3 intersect;
    glm::vec3 verts[4];

    for (unsigned long i = 0; i < portals.size(); i++) {
        portals.at(i)->getVertices(vertices);
        verts[0] = glm::vec3(vertices[0][0], vertices[0][1], vertices[0][2]);
        verts[1] = glm::vec3(vertices[1][0], vertices[1][1], vertices[1][2]);
        verts[2] = glm::vec3(vertices[2][0], vertices[2][1], vertices[2][2]);
        verts[3] = glm::vec3(vertices[3][0], vertices[3][1], vertices[3][2]);

        if ((glm::intersectLineTriangle(orig, dir, verts[0], verts[1], verts[2], intersect))
            || (glm::intersectLineTriangle(orig, dir, verts[0], verts[3], verts[1], intersect)))
            return portals.at(i)->getAdjoiningRoom();
    }

    return -1;
}

// --------------------------------------

unsigned long Room::sizeAdjacentRooms() {
    return adjacentRooms.size();
}

long Room::getAdjacentRoom(unsigned long index) {
    assert(index < adjacentRooms.size());
    return adjacentRooms.at(index);
}

void Room::addAdjacentRoom(long r) {
    adjacentRooms.emplace_back(r);
}

unsigned long Room::sizePortals() {
    return portals.size();
}

Portal& Room::getPortal(unsigned long index) {
    assert(index < portals.size());
    return *portals.at(index);
}

void Room::addPortal(Portal* p) {
    portals.emplace_back(p);
}

unsigned long Room::sizeSectors() {
    return sectors.size();
}

Sector& Room::getSector(unsigned long index) {
    assert(index < sectors.size());
    return *sectors.at(index);
}

void Room::addSector(Sector* s) {
    sectors.emplace_back(s);
}

unsigned long Room::sizeModels() {
    return models.size();
}

StaticModel& Room::getModel(unsigned long index) {
    assert(index < models.size());
    return *models.at(index);
}

void Room::addModel(StaticModel* s) {
    models.emplace_back(s);
}

unsigned long Room::sizeLights() {
    return lights.size();
}

Light& Room::getLight(unsigned long index) {
    assert(index < lights.size());
    return *lights.at(index);
}

void Room::addLight(Light* l) {
    lights.emplace_back(l);
}

unsigned long Room::sizeSprites() {
    return sprites.size();
}

Sprite& Room::getSprite(unsigned long index) {
    assert(index < sprites.size());
    return *sprites.at(index);
}

void Room::addSprite(Sprite* s) {
    sprites.emplace_back(s);
}

