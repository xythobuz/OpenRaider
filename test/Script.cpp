/*!
 * \file test/Script.cpp
 * \brief Game script loader unit test
 *
 * \author xythobuz
 */

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <zlib.h>

#include "global.h"
#include "Script.h"
#include "ScriptTest.h"

#define printStrings(cnt, acc, name) { \
    std::cout << name << " (" << cnt << ")" << std::endl; \
    for (unsigned int i = 0; i < cnt; i++) { \
        std::cout << "    " << acc(i) << std::endl; \
    } \
    std::cout << std::endl; \
}

#define printStrings2D(c, cnt, acc, name) { \
    std::cout << name << " (" << c << "*" << cnt << ")" << std::endl; \
    for (unsigned int a = 0; a < cnt; a++) { \
        std::cout << "    "; \
        for (unsigned int i = 0; i < c; i++) { \
            std::cout << acc(i, a); \
            if (i < (c - 1)) \
                std::cout << " | "; \
        } \
        std::cout << std::endl; \
    } \
    std::cout << std::endl; \
}

#define registerLambda(x, y) { \
    s.registerScriptHandler(x, [](bool hasOperand, uint16_t operand) { \
        std::cout << "\t" << y; \
        if (hasOperand) \
            std::cout << " (" << operand << ")"; \
        std::cout << std::endl; \
        return 0; \
    }); \
}

namespace {
    int printDataScript(Script& s, bool strings) {
        if (strings) {
            printStrings(s.levelCount(), s.getLevelName, "Level Names");
            printStrings(s.levelCount(), s.getLevelFilename, "Level Filenames");
            printStrings(s.pictureCount(), s.getPictureFilename, "Picture Filenames");
            printStrings(s.cutsceneCount(), s.getCutsceneFilename, "Cutscenes");
            printStrings(s.titleCount(), s.getTitleFilename, "Titles");
            printStrings(s.videoCount(), s.getVideoFilename, "Videos");
            printStrings(s.gameStringCount(), s.getGameString, "Game Strings");
            printStrings(s.pcStringCount(), s.getPCString, "PC Strings");

            printStrings2D(4, s.levelCount(), s.getPuzzleString, "Puzzles");
            printStrings2D(2, s.levelCount(), s.getPickupString, "Pickups");
            printStrings2D(4, s.levelCount(), s.getKeyString, "Keys");
        } else {
            registerLambda(Script::OP_PICTURE, "Picture");
            registerLambda(Script::OP_PSX_TRACK, "PSX-Track");
            registerLambda(Script::OP_PSX_FMV, "PSX-FMV");
            registerLambda(Script::OP_FMV, "Show FMV");
            registerLambda(Script::OP_GAME, "Load level");
            registerLambda(Script::OP_CUT, "Cutscene");
            registerLambda(Script::OP_COMPLETE, "Level finished");
            registerLambda(Script::OP_DEMO, "Demo sequence");
            registerLambda(Script::OP_PSX_DEMO, "PSX-Demo");
            registerLambda(Script::OP_END, "End of script");
            registerLambda(Script::OP_TRACK, "Sound Track");
            registerLambda(Script::OP_SUNSET, "Sunset");
            registerLambda(Script::OP_LOAD_PIC, "Load picture");
            registerLambda(Script::OP_DEADLY_WATER, "Deadly water");
            registerLambda(Script::OP_REMOVE_WEAPONS, "Remove weapons");
            registerLambda(Script::OP_GAMECOMPLETE, "End of game!");
            registerLambda(Script::OP_CUTANGLE, "Cutscene angle");
            registerLambda(Script::OP_NOFLOOR, "No floor, fall death");
            registerLambda(Script::OP_STARTINV, "Inventory/Bonus");
            registerLambda(Script::OP_STARTANIM, "Start animation");
            registerLambda(Script::OP_SECRETS, "Secrets");
            registerLambda(Script::OP_KILLTOCOMPLETE, "Kill to complete level");
            registerLambda(Script::OP_REMOVE_AMMO, "Remove ammo");

            for (unsigned int i = 0; i < (s.levelCount() + 1); i++) {
                if (i == 0)
                    std::cout << "Script for Title:" << std::endl;
                else
                    std::cout << "Script for \"" << s.getLevelName(i - 1) << "\" (" << i - 1 << "):" << std::endl;
                int error = s.runScript(i);
                if (error != 0) {
                    std::cout << "Returned " << error << "..." << std::endl;
                    return error;
                }
                std::cout << std::endl;
            }
        }

        return 0;
    }

