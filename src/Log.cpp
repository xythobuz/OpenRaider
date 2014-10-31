/*!
 * \file src/Log.cpp
 * \brief Log
 *
 * \author xythobuz
 */

#include "global.h"
#include "Log.h"

Log::Log() {
    printBuffer << std::boolalpha;
}

unsigned long Log::size() {
    return mHistory.size();
}

std::string Log::get(unsigned long i) {
    assert(i < mHistory.size());
    return mHistory.at(i);
}

