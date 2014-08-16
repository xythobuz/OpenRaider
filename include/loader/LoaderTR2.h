/*!
 * \file include/loader/LoaderTR2.h
 * \brief TR2 level file loader
 *
 * \author xythobuz
 */

#ifndef _LOADER_LOADER_TR2_H_
#define _LOADER_LOADER_TR2_H_

#include <cstdint>
#include <array>

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

    std::array<uint32_t, 256> palette; //!< RGBA, A unused
    uint32_t numTextiles;
    uint16_t **textiles; //!< numTextiles, 256 * 256 * 2 bytes, ARGB (MSB + 3x5bit)
};

#endif

