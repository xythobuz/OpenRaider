/*!
 * \file include/Log.h
 * \brief Log
 *
 * \author xythobuz
 */

#ifndef _LOG_H_
#define _LOG_H_

#include <string>
#include <sstream>
#include <vector>

#include <glm/gtc/type_precision.hpp>

class Log {
  public:

    const static char endl = '\n';

    Log();

    unsigned long size();
    std::string get(unsigned long i);

    template<typename T>
    Log& operator<< (const T t) {
        printBuffer << t;
        if (printBuffer.str().back() == endl) {
            mHistory.push_back(printBuffer.str().substr(0, printBuffer.str().length() - 1));
#ifdef DEBUG
            std::cout << printBuffer.str().substr(0, printBuffer.str().length() - 1) << std::endl;
#endif
            printBuffer.str("");
        }
        return (*this);
    }

    Log& operator<< (const glm::vec2& v) {
        return (*this) << v.x << " " << v.y;
    }

    Log& operator<< (const glm::i32vec2& v) {
        return (*this) << v.x << " " << v.y;
    }

    Log& operator<< (const glm::vec3& v) {
        return (*this) << v.x << " " << v.y << " " << v.z;
    }

  private:
    std::vector<std::string> mHistory;
    std::ostringstream printBuffer;
};

Log& getLog();

#endif

