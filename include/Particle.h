/*!
 * \file include/Particle.h
 * \brief Particle system base header
 *
 * \author Mongoose
 */

#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "MatMath.h"

/*!
 * \brief Partcle systems atomic base
 */
class Particle {
public:

    /*!
     * \brief Constructs an object of Sound
     */
    Particle();

    /*!
     * \brief Deconstructs an object of Sound
     */
    ~Particle();

    /*!
     * \brief Toggles active state of particle
     * \param active new state
     */
    void setActive(bool active);

    /*!
     * \brief Sets gravity/force acting on particle
     * \param x X part of force vector
     * \param y Y part
     * \param z Z part
     */
    void Force(float x, float y, float z);

    /*!
     * \brief Resets particle to defaults
     */
    void Reset();

    /*!
     * \brief Sets gravity/force acting on particle
     * \note speed inits at 2000, lower is faster
     * \param x X part of speed vector
     * \param y Y part
     * \param z Z part
     */
    void Speed(float x, float y, float z);

    /*!
     * \brief Sets new particle coloring
     * \note White {1.0, 1.0, 1.0} is the init color
     * \param r Red part, from 0.0 to 1.0
     * \param g Green part, from 0.0 to 1.0
     * \param b Blue part, from 0.0 to 1.0
     */
    void Color(float r, float g, float b);

    /*!
     * \brief Returns position of particle in 3 space
     * \param x not NULL!
     * \param y not NULL!
     * \param z not NULL!
     */
    void Pos(float *x, float *y, float *z);

    /*!
     * \brief Returns current color of particle
     * \param r not NULL!
     * \param g not NULL!
     * \param b not NULL!
     */
    void Color(float *r, float *g, float *b);

    /*!
     * \brief Returns current life (blend) of particle
     * \returns fade of particle
     */
    float Life();

    /*!
     * \brief Adjusts for particle life cycle
     */
    void Update();

    /*!
     * \brief Returns texture binding for this particle
     * \returns texture id
     */
    int Texture();

    /*!
     * \brief Returns active value
     * \returns state
     */
    bool isActive();

    /*!
     * \brief Set the texture for this particle
     * \param t new texture id
     */
    void TextureId(int t);


private:
    bool _active;  //!< Is this particle in use?
    float _life;   //!< Life of particle
    float _blend;  //!< Blend amount or fade
    int _texture;  //!< Texture polygon to use
    vec3_t _pos;   //!< Current position in 3 space
    vec3_t _color; //!< Current color
    vec3_t _dir;   //!< Current direction
    vec3_t _force; //!< Current force or 'gravity'
    vec3_t _speed; //!< Speed of particle movement
};

#endif
