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

class Sound {
public:

    typedef enum {
        SoundFlagsNone = 0, //!< No FX
        SoundFlagsLoop = 1  //!< Enable looping during playback
    } SoundFlags;

    /*!
     * \brief Constructs an object of Sound
     */
    Sound();

    /*!
     * \brief Deconstructs an object of Sound
     */
    ~Sound();

    /*!
     * \brief Initialize sound system
     * \returns 0 on success or < 0 error flags
     */
    int init();

    /*!
     * \brief Move listener and repositions them
     * \param pos New position for listener
     * \param angle New orientation for listener
     */
    void listenAt(float pos[3], float angle[3]);

    /*!
     * \brief Move sound source to position
     * \param source valid source id
     * \param pos new position for source
     */
    void sourceAt(int source, float pos[3]);

    /*!
     * \brief Load wav file from disk
     * \param filename not NULL!
     * \param source not NULL! Returns new source ID or -1 on error.
     * \param flags (un)set options. Use SoundFlags enum
     * \returns 0 for no error or < 0 error flag
     */
    int addFile(char *filename, int *source, unsigned int flags);

    /*!
     * \brief Load wav file from buffer
     * \param wav not NULL! Is a valid waveform buffer!
     * \param source not NULL! Returns new source ID or -1 on error.
     * \param flags (un)set options. Use SoundFlags enum
     * \returns 0 for no error or < 0 error flag
     */
    int addWave(unsigned char *wav, int *source, unsigned int flags);

    /*!
     * \brief Play sound source
     * \param source sound source to play
     */
    void play(int source);

    /*!
     * \brief Stop playing sound source
     * \param source sound source to stop
     */
    void stop(int source);

private:
    bool mInit;                    //!< Guard to ensure ausio system is active
    unsigned int mBuffer[256];     //!< Audio buffer id list
    unsigned int mSource[256];     //!< Audio source id list
    unsigned int mNextBuffer;      //!< Audio buffer id cursor
    unsigned int mNextSource;      //!< Audio source id cursor
};

#endif
