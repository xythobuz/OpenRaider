/*!
 * \file src/Sound.cpp
 * \brief This is the audio manager Implementation
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

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "Sound.h"

Sound::Sound()
{
    mSource[0] = 0;
    mBuffer[0] = 0;
    mNextBuffer = 0;
    mNextSource = 0;
    mInit = false;
}


Sound::~Sound()
{
    if (mInit)
    {
        alutExit();
    }
}


int Sound::init()
{
#ifndef __APPLE__
    int fd;

    fd = open("/dev/dsp", O_RDWR);

    if (fd < 0)
    {
        perror("Sound::Init> Could not open /dev/dsp : ");
        return -1;
    }

    close(fd);
#endif

    ALCdevice *Device = alcOpenDevice("OSS");
    ALCcontext *Context = alcCreateContext(Device, NULL);
    alcMakeContextCurrent(Context);

    if (alutInitWithoutContext(NULL, NULL) == AL_FALSE) {
        printf("Sound::Init> Could not initialize alut (%s)\n", alutGetErrorString(alutGetError()));
        return -2;
    }

    mInit = true;
    printf("Created OpenAL Context\n");

    return 0;
}


void Sound::listenAt(float pos[3], float angle[3])
{
    if (!mInit)
        return;

    alListenerfv(AL_POSITION, pos);
    alListenerfv(AL_ORIENTATION, angle);
}


void Sound::sourceAt(int source, float pos[3])
{
    if (!mInit || source < 0)
        return;

    alSourcefv(mSource[source-1], AL_POSITION, pos);
}


//! \fixme Seperate sourcing and buffering, Mongoose 2002.01.04
int Sound::addFile(const char *filename, int *source, unsigned int flags)
{
    ALsizei size;
    ALfloat freq;
    ALenum format;
    ALvoid *data;


    if (!mInit || !filename || !source)
    {
        printf("Sound::AddFile> ERROR pre condition assertion failed\n");
        return -1000;
    }

    *source = -1;

    alGetError();

    alGenBuffers(1, &mBuffer[mNextBuffer]);

    if (alGetError() != AL_NO_ERROR)
    {
        fprintf(stderr, "Sound::AddFile> alGenBuffers call failed\n");
        return -1;
    }

    alGetError();

    alGenSources(1, &mSource[mNextSource]);

    if (alGetError() != AL_NO_ERROR)
    {
        fprintf(stderr, "Sound::AddFile> alGenSources call failed\n");
        return -2;
    }

    // err = alutLoadWAV(filename, &data, &format, &size, &bits, &freq);
    // is deprecated!
    data = alutLoadMemoryFromFile(filename, &format, &size, &freq);

    if (alutGetError() != ALUT_ERROR_NO_ERROR)
    {
        fprintf(stderr, "Could not load %s\n", filename);
        return -3;
    }

    alBufferData(mBuffer[mNextBuffer], format, data, size, static_cast<ALsizei>(freq));

    alSourcei(mSource[mNextSource], AL_BUFFER, mBuffer[mNextBuffer]);

    if (flags & SoundFlagsLoop)
    {
        alSourcei(mSource[mNextSource], AL_LOOPING, 1);
    }

    ++mNextBuffer;
    ++mNextSource;

    *source = mNextBuffer;

    return 0;
}

int Sound::addWave(unsigned char *wav, unsigned int length, int *source, unsigned int flags)
{
    ALsizei size;
    ALfloat freq;
    ALenum format;
    ALvoid *data;
    int error = 0;

    if (!mInit || !wav || !source)
    {
        printf("Sound::AddWave> ERROR pre condition assertion failed\n");
        return -1000;
    }

    *source = -1;

    data = wav;

    alGetError();

    alGenBuffers(1, &mBuffer[mNextBuffer]);

    if (alGetError() != AL_NO_ERROR)
    {
        fprintf(stderr, "Sound::AddWave> alGenBuffers call failed\n");
        return -1;
    }

    alGetError();

    alGenSources(1, &mSource[mNextSource]);

    if (alGetError() != AL_NO_ERROR)
    {
        fprintf(stderr, "Sound::AddWave> alGenSources call failed\n");
        return -2;
    }

    //AL_FORMAT_WAVE_EXT does not exist on Mac!"
    // alBufferData(mBuffer[mNextBuffer], AL_FORMAT_WAVE_EXT, data, size, freq);
    // Idea: Fill Buffer with
    // alutLoadMemoryFromFileImage
    //     (const ALvoid *data, ALsizei length, ALenum *format, ALsizei *size, ALfloat *frequency)

    data = alutLoadMemoryFromFileImage(wav, length, &format, &size, &freq);

    if (((error = alutGetError()) != ALUT_ERROR_NO_ERROR) || (data == NULL)) {
        fprintf(stderr, "Could not load wav buffer (%s)\n", alutGetErrorString(error));
        return -3;
    }


    alBufferData(mBuffer[mNextBuffer], format, data, size, static_cast<ALsizei>(freq));

    alSourcei(mSource[mNextSource], AL_BUFFER, mBuffer[mNextBuffer]);

    if (flags & SoundFlagsLoop)
    {
        alSourcei(mSource[mNextSource], AL_LOOPING, 1);
    }

    ++mNextBuffer;
    ++mNextSource;

    *source = mNextBuffer;

    //! \fixme Should free alut buffer?

    return 0;
}


void Sound::play(int source)
{
    if (!mInit)
    {
        printf("Sound::Play> ERROR: Sound not initialized\n");
        return;
    }

    if (source < 0)
    {
        printf("Sound::Play> ERROR: Source Id invalid\n");
        return;
    }

    alSourcePlay(mSource[source-1]);
}


void Sound::stop(int source)
{
    if (!mInit || source < 0)
    {
        printf("Sound::Stop> ERROR pre condition assertion failed\n");
        return;
    }

    alSourceStop(mSource[source-1]);
}

