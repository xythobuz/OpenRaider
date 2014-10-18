/*!
 * \file include/loader/LoaderTR3.h
 * \brief TR3 level file loader
 *
 * \author xythobuz
 */

#ifndef _LOADER_LOADER_TR3_H_
#define _LOADER_LOADER_TR3_H_

#include "loader/Loader.h"

class LoaderTR3 : public Loader {
  public:
    LoaderTR3();
    virtual ~LoaderTR3();

    virtual int load(std::string f);

  private:

};

#endif

