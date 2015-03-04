/*!
 * \file include/loader/LoaderTR1.h
 * \brief TR1 level file loader
 *
 * \author xythobuz
 */

#ifndef _LOADER_LOADER_TR1_H_
#define _LOADER_LOADER_TR1_H_

#include "loader/LoaderTR2.h"

class LoaderTR1 : public LoaderTR2 {
  public:
    virtual int load(std::string f);

  protected:
    virtual void loadPalette();
    virtual void loadTextures();
    virtual void loadRoomLights();
    virtual void loadRoomStaticMeshes(std::vector<StaticModel*>& staticModels);
    virtual void loadRoomVertex(RoomVertexTR2& vert);
    virtual void loadItems();
    virtual void loadBoxesOverlapsZones();
    virtual void loadSoundMap();
    virtual void loadSoundSamples();

    virtual int getPaletteIndex(uint16_t index);
};

#endif

