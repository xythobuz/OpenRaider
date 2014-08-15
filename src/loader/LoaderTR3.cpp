/*!
 * \file src/loader/LoaderTR3.cpp
 * \brief TR3 level file loader
 *
 * \author xythobuz
 */

#include "global.h"
#include "loader/LoaderTR3.h"

LoaderTR3::LoaderTR3() {
}

LoaderTR3::~LoaderTR3() {
}

int LoaderTR3::load(std::string f) {
    if (file.open(f.c_str()) != 0) {
        return 1; // Could not open file
    }



    return 1; // stub
}

