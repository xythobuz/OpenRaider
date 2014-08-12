/*!
 * \file src/loader/LoaderTR2.cpp
 * \brief TR2 level file loader
 *
 * \author xythobuz
 */

#include "loader/LoaderTR2.h"

LoaderTR2::LoaderTR2() {
    palette = new uint32_t[256];
    numTextiles = 0;
    textiles = nullptr;
}

LoaderTR2::~LoaderTR2() {
    delete [] palette;
    palette = nullptr;

    if (textiles != nullptr) {
        for (unsigned int i = 0; i < numTextiles; i++)
            delete [] textiles[i];
        delete [] textiles;
    }
}

int LoaderTR2::load(std::string f) {
    if (file.open(f.c_str()) != 0) {
        return 1; // Could not open file
    }

    if (file.readU32() != 0x2D) {
        return 2; // Not a TR2 level?!
    }

    loadPaletteTextiles();

    file.seek(file.tell() + 4); // Unused value?

    loadRooms();



    return 0;
}

void LoaderTR2::loadPaletteTextiles() {
    file.seek(file.tell() + 768); // Skip 8bit palette, 256 * 3 bytes

    // Read the 16bit palette, 256 * 4 bytes, RGBA, A unused
    for (unsigned int i = 0; i < 256; i++)
        palette[i] = file.readU32();

    numTextiles = file.readU32();

    file.seek(file.tell() + (numTextiles * 256 * 256)); // Skip 8bit textiles

    // Read the 16bit textiles, numTextiles * 256 * 256 * 2 bytes
    textiles = new uint16_t *[numTextiles];
    for (unsigned int i = 0; i < numTextiles; i++) {
        textiles[i] = new uint16_t[256 * 256];
        for (unsigned int j = 0; j < (256 * 256); j++) {
            textiles[i][j] = file.readU16();
        }
    }
}

void LoaderTR2::loadRooms() {
    numRooms = file.readU16();

    for (unsigned int i = 0; i < numRooms; i++) {
        // Room Header
        int32_t xOffset = file.read32();
        int32_t zOffset = file.read32();
        int32_t yBottom = file.read32(); // lowest point == largest y value
        int32_t yTop = file.read32(); // highest point == smallest y value

        // Number of data words (2 bytes) to follow
        uint32_t dataToFollow = file.readU32();


    }
}

