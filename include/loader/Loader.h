/*!
 * \file include/loader/Loader.h
 * \brief Level file loader
 *
 * \author xythobuz
 */

#ifndef _LOADER_LOADER_H_
#define _LOADER_LOADER_H_

#include <memory>
#include <string>

#include "utils/binary.h"

class Loader {
public:

    typedef enum {
        TR_UNKNOWN = 0,
        TR_1 = 1,
        TR_2 = 2,
        TR_3 = 3,
        TR_4 = 4,
        TR_5 = 5
    } LoaderVersion;

    static LoaderVersion checkFile(std::string f);

    static std::unique_ptr<Loader> createLoader(std::string f);

    virtual ~Loader();

    virtual int load(std::string f) = 0;

protected:
    BinaryFile file;
};

#endif

