/*!
 * \file src/utils/binary.cpp
 * \brief Binary file reading utilities
 *
 * \author xythobuz
 */

#include "global.h"
#include "utils/binary.h"

/*! \fixme Left-Shift with signed integer is undefined!
 *  Is there a portable way to read multi-byte signed integers?
 *  Without having to detect the endianness at run-time?
 */
const int bigendiandetection = 1;
#define ISBIGENDIAN() ((*(char *)&bigendiandetection) == 0)

BinaryFile::BinaryFile(const char *f) {
    file.open(f, std::ios_base::in | std::ios_base:: binary);
}

BinaryFile::~BinaryFile() {
    file.close();
}

long long BinaryFile::tell() {
    return file.tellg();
}

void BinaryFile::seek(long long pos) {
    assert(pos >= 0);
    file.seekg(pos);
}

int8_t BinaryFile::read8() {
    int8_t ret;
    file.read(reinterpret_cast<char *>(&ret), sizeof(ret));
    return ret;
}

uint8_t BinaryFile::readU8() {
    uint8_t ret;
    file.read(reinterpret_cast<char *>(&ret), sizeof(ret));
    return ret;
}

int16_t BinaryFile::read16() {
    int16_t ret;
    file.read(reinterpret_cast<char *>(&ret), sizeof(ret));
    if (ISBIGENDIAN()) {
        char *p = reinterpret_cast<char *>(&ret);
        char tmp = p[0];
        p[0] = p[1];
        p[1] = tmp;
    }
    return ret;
}

uint16_t BinaryFile::readU16() {
    uint16_t a = readU8();
    uint16_t b = readU8();
    return (uint16_t)(a | (b << 8));
}

int32_t BinaryFile::read32() {
    int32_t ret;
    file.read(reinterpret_cast<char *>(&ret), sizeof(ret));
    if (ISBIGENDIAN()) {
        char *p = reinterpret_cast<char *>(&ret);
        char tmp = p[0];
        p[0] = p[3];
        p[3] = tmp;
        tmp = p[1];
        p[1] = p[2];
        p[2] = tmp;
    }
    return ret;
}

uint32_t BinaryFile::readU32() {
    uint32_t a = readU8();
    uint32_t b = readU8();
    uint32_t c = readU8();
    uint32_t d = readU8();
    return (uint32_t)(a | (b << 8) | (c << 16) | (d << 24));
}

int64_t BinaryFile::read64() {
    int64_t ret;
    file.read(reinterpret_cast<char *>(&ret), sizeof(ret));
    if (ISBIGENDIAN()) {
        char *p = reinterpret_cast<char *>(&ret);
        char tmp = p[0];
        p[0] = p[7];
        p[7] = tmp;
        tmp = p[1];
        p[1] = p[6];
        p[6] = tmp;
        tmp = p[2];
        p[2] = p[5];
        p[5] = tmp;
        tmp = p[3];
        p[3] = p[4];
        p[4] = tmp;
    }
    return ret;
}

uint64_t BinaryFile::readU64() {
    uint64_t a = readU8();
    uint64_t b = readU8();
    uint64_t c = readU8();
    uint64_t d = readU8();
    uint64_t e = readU8();
    uint64_t f = readU8();
    uint64_t g = readU8();
    uint64_t h = readU8();
    return (uint64_t)(a | (b << 8) | (c << 16) | (d << 24)
            | (e << 32) | (f << 40) | (g << 48) | (h << 56));
}

