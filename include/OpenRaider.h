/*!
 * \file include/OpenRaider.h
 * \brief Main Game Object
 *
 * \author xythobuz
 */

#ifndef _OPENRAIDER_H_
#define _OPENRAIDER_H_

#include <vector>

#include "global.h"
#include "Console.h"
#include "Game.h"
#include "Menu.h"
#include "Sound.h"
#include "Window.h"

/*!
 * \brief Main Game Singleton
 */
class OpenRaider {
public:

    /*!
     * \brief Constructs an object of OpenRaider
     */
    OpenRaider();

    /*!
     * \brief Deconstructs an object of OpenRaider
     */
    ~OpenRaider();

    /*!
     * \brief Load the configuration file
     * \returns 0 on success
     */
    int loadConfig(const char *config);

    int command(const char *command);

    void run();

    void handleKeyboard(KeyboardButton key, bool pressed);

    void handleText(char *text, bool notFinished);

    void handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released);

    void handleMouseMotion(int xrel, int yrel);

    void handleMouseScroll(int xrel, int yrel);

    //! \fixme should be private

    bool mMapListFilled;
    std::vector<char *> mMapList;

    float mCameraRotationDeltaX;
    float mCameraRotationDeltaY;

    char *mBaseDir;
    char *mPakDir;
    char *mAudioDir;
    char *mDataDir;

private:

    int command(const char *command, std::vector<char *> *args);

    char *expandDirectoryNames(const char *s);

    int help(const char *cmd);

    int set(const char *var, const char *value);

    int bind(const char *action, const char *key);

    int bind(ActionEvents action, const char *key);

    void loadPakFolderRecursive(const char *dir);

    void fillMapList();

    bool mRunning;
    bool mFPS;

    KeyboardButton keyBindings[ActionEventCount];
};

#endif
