/*!
 * \file include/utils/time.h
 * \brief Time handling utilities
 *
 * \author xythobuz
 */

#include "utils/time.h"

#define CLOCKS_PER_MS (CLOCKS_PER_SEC / 1000)

clock_t system_timer_start;
clock_t system_timer_stop;

clock_t systemTimerGet() {
    system_timer_stop = clock();
    return (system_timer_stop - system_timer_start) / CLOCKS_PER_MS;
}

void systemTimerReset() {
    system_timer_start = clock();
}

