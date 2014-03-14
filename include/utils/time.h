/*!
 * \file include/utils/time.h
 * \brief Time handling utilities
 *
 * \author xythobuz
 */

#ifndef _UTILS_TIME_H_
#define _UTILS_TIME_H_

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

