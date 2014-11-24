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
    BinaryFile(const char* f = nullptr);
    ~BinaryFile();

    int open(const char* f = nullptr);

    long long tell();
    void seek(long long pos = 0);

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

class BinaryMemory {
  public:
    BinaryMemory(char* d = nullptr);

    int open(char* d = nullptr);

    long long tell();
    void seek(long long pos = 0);

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
    void read(char* d, int c);

    char* data;
    long long offset;
};

#endif

