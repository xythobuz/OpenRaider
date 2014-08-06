/*!
 * \file test/Script.cpp
 * \brief Game script loader unit test
 *
 * \author xythobuz
 */

#include <iostream>

#include "global.h"
#include "test.h"
#include "utils/strings.h"
#include "Script.h"

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
            std::cout << acc(i, a) << " "; \
        } \
        std::cout << std::endl; \
    } \
    std::cout << std::endl; \
}

int main() {
    Script s;
    char *f = fullPath("~/.OpenRaider/paks/tr2/TOMBPC.DAT", 0);
    assertEqual(s.load(f), 0);
    delete [] f;

    printStrings(s.levelCount(), s.getLevelName, "Level Names");
    printStrings(s.levelCount(), s.getLevelFilename, "Level Filenames");
    printStrings(s.cutsceneCount(), s.getCutsceneFilename, "Cutscenes");
    printStrings(s.titleCount(), s.getTitleFilename, "Titles");
    printStrings(s.videoCount(), s.getVideoFilename, "Videos");
    printStrings(s.gameStringCount(), s.getGameString, "Game Strings");
    printStrings(s.pcStringCount(), s.getPCString, "PC Strings");

    printStrings2D(4, s.levelCount(), s.getPuzzleString, "Puzzles");
    printStrings2D(2, s.levelCount(), s.getPickupString, "Pickups");
    printStrings2D(4, s.levelCount(), s.getKeyString, "Keys");

    //std::cout << "This test always fails so you can see its output!" << std::endl;
    //return 1;

    return 0;
}

