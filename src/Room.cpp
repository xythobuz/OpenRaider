/*!
 * \file src/Room.cpp
 * \brief World Room Mesh
 *
 * \author xythobuz
 */

#include <algorithm>
#include <glm/gtx/intersect.hpp>

#include "global.h"
#include "Game.h"
#include "Log.h"
#include "Render.h"
#include "Room.h"
#include "TextureManager.h"

Room::Room(float p[3], unsigned int f, unsigned int x, unsigned int z)
    : flags(f), numXSectors(x), numZSectors(z) {
    if (p == nullptr) {
        pos[0] = 0.0f;
        pos[1] = 0.0f;
        pos[2] = 0.0f;
    } else {
        pos[0] = p[0];
        pos[1] = p[0];
        pos[2] = p[0];
    }
}

void Room::setNumXSectors(unsigned int n) {
    numXSectors = n;
}

void Room::setNumZSectors(unsigned int n) {
    numZSectors = n;
}

void Room::setPos(float p[3]) {
    for (int i = 0; i < 3; i++)
        pos[i] = p[i];
}

#define EMPTY_VECTOR(x)     \
while (!x.empty()) {        \
    delete x[x.size() - 1]; \
    x.pop_back();           \
}

Room::~Room() {
    EMPTY_VECTOR(sprites);
    EMPTY_VECTOR(models);
    EMPTY_VECTOR(portals);
    EMPTY_VECTOR(sectors);
    EMPTY_VECTOR(lights);
}

void Room::display(bool alpha) {
    /*
    glPushMatrix();
    //LightingSetup();

    getTextureManager().bindTextureId(TEXTURE_WHITE, TextureManager::TextureStorage::SYSTEM);

    if ((!alpha) && Render::getMode() == RenderMode::Wireframe) {
        glLineWidth(2.0);
        glColor3ubv(RED);

        for (unsigned int i = 0; i < sizePortals(); i++) {
            Portal& portal = getPortal(i);
            float vertices[4][3];
            portal.getVertices(vertices);

            glBegin(GL_LINE_LOOP);
            glVertex3fv(vertices[0]);
            glVertex3fv(vertices[1]);
            glVertex3fv(vertices[2]);
            glVertex3fv(vertices[3]);
            glEnd();
        }

        glLineWidth(1.0);
    }

    if (Render::getMode() == RenderMode::Wireframe && (!alpha)) {
        bbox.display(true, RED, GREEN);
    }

    glTranslated(pos[0], pos[1], pos[2]);

    // Reset since GL_MODULATE used, reset to WHITE
    glColor3ubv(WHITE);

    switch (Render::getMode()) {
        case RenderMode::Wireframe:
            mesh.mMode = Mesh::MeshModeWireframe;
            break;
        case RenderMode::Solid:
            mesh.mMode = Mesh::MeshModeSolid;
            break;
        default:
            mesh.mMode = Mesh::MeshModeTexture;
            break;
    }

    if (alpha)
        mesh.drawAlpha();
    else
        mesh.drawSolid();

    glPopMatrix();

    // Draw other room meshes and sprites
    if (alpha || (Render::getMode() == RenderMode::Wireframe)
        || (Render::getMode() == RenderMode::Solid)) {
        //sortModels(); // TODO
        for (unsigned int i = 0; i < sizeModels(); i++)
            getModel(i).display();

        for (unsigned int i = 0; i < sizeSprites(); i++)
            getSprite(i).display();
    }
    */
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

void Room::setFlags(unsigned int f) {
    flags = f;
}

unsigned int Room::getFlags() {
    return flags;
}

unsigned int Room::getNumXSectors() {
    return numXSectors;
}

unsigned int Room::getNumZSectors() {
    return numZSectors;
}

void Room::getPos(float p[3]) {
    for (unsigned int i = 0; i < 3; i++)
        p[i] = pos[i];
}

unsigned long Room::sizeAdjacentRooms() {
    return adjacentRooms.size();
}

long Room::getAdjacentRoom(unsigned long index) {
    assert(index < adjacentRooms.size());
    return adjacentRooms.at(index);
}

void Room::addAdjacentRoom(long r) {
    adjacentRooms.push_back(r);
}

unsigned long Room::sizePortals() {
    return portals.size();
}

Portal& Room::getPortal(unsigned long index) {
    assert(index < portals.size());
    return *portals.at(index);
}

void Room::addPortal(Portal* p) {
    portals.push_back(p);
}

unsigned long Room::sizeSectors() {
    return sectors.size();
}

Sector& Room::getSector(unsigned long index) {
    assert(index < sectors.size());
    return *sectors.at(index);
}

void Room::addSector(Sector* s) {
    sectors.push_back(s);
}

unsigned long Room::sizeModels() {
    return models.size();
}

StaticModel& Room::getModel(unsigned long index) {
    assert(index < models.size());
    return *models.at(index);
}

void Room::addModel(StaticModel* s) {
    models.push_back(s);
}

unsigned long Room::sizeLights() {
    return lights.size();
}

Light& Room::getLight(unsigned long index) {
    assert(index < lights.size());
    return *lights.at(index);
}

void Room::addLight(Light* l) {
    lights.push_back(l);
}

unsigned long Room::sizeSprites() {
    return sprites.size();
}

Sprite& Room::getSprite(unsigned long index) {
    assert(index < sprites.size());
    return *sprites.at(index);
}

void Room::addSprite(Sprite* s) {
    sprites.push_back(s);
}

BoundingBox& Room::getBoundingBox() {
    return bbox;
}

