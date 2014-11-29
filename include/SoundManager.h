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
    void clear();
    void addSoundSource(float x, float y, float z, int id, int flags);
    void addSoundMapEntry(int id);
    void addSoundDetail(int sample, float volume);
    void addSampleIndex(int index);

    int playSound(int index);

  private:
    std::vector<SoundSource> soundSources;
    std::vector<int> soundMap;
    std::vector<SoundDetail> soundDetails;
    std::vector<int> sampleIndices;
};

SoundManager& getSoundManager();

#endif

