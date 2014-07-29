/*!
 * \file test/binary.cpp
 * \brief Binary Unit Test
 *
 * \author xythobuz
 */

#include <cstdio>
#include <iostream>
#include <fstream>

#include "utils/binary.h"


namespace {
    const char testData[] = {
        // Unsigned Integers
        -1,                     // u 8 -> 255
        -1, -1,                 // u16 -> 65535
        42, 0, 0, 0,            // u32 -> 42
        1, 0, 0, 0, 0, 0, 0, 0, // u64 -> 1

        // Signed Integers
        -1,                     //  8 -> -1
        92, -2,                 // 16 -> -420
        102, -3, -1, -1,        // 32 -> -666
        -5, -1, -1, -1,
        -1, -1, -1, -1          // 64 -> -5
    };

    float f1 = 3.1415926f;
    float f2 = 42.23f;

    void fillFile(const char *name) {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(testData, sizeof(testData) / sizeof(testData[0]));
        file.write(reinterpret_cast<char *>(&f1), sizeof(f1));
        file.write(reinterpret_cast<char *>(&f2), sizeof(f2));
    }

    int assertImplementation(const char *exp, const char *file, int line) {
        std::cout << "Failed: \"" << exp << "\" in " << file << " at " << line << std::endl;
        return 1;
    }

#define assert(x) if (!(x)) { return assertImplementation(#x, __FILE__, __LINE__); }

    int test(const char *name) {
        BinaryFile file(name);

        uint8_t a = file.readU8();
        uint16_t b = file.readU16();
        uint32_t c = file.readU32();
        uint64_t d = file.readU64();

        int8_t e = file.read8();
        int16_t f = file.read16();
        int32_t g = file.read32();
        int64_t h = file.read64();

        float i = file.readFloat();
        float j = file.readFloat();

        assert(a == 255);
        assert(b == 65535);
        assert(c == 42);
        assert(d == 1);

        assert(e == -1);
        assert(f == -420);
        assert(g == -666);
        assert(h == -5);

        assert(i == f1);
        assert(j == f2);

        return 0;
    }
}

int main(int argc, char *argv[]) {
    char *tmpFile = tmpnam(NULL);
    std::cout << "Temporary test-file: " << tmpFile << std::endl;

    fillFile(tmpFile);
    int error = test(tmpFile);
    remove(tmpFile);

    return error;
}

