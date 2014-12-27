/*!
 * \file include/RunTime.h
 * \brief run time configuration storage
 *
 * \author xythobuz
 */

#ifndef _RUNTIME_H_
#define _RUNTIME_H_

#include <string>
#include <vector>

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

    bool getShowFPS();
    void setShowFPS(bool fps);

    void updateFPS();
    unsigned long getFPS();
    const std::vector<float>& getHistoryFPS();

    float getLastFrameTime();

  private:
    std::string baseDir;
    std::string pakDir;
    std::string audioDir;
    std::string dataDir;

    KeyboardButton keyBindings[ActionEventCount];
    bool gameIsRunning;
    bool showFPS;

    unsigned long lastTime, lastFrameTime;
    unsigned long frameCount, frameCount2;
    unsigned long frameTimeSum, frameTimeSum2;
    unsigned long fps;
    std::vector<float> history;
};

RunTime& getRunTime();

#endif

