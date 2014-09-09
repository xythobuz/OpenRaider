/*!
 * \file src/utils/time.cpp
 * \brief Time handling utilities
 *
 * \author xythobuz
 */

#include <chrono>

#include "global.h"
#include "utils/time.h"

static unsigned long systemTimerStart = 0;

unsigned long systemTimerGet() {
    auto tp = std::chrono::steady_clock::now();
    auto dtn = tp.time_since_epoch();
    return (dtn.count() * 1000 * std::chrono::steady_clock::period::num
            / std::chrono::steady_clock::period::den) - systemTimerStart;
}

void systemTimerReset() {
    auto tp = std::chrono::steady_clock::now();
    auto dtn = tp.time_since_epoch();
    systemTimerStart = dtn.count() * 1000 * std::chrono::steady_clock::period::num
            / std::chrono::steady_clock::period::den;
}

