/*!
 * \file src/Log.cpp
 * \brief Log
 *
 * \author xythobuz
 */

#include "global.h"
#include "Log.h"

LogLevel Log::logs[LOG_COUNT] = { 0, 1, 2, 3, 4 };
std::vector<LogEntry> Log::wholeLog;

LogLevel& Log::get(int level) {
    assert(level >= 0);
    assert(level < LOG_COUNT);
    return logs[level];
}

