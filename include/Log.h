/*!
 * \file include/Log.h
 * \brief Global Logging Utility
 *
 * \author xythobuz
 */

#ifndef _LOG_H_
#define _LOG_H_

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <glm/gtc/type_precision.hpp>

#define LOG_USER    0
#define LOG_ERROR   1
#define LOG_WARNING 2
#define LOG_INFO    3
#define LOG_DEBUG   4
#define LOG_COUNT   5

struct LogEntry {
  public:
    std::string text;
    int level;
    LogEntry(std::string t, int l) : text(t), level(l) { }
};

class LogLevel;
class Log {
  public:
    const static char endl = '\n';

    static void initialize();
    static LogLevel& get(int level);
    static unsigned long size() { return wholeLog.size(); }
    static LogEntry& getEntry(unsigned long i) { return wholeLog.at(i); }

  private:
    static std::vector<LogLevel> logs;
    static std::vector<LogEntry> wholeLog;
    friend class LogLevel;
};

class LogLevel {
  public:
    LogLevel(int l) : level(l) { printBuffer << std::boolalpha; }

    LogLevel& operator<< (const glm::vec2& v) {
        return (*this) << v.x << " " << v.y;
    }

    LogLevel& operator<< (const glm::i32vec2& v) {
        return (*this) << v.x << " " << v.y;
    }

    LogLevel& operator<< (const glm::vec3& v) {
        return (*this) << v.x << " " << v.y << " " << v.z;
    }

    template<typename T>
    LogLevel& operator<< (const T t) {
        printBuffer << t;
        if (printBuffer.str().back() == Log::endl) {
            std::string s = printBuffer.str().substr(0, printBuffer.str().length() - 1);
            printBuffer.str("");
            Log::wholeLog.emplace_back(s, level);
#ifdef DEBUG
            std::cout << s << std::endl;
#endif
        }
        return (*this);
    }

  private:
    int level;
    std::ostringstream printBuffer;
};

#endif

