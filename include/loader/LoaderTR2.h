/*!
 * \file include/loader/LoaderTR2.h
 * \brief TR2 level file loader
 *
 * \author xythobuz
 */

#ifndef _LOADER_LOADER_TR2_H_
#define _LOADER_LOADER_TR2_H_

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

    uint32_t *palette; //!< 256 * 4 bytes, RGBA, A unused
    uint32_t numTextiles;
    uint16_t **textiles; //!< numTextiles, 256 * 256 * 2 bytes, ARGB (MSB + 3x5bit)

    uint16_t numRooms;
};

#endif

