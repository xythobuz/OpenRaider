/*!
 * \file include/utils/time.h
 * \brief Time handling utilities
 *
 * \author xythobuz
 */

#ifndef _UTILS_TIME_H_
#define _UTILS_TIME_H_

#include <ctime>

/*!
 * \brief Read the system timer
 * \returns number of ticks
 */
clock_t systemTimerGet();

/*!
 * \brief Reset the system timer
 */
void systemTimerReset();

#endif

