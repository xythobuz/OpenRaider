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

    /*!
     * \brief Constructs an object of OpenRaider
     */
    OpenRaider();

    /*!
     * \brief Deconstructs an object of OpenRaider
     */
    ~OpenRaider();

    int initialize();

    /*!
     * \brief Load the configuration file
     * \returns 0 on success
     */
    int loadConfig(const char *config);

    int command(std::string command);

    void run();
    void frame();

    //! \fixme should be private
    char *mBaseDir;
    char *mPakDir;
    char *mAudioDir;
    char *mDataDir;
    KeyboardButton keyBindings[ActionEventCount];
    bool mRunning;
    bool mFPS;
};

OpenRaider &getOpenRaider();

#endif

