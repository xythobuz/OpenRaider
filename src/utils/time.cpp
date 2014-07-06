/*!
 * \file src/utils/time.cpp
 * \brief Time handling utilities
 *
 * \author xythobuz
 */

#include "global.h"
#include "utils/time.h"

unsigned long systemTimerStart = 0;

#if defined(HAVE_SYS_TIME_H) && defined(HAVE_GETTIMEOFDAY)

#include <sys/time.h>

unsigned long systemTimerGet() {
    struct timeval time;
    struct timezone zone;
    gettimeofday(&time, &zone);
    return ((time.tv_sec * 1000) + (time.tv_usec / 1000)) - systemTimerStart;
}

void systemTimerReset() {
    struct timeval time;
    struct timezone zone;
    gettimeofday(&time, &zone);
    systemTimerStart = (time.tv_sec * 1000) + (time.tv_usec / 1000);
}

#elif defined(_WIN32)

#include <Windows.h>

unsigned long systemTimerGet() {
    return GetTickCount() - systemTimerStart;
}

void systemTimerReset() {
    systemTimerStart = GetTickCount();
}

#else
#error "No support for timer on this platform!"
#endif

