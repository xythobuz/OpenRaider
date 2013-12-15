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
 *-- Test Defines -----------------------------------------------
 *           
 * UNIT_TEST_SOUND - Builds Sound class as a console unit test 
 *
 *-- History ---------------------------------------------------- 
 *
 * 2002.09.13:
 * Mongoose - API follows new code style guidelines
 *
 * 2002.01.03:
 * Mongoose - Flags use added
 *
 * 2001.05.23:
 * Mongoose - Created
 ================================================================*/

#ifndef __OPENRAIDER_MONGOOSE_SOUND_H_
#define __OPENRAIDER_MONGOOSE_SOUND_H_

typedef enum
{
	SoundFlagsNone = 0,            /* No FX */
	SoundFlagsLoop = 1             /* Enable looping during playback */

} SoundFlags;


class Sound
{
 public:

  Sound();
  /*------------------------------------------------------
   * Pre  : 
   * Post : Constructs an object of Sound
   *
   *-- History ------------------------------------------
   *
   * 2001.05.23: 
   * Mongoose - Created
   ------------------------------------------------------*/

  ~Sound();
  /*------------------------------------------------------
   * Pre  : Sound object is allocated
   * Post : Deconstructs an object of Sound
   *
   *-- History ------------------------------------------
   *
   * 2001.05.23: 
   * Mongoose - Created
   ------------------------------------------------------*/

  int init();
  /*------------------------------------------------------
   * Pre  : 
   * Post : Initializes sound system
	*
	*        No error returns 0 all others returns < 0
	*        error flags
   *
   *-- History ------------------------------------------
   *
   * 2001.05.23: 
   * Mongoose - Created
   ------------------------------------------------------*/

  void listenAt(float pos[3], float angle[3]);
  /*------------------------------------------------------
   * Pre  : pos and angles are valid for listener
   * Post : Moves listener and repositions them
   *
   *-- History ------------------------------------------
   *
   * 2001.05.23: 
   * Mongoose - Created
   ------------------------------------------------------*/

  void sourceAt(int source, float pos[3]);
  /*------------------------------------------------------
   * Pre  : source and pos are valid for source
   * Post : Moves sound source to position
   *
   *-- History ------------------------------------------
   *
   * 2001.05.23: 
   * Mongoose - Created
   ------------------------------------------------------*/

  int add(char *filename, int *source, unsigned int flags);
  /*------------------------------------------------------
   * Pre  : filename and source aren't NULL
	*        flags (un)set options
	*
   * Post : Loads wav file from disk
	*
	*        Source returns it's sound source id, 
	*        source id is set -1 for error
	*
	*        Returns either 0 for no error or < 0 error 
	*        flag
   *
   *-- History ------------------------------------------
   *
   * 2001.05.23: 
   * Mongoose - Created
   ------------------------------------------------------*/

  // FIXME: Not implemented yet
  int add(unsigned char *wav, int *source, unsigned int flags);
  /*------------------------------------------------------
   * Pre  : wav and source aren't NULL
	*        wav is a valid waveform buffer
	*
	*        flags (un)set options
	*
   * Post : Loads wav file from buffer
	*
	*        Source returns it's sound source id, 
	*        source id is set -1 for error
	*
	*        Returns either 0 for no error or < 0 error 
	*        flag
   *
   *-- History ------------------------------------------
   *
   * 2001.05.23: 
   * Mongoose - Created
   ------------------------------------------------------*/

  void play(int source);
  /*------------------------------------------------------
   * Pre  : 
   * Post : Play sound source
   *
   *-- History ------------------------------------------
   *
   * 2001.05.23: 
   * Mongoose - Created
   ------------------------------------------------------*/

  void stop(int source);
  /*------------------------------------------------------
   * Pre  : 
   * Post : Stop playing sound source
   *
   *-- History ------------------------------------------
   *
   * 2001.05.23: 
   * Mongoose - Created
   ------------------------------------------------------*/

 private:

  bool mInit;                    /* Guard to ensure ausio system is active */

  unsigned int mBuffer[256];     /* Audio buffer id list */

  unsigned int mSource[256];     /* Audio source id list */

  unsigned int mNextBuffer;      /* Audio buffer id cursor */

  unsigned int mNextSource;      /* Audio source id cursor */
};

#endif