    int test(const char* file, unsigned int n) {
        Script s;

        std::cout << "Testing " << testDescription[n] << std::endl;

        if (s.load(file) != 0) {
            std::cout << "Could not open file " << file << std::endl;
            return 1;
        }

        if (s.gameStringCount() != testExpectedGameStringCount[n]) {
            std::cout << "Game String Count " << s.gameStringCount() << " != " << testExpectedGameStringCount[n]
                      << std::endl;
            return 2;
        }

        if (s.pcStringCount() != testExpectedPlatformStringCount[n]) {
            std::cout << "Platform String Count " << s.pcStringCount() << " != " <<
                      testExpectedPlatformStringCount[n] << std::endl;
            return 3;
        }

        std::cout << "Success!" << std::endl << std::endl;
        return 0;
    }

    int readPayloadChunk(const unsigned char* data, unsigned int size, const char* file) {
        static const unsigned int bufferSize = 16384; // 16K should be enough for everybody :)
        unsigned char buffer[bufferSize];

        // Initialize decompression
        z_stream stream;
        stream.zalloc = Z_NULL;
        stream.zfree =  Z_NULL;
        stream.opaque = Z_NULL;
        int error = inflateInit(&stream);
        if (error != Z_OK) {
            std::cout << "inflateInit() Error " << error << std::endl;
            return 1;
        }

        // Inflate data in one go
        stream.avail_in = size;
        stream.next_in = const_cast<unsigned char*>(data);
        stream.avail_out = bufferSize;
        stream.next_out = buffer;
        error = inflate(&stream, Z_FINISH);
        if (error != Z_STREAM_END) {
            std::cout << "inflate() Error " << error << std::endl;
            return 2;
        }
        inflateEnd(&stream);

        // Write buffer to file
        std::ofstream s(file, std::ios_base::out | std::ios_base::binary);
        s.write(reinterpret_cast<const char*>(buffer), bufferSize - stream.avail_out);

        return 0;
    }

    int runForPayload(unsigned int n, bool print, bool printData) {
        assert(n < testPayloadCount);
        // Get temp file name
        char tmpFile[] = "/tmp/openraider_unit_test_0";
        FILE* f;
        while ((f = fopen(tmpFile, "r")) != NULL) {
            fclose(f);
            tmpFile[26]++;
        }

        std::cout << "Temporary test file: " << tmpFile << std::endl;

        int error = readPayloadChunk(testPayloads[n], testSizes[n], tmpFile);
        if (error == 0) {
            if (print) {
                Script s;
                error = s.load(tmpFile);
                if (error == 0)
                    error = printDataScript(s, printData);
                else
                    std::cout << "Error loading script!" << std::endl;
            } else {
                error = test(tmpFile, n);
            }
        }

        remove(tmpFile);
        return error;
    }
}

int main(int argc, char* argv[]) {
    bool printHelp = false;
    bool print = false;
    bool printData = true;
    int whichFile = -1;

    if (argc == 3) {
        if ((strcmp(argv[1], "--printData") == 0)
            || (strcmp(argv[1], "--printScript") == 0)) {
            print = true;
            if (strcmp(argv[1], "--printScript") == 0) {
                printData = false;
            }
            assert(testPayloadCount < 10);
            if ((argv[2][0] >= '0')
                && (static_cast<unsigned int>(argv[2][0]) <= (testPayloadCount + '0'))) {
                whichFile = argv[2][0] - '0';
            }
        } else {
            printHelp = true;
        }
    } else if (argc != 1) {
        printHelp = true;
    }

    if (printHelp) {
        std::cout << "Usage:" << std::endl;
        std::cout << "\t" << argv[0] << " [--printData | --printScript] [N | /path]" << std::endl;
        return 1;
    }

    if (print) {
        // Print single script
        if (whichFile == -1) {
            // From given path
            Script s;
            assertEqual(s.load(argv[2]), 0);
            return printDataScript(s, printData);
        } else {
            // From payload
            return runForPayload(static_cast<unsigned int>(whichFile), true, printData);
        }
    } else {
        // Run test on all scripts in payload
        for (unsigned int i = 0; i < testPayloadCount; i++) {
            int error = runForPayload(i, false, false);
            if (error != 0)
                return error;
        }
        return 0;
    }
}

