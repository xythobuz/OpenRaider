/*!
 * \file include/loader/LoaderTR2.h
 * \brief TR2 level file loader
 *
 * \author xythobuz
 */

#ifndef _LOADER_LOADER_TR2_H_
#define _LOADER_LOADER_TR2_H_

#include <array>
#include <cstdint>

#include "RoomData.h"
#include "RoomMesh.h"
#include "loader/Loader.h"

class LoaderTR2 : public Loader {
  public:
    virtual int load(std::string f);

  protected:
    virtual void loadPalette();
    virtual void loadTextures();
    virtual void loadRoomLights();
    virtual void loadRoomStaticMeshes(std::vector<StaticModel*>& staticModels);
    virtual void loadRoomDataEnd(int16_t& alternateRoom, unsigned int& roomFlags);
    virtual void loadRoomVertex(RoomVertexTR2& vert);
    virtual void loadRooms();
    virtual void loadFloorData();
    virtual void loadMeshes();
    virtual void loadMoveables();
    virtual void loadStaticMeshes();
    virtual void loadTextiles();
    virtual void loadSprites();
    virtual void loadCameras();
    virtual void loadSoundSources();
    virtual void loadBoxesOverlapsZones();
    virtual void loadAnimatedTextures();
    virtual void loadItems();
    virtual void loadCinematicFrames();
    virtual void loadDemoData();
    virtual void loadSoundMap();
    virtual void loadSoundDetails();
    virtual void loadSampleIndices();

    virtual void loadExternalSoundFile(std::string f);
    virtual void loadSoundFiles(BinaryReader& sfx);

    virtual int getPaletteIndex(uint16_t index);
};

#endif

