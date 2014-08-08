/*!
 * \file include/Exception.h
 * \brief Custom global Exception
 *
 * \author xythobuz
 */

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <memory>
#include <stdexcept>
#include <string>

class Exception : std::runtime_error {
public:
    Exception(const char *what);
    Exception(const std::string &what);

    static std::string getLastException();

private:
    static std::string lastException;

    virtual void foo(); //!< We don't want to emit a vtable in every translation unit
};

#endif

