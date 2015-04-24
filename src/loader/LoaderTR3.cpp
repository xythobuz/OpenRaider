/*!
 * \file src/loader/LoaderTR3.cpp
 * \brief TR3 level file loader
 *
 * \author xythobuz
 */

#include "global.h"
#include "loader/LoaderTR3.h"

int LoaderTR3::load(std::string f) {
    if (file.open(f) != 0) {
        return 1; // Could not open file
    }

    uint32_t version = file.readU32();
    if ((version != 0xFF080038) && (version != 0xFF180038)) {
        return 2; // Not a TR3 level?!
    }

    loadPalette();
    loadTextures();

    file.seek(file.tell() + 4); // Unused value?

    loadRooms();
    loadFloorData();
    loadMeshes();
    loadMoveables();
    loadStaticMeshes();
    loadSprites();
    loadCameras();
    loadSoundSources();
    loadBoxesOverlapsZones();
    loadAnimatedTextures();
    loadTextiles();
    loadItems();

    file.seek(file.tell() + 8192); // Skip Light map, only for 8bit coloring

    loadCinematicFrames();
    loadDemoData();
    loadSoundMap();
    loadSoundDetails();
    loadSampleIndices();

    loadExternalSoundFile(f);

    // TODO load TR3 PC version CDAUDIO.WAD file
    // TODO load TR3 Mac version CDAudio.db and WAV files

    return 0;
}

void LoaderTR3::loadRoomLights() {
    int16_t roomIntensity1 = file.read16();
    int16_t roomIntensity2 = file.read16();

    uint16_t numLights = file.readU16();
    for (unsigned int l = 0; l < numLights; l++) {
        // Position of light, in world coordinates
        int32_t x = file.read32();
        int32_t y = file.read32();
        int32_t z = file.read32();

        uint16_t intensity1 = file.readU16();
        uint16_t intensity2 = file.readU16(); // Almost always equal to intensity1

        uint32_t fade1 = file.readU32(); // Falloff value?
        uint32_t fade2 = file.readU32(); // Falloff value?

        // TODO store light somewhere
    }
}

void LoaderTR3::loadRoomDataEnd(int16_t& alternateRoom, unsigned int& roomFlags) {
    LoaderTR2::loadRoomDataEnd(alternateRoom, roomFlags);

    uint8_t r = file.readU8();
    uint8_t g = file.readU8();
    uint8_t b = file.readU8();

    // TODO store room-light color (?) somewhere
}

void LoaderTR3::loadRoomMesh(std::vector<IndexedRectangle>& rectangles,
                             std::vector<IndexedRectangle>& triangles,
                             uint16_t& numRectangles, uint16_t& numTriangles) {
    numRectangles = file.readU16();
    for (unsigned int r = 0; r < numRectangles; r++) {
        // Indices into the vertex list read just before
        uint16_t vertex1 = file.readU16();
        uint16_t vertex2 = file.readU16();
        uint16_t vertex3 = file.readU16();
        uint16_t vertex4 = file.readU16();

        // Index into the object-texture list
        uint16_t texture = file.readU16() & 0x7FFF;

        rectangles.emplace_back(texture, vertex1, vertex2, vertex3, vertex4);
    }

    numTriangles = file.readU16();
    for (unsigned int t = 0; t < numTriangles; t++) {
        // Indices into the room vertex list
        uint16_t vertex1 = file.readU16();
        uint16_t vertex2 = file.readU16();
        uint16_t vertex3 = file.readU16();

        // Index into the object-texture list
        uint16_t texture = file.readU16() & 0x7FFF;

        triangles.emplace_back(texture, vertex1, vertex2, vertex3);
    }
}

