/*!
 * \file test/binary.cpp
 * \brief Binary Unit Test
 *
 * \author xythobuz
 */

#include <cstdio>
#include <iostream>
#include <fstream>

#include "global.h"
#include "utils/binary.h"


namespace {
    const char testData[] = {
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

    float f1 = 3.1415926f;
    float f2 = 42.23f;

    void fillFile(const char *name) {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(testData, sizeof(testData) / sizeof(testData[0]));
        file.write(reinterpret_cast<char *>(&f1), sizeof(f1));
        file.write(reinterpret_cast<char *>(&f2), sizeof(f2));
    }

    //! \fixme Don't use assert(), as it will prevent removing the created test file!
    int test(const char *name) {
        BinaryFile file;
        assertEqual(file.open(name), 0);

        assertEqual(file.readU8(), 255);
        assertEqual(file.readU16(), 65535);
        assertEqual(file.readU32(), 707406378);
        assertEqual(file.readU64(), 1099511627775);

        assertEqual(file.read8(), -1);
        assertEqual(file.read16(), -420);
        assertEqual(file.read32(), -666);
        assertEqual(file.read64(), 4774451407313060418);

        std::cout << "This test will fail on big-endian machines!" << std::endl;
        assertEqual(file.readFloat(), f1);
        assertEqual(file.readFloat(), f2);

        assertEqual(file.tell(), 38);

        return 0;
    }
}

int main() {
    char tmpFile[] = "/tmp/openraider_unit_test_0";
    FILE *f;
    while ((f = fopen(tmpFile, "r")) != NULL) {
        fclose(f);
        tmpFile[26]++;
    }

    fillFile(tmpFile);
    int error = test(tmpFile);
    remove(tmpFile);

    return error;
}

