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

#include "loader/Loader.h"

class LoaderTR2 : public Loader {
  public:
    LoaderTR2();
    virtual ~LoaderTR2();

    virtual int load(std::string f);

  private:
    void loadPaletteTextiles();
    void loadRooms();
    void loadFloorData();
    void loadMeshes();
    void loadMoveables();
    void loadStaticMeshes();
    void loadTextures();
    void loadSprites();
    void loadCameras();
    void loadSoundSources();
    void loadBoxesOverlapsZones();
    void loadAnimatedTextures();
    void loadItems();
    void loadCinematicFrames();
    void loadDemoData();
    void loadSoundMap();
    void loadSoundDetails();
    void loadSampleIndices();

    void loadExternalSoundFile(std::string f);

    std::array<uint32_t, 256> palette;
};

struct RoomVertexTR2 {
    int16_t x, y, z; // Vertex coordinates, relative to x/zOffset
    int16_t light1, light2; // Almost always equal

    // Set of flags for special rendering effects
    // 0x8000 - Something to do with water surface?
    // 0x4000 - Underwater lighting modulation/movement if seen from above
    // 0x2000 - Water/Quicksand surface movement
    // 0x0010 - Normal?
    uint16_t attributes;
};

struct RoomRectangleTR2 {
    uint16_t v1, v2, v3, v4; // Vertex list indices
    uint16_t texture; // Index into object-texture list

    RoomRectangleTR2(uint16_t _v1, uint16_t _v2, uint16_t _v3, uint16_t _v4, uint16_t t)
        : v1(_v1), v2(_v2), v3(_v3), v4(_v4), texture(t) { }
};

struct RoomTriangleTR2 {
    uint16_t v1, v2, v3; // Vertex list indices
    uint16_t texture; // Index into object-texture list

    RoomTriangleTR2(uint16_t _v1, uint16_t _v2, uint16_t _v3, uint16_t t)
        : v1(_v1), v2(_v2), v3(_v3), texture(t) { }
};

#endif

