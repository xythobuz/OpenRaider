/*!
 * \file include/utils/time.h
 * \brief Time handling utilities
 *
 * \author xythobuz
 * \author Mongoose
 */

#ifndef _UTILS_TIME_H_
#define _UTILS_TIME_H_

#if defined(linux) || defined(__APPLE__)
#include <time.h>
#include <sys/time.h>
#endif

extern struct timeval system_timer_start; //!< System timer start time
extern struct timeval system_timer_stop; //!< System timer last read time
extern struct timezone system_timer_tz; //!< System timer timezone info

/*!
 * \brief Read the system timer
 * \returns number of ticks
 */
unsigned int systemTimerGet();

/*!
 * \brief Reset the system timer
 */
void systemTimerReset();

#endif

