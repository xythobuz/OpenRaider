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
    assert(gameflowSize == 128);

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
    bool tag = (flags & S_UseSecurityTag) != 0;

    // Filler 3 (6 bytes)
    f.seek(f.tell() + 6);

    cypherCode = f.readU8();
    language = f.readU8();
    secretTrack = f.readU16();

    // Filler 4 (4 bytes)
    f.seek(f.tell() + 4);

    // Strings
    readStringPackage(f, levelNames, numLevels, tag, 0);
    readStringPackage(f, pictureFilenames, numPictures, tag, 0);
    readStringPackage(f, titleFilenames, numTitles, tag, 0);
    readStringPackage(f, fmvFilenames, numFMVs, tag, 0);
    readStringPackage(f, levelFilenames, numLevels, tag, 0);
    readStringPackage(f, cutsceneFilenames, numCutscenes, tag, 0);

    // Offset definitely TR2 specific?!
    readStringPackage(f, script, numLevels + 1, false, 6);

    numGameStrings = f.readU16();
    assert(numGameStrings == 89);

    readStringPackage(f, gameStrings, numGameStrings, tag, 0);

    readStringPackage(f, pcStrings, 41, tag, 0);

    readStringPackage(f, puzzles[0], numLevels, tag, 0);
    readStringPackage(f, puzzles[1], numLevels, tag, 0);
    readStringPackage(f, puzzles[2], numLevels, tag, 0);
    readStringPackage(f, puzzles[3], numLevels, tag, 0);

    readStringPackage(f, pickups[0], numLevels, tag, 0);
    readStringPackage(f, pickups[1], numLevels, tag, 0);

    readStringPackage(f, keys[0], numLevels, tag, 0);
    readStringPackage(f, keys[1], numLevels, tag, 0);
    readStringPackage(f, keys[2], numLevels, tag, 0);
    readStringPackage(f, keys[3], numLevels, tag, 0);

    return 0;
}

void Script::readStringPackage(BinaryFile &f, std::vector<std::string> &v, unsigned int n, bool tag, uint16_t off) {
    uint16_t *offset = new uint16_t[n];
    for (unsigned int i = 0; i < n; i++)
        offset[i] = f.readU16();

    uint16_t numBytes = f.readU16() + off;

    char *list = new char[numBytes];
    for (uint16_t i = 0; i < numBytes; i++) {
        list[i] = f.read8();
        if (tag) {
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

