/*!
 * \file include/RunTime.h
 * \brief run time configuration storage
 *
 * \author xythobuz
 */

#ifndef _RUNTIME_H_
#define _RUNTIME_H_

#include <string>

// Defaults
#define DEFAULT_CONFIG_PATH "~/.OpenRaider"
#define DEFAULT_CONFIG_FILE "OpenRaider.ini"
#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

/*!
 * \brief Main Game Singleton
 */
class RunTime {
public:

    RunTime();

    std::string getBaseDir();
    void setBaseDir(std::string dir);
    std::string getPakDir();
    void setPakDir(std::string dir);
    std::string getAudioDir();
    void setAudioDir(std::string dir);
    std::string getDataDir();
    void setDataDir(std::string dir);

    KeyboardButton getKeyBinding(ActionEvents event);
    void setKeyBinding(ActionEvents event, KeyboardButton button);

    bool isRunning();
    void setRunning(bool run);

    bool getFPS();
    void setFPS(bool fps);

private:
    std::string baseDir;
    std::string pakDir;
    std::string audioDir;
    std::string dataDir;

    KeyboardButton keyBindings[ActionEventCount];
    bool gameIsRunning;
    bool showFPS;
};

RunTime &getRunTime();

#endif

