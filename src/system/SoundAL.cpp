/*!
 * \file src/system/SoundAL.cpp
 * \brief This is the OpenAL audio manager Implementation
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifdef __APPLE__
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#endif

#include <AL/alut.h>

#include "global.h"
#include "Log.h"
#include "system/SoundAL.h"

bool SoundAL::init = false;
bool SoundAL::enabled = true;
float SoundAL::volume = 1.0f;
std::vector<unsigned int> SoundAL::buffers;
std::vector<unsigned int> SoundAL::sources;
std::vector<unsigned int> SoundAL::listenerSources;
glm::vec3 SoundAL::lastPosition(0.0f, 0.0f, 0.0f);

int SoundAL::initialize() {
    if (init)
        return 0;

    ALCdevice* device = alcOpenDevice(nullptr);
    ALCcontext* context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);

    if (alutInitWithoutContext(nullptr, nullptr) == AL_FALSE) {
        Log::get(LOG_ERROR) << "SoundAL Error: " << alutGetErrorString(alutGetError()) << Log::endl;
        return -1;
    }

    init = true;
    setVolume(volume);

    lastPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 at(0.0f, 0.0f, -1.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    listenAt(lastPosition, at, up);

    return 0;
}

void SoundAL::shutdown() {
    if (!init)
        return;

    clear();
    if (alutExit() == AL_FALSE)
        Log::get(LOG_ERROR) << "SoundAL Error: " << alutGetErrorString(alutGetError()) << Log::endl;
    init = false;
}

void SoundAL::clear() {
    if (!init)
        return;

    stopAll();

    alGetError();
    alDeleteSources(sources.size(), &sources[0]);
    sources.clear();
    if (alGetError() != AL_NO_ERROR) {
        Log::get(LOG_ERROR) << "SoundAL: Error while deleting sources!" << Log::endl;
    }

    alGetError();
    alDeleteSources(listenerSources.size(), &listenerSources[0]);
    listenerSources.clear();
    if (alGetError() != AL_NO_ERROR) {
        Log::get(LOG_ERROR) << "SoundAL: Error while deleting listener sources!" << Log::endl;
    }

    alGetError();
    alDeleteBuffers(buffers.size(), &buffers[0]);
    buffers.clear();
    if (alGetError() != AL_NO_ERROR) {
        Log::get(LOG_ERROR) << "SoundAL: Error while deleting buffers!" << Log::endl;
    }

    lastPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 at(0.0f, 0.0f, -1.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    listenAt(lastPosition, at, up);
}

int SoundAL::numBuffers() {
    return buffers.size();
}

int SoundAL::loadBuffer(unsigned char* buffer, unsigned int length) {
    if (!init)
        return -1;

    alGetError();
    unsigned int r = alutCreateBufferFromFileImage(buffer, length);
    if (r == AL_NONE) {
        Log::get(LOG_ERROR) << "SoundAL Error: " << alutGetErrorString(alutGetError()) << Log::endl;
        return -2;
    }
    buffers.push_back(r);
    return r;
}

int SoundAL::numSources(bool atListener) {
    if (atListener)
        return listenerSources.size();
    else
        return sources.size();
}

int SoundAL::addSource(int buffer, float volume, bool atListener, bool loop) {
    if ((!init) || (buffer < 0) || (buffer >= buffers.size()))
        return -1;

    unsigned int id;

    alGetError();
    alGenSources(1, &id);
    if (alGetError() != AL_NO_ERROR) {
        Log::get(LOG_ERROR) << "SoundAL Error: Could not create source!" << Log::endl;
        return -2;
    }

    alSourcei(id, AL_BUFFER, buffers.at(buffer));
    alSourcef(id, AL_GAIN, volume);

    if (loop)
        alSourcei(id, AL_LOOPING, AL_TRUE);

    if (atListener) {
        alSourcefv(id, AL_POSITION, &lastPosition[0]);
        listenerSources.push_back(id);
        return listenerSources.size() - 1;
    } else {
        sources.push_back(id);
        return sources.size() - 1;
    }
}

int SoundAL::sourceAt(int source, glm::vec3 pos) {
    if (!init)
        return -1;

    if ((source < 0) || (source >= sources.size())) {
        Log::get(LOG_ERROR) << "SoundAL: Can't position non-existing source!" << Log::endl;
        return -2;
    }

    alSourcefv(sources.at(source), AL_POSITION, &pos[0]);

    return 0;
}

void SoundAL::listenAt(glm::vec3 pos, glm::vec3 at, glm::vec3 up) {
    if (!init)
        return;

    float orientation[6] = { at.x, at.y, at.z, up.x, up.y, up.z };
    alListenerfv(AL_POSITION, &pos[0]);
    alListenerfv(AL_ORIENTATION, orientation);

    for (auto& s : listenerSources) {
        alSourcefv(s, AL_POSITION, &pos[0]);
    }

    lastPosition = pos;
}

void SoundAL::play(int source, bool atListener) {
    if ((!init) || (!enabled))
        return;

    if (atListener) {
        if ((source >= 0) && (source < listenerSources.size()))
            alSourcePlay(listenerSources.at(source));
        else
            Log::get(LOG_ERROR) << "SoundAL: Can't play non-existing listener source!" << Log::endl;
    } else {
        if ((source >= 0) && (source < sources.size()))
            alSourcePlay(sources.at(source));
        else
            Log::get(LOG_ERROR) << "SoundAL: Can't play non-existing source!" << Log::endl;
    }
}

void SoundAL::stopAll() {
    if (!init)
        return;

    alSourceStopv(sources.size(), &sources[0]);
    alSourceStopv(listenerSources.size(), &listenerSources[0]);
}

void SoundAL::setEnabled(bool on) {
    enabled = on;
}

bool SoundAL::getEnabled() {
    return enabled;
}

void SoundAL::setVolume(float vol) {
    volume = vol;

    if (init)
        alListenerf(AL_GAIN, volume);
}

float SoundAL::getVolume() {
    return volume;
}

