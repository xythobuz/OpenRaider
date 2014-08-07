/*!
 * \file test/Script.cpp
 * \brief Game script loader unit test
 *
 * \author xythobuz
 */

#include <iostream>

#include "global.h"
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
    int test(const char *f, bool strings) {
        Script s;
        assertEqual(s.load(f), 0);

        if (strings) {
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
                if (error != 0)
                    std::cout << "Returned " << error << "..." << std::endl;
                std::cout << std::endl;
            }
        }

        return 0;
    }
}

int main(int argc, char *argv[]) {
    char *f = fullPath("~/.OpenRaider/paks/tr2/TOMBPC.DAT", 0);
    int error = test(f, !((argc > 1) && (argv[1][0] == 's')));
    delete [] f;

    if (error != 0)
        return error;

    f = fullPath("~/.OpenRaider/paks/tr3/TOMBPC.DAT", 0);
    error = test(f, !((argc > 1) && (argv[1][0] == 's')));
    delete [] f;

    return error;
}

