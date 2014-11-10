/*!
 * \file src/Script.cpp
 * \brief Game script loader
 *
 * \author xythobuz
 */

#include "global.h"
#include "Script.h"

Script::Script() : puzzles(4), pickups(2), keys(4) {
    version = 0;
    firstOption = 0;
    titleReplace = 0;
    onDeathDemoMode = 0;
    onDeathInGame = 0;
    noInputTime = 0;
    onDemoInterrupt = 0;
    onDemoEnd = 0;
    numLevels = 0;
    numPictures = 0;
    numTitles = 0;
    numFMVs = 0;
    numCutscenes = 0;
    numDemos = 0;
    titleTrack = 0;
    singleLevel = 0;
    flags = 0;
    cypherCode = 0;
    language = 0;
    secretTrack = 0;
    numPCStrings = 41;
    numGameStrings = 0;
}

Script::~Script() {

}

int Script::load(const char* file) {
    BinaryFile f;

    if (f.open(file) != 0)
        return 1;

    version = f.readU32();

    char desc[256];
    for (int i = 0; i < 256; i++)
        desc[i] = f.read8();
    description = desc;

    uint16_t gameflowSize = f.readU16();
    assertEqual(gameflowSize, 128);

    firstOption = f.readU32();
    titleReplace = f.read32();
    onDeathDemoMode = f.readU32();
    onDeathInGame = f.readU32();
    noInputTime = f.readU32(); // Scaled *100 in TR3
    onDemoInterrupt = f.readU32();
    onDemoEnd = f.readU32();

    // Filler 1 (36 bytes)
    f.seek(f.tell() + 36);

    numLevels = f.readU16();
    numPictures = f.readU16();
    numTitles = f.readU16();
    numFMVs = f.readU16();
    numCutscenes = f.readU16();
    numDemos = f.readU16();
    titleTrack = f.readU16();
    singleLevel = f.read16();

    // Filler 2 (32 bytes)
    f.seek(f.tell() + 32);

    flags = f.readU16();

    // Filler 3 (6 bytes)
    f.seek(f.tell() + 6);

    cypherCode = f.readU8();
    language = f.readU8();
    secretTrack = f.readU16(); // Zero in TR3, Part of filler or real number?

    // Filler 4 (4 bytes)
    f.seek(f.tell() + 4);

    // Strings
    readStringPackage(f, levelNames, numLevels);
    readStringPackage(f, pictureFilenames, numPictures);
    readStringPackage(f, titleFilenames, numTitles);
    readStringPackage(f, fmvFilenames, numFMVs);
    readStringPackage(f, levelFilenames, numLevels);
    readStringPackage(f, cutsceneFilenames, numCutscenes);

    // Level Scripts
    readScriptPackage(f, script, numLevels + 1);

    numGameStrings = f.readU16();

    // More strings...
    readStringPackage(f, gameStrings, numGameStrings);
    readStringPackage(f, pcStrings, numPCStrings);
    readStringPackage(f, puzzles[0], numLevels);
    readStringPackage(f, puzzles[1], numLevels);
    readStringPackage(f, puzzles[2], numLevels);
    readStringPackage(f, puzzles[3], numLevels);
    readStringPackage(f, pickups[0], numLevels);
    readStringPackage(f, pickups[1], numLevels);
    readStringPackage(f, keys[0], numLevels);
    readStringPackage(f, keys[1], numLevels);
    readStringPackage(f, keys[2], numLevels);
    readStringPackage(f, keys[3], numLevels);

    return 0;
}

void Script::readStringPackage(BinaryFile& f, std::vector<std::string>& v, unsigned int n) {
    uint16_t* offset = new uint16_t[n];
    for (unsigned int i = 0; i < n; i++)
        offset[i] = f.readU16();

    uint16_t numBytes = f.readU16();

    char* list = new char[numBytes];
    for (uint16_t i = 0; i < numBytes; i++) {
        list[i] = f.read8();
        if (flags & S_UseSecurityTag) {
            list[i] ^= cypherCode;
        }
    }

    for (unsigned int i = 0; i < n; i++) {
        std::string tmp(list + offset[i]);
        v.push_back(tmp);
    }

    delete [] list;
    delete [] offset;
}

