/*!
 * \file src/SoundManager.cpp
 * \brief This is the audio manager Implementation
 *
 * \author xythobuz
 */

#include "imgui/imgui.h"

#include "global.h"
#include "system/Sound.h"
#include "SoundManager.h"

std::vector<SoundSource> SoundManager::soundSources;
std::vector<int> SoundManager::soundMap;
std::vector<SoundDetail> SoundManager::soundDetails;
std::vector<int> SoundManager::sampleIndices;

void SoundManager::clear() {
    soundSources.clear();
    soundMap.clear();
    soundDetails.clear();
    sampleIndices.clear();

    Sound::clear();
}

int SoundManager::prepareSources() {
    for (int i = 0; i < soundSources.size(); i++) {
        float vol;
        int index = getIndex(soundSources.at(i).id, &vol);
        int ret = Sound::addSource(index, vol, false, true);
        assertEqual(ret, i);
        float pos[3] = { soundSources.at(i).x, soundSources.at(i).y, soundSources.at(i).z };
        ret = Sound::sourceAt(i, pos);
        assertEqual(ret, 0);
        Sound::play(i, false);
    }

    for (int i = 0; i < soundMap.size(); i++) {
        float vol;
        int index = getIndex(i, &vol);
        if ((index >= 0) && (index < Sound::numBuffers())) {
            int ret = Sound::addSource(index, vol, true, false);
            assertGreaterThanEqual(ret, 0);
        }
    }

    return 0;
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

int SoundManager::sizeSoundMap() {
    return soundMap.size();
}

int SoundManager::getIndex(int index, float* volume) {
    if (index <= -1)
        return -1;

    if (index >= soundMap.size())
        return -2; // SoundMap not big enough

    index = soundMap.at(index);

    if (index <= -1)
        return -3; // SoundMap has no entry here (-1)

    if (index >= soundDetails.size())
        return -4; // SoundMap entry is bigger than SoundDetails

    SoundDetail s = soundDetails.at(index);

    if (volume != nullptr)
        *volume = s.volume;

    if (s.sample <= -1)
        return -5; // SoundDetail has no entry here (-1)

    if (s.sample >= sampleIndices.size())
        return -6; // SoundDetail entry is bigger than SampleIndices

    index = sampleIndices.at(s.sample);

    if (index <= -1)
        return -7; // SampleIndices has no entry here (-1)

    return index;
}

int SoundManager::playSound(int index) {
    if ((index >= 0) && (index < soundMap.size())) {
        if (soundMap.at(index) == -1)
            return 0;

        int c = 1;
        for (int i = 0; i < index; i++)
            if (soundMap.at(i) != -1)
                c++;

        Sound::play(c, true);
        return 0;
    } else {
        return -1;
    }
}

void SoundManager::display() {
    if (ImGui::CollapsingHeader("Sound Map Player")) {
        if (!Sound::getEnabled()) {
            ImGui::Text("Please enable Sound first!");
            if (ImGui::Button("Enable Sound!")) {
                Sound::setEnabled(true);
            }
        } else if (Sound::numBuffers() == 0) {
            ImGui::Text("Please load a level!");
        } else {
            static int index = 0;
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SliderInt("##soundslide", &index, 0, sizeSoundMap() - 1);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (ImGui::Button("+##soundplus", ImVec2(0, 0), true)) {
                if (index < (sizeSoundMap() - 1))
                    index++;
                else
                    index = 0;
            }
            ImGui::SameLine();
            if (ImGui::Button("-##soundminus", ImVec2(0, 0), true)) {
                if (index > 0)
                    index--;
                else
                    index = sizeSoundMap() - 1;
            }
            ImGui::SameLine();
            if (ImGui::Button("Play##soundplay")) {
                playSound(index);
            }

            ImGui::Text("Index: %d", getIndex(index));
        }
    }
}

