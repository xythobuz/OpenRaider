/*!
 * \file include/SoundManager.h
 * \brief This is the audio manager Header
 *
 * \author xythobuz
 */

#ifndef _SOUNDMANAGER_H_
#define _SOUNDMANAGER_H_

#include <vector>

struct SoundSource {
    float x, y, z;
    int id, flags;

    SoundSource(float _x, float _y, float _z, int i, int f)
        : x(_x), y(_y), z(_z), id(i), flags(f) { }
};

struct SoundDetail {
    int sample;
    float volume;

    SoundDetail(int s, float v) : sample(s), volume(v) { }
};

class SoundManager {
  public:
    static void clear();
    static int prepareSources();

    static void addSoundSource(float x, float y, float z, int id, int flags);
    static void addSoundMapEntry(int id);
    static void addSoundDetail(int sample, float volume);
    static void addSampleIndex(int index);

    static int sizeSoundMap();

    static int getIndex(int index, float* volume = nullptr);

    // index --> SoundMap --> SoundDetails --> SampleIndices --> play
    static int playSound(int index);

  private:
    static std::vector<SoundSource> soundSources;
    static std::vector<int> soundMap;
    static std::vector<SoundDetail> soundDetails;
    static std::vector<int> sampleIndices;
};

#endif

