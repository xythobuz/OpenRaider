/*!
 * \file src/loader/Loader.cpp
 * \brief Level file loader
 *
 * \author xythobuz
 */

#include "global.h"
#include "loader/Loader.h"
#include "loader/LoaderTR2.h"

Loader::LoaderVersion Loader::checkFile(std::string f) {
    BinaryFile file;
    if (file.open(f.c_str()) != 0)
        return TR_UNKNOWN;

    uint32_t start = file.readU32();
    switch (start) {
        case 0x00000020:
            return TR_1;

        case 0x0000002D:
            return TR_2;

        case 0xFF080038:
        case 0xFF180038:
            return TR_3;

        case 0xFFFFFFF0: // bogus
        case 0x00345254: // "TR4\0"
            return TR_4;
    }

    return TR_UNKNOWN;
}

std::unique_ptr<Loader> Loader::createLoader(std::string f) {
    LoaderVersion v = checkFile(f);
    switch (v) {
        case TR_1:
        case TR_3:
        case TR_4:
        case TR_5:
        case TR_UNKNOWN:
            return NULL;

        case TR_2:
            return std::unique_ptr<Loader>(new LoaderTR2());
    }
}

Loader::~Loader() {

}