void Script::readScriptPackage(BinaryFile& f, std::vector<std::vector<uint16_t>>& v,
                               unsigned int n) {
    uint16_t* offset = new uint16_t[n];
    for (unsigned int i = 0; i < n; i++) {
        offset[i] = f.readU16();
        assertEqual(offset[i] % 2, 0);
    }

    uint16_t numBytes = f.readU16();
    assertEqual(numBytes % 2, 0); // 16 bit opcodes and operands

    uint16_t* list = new uint16_t[(numBytes + 6) / 2];
    for (uint16_t i = 0; i < (numBytes / 2); i++) {
        list[i] = f.readU16();
    }

    // TR2 for PC and PSX has 6 "filler bytes" hex 13 00 14 00 15 00
    // (for TR3 for PSX, the filler is hex 15 00 16 00 17 00 instead)
    // at the end of the script block. We need to skip these...
    uint16_t hack[3];
    hack[0] = f.readU16();
    hack[1] = f.readU16();
    hack[2] = f.readU16();
    if (((hack[0] == 19) && (hack[1] == 20) && (hack[2] == 21))
        || ((hack[0] == 21) && (hack[1] == 22) && (hack[2] == 23))) {
        list[numBytes / 2] = hack[0];
        list[(numBytes / 2) + 1] = hack[1];
        list[(numBytes / 2) + 2] = hack[2];
    } else {
        f.seek(f.tell() - 6);
    }

    // TR2 for PSX has 64 bytes with unknown content (not zero!) here,
    // TR3 for PSX has 40 bytes. We try to identify and skip them...
    // This is also currently used to set the platform specific string count
    hack[0] = f.readU16();
    hack[1] = f.readU16();
    hack[2] = f.readU16();
    if ((hack[0] == 1) && (hack[1] == 0) && (hack[2] == 864)) {
        f.seek(f.tell() + 58);
        numPCStrings = 80; // TR2 has 80 PSX Strings
    } else if ((hack[0] == 1) && (hack[1] == 0) && (hack[2] == 817)) {
        f.seek(f.tell() + 34);
        numPCStrings = 80; // TR3 also has 80 PSX Strings
    } else {
        f.seek(f.tell() - 6);
        numPCStrings = 41;
    }

    for (unsigned int i = 0; i < n; i++) {
        unsigned int end = offset[i] / 2;

        // We need to detect the OP_END opcode marking the end of a
        // script sequence (like the '\0' for the strings).
        // However, the numerical value of OP_END could also be used
        // as an operand for another opcode, so we have to check for this
        bool readingOperand = false;
        while (readingOperand || (list[end] != OP_END)) {
            if (readingOperand) {
                readingOperand = false;
                end++;
            } else {
                if (opcodeHasOperand[list[end]]) {
                    readingOperand = true;
                }
                end++;
            }
        }
        end++;

        std::vector<uint16_t> tmp;
        for (unsigned int a = (offset[i] / 2); a < end; a++)
            tmp.push_back(list[a]);

        v.push_back(tmp);
    }

    delete [] list;
    delete [] offset;
}

unsigned int Script::levelCount() {
    return numLevels;
}

std::string Script::getLevelName(unsigned int i) {
    assert(i < numLevels);
    return levelNames.at(i);
}

std::string Script::getLevelFilename(unsigned int i) {
    assert(i < numLevels);
    return levelFilenames.at(i);
}

unsigned int Script::pictureCount() {
    return numPictures;
}

std::string Script::getPictureFilename(unsigned int i) {
    assert(i < numPictures);
    return pictureFilenames.at(i);
}

unsigned int Script::cutsceneCount() {
    return numCutscenes;
}

std::string Script::getCutsceneFilename(unsigned int i) {
    assert(i < numCutscenes);
    return cutsceneFilenames.at(i);
}

unsigned int Script::titleCount() {
    return numTitles;
}

std::string Script::getTitleFilename(unsigned int i) {
    assert(i < numTitles);
    return titleFilenames.at(i);
}

unsigned int Script::videoCount() {
    return numFMVs;
}

std::string Script::getVideoFilename(unsigned int i) {
    assert(i < numFMVs);
    return fmvFilenames.at(i);
}

unsigned int Script::gameStringCount() {
    return numGameStrings;
}

std::string Script::getGameString(unsigned int i) {
    assert(i < numGameStrings);
    return gameStrings.at(i);
}

unsigned int Script::pcStringCount() {
    return numPCStrings;
}

std::string Script::getPCString(unsigned int i) {
    assert(i < numPCStrings);
    return pcStrings.at(i);
}

std::string Script::getPuzzleString(unsigned int i, unsigned int j) {
    assert(i < 4);
    assert(j < numLevels);
    return puzzles.at(i).at(j);
}

std::string Script::getPickupString(unsigned int i, unsigned int j) {
    assert(i < 2);
    assert(j < numLevels);
    return pickups.at(i).at(j);
}

std::string Script::getKeyString(unsigned int i, unsigned int j) {
    assert(i < 4);
    assert(j < numLevels);
    return keys.at(i).at(j);
}

void Script::registerScriptHandler(ScriptOpCode op, std::function<int (bool, uint16_t)> func) {
    assert(op < OP_UNKNOWN);
    scriptHandlers[op] = func;
}

int Script::runScript(unsigned int level) {
    assert(level < (numLevels + 1));
    std::vector<uint16_t> s = script.at(level);

    for (unsigned int i = 0; i < s.size(); i++) {
        uint16_t opcode = s.at(i);
        if (opcode >= OP_UNKNOWN) {
            return 1;
        }

        uint16_t operand = 0;
        if (opcodeHasOperand[opcode]) {
            if ((i + 1) >= s.size())
                return 2; // Can't read operand!

            operand = s.at(++i);
        }

        if (scriptHandlers[opcode]) {
            int error = scriptHandlers[opcode](opcodeHasOperand[opcode], operand);
            if (error != 0)
                return error;
        } else {
            return 3;
        }
    }

    return 0;
}

