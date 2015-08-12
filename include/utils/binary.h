/*!
 * \file include/utils/binary.h
 * \brief Binary file reading utilities
 *
 * \author xythobuz
 */

#ifndef _UTILS_BINARY_H_
#define _UTILS_BINARY_H_

#include <fstream>
#include <string>

class BinaryReader {
  public:
    virtual ~BinaryReader();

    virtual long long tell() = 0;
    virtual void seek(long long pos = 0) = 0;
    virtual bool eof() = 0;

    virtual int8_t read8();
    virtual uint8_t readU8();

    virtual int16_t read16();
    virtual uint16_t readU16();

    virtual int32_t read32();
    virtual uint32_t readU32();

    virtual int64_t read64();
    virtual uint64_t readU64();

    virtual float readFloat();

  private:
    virtual void read(char* d, int c) = 0;
};

class BinaryFile : public BinaryReader {
  public:
    explicit BinaryFile(std::string f = "");
    virtual ~BinaryFile();

    int open(std::string f = "");

    virtual long long tell();
    virtual void seek(long long pos = 0);
    virtual bool eof();

  private:
    virtual void read(char* d, int c);

    std::ifstream file;
};

class BinaryMemory : public BinaryReader {
  public:
    BinaryMemory(const char* d = nullptr, long long max = -1);
    virtual ~BinaryMemory();

    int open(const char* d = nullptr, long long max = -1);

    virtual long long tell();
    virtual void seek(long long pos = 0);
    virtual bool eof();

  private:
    virtual void read(char* d, int c);

    const char* data;
    long long offset;
    long long max;
};

#endif

