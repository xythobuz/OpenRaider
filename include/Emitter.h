/*!
 * \file include/Emitter.h
 * \brief Particle emitter class.
 *
 * \author Mongoose
 */

#ifndef _EMITTER_H_
#define _EMITTER_H_

#include "Particle.h"

/*!
 * \brief Particle emitter class.
 */
class Emitter {
public:

    /*!
     * \brief Flags an Emitter can have
     */
    typedef enum {
        fUseFrustumCulling = (1 << 0), //!< Use frustum culling
        fUseDepthSorting   = (1 << 1)  //!< Use depth sorting
    } EmitterFlags;


    /*!
     * \brief Constructs an object of Emitter
     * \param name valid C string
     * \param n greater than 0
     */
    Emitter(const char *name, int n);

    /*!
     * \brief Deconstructs an object of Emitter
     */
    ~Emitter();

    /*!
     * \brief Get the particles
     * \returns array of Particles
     */
    Particle *Particles();

    /*!
     * \brief Number of particles emitted
     * \returns Number of particles emitted
     */
    int Count();

    /*!
     * \brief Sets position of emitter in 3D space
     * \param x X coordinate
     * \param y Y coordinate
     * \param z Z coordinate
     */
    void Pos(float x, float y, float z);

    /*!
     * \brief Returns position of emitter in 3D space
     * \param x Where X coordinate will be stored
     * \param y Where Y coordinate will be stored
     * \param z Where Z coordinate will be stored
     */
    void Pos(float *x, float *y, float *z);

    /*!
     * \brief Sets orientation of emitter in 3D space
     * \param x X coordinate
     * \param y Y coordinate
     * \param z Z coordinate
     */
    void Orientation(float x, float y, float z);

    /*!
     * \brief Returns orientation of emitter in 3D space
     * \param x Where X coordinate will be stored
     * \param y Where Y coordinate will be stored
     * \param z Where Z coordinate will be stored
     */
    void Orientation(float *x, float *y, float *z);

    /*!
     * \brief Get the flags of this Emitter
     * \returns EmitterFlags
     */
    unsigned int Flags();

    /*!
     * \brief Set or Unset a flag
     * \param flag EmitterFlag to change
     * \param op new state (true - set)
     */
    void Flags(unsigned int flag, bool op);

    /*!
     * \brief Allocates the particle array and sets the count.
     * If the array has been allocated previously it will be
     * deallocated and a new one made.
     * \param n new size, greater than 0
     */
    void ParticleArray(int n);

    /*!
     * \brief Renders the particles
     */
    void Draw();

    /*!
     * \brief Sets the emitters name
     * \param name is a valid C string
     */
    void Name(const char *name);

    /*!
     * \brief Resets all particle texture ids
     * \param id new id
     * \sa Particle::TextureId()
     */
    void SetTextureId(int id);

    /*!
     * \brief Set the texture id for a range of particles in the array
     * \param particle_start start of range
     * \param particle_end end of range
     * \param id new id
     * \sa Particle::TextureId()
     */
    void TextureId(unsigned int particle_start, unsigned int particle_end, int id);

    /*!
     * \brief Set the color of a range of particles in the array
     * \param particle_start start of range
     * \param particle_end end of range
     * \param r new red part of color (0.0 to 1.0)
     * \param g new green part of color (0.0 to 1.0)
     * \param b new blue part of color (0.0 to 1.0)
     * \sa Particle::Color()
     */
    void Color(unsigned int particle_start, unsigned int particle_end, float r, float g, float b);

    /*!
     * \brief Set the speed of a range of particles in the array.
     * Take note that the speed starts out at 2000, and lower means faster.
     * \param particle_start start of range
     * \param particle_end end of range
     * \param x X speed
     * \param y Y speed
     * \param z Z speed
     * \sa Particle::Speed
     */
    void Speed(unsigned int particle_start, unsigned int particle_end, float x, float y, float z);

    /*!
     * \brief Let a force (eg. Gravity) act on a range of particles in the array
     * \param particle_start start of range
     * \param particle_end end of range
     * \param x X force
     * \param y Y force
     * \param z Z force
     * \sa Particle::Force()
     */
    void Force(unsigned int particle_start, unsigned int particle_end, float x, float y, float z);

    static vec_t mFrustum[6][4]; //!< View Volume copy

private:
    static int compareParticleDist(const void *voidA, const void *voidB);

    char *_name;         //!< Emitter name
    unsigned int _flags; //!< Emitter flags
    vec3_t _pos;         //!< Position in 3D space
    vec3_t _mangle;      //!< Oreintation in 3D space
    Particle *_particle; //!< Array of particles
    unsigned int _count; //!< Particle count
};

#endif
