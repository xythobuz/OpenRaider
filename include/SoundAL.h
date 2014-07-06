/*!
 * \file include/SoundAL.h
 * \brief This is the OpenAL audio manager Header
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _SOUND_AL_H_
#define _SOUND_AL_H_

#include <vector>

#include "Sound.h"

/*!
 * \brief This is the OpenAL audio manager
 */
class SoundAL : public Sound {
public:

    /*!
     * \brief Constructs an object of SoundAL
     */
    SoundAL();

    /*!
     * \brief Deconstructs an object of SoundAL
     */
    virtual ~SoundAL();

    /*!
     * \brief Initialize sound system
     * \returns 0 on success or < 0 error flags
     */
    virtual int initialize();

    virtual void setEnabled(bool on);

    /*!
     * \brief Set the volume
     * \param vol new source gain
     */
    virtual void setVolume(float vol);

    /*!
     * \brief Get number of registered sources
     * \returns number of registered sources
     */
    virtual unsigned long registeredSources();

    /*!
     * \brief Remove all loaded sounds
     */
    virtual void clear();

    /*!
     * \brief Move listener and repositions them
     * \param pos New position for listener
     * \param angle New orientation for listener
     */
    virtual void listenAt(float pos[3], float angle[3]);

    /*!
     * \brief Move sound source to position
     * \param source valid source id
     * \param pos new position for source
     */
    virtual void sourceAt(unsigned long source, float pos[3]);

    /*!
     * \brief Load wav file from disk
     * \param filename not NULL!
     * \param source not NULL! Returns new source ID or -1 on error.
     * \param flags set options. Use SoundFlags enum bitwise OR-ed
     * \returns 0 for no error or < 0 error flag
     */
    virtual int addFile(const char *filename, unsigned long *source, unsigned int flags);

    /*!
     * \brief Load wav file from buffer
     * \param wav not NULL! Is a valid waveform buffer!
     * \param length length of wav buffer
     * \param source not NULL! Returns new source ID or -1 on error.
     * \param flags set options. Use SoundFlags enum bitwise OR-ed
     * \returns 0 for no error or < 0 error flag
     */
    virtual int addWave(unsigned char *wav, unsigned int length, unsigned long *source, unsigned int flags);

    /*!
     * \brief Play sound source
     * \param source sound source to play
     */
    virtual void play(unsigned long source);

    /*!
     * \brief Stop playing sound source
     * \param source sound source to stop
     */
    virtual void stop(unsigned long source);

private:

    bool mEnabled;
    bool mInit;                         //!< Guard to ensure ausio system is active
    float mVolume;                      //!< Listener gain
    std::vector<unsigned int> mBuffer; //!< Audio buffer id list
    std::vector<unsigned int> mSource; //!< Audio source id list
};

#endif

