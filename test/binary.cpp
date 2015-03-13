/*!
 * \file test/binary.cpp
 * \brief Binary File Reading Unit Test
 *
 * \author xythobuz
 */

#include <cstdio>
#include <iostream>
#include <fstream>

#include "global.h"
#include "utils/binary.h"

const static char testData[] = {
    // Unsigned Integers
    -1,
    -1, -1,
    42, 42, 42, 42,
    -1, -1, -1, -1, -1, 0, 0, 0,

    // Signed Integers
    -1,
    92, -2,
    102, -3, -1, -1,
    66, 66, 66, 66, 66, 66, 66, 66
};

static float f1 = 3.1415926f;
static float f2 = 42.23f;

static void fillFile(const char* name) {
    std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
    file.write(testData, sizeof(testData) / sizeof(testData[0]));
    file.write(reinterpret_cast<char*>(&f1), sizeof(f1));
    file.write(reinterpret_cast<char*>(&f2), sizeof(f2));
}

static int test(const char* name) {
    BinaryFile file;
    if (file.open(name) != 0) {
        std::cout << "Error opening file " << name << std::endl;
        return 1;
    }

    if (file.readU8() != 255) {
        std::cout << "Error reading U8!" << std::endl;
        return 2;
    }

    if (file.readU16() != 65535) {
        std::cout << "Error reading U16!" << std::endl;
        return 3;
    }

    if (file.readU32() != 707406378) {
        std::cout << "Error reading U32!" << std::endl;
        return 4;
    }

    if (file.readU64() != 1099511627775) {
        std::cout << "Error reading U64!" << std::endl;
        return 5;
    }

    if (file.read8() != -1) {
        std::cout << "Error reading 8!" << std::endl;
        return 6;
    }

    if (file.read16() != -420) {
        std::cout << "Error reading 16!" << std::endl;
        return 7;
    }

    if (file.read32() != -666) {
        std::cout << "Error reading 32!" << std::endl;
        return 8;
    }

    if (file.read64() != 4774451407313060418) {
        std::cout << "Error reading 64!" << std::endl;
        return 9;
    }

    std::cout << "This test will fail on big-endian machines!" << std::endl;

    if (file.readFloat() != f1) {
        std::cout << "Error reading float1!" << std::endl;
        return 10;
    }

    if (file.readFloat() != f2) {
        std::cout << "Error reading float2!" << std::endl;
        return 11;
    }

    if (file.tell() != 38) {
        std::cout << "Error, file position wrong!" << std::endl;
        return 12;
    }

    return 0;
}

int main() {
    char tmpFile[] = "/tmp/openraider_unit_test_0";
    FILE* f;
    while ((f = fopen(tmpFile, "r")) != NULL) {
        fclose(f);
        tmpFile[26]++;
    }

    fillFile(tmpFile);
    int error = test(tmpFile);
    remove(tmpFile);

    return error;
}

