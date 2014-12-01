/*!
 * \file include/system/SoundAL.h
 * \brief This is the OpenAL audio manager Header
 *
 * \author xythobuz
 */

#ifndef _SOUND_AL_H_
#define _SOUND_AL_H_

#include <vector>

class SoundAL {
  public:
    static int initialize();
    static void shutdown();
    static void clear();

    static int numBuffers();
    static int loadBuffer(unsigned char* buffer, unsigned int length);

    static int numSources(bool atListener);
    static int addSource(int buffer, float volume, bool atListener, bool loop);

    static int sourceAt(int source, float pos[3]);
    static void listenAt(float pos[3], float orientation[6]);

    static void play(int source, bool atListener);
    static void stopAll();

    static void setEnabled(bool on);
    static bool getEnabled();

    static void setVolume(float vol);
    static float getVolume();

  private:
    static bool init;
    static bool enabled;
    static float volume;

    static std::vector<unsigned int> buffers;
    static std::vector<unsigned int> sources;
    static std::vector<unsigned int> listenerSources;

    static float lastPosition[3];
};

#endif

