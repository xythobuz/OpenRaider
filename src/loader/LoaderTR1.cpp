/*!
 * \file src/loader/LoaderTR1.cpp
 * \brief TR1 level file loader
 *
 * \author xythobuz
 */

#include "global.h"
#include "loader/LoaderTR1.h"

LoaderTR1::LoaderTR1() {
}

LoaderTR1::~LoaderTR1() {
}

int LoaderTR1::load(std::string f) {
    if (file.open(f.c_str()) != 0) {
        return 1; // Could not open file
    }



    return 1; // stub
}

