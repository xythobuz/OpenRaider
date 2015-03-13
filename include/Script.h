/*!
 * \file include/Script.h
 * \brief Tomb Raider 2/3 Script Loader
 *
 * \author xythobuz
 */

#ifndef _SCRIPT_H_
#define _SCRIPT_H_

#include "utils/binary.h"

#include <cstdint>
#include <functional>
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

    typedef enum {
        OP_PICTURE        = 0,  //!< Unused in TR2. Or PSX? Used in TR3.
        OP_PSX_TRACK      = 1,  //!< Does not compile. PSX?
        OP_PSX_FMV        = 2,  //!< Does not compile. PSX?
        OP_FMV            = 3,  //!< Display FMV
        OP_GAME           = 4,  //!< Start a playable level
        OP_CUT            = 5,  //!< Display a cutscene
        OP_COMPLETE       = 6,  //!< Display level-completion stats
        OP_DEMO           = 7,  //!< Display demo sequence
        OP_PSX_DEMO       = 8,  //!< Does not compile. PSX?
        OP_END            = 9,  //!< Closes script sequence
        OP_TRACK          = 10, //!< Play soundtrack (precedes level opcode)
        OP_SUNSET         = 11, //!< Unknown, nothing changes in TR2. Start in Motorboat?
        OP_LOAD_PIC       = 12, //!< Does not compile. PSX? Used in TR3.
        OP_DEADLY_WATER   = 13, //!< Unknown, nothing changes in TR2.
        OP_REMOVE_WEAPONS = 14, //!< Start level without weapons
        OP_GAMECOMPLETE   = 15, //!< End of game. Show stats, start credits sequence, music ID 52 in TR2.
        OP_CUTANGLE       = 16, //!< Match N-S orientation of Room and animated characters.
        OP_NOFLOOR        = 17, //!< Lara dies when her feet reach given depth.
        OP_STARTINV       = 18, //!< Items given to Lara at level start (+1000), or at all secrets found (+0)
        OP_STARTANIM      = 19, //!< Special animation of Lara when level starts
        OP_SECRETS        = 20, //!< If zero, level does not account for secrets
        OP_KILLTOCOMPLETE = 21, //!< Kill all enemies to finish the level
        OP_REMOVE_AMMO    = 22, //!< Lara starts level without ammo or medi packs
        OP_UNKNOWN        = 23
    } ScriptOpCode;

    // Items for all-secrets-found go from 0 to 26,
    // for start-inventory add 1000, so 1000 - 1026
    // Atleast in TR2
    typedef enum {
        OP_WEAPON_PISTOLS     = 0,  //!< Add standard pistols (2)
        OP_WEAPON_SHOTGUN     = 1,  //!< Add shotgun (1)
        OP_WEAPON_AUTOPISTOLS = 2,  //!< Add automatic pistols (2)
        OP_WEAPON_UZIS        = 3,  //!< Add uzis (2)
        OP_WEAPON_HARPOON     = 4,  //!< Add harpoon gun (1)
        OP_WEAPON_M16         = 5,  //!< Add M16 (1)
        OP_WEAPON_ROCKET      = 6,  //!< Add grenade launcher (1)
        OP_AMMO_PISTOLS       = 7,  //!< No effect, infinite ammo
        OP_AMMO_SHOTGUN       = 8,  //!< Add 2 shells
        OP_AMMO_AUTOPISTOLS   = 9,  //!< Add 2 shells
        OP_AMMO_UZIS          = 10, //!< Add 2 shells
        OP_AMMO_HARPOON       = 11, //!< Add 2 harpoons
        OP_AMMO_M16           = 12, //!< Add 2 shells
        OP_AMMO_ROCKET        = 13, //!< Add 1 grenade
        OP_ITEM_FLARE         = 14, //!< Add 1 flare
        OP_ITEM_MEDI          = 15, //!< Add 1 small MediPack
        OP_ITEM_BIGMEDI       = 16, //!< Add 1 big MediPack
        OP_ITEM_PICKUP1       = 17, //!< Add Pickup Item 1
        OP_ITEM_PICKUP2       = 18, //!< Add Pickup Item 2
        OP_ITEM_PUZZLE1       = 19, //!< Add Puzzle Item 1
        OP_ITEM_PUZZLE2       = 20, //!< Add Puzzle Item 2
        OP_ITEM_PUZZLE3       = 21, //!< Add Puzzle Item 3
        OP_ITEM_PUZZLE4       = 22, //!< Add Puzzle Item 4
        OP_ITEM_KEY1          = 23, //!< Add Key Item 1
        OP_ITEM_KEY2          = 24, //!< Add Key Item 2
        OP_ITEM_KEY3          = 25, //!< Add Key Item 3
        OP_ITEM_KEY4          = 26, //!< Add Key Item 4
        OP_ITEM_UNKNOWN       = 27
    } ScriptItem;

    Script();

    int load(std::string file);

    unsigned int levelCount();
    std::string getLevelName(unsigned int i);
    std::string getLevelFilename(unsigned int i);

    unsigned int pictureCount();
    std::string getPictureFilename(unsigned int i);

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

    void registerScriptHandler(ScriptOpCode op, std::function<int (bool, uint16_t)> func);
    int runScript(unsigned int level);

    std::string getDescription() { return description; }
    std::string getLanguage();

  private:

    typedef enum {
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
    } ScriptFlag;

    void readStringPackage(BinaryFile& f, std::vector<std::string>& v, unsigned int n);
    void readScriptPackage(BinaryFile& f, std::vector<std::vector<uint16_t>>& v, unsigned int n);

    const static bool opcodeHasOperand[OP_UNKNOWN];

    // Header
    uint32_t version; // Always 3, for TR2/3 on PC and PSX
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
    uint16_t flags; // See ScriptFlag enum
    uint8_t cypherCode;
    uint8_t language; // See ScriptLanguage enum
    uint16_t secretTrack;

    uint16_t numPCStrings;
    uint16_t numGameStrings;

    // Strings
    std::vector<std::string> levelNames; // numLevels
    std::vector<std::string> pictureFilenames; // numPictures
    std::vector<std::string> titleFilenames; // numTitles
    std::vector<std::string> fmvFilenames; // numFMVs
    std::vector<std::string> levelFilenames; // numLevels
    std::vector<std::string> cutsceneFilenames; // numCutscenes
    std::vector<std::vector<uint16_t>> script; // numLevels + 1
    std::vector<std::string> gameStrings; // numGameStrings
    std::vector<std::string> pcStrings; // 41 for TR2/3 on PC; 80 for TR2 on PSX
    std::vector<std::vector<std::string>> puzzles; // 4 * numLevels
    std::vector<std::vector<std::string>> pickups; // 2 * numLevels
    std::vector<std::vector<std::string>> keys; // 4 * numLevels

    std::function<int (bool, uint16_t)> scriptHandlers[OP_UNKNOWN];
};

#endif

