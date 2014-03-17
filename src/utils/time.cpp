/*!
 * \file include/utils/time.h
 * \brief Time handling utilities
 *
 * \author xythobuz
 */

#include <sys/time.h>

#include "utils/time.h"

struct timeval system_timer_start;
struct timeval system_timer_stop;
struct timezone system_timer_zone;

unsigned long systemTimerGet() {
    gettimeofday(&system_timer_stop, &system_timer_zone);
    return ((system_timer_stop.tv_sec - system_timer_start.tv_sec) * 1000)
        + (((system_timer_stop.tv_usec - system_timer_start.tv_usec) / 1000));
}

void systemTimerReset() {
    gettimeofday(&system_timer_start, &system_timer_zone);
}

