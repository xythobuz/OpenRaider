/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 * 
 * Project : OpenRaider
 * Author  : Mongoose
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Sound
 * License : No use w/o permission (C) 2001 Mongoose
 * Comments: This is the audio manager for OpenRaider
 *
 *
 *           This file was generated using Mongoose's C++ 
 *           template generator script.  <stu7440@westga.edu>
 * 
 *-- History ------------------------------------------------- 
 *
 * 2001.05.23:
 * Mongoose - Created
 =================================================================*/

#ifdef HAVE_OPENAL
#   include <AL/al.h>
#   include <AL/alc.h>
#   include <AL/alut.h>
#   include <AL/alext.h>
#endif

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "Sound.h"

#ifdef DEBUG_MEMEORY
#   include "memeory_test.h"
#endif



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
#ifdef HAVE_OPENAL
		alutExit();
#endif
	}
}


int Sound::init()
{
	int fd;	

	fd = open("/dev/dsp", O_RDWR);

	if (fd < 0)
	{
		perror("Sound::Init> Could not open /dev/dsp : ");
		return -1;
	}

	close(fd);

#ifdef HAVE_OPENAL
	alutInit(NULL, 0);

	mInit = true;
	printf("@Created OpenAL Context...\n");
#else
	printf("*Couldn't create sound Context...\n");
#endif

	return 0;
}


void Sound::listenAt(float pos[3], float angle[3])
{
	if (!mInit)
		return;

#ifdef HAVE_OPENAL
	alListenerfv(AL_POSITION, pos);
	alListenerfv(AL_ORIENTATION, angle);	
#endif
}


void Sound::sourceAt(int source, float pos[3])
{
	if (!mInit || source < 0)
		return;

#ifdef HAVE_OPENAL
	alSourcefv(mSource[source-1], AL_POSITION, pos);
#endif
}


// Mongoose 2002.01.04, FIXME seperate sourcing and buffering
int Sound::add(char *filename, int *source, unsigned int flags)
{
#ifdef HAVE_OPENAL
   ALsizei size, freq, bits;
   ALenum format;
   ALvoid *data;
   ALboolean err;
#endif


	if (!mInit || !filename || !source)
	{
		printf("Sound::Add> ERROR pre condition assertion failed\n");
		return -1000;
	}

	*source = -1;

#ifdef HAVE_OPENAL
   alGetError();

	alGenBuffers(1, &mBuffer[mNextBuffer]);

	if (alGetError() != AL_NO_ERROR)	
   {
		fprintf(stderr, "Sound::Init> alGenBuffers call failed\n");
		return -1;
	}

   alGetError();

   alGenSources(1, &mSource[mNextSource]);

	if (alGetError() != AL_NO_ERROR)
   {
		fprintf(stderr, "Sound::Init> alGenSources call failed\n");
		return -2;
	}
   
   err = alutLoadWAV(filename, &data, &format, &size, &bits, &freq);

   if (err == AL_FALSE) 
	{
	   fprintf(stderr, "Could not load %s\n", filename);
	   return -3;
   }

   alBufferData(mBuffer[mNextBuffer], format, data, size, freq);

   alSourcei(mSource[mNextSource], AL_BUFFER, mBuffer[mNextBuffer]);

	if (flags & SoundFlagsLoop)
	{
		alSourcei(mSource[mNextSource], AL_LOOPING, 1);
	}

   ++mNextBuffer;
   ++mNextSource;

	*source = mNextBuffer;

   return 0;
#else
	return -1;
#endif
}


int Sound::add(unsigned char *wav, int *source, unsigned int flags)
{
#ifdef HAVE_OPENAL
   ALsizei size, freq;
   ALvoid *data;
#endif

	if (!mInit || !wav || !source)
	{
		printf("Sound::Add> ERROR pre condition assertion failed\n");
		return -1000;
	}

	*source = -1;

#ifdef HAVE_OPENAL
	data = wav;

   alGetError();

	alGenBuffers(1, &mBuffer[mNextBuffer]);

	if (alGetError() != AL_NO_ERROR)	
   {
		fprintf(stderr, "Sound::Init> alGenBuffers call failed\n");
		return -1;
	}

   alGetError();

   alGenSources(1, &mSource[mNextSource]);

	if (alGetError() != AL_NO_ERROR)
   {
		fprintf(stderr, "Sound::Init> alGenSources call failed\n");
		return -2;
	}
   
	// AL_FORMAT_WAVE_EXT
   alBufferData(mBuffer[mNextBuffer], AL_FORMAT_WAVE_EXT, data, size, freq);

   alSourcei(mSource[mNextSource], AL_BUFFER, mBuffer[mNextBuffer]);

	if (flags & SoundFlagsLoop)
	{
		alSourcei(mSource[mNextSource], AL_LOOPING, 1);
	}

   ++mNextBuffer;
   ++mNextSource;

	*source = mNextBuffer;

   return 0;
#else
	return -1;
#endif
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

#ifdef HAVE_OPENAL
	alSourcePlay(mSource[source-1]);
#endif
}


void Sound::stop(int source)
{
	if (!mInit || source < 0)
	{
		printf("Sound::Stop> ERROR pre condition assertion failed\n");
		return;
	}

#ifdef HAVE_OPENAL
	alSourceStop(mSource[source-1]);
#endif
}


///////////////////////////////////////////////////////


#ifdef UNIT_TEST_SOUND
int main(int argc, char* argv[]) 
{
	Sound snd;
	FILE *f;
	unsigned int l;
	unsigned char *buf;
   int id, ret;


	if (argc > 1)
	{
		snd.init();
		printf("Loading %s\n", argv[1]);
		ret = snd.add(argv[1], &id, SoundFlagsNone);
		printf("Load returned %i\n", ret);
		printf("Playing %u::%s\n", id, argv[1]);
		snd.play(id);

		printf("Waiting...\n");
		sleep(5);

		f = fopen(argv[1], "rb");

		if (f)
		{
			fseek(f, 0, SEEK_END);
			l = ftell(f);
			fseek(f, 0, SEEK_SET);

			buf = new unsigned char[l];
			fread(buf, l, 1, f);

			fclose(f);
			
			printf("Loading buffer of %s\n", argv[1]);
			ret = snd.add(buf, &id, SoundFlagsNone);
			printf("Load returned %i\n", ret);
			printf("Playing buffer of %u::%s\n", id, argv[1]);
			snd.play(id);

			delete [] buf;
			printf("Waiting...\n");
			sleep(5);
		}
	}
	else
	{
		printf("%s filename.wav\n", argv[0]);
	}

	return 0;
}
#endif

