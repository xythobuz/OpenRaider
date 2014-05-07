/*!
 * \file include/utils/time.h
 * \brief Time handling utilities
 *
 * \author xythobuz
 */

#include "utils/time.h"

#if defined(unix) || defined(__APPLE__) || defined (__linux__)

#include <sys/time.h>

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

#elif defined(WIN32)

#include <Windows.h>

DWORD system_timer_start = 0;

unsigned long systemTimerGet() {
    return GetTickCount() - system_timer_start;
}

void systemTimerReset() {
    system_timer_start = GetTickCount();
}

#else
#warn "No support for timer on this platform!"

unsigned long systemTimerGet() { return 0; }

void systemTimerReset() { }

#endif

