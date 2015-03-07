/*!
 * \file include/loader/LoaderTR3.h
 * \brief TR3 level file loader
 *
 * \author xythobuz
 */

#ifndef _LOADER_LOADER_TR3_H_
#define _LOADER_LOADER_TR3_H_

#include "loader/LoaderTR2.h"

class LoaderTR3 : public LoaderTR2 {
  public:
    virtual int load(std::string f);

  protected:
    virtual void loadRoomLights();
    virtual void loadRoomDataEnd(int16_t& alternateRoom, unsigned int& roomFlags);
    virtual void loadRoomMesh(std::vector<IndexedRectangle>& rectangles,
                              std::vector<IndexedRectangle>& triangles,
                              uint16_t& numRectangles, uint16_t& numTriangles);
};

#endif

