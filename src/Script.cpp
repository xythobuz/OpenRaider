/*!
 * \file src/Script.cpp
 * \brief Game script loader
 *
 * \author xythobuz
 */

#include "global.h"
#include "Script.h"

Script::Script() : puzzles(4), pickups(2), keys(4) {
}

Script::~Script() {

}

int Script::load(const char *file) {
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
    noInputTime = f.readU32();
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
    secretTrack = f.readU16();

    // Filler 4 (4 bytes)
    f.seek(f.tell() + 4);

    // Strings
    readStringPackage(f, levelNames, numLevels);
    readStringPackage(f, pictureFilenames, numPictures);
    readStringPackage(f, titleFilenames, numTitles);
    readStringPackage(f, fmvFilenames, numFMVs);
    readStringPackage(f, levelFilenames, numLevels);
    readStringPackage(f, cutsceneFilenames, numCutscenes);

    // Script Package
    uint16_t *offset = new uint16_t[numLevels + 1];
    for (unsigned int i = 0; i < (numLevels + 1); i++) {
        offset[i] = f.readU16();
        assertEqual(offset[i] % 2, 0);
    }

    uint16_t numBytes = f.readU16() + 6; // Offset TR2 specific?!
    assertEqual(numBytes % 2, 0); // 16 bit opcodes and operands

    uint16_t *list = new uint16_t[numBytes / 2];
    for (uint16_t i = 0; i < (numBytes / 2); i++) {
        list[i] = f.readU16();
    }

    for (unsigned int i = 0; i < (numLevels + 1); i++) {
        unsigned int end = offset[i] / 2;

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

        script.push_back(tmp);
    }

    delete [] list;
    delete [] offset;

    // Engine expects 89 game strings!
    numGameStrings = f.readU16();
    assertEqual(numGameStrings, 89);

    // More strings...
    readStringPackage(f, gameStrings, numGameStrings);
    readStringPackage(f, pcStrings, 41);
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

void Script::readStringPackage(BinaryFile &f, std::vector<std::string> &v, unsigned int n) {
    uint16_t *offset = new uint16_t[n];
    for (unsigned int i = 0; i < n; i++)
        offset[i] = f.readU16();

    uint16_t numBytes = f.readU16();

    char *list = new char[numBytes];
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
    return 41;
}

std::string Script::getPCString(unsigned int i) {
    assert(i < 41);
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

