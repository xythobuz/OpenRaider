/*!
 * \file src/SoundManager.cpp
 * \brief Sound Source Manager
 *
 * \author xythobuz
 */

#include "imgui/imgui.h"

#include "global.h"
#include "Camera.h"
#include "Log.h"
#include "system/Sound.h"
#include "SoundManager.h"

void SoundSource::prepare() {
    if (source != -1)
        return;

    float vol;
    int index = SoundManager::getIndex(id, &vol);
    if ((index < 0) || (index > Sound::numBuffers())) {
        Log::get(LOG_ERROR) << "Invalid SoundSource ID (" << index << ", "
                            << Sound::numBuffers() << ")!" << Log::endl;
        source = -2;
    } else {
        source = Sound::addSource(index, vol, false, true);
        if (source < 0) {
            source = -2;
        } else {
            int ret = Sound::sourceAt(source, pos);
            if (ret < 0) {
                Log::get(LOG_ERROR) << "Error positioning SoundSource " << id << Log::endl;
            }
        }
    }
}

void SoundSource::play() {
    playing = true;

    if (source >= 0)
        Sound::play(source, false);
}

void SoundSource::stop() {
    playing = false;

    if (source >= 0)
        Sound::stop(source);
}

// ----------------------------------------------------------------------------

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
    for (int i = 0; i < soundMap.size(); i++) {
        float vol;
        SoundDetail* sd;
        int index = getIndex(i, &vol, &sd);
        if ((index >= 0) && (index < Sound::numBuffers())) {
            int ret = Sound::addSource(index, vol, true, false);
            if (ret < 0) {
                Log::get(LOG_ERROR) << "Error adding SoundSource " << index << Log::endl;
            }
            sd->setSource(ret);
        }
    }

    for (int i = 0; i < soundSources.size(); i++) {
        soundSources.at(i).prepare();
        soundSources.at(i).play();
    }

    return 0;
}

void SoundManager::addSoundSource(glm::vec3 p, int id, int flags) {
    soundSources.emplace_back(p, id, flags);
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

int SoundManager::getIndex(int index, float* volume, SoundDetail** sd) {
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

    if (sd != nullptr)
        *sd = &soundDetails.at(index);

    if (volume != nullptr)
        *volume = s.getVolume();

    if (s.getSample() <= -1)
        return -5; // SoundDetail has no entry here (-1)

    if (s.getSample() >= sampleIndices.size())
        return -6; // SoundDetail entry is bigger than SampleIndices

    index = sampleIndices.at(s.getSample());

    if (index <= -1)
        return -7; // SampleIndices has no entry here (-1)

    return index;
}

int SoundManager::playSound(int index) {
    if ((index >= 0) && (index < soundMap.size())) {
        SoundDetail* sd;
        int i = getIndex(index, nullptr, &sd);
        if ((i < 0) || (i >= Sound::numBuffers()))
            return -2;

        Sound::play(sd->getSource(), true);
        return 0;
    } else {
        return -1;
    }
}

void SoundManager::display() {
    if (ImGui::CollapsingHeader("Sound Sources")) {
        ImGui::Columns(5, "soundsources");
        ImGui::Text("No");
        ImGui::NextColumn();
        ImGui::Text("ID");
        ImGui::NextColumn();
        ImGui::Text("Flags");
        ImGui::NextColumn();
        ImGui::Text("Pos");
        ImGui::NextColumn();
        ImGui::Text("Tools");
        ImGui::NextColumn();
        ImGui::Separator();

        static bool offsets = false;
        if (!offsets) {
            ImGui::SetColumnOffset(1, 40.0f);
            ImGui::SetColumnOffset(2, 80.0f);
            ImGui::SetColumnOffset(3, 130.0f);
            ImGui::SetColumnOffset(4, 350.0f);
            offsets = true;
        }

        for (int i = 0; i < soundSources.size(); i++) {
            auto& ss = soundSources.at(i);
            ImGui::Text("%03d", i);
            ImGui::NextColumn();
            ImGui::Text("%d", ss.getID());
            ImGui::NextColumn();
            ImGui::Text("0x%X", ss.getFlags());
            ImGui::NextColumn();
            ImGui::Text("%.1f %.1f %.1f", ss.getPos().x, ss.getPos().y, ss.getPos().z);
            ImGui::NextColumn();
            ImGui::PushID(i);
            if (ImGui::Button("Warp")) {
                Camera::setPosition(ss.getPos());
            }
            ImGui::SameLine();
            if (ImGui::Button("Play")) {
                playSound(soundSources.at(i).getID());
            }
            ImGui::SameLine();
            if (ss.isPlaying()) {
                if (ImGui::Button("Stop")) {
                    ss.stop();
                }
            } else {
                if (ImGui::Button("Start")) {
                    ss.play();
                }
            }
            ImGui::PopID();
            ImGui::NextColumn();
        }

        ImGui::Columns(1);
    }

    if (ImGui::CollapsingHeader("Sound Details")) {
        ImGui::Columns(4, "sounddetails");
        ImGui::Text("No");
        ImGui::NextColumn();
        ImGui::Text("Vol");
        ImGui::NextColumn();
        ImGui::Text("Sample");
        ImGui::NextColumn();
        ImGui::Text("Tools");
        ImGui::NextColumn();
        ImGui::Separator();

        static bool offsets2 = false;
        if (!offsets2) {
            ImGui::SetColumnOffset(1, 40.0f);
            ImGui::SetColumnOffset(2, 80.0f);
            ImGui::SetColumnOffset(3, 180.0f);
            offsets2 = true;
        }

        for (int i = 0; i < soundDetails.size(); i++) {
            auto& sd = soundDetails.at(i);
            ImGui::Text("%03d", i);
            ImGui::NextColumn();
            ImGui::Text("%.2f", sd.getVolume());
            ImGui::NextColumn();
            if ((sd.getSample() < 0) || (sd.getSample() >= sampleIndices.size())) {
                ImGui::Text("%03d --> ???", sd.getSample());
            } else {
                ImGui::Text("%03d --> %03d", sd.getSample(), sampleIndices.at(sd.getSample()));
            }
            ImGui::NextColumn();
            ImGui::PushID(i);
            if (sd.getSource() >= 0) {
                if (ImGui::Button("Play")) {
                    Sound::play(sd.getSource(), true);
                }
            }
            ImGui::PopID();
            ImGui::NextColumn();
        }

        ImGui::Columns(1);
    }

    if (ImGui::CollapsingHeader("Sound Map Player")) {
        if (!Sound::getEnabled()) {
            ImGui::Text("Please enable Sound first!");
            if (ImGui::Button("Enable Sound!")) {
                Sound::setEnabled(true);
            }
            return;
        } else if (Sound::numBuffers() == 0) {
            ImGui::Text("No Sounds currently loaded!");
            return;
        }

        static int index = 0;
        ImGui::Text("Map");
        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
        ImGui::SliderInt("##soundslide", &index, 0, soundMap.size() - 1);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("+##soundplus", ImVec2(0, 0))) {
            if (index < (soundMap.size() - 1))
                index++;
            else
                index = 0;
        }
        ImGui::SameLine();
        if (ImGui::Button("-##soundminus", ImVec2(0, 0))) {
            if (index > 0)
                index--;
            else
                index = soundMap.size() - 1;
        }
        ImGui::SameLine();
        if (ImGui::Button("Play##soundplay")) {
            playSound(index);
        }
        ImGui::SameLine();
        ImGui::Text("%d", getIndex(index));
    }
}

