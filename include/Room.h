/*!
 * \file include/Room.h
 * \brief World Room Mesh
 *
 * \author xythobuz
 */

#ifndef _ROOM_H_
#define _ROOM_H_

#include <memory>
#include <vector>

#include "Sprite.h"
#include "RoomData.h"
#include "RoomMesh.h"

enum RoomFlags {
    RoomFlagUnderWater = (1 << 0)
};

class Room {
  public:
    Room(glm::vec3 _pos, BoundingBox* _bbox, RoomMesh* _mesh, unsigned int f,
         int a, int x, int z);

    void prepare() { mesh->prepare(); }
    void display(glm::mat4 VP);

    bool isWall(unsigned long sector);
    long getSector(float x, float z, float* floor, float* ceiling);
    long getSector(float x, float z);
    void getHeightAtPosition(float x, float* y, float z);
    int getAdjoiningRoom(float x, float y, float z,
                         float x2, float y2, float z2);

    BoundingBox& getBoundingBox() { return *bbox; }
    RoomMesh& getMesh() { return *mesh; }

    unsigned int getNumXSectors() { return numXSectors; }
    unsigned int getNumZSectors() { return numZSectors; }

    unsigned int getFlags() { return flags; }

    void addSprite(RoomSprite* s) { sprites.emplace_back(s); }
    void addModel(StaticModel* s) { models.emplace_back(s); }
    void addSector(Sector* s) { sectors.emplace_back(s); }
    void addLight(Light* l) { lights.emplace_back(l); }

    void addPortal(Portal* p) { portals.emplace_back(p); }
    unsigned long sizePortals() { return portals.size(); }
    Portal& getPortal(unsigned long index) { return *portals.at(index); }

    static void setShowBoundingBox(bool s) { showBoundingBox = s; }
    static bool getShowBoundingBox() { return showBoundingBox; }

    static void setShowRoomModels(bool s) { showRoomModels = s; }
    static bool getShowRoomModels() { return showRoomModels; }

    static void setShowRoomSprites(bool s) { showRoomSprites = s; }
    static bool getShowRoomSprites() { return showRoomSprites; }

    static void setShowRoomGeometry(bool s) { showRoomGeometry = s; }
    static bool getShowRoomGeometry() { return showRoomGeometry; }

  private:
    glm::vec3 pos;
    glm::mat4 model;
    std::unique_ptr<BoundingBox> bbox;
    std::unique_ptr<RoomMesh> mesh;

    unsigned int flags;
    int alternateRoom;
    int numXSectors;
    int numZSectors;

    std::vector<std::unique_ptr<RoomSprite>> sprites;
    std::vector<std::unique_ptr<StaticModel>> models;
    std::vector<std::unique_ptr<Portal>> portals;
    std::vector<std::unique_ptr<Sector>> sectors;
    std::vector<std::unique_ptr<Light>> lights;

    static bool showBoundingBox;
    static bool showRoomModels;
    static bool showRoomSprites;
    static bool showRoomGeometry;
};

#endif

