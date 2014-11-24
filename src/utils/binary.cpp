/*!
 * \file src/utils/binary.cpp
 * \brief Binary file reading utilities
 *
 * \author xythobuz
 */

#include "global.h"
#include "utils/binary.h"

BinaryFile::BinaryFile(const char* f) {
    int r = open(f);
    if (r != 0)
        throw r;
}

BinaryFile::~BinaryFile() {
    if (file.is_open())
        file.close();
}

int BinaryFile::open(const char* f) {
    if (file.is_open()) {
        return 1;
    } else {
        if (f == nullptr)
            return 0;
        file.open(f, std::ios_base::in | std::ios_base::binary);
        return (file ? 0 : 1);
    }
}

long long BinaryFile::tell() {
    assert(file.is_open());
    return file.tellg();
}

void BinaryFile::seek(long long pos) {
    assert(file.is_open());
    file.seekg(pos);
}

uint8_t BinaryFile::readU8() {
    assert(file.is_open());
    assert(file.good());
    uint8_t ret;
    char* c = reinterpret_cast<char*>(&ret);
    file.read(c, 1);
    return ret;
}

uint16_t BinaryFile::readU16() {
    assert(file.is_open());
    assert(file.good());
    uint8_t a = readU8();
    uint8_t b = readU8();
    return ((uint16_t)a | (uint16_t)(b << 8));
}

uint32_t BinaryFile::readU32() {
    assert(file.is_open());
    assert(file.good());
    uint16_t a = readU16();
    uint16_t b = readU16();
    return ((uint32_t)a | (uint32_t)(b << 16));
}

uint64_t BinaryFile::readU64() {
    assert(file.is_open());
    assert(file.good());
    uint32_t a = readU32();
    uint32_t b = readU32();
    return ((uint64_t)a | ((uint64_t)b << 32));
}

float BinaryFile::readFloat() {
    assert(file.is_open());
    assert(file.good());
    uint32_t val = readU32();
    char* a = reinterpret_cast<char*>(&val);

    float ret;
    char* b = reinterpret_cast<char*>(&ret);

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

    void swapByteOrder(char* d, unsigned int n) {
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
    assert(file.is_open());
    assert(file.good());
    int8_t ret;
    char* p = reinterpret_cast<char*>(&ret);
    file.read(p, sizeof(ret));
    return ret;
}

int16_t BinaryFile::read16() {
    assert(file.is_open());
    assert(file.good());
    int16_t ret;
    char* p = reinterpret_cast<char*>(&ret);
    file.read(p, sizeof(ret));
    swapByteOrder(p, 2);
    return ret;
}

int32_t BinaryFile::read32() {
    assert(file.is_open());
    assert(file.good());
    int32_t ret;
    char* p = reinterpret_cast<char*>(&ret);
    file.read(p, sizeof(ret));
    swapByteOrder(p, 4);
    return ret;
}

int64_t BinaryFile::read64() {
    assert(file.is_open());
    assert(file.good());
    int64_t ret;
    char* p = reinterpret_cast<char*>(&ret);
    file.read(p, sizeof(ret));
    swapByteOrder(p, 8);
    return ret;
}

// ----------------------------------------------------------------------------

BinaryMemory::BinaryMemory(char* d) : data(nullptr), offset(0) {
    int r = open(d);
    if (r != 0)
        throw r;
}

int BinaryMemory::open(char* d) {
    if (data != nullptr)
        return 1;

    if (d != nullptr) {
        data = d;
        offset = 0;
    }

    return 0;
}

long long BinaryMemory::tell() {
    assert(offset >= 0);
    return offset;
}

void BinaryMemory::seek(long long pos) {
    assert(pos >= 0);
    offset = pos;
}

void BinaryMemory::read(char* d, int c) {
    for (int i = 0; i < c; i++) {
        d[i] = data[offset + i];
    }
    offset += c;
}

uint8_t BinaryMemory::readU8() {
    assert(offset >= 0);
    uint8_t ret;
    char* c = reinterpret_cast<char*>(&ret);
    read(c, 1);
    return ret;
}

uint16_t BinaryMemory::readU16() {
    assert(offset >= 0);
    uint8_t a = readU8();
    uint8_t b = readU8();
    return ((uint16_t)a | (uint16_t)(b << 8));
}

uint32_t BinaryMemory::readU32() {
    assert(offset >= 0);
    uint16_t a = readU16();
    uint16_t b = readU16();
    return ((uint32_t)a | (uint32_t)(b << 16));
}

uint64_t BinaryMemory::readU64() {
    assert(offset >= 0);
    uint32_t a = readU32();
    uint32_t b = readU32();
    return ((uint64_t)a | (uint64_t)(b << 32));
}

float BinaryMemory::readFloat() {
    assert(offset >= 0);
    uint32_t val = readU32();
    char* a = reinterpret_cast<char*>(&val);

    float ret;
    char* b = reinterpret_cast<char*>(&ret);

    for (int i = 0; i < 4; i++)
        b[i] = a[i];

    return ret;
}

int8_t BinaryMemory::read8() {
    assert(offset >= 0);
    int8_t ret;
    char* p = reinterpret_cast<char*>(&ret);
    read(p, sizeof(ret));
    return ret;
}

int16_t BinaryMemory::read16() {
    assert(offset >= 0);
    int16_t ret;
    char* p = reinterpret_cast<char*>(&ret);
    read(p, sizeof(ret));
    swapByteOrder(p, 2);
    return ret;
}

int32_t BinaryMemory::read32() {
    assert(offset >= 0);
    int32_t ret;
    char* p = reinterpret_cast<char*>(&ret);
    read(p, sizeof(ret));
    swapByteOrder(p, 4);
    return ret;
}

int64_t BinaryMemory::read64() {
    assert(offset >= 0);
    assert(offset >= 0);
    int64_t ret;
    char* p = reinterpret_cast<char*>(&ret);
    read(p, sizeof(ret));
    swapByteOrder(p, 8);
    return ret;
}

