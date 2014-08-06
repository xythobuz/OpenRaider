/*!
 * \file include/utils/binary.h
 * \brief Binary file reading utilities
 *
 * \author xythobuz
 */

#ifndef _UTILS_BINARY_H_
#define _UTILS_BINARY_H_

#include <fstream>

class BinaryFile {
public:

    ~BinaryFile();

    int open(const char *f);

    long long tell();
    void seek(long long pos);

    int8_t read8();
    uint8_t readU8();

    int16_t read16();
    uint16_t readU16();

    int32_t read32();
    uint32_t readU32();

    int64_t read64();
    uint64_t readU64();

    float readFloat();

private:
    std::ifstream file;
};

#endif

