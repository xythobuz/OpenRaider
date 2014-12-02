/*!
 * \file src/utils/binary.cpp
 * \brief Binary file reading utilities
 *
 * \author xythobuz
 */

#include <sstream>

#include "global.h"
#include "Exception.h"
#include "utils/binary.h"

BinaryReader::~BinaryReader() {
}

uint8_t BinaryReader::readU8() {
    uint8_t ret;
    char* c = reinterpret_cast<char*>(&ret);
    read(c, 1);
    return ret;
}

uint16_t BinaryReader::readU16() {
    uint16_t a = readU8();
    uint16_t b = readU8();
    return (a | (b << 8));
}

uint32_t BinaryReader::readU32() {
    uint32_t a = readU16();
    uint32_t b = readU16();
    return (a | (b << 16));
}

uint64_t BinaryReader::readU64() {
    uint64_t a = readU32();
    uint64_t b = readU32();
    return (a | (b << 32));
}

float BinaryReader::readFloat() {
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
#define ISBIGENDIAN() ((*reinterpret_cast<const char *>(&bigendiandetection)) == 0)

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

int8_t BinaryReader::read8() {
    int8_t ret;
    char* p = reinterpret_cast<char*>(&ret);
    read(p, sizeof(ret));
    return ret;
}

int16_t BinaryReader::read16() {
    int16_t ret;
    char* p = reinterpret_cast<char*>(&ret);
    read(p, sizeof(ret));
    swapByteOrder(p, 2);
    return ret;
}

int32_t BinaryReader::read32() {
    int32_t ret;
    char* p = reinterpret_cast<char*>(&ret);
    read(p, sizeof(ret));
    swapByteOrder(p, 4);
    return ret;
}

int64_t BinaryReader::read64() {
    int64_t ret;
    char* p = reinterpret_cast<char*>(&ret);
    read(p, sizeof(ret));
    swapByteOrder(p, 8);
    return ret;
}

// ----------------------------------------------------------------------------

BinaryFile::BinaryFile(std::string f) {
    int r = open(f);
    if (r != 0)
        throw r;
}

BinaryFile::~BinaryFile() {
    if (file.is_open())
        file.close();
}

int BinaryFile::open(std::string f) {
    if (file.is_open()) {
        return 1;
    } else {
        if (f == "")
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

bool BinaryFile::eof() {
    file.peek();
    return file.eof();
}

void BinaryFile::read(char* d, int c) {
    assert(file.is_open());
    assert(file.good());
    file.read(d, c);
}

// ----------------------------------------------------------------------------

BinaryMemory::BinaryMemory(const char* d, long long m) : data(nullptr), offset(0), max(-1) {
    int r = open(d, m);
    if (r != 0)
        throw r;
}

BinaryMemory::~BinaryMemory() {
}

int BinaryMemory::open(const char* d, long long m) {
    if (data != nullptr)
        return 1;

    if (d != nullptr) {
        data = d;
        offset = 0;
        max = m;
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

bool BinaryMemory::eof() {
    return (offset > max);
}

void BinaryMemory::read(char* d, int c) {
    assert(offset >= 0);
    assert(c > 0);
    if ((offset + c) > max) {
        std::ostringstream ss;
        ss << "BinaryMemory read out of bounds ("
           << offset << " + " << c << " > " << max << ")";
        throw new Exception(ss.str());
    }

    for (int i = 0; i < c; i++) {
        d[i] = data[offset + i];
    }
    offset += c;
}

