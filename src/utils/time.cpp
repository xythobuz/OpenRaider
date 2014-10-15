/*!
 * \file src/utils/time.cpp
 * \brief Time handling utilities
 *
 * \author xythobuz
 */

#include <chrono>

#include "global.h"
#include "utils/time.h"

static auto systemTimerStart = std::chrono::steady_clock::now();

or_time_t systemTimerGet() {
    auto tp = std::chrono::steady_clock::now();

    return static_cast<or_time_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>
            (tp - systemTimerStart).count());
}

void systemTimerReset() {
    systemTimerStart = std::chrono::steady_clock::now();
}

