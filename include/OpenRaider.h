/*!
 * \file include/OpenRaider.h
 * \brief Main Game Object
 *
 * \author xythobuz
 */

#ifndef _OPENRAIDER_H_
#define _OPENRAIDER_H_

#include <string>

/*!
 * \brief Main Game Singleton
 */
class OpenRaider {
public:

    OpenRaider();

    int initialize();

    /*!
     * \brief Load the configuration file
     * \returns 0 on success
     */
    int loadConfig(std::string config);

    std::string getBaseDir();
    void setBaseDir(std::string dir);
    std::string getPakDir();
    void setPakDir(std::string dir);
    std::string getAudioDir();
    void setAudioDir(std::string dir);
    std::string getDataDir();
    void setDataDir(std::string dir);

    void run();
    void frame();

    //! \fixme should be private
    KeyboardButton keyBindings[ActionEventCount];
    bool mRunning;
    bool mFPS;

private:
    std::string baseDir;
    std::string pakDir;
    std::string audioDir;
    std::string dataDir;
};

OpenRaider &getOpenRaider();

#endif

