/*!
 * \file src/Log.cpp
 * \brief Global Logging Utility
 *
 * \author xythobuz
 */

#include "global.h"
#include "Log.h"

LogLevel Log::logs[LOG_COUNT] = { 0, 1, 2, 3, 4 };
std::vector<LogEntry> Log::wholeLog;

LogLevel& Log::get(int level) {
    assertGreaterThanEqual(level, 0);
    assertLessThan(level, LOG_COUNT);
    return logs[level];
}

