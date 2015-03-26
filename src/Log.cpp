/*!
 * \file src/Log.cpp
 * \brief Global Logging Utility
 *
 * \author xythobuz
 */

#include "global.h"
#include "Log.h"

std::vector<LogLevel> Log::logs;
std::vector<LogEntry> Log::wholeLog;

void Log::initialize() {
    for (int i = 0; i < LOG_COUNT; i++)
        logs.emplace_back(i);
}

LogLevel& Log::get(int level) {
    orAssertGreaterThanEqual(level, 0);
    orAssertLessThan(level, LOG_COUNT);
    return logs.at(level);
}

