/*!
 * \file src/SoundManager.cpp
 * \brief This is the audio manager Implementation
 *
 * \author xythobuz
 */

#include "global.h"
#include "Sound.h"
#include "SoundManager.h"

void SoundManager::clear() {
    soundSources.clear();
    soundMap.clear();
    soundDetails.clear();
    sampleIndices.clear();
}

void SoundManager::addSoundSource(float x, float y, float z, int id, int flags) {
    soundSources.emplace_back(x, y, z, id, flags);
}

void SoundManager::addSoundMapEntry(int id) {
    soundMap.push_back(id);
}

void SoundManager::addSoundDetail(int sample, float volume) {
    soundDetails.emplace_back(sample, volume);
}

void SoundManager::addSampleIndex(int index) {
    sampleIndices.push_back(index);
}

int SoundManager::playSound(int index) {
    if (index >= soundMap.size())
        return -1; // SoundMap not big enough

    index = soundMap.at(index);

    if (index == -1)
        return -2; // SoundMap has no entry here (-1)

    if (index >= soundDetails.size())
        return -3; // SoundMap entry is bigger than SoundDetails

    SoundDetail s = soundDetails.at(index);

    if (s.sample == -1)
        return -4; // SoundDetail has no entry here (-1)

    if (s.sample >= sampleIndices.size())
        return -5; // SoundDetail entry is bigger than SampleIndices

    index = sampleIndices.at(s.sample);

    if (index == -1)
        return -6; // SampleIndices has no entry here (-1)

    if (index >= getSound().registeredSources())
        return -7; // SampleIndices entry is bigger than number of sounds loaded

    // TODO play sound _index_ with matching volume

    return 0;
}

