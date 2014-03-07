/*!
 * \file include/Sound.h
 * \brief This is the audio manager Header
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _SOUND_H_
#define _SOUND_H_

/*!
 * \brief This is the audio manager for OpenRaider
 */
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
     * \brief Get number of registered sources
     * \returns number of registered sources
     */
    int registeredSources();

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
    int addFile(const char *filename, int *source, unsigned int flags);

    /*!
     * \brief Load wav file from buffer
     * \param wav not NULL! Is a valid waveform buffer!
     * \param length length of wav buffer
     * \param source not NULL! Returns new source ID or -1 on error.
     * \param flags (un)set options. Use SoundFlags enum
     * \returns 0 for no error or < 0 error flag
     */
    int addWave(unsigned char *wav, unsigned int length, int *source, unsigned int flags);

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
    bool mInit;                //!< Guard to ensure ausio system is active
    unsigned int mBuffer[256]; //!< Audio buffer id list
    unsigned int mSource[256]; //!< Audio source id list
    int mNext;                 //!< Audio id cursor
};

#endif
