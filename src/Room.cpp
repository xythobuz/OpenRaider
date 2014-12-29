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

Room::Room(glm::vec3 _pos, BoundingBox* _bbox, RoomMesh* _mesh, unsigned int f,
           int a, int x, int z) : pos(_pos), bbox(_bbox), mesh(_mesh), flags(f),
                                  alternateRoom(a), numXSectors(x), numZSectors(z) {
    model = glm::translate(glm::mat4(1.0f), pos);
}

void Room::display(glm::mat4 VP) {
    glm::mat4 MVP = VP * model;

    mesh->display(MVP);

    for (auto& m : models) {
        m->display(VP);
    }

    if (Render::getMode() == RenderMode::Wireframe)
        bbox->display(VP, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f));
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
    glm::vec3 orig(x, y, z);
    glm::vec3 dir(x2 - x, y2 - y, z2 - z);
    glm::vec3 intersect;

    for (unsigned long i = 0; i < portals.size(); i++) {
        if ((glm::intersectLineTriangle(orig, dir, portals.at(i)->getVertex(0),
                                        portals.at(i)->getVertex(1),
                                        portals.at(i)->getVertex(2), intersect))
            || (glm::intersectLineTriangle(orig, dir, portals.at(i)->getVertex(0),
                                           portals.at(i)->getVertex(3),
                                           portals.at(i)->getVertex(1), intersect)))
            return portals.at(i)->getAdjoiningRoom();
    }

    return -1;
}

// --------------------------------------

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

