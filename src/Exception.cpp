/*!
 * \file src/Exception.cpp
 * \brief Custom global Exception
 *
 * \author xythobuz
 */

#include "global.h"
#include "Exception.h"

std::string Exception::lastException("No custom exception since start!");

Exception::Exception(const char *what) : runtime_error(what) {
    lastException = what;
}

Exception::Exception(const std::string &what) : runtime_error(what) {
    lastException = what;
}

std::string Exception::getLastException() {
    return lastException;
}

void Exception::foo() { }

