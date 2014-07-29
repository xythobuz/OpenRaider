/*!
 * \file src/utils/binary.cpp
 * \brief Binary file reading utilities
 *
 * \author xythobuz
 */

#include "utils/binary.h"

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
    file.seekg(pos);
}

uint8_t BinaryFile::readU8() {
    uint8_t ret;
    char *c = reinterpret_cast<char *>(&ret);
    file.read(c, 1);
    return ret;
}

uint16_t BinaryFile::readU16() {
    uint8_t a = readU8();
    uint8_t b = readU8();
    return ((uint16_t)a | (uint16_t)(b << 8));
}

uint32_t BinaryFile::readU32() {
    uint16_t a = readU16();
    uint16_t b = readU16();
    return ((uint32_t)a | (uint32_t)(b << 16));
}

uint64_t BinaryFile::readU64() {
    uint32_t a = readU32();
    uint32_t b = readU32();
    return ((uint64_t)a | ((uint64_t)b << 32));
}

float BinaryFile::readFloat() {
    uint32_t val = readU32();
    char *a = reinterpret_cast<char *>(&val);

    float ret;
    char *b = reinterpret_cast<char *>(&ret);

    for (int i = 0; i < 4; i++)
        b[i] = a[i];

    return ret;
}

namespace {
    /*! \fixme Left-Shift with signed integer is undefined!
     *  So we can't use the same method as for unsigned integers.
     *  Is there a portable way to read multi-byte signed integers,
     *  without having to detect the endianness at run-time?
     */
    const int bigendiandetection = 1;
#define ISBIGENDIAN() ((*(char *)&bigendiandetection) == 0)

    void swapByteOrder(char *d, unsigned int n) {
        if (ISBIGENDIAN()) {
            for (unsigned int i = 0; i < (n / 2); i++) {
                char tmp = d[i];
                d[i] = d[n - i - 1];
                d[n - i - 1] = tmp;
            }
        }
    }
}

int8_t BinaryFile::read8() {
    int8_t ret;
    char *p = reinterpret_cast<char *>(&ret);
    file.read(p, sizeof(ret));
    return ret;
}

int16_t BinaryFile::read16() {
    int16_t ret;
    char *p = reinterpret_cast<char *>(&ret);
    file.read(p, sizeof(ret));
    swapByteOrder(p, 2);
    return ret;
}

int32_t BinaryFile::read32() {
    int32_t ret;
    char *p = reinterpret_cast<char *>(&ret);
    file.read(p, sizeof(ret));
    swapByteOrder(p, 4);
    return ret;
}

int64_t BinaryFile::read64() {
    int64_t ret;
    char *p = reinterpret_cast<char *>(&ret);
    file.read(p, sizeof(ret));
    swapByteOrder(p, 8);
    return ret;
}

