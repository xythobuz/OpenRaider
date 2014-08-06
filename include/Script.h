/*!
 * \file include/Script.h
 * \brief Game script loader
 *
 * \author xythobuz
 */

#ifndef _SCRIPT_H_
#define _SCRIPT_H_

#include "utils/binary.h"

#include <cstdint>
#include <string>
#include <vector>

/*!
 * \brief Game script loader
 */
class Script {
public:

    typedef enum {
        S_English  = 0,
        S_French   = 1,
        S_German   = 2,
        S_American = 3,
        S_Japanese = 4
    } ScriptLanguage;

    Script();
    ~Script();

    int load(const char *file);

    unsigned int levelCount();
    std::string getLevelName(unsigned int i);
    std::string getLevelFilename(unsigned int i);

    unsigned int cutsceneCount();
    std::string getCutsceneFilename(unsigned int i);

    unsigned int titleCount();
    std::string getTitleFilename(unsigned int i);

    unsigned int videoCount();
    std::string getVideoFilename(unsigned int i);

    unsigned int gameStringCount();
    std::string getGameString(unsigned int i);

    unsigned int pcStringCount();
    std::string getPCString(unsigned int i);

    std::string getPuzzleString(unsigned int i, unsigned int j);
    std::string getPickupString(unsigned int i, unsigned int j);
    std::string getKeyString(unsigned int i, unsigned int j);

private:

    void readStringPackage(BinaryFile &f, std::vector<std::string> &v, unsigned int n, bool tag, uint16_t off);

    enum ScriptFlags {
        S_DemoVersion            = (1 << 0),  //!< Don't load a MAIN.SFX
        S_TitleDisabled          = (1 << 1),  //!< If set, game has no title screen
        S_CheatModeCheckDisabled = (1 << 2),  //!< Disable flare/step/rotate/jump sequence
        S_NoInputTimeout         = (1 << 3),  //!< If set don't timeout input to start demo
        S_LoadSaveDisabled       = (1 << 4),  //!< Don't allow load/save
        S_ScreenSizingDisabled   = (1 << 5),  //!< Don't change screen resolution
        S_LockOutOptionRing      = (1 << 6),  //!< No option ring while in level
        S_DOZYCheatEnabled       = (1 << 7),  //!< DOZY flying cheat
        S_UseSecurityTag         = (1 << 8),  //!< Strings XORed with cypherCode
        S_Unknown                = (1 << 9),  //!< Usually set, no known effect
        S_SelectAnyLevel         = (1 << 10), //!< Level selectable in Title
        S_EnableCheatCode        = (1 << 11)  //!< No known effect
    };

    // Header
    uint32_t version;
    std::string description;

    // Gameflow data
    uint32_t firstOption;
    int32_t titleReplace;
    uint32_t onDeathDemoMode;
    uint32_t onDeathInGame;
    uint32_t noInputTime;
    uint32_t onDemoInterrupt;
    uint32_t onDemoEnd;
    uint16_t numLevels;
    uint16_t numPictures;
    uint16_t numTitles;
    uint16_t numFMVs;
    uint16_t numCutscenes;
    uint16_t numDemos;
    uint16_t titleTrack;
    int16_t singleLevel;
    uint16_t flags;
    uint8_t cypherCode;
    uint8_t language;
    uint16_t secretTrack;

    uint16_t numGameStrings;

    // Strings
    std::vector<std::string> levelNames; // numLevels
    std::vector<std::string> pictureFilenames; // numPictures
    std::vector<std::string> titleFilenames; // numTitles
    std::vector<std::string> fmvFilenames; // numFMVs
    std::vector<std::string> levelFilenames; // numLevels
    std::vector<std::string> cutsceneFilenames; // numCutscenes
    std::vector<std::string> script; // numLevels + 1
    std::vector<std::string> gameStrings; // numGameStrings, 89
    std::vector<std::string> pcStrings; // 41
    std::vector<std::vector<std::string>> puzzles; // 4 * numLevels
    std::vector<std::vector<std::string>> pickups; // 2 * numLevels
    std::vector<std::vector<std::string>> keys; // 4 * numLevels
};

#endif

