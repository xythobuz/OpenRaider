/*!
 * \file Sound.cpp
 * \brief This is the audio manager Unit Test
 *
 * \author Mongoose
 * \author xythobuz
 */
#include <stdio.h>
#include <unistd.h>

#include <Sound.h>

/*!
 * \brief Sound Unit Test
 * \param argc Number of arguments. Has to be > 1
 * \param argv Argument array. argv[1] will be played as wav file
 * \returns always zero
 */
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
		ret = snd.addFile(argv[1], &id, snd.SoundFlagsNone);
		printf("Load returned %i\n", ret);
		printf("Playing %u::%s\n", id, argv[1]);
		snd.play(id);

		printf("Waiting...\n");
		sleep(1);

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
			ret = snd.addWave(buf, l, &id, snd.SoundFlagsNone);
			printf("Load returned %i\n", ret);
			printf("Playing buffer of %u::%s\n", id, argv[1]);
			snd.play(id);

			delete [] buf;
			printf("Waiting...\n");
			sleep(1);
		}
	}
	else
	{
		printf("%s filename.wav\n", argv[0]);
	}

	return 0;
}

