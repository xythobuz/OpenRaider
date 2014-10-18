/*!
 * \file include/loader/LoaderTR1.h
 * \brief TR1 level file loader
 *
 * \author xythobuz
 */

#ifndef _LOADER_LOADER_TR1_H_
#define _LOADER_LOADER_TR1_H_

#include "loader/Loader.h"

class LoaderTR1 : public Loader {
  public:
    LoaderTR1();
    virtual ~LoaderTR1();

    virtual int load(std::string f);

  private:

};

#endif

