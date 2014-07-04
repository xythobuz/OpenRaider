/*!
 * \file include/math/Quaternion.h
 * \brief Quaternion
 *
 * \author Mongoose
 */

#ifndef _MATH_QUATERNION_H_
#define _MATH_QUATERNION_H_

#include "math/math.h"

/*!
 * \brief Quaternion
 */
class Quaternion {
public:

    /*!
     * \brief Constructs an object of Quaternion
     */
    Quaternion();

    /*!
     * \brief Constructs an object of Quaternion
     * \param w W part of new Quaternion
     * \param x X part of new Quaternion
     * \param y Y part of new Quaternion
     * \param z Z part of new Quaternion
     */
    Quaternion(float w, float x, float y, float z);

    /*!
     * \brief Constructs an object of Quaternion
     * \param v contents of new Quaternion
     */
    Quaternion(float v[4]);

    /*!
     * \brief Get column order matrix equivalent of this quaternion
     * \param m where matrix will be stored
     */
    void getMatrix(float m[16]);

    /*!
     * \brief Multiplies this quaternion.
     *
     * Use normalize() call for unit quaternion.
     *
     * \param q what to multiply this quaternion with
     * \returns resultant quaternion
     * \sa Quaternion::normalize()
     */
    Quaternion operator *(const Quaternion &q);

    /*!
     * \brief Divide from this quaternion
     * \param q what to divide from this quaternion
     * \returns resultant quaternion
     */
    Quaternion operator /(const Quaternion &q);

    /*!
     * \brief Add to this quaternion
     * \param q what to add to this quaternion
     * \returns resultant quaternion
     */
    Quaternion operator +(const Quaternion &q);

    /*!
     * \brief Subtract from this quaternion
     * \param q what to subtract from this quaternion
     * \returns resultant quaternion
     */
    Quaternion operator -(const Quaternion &q);

    /*!
     * \brief Compares q to this quaternion
     * \param q what to compare this quaternion to
     * \returns true if equal, false otherwise
     */
    bool operator ==(const Quaternion &q);

    /*!
     * \brief Conjugate this quaternion
     * \returns Conjugate of this quaternion
     */
    Quaternion conjugate();

    /*!
     * \brief Scale this quaternion
     * \param s scaling factor
     * \returns Scaled result of this quaternion
     */
    Quaternion scale(float s);

    /*!
     * \brief Inverse this quaternion
     * \returns inverse of this quaternion
     */
    Quaternion inverse();

    /*!
     * \brief Dot Product of quaternions
     * \param a first argument to dot product
     * \param b second argument to dot product
     * \returns dot product between a and b quaternions
     */
    static float dot(Quaternion a, Quaternion b);

    /*!
     * \brief Magnitude of this quaternion
     * \returns Magnitude of this quaternion
     */
    float magnitude();

    /*!
     * \brief Interpolates between a and b rotations.
     *
     * Using spherical linear interpolation:
     * `I = (((B . A)^-1)^Time)A`
     *
     * \param a first argument for slerp
     * \param b second argument for slerp
     * \param time time argument for slerp
     * \returns resultant quaternion
     */
    static Quaternion slerp(Quaternion a, Quaternion b, float time);

    /*!
     * \brief Sets this quaternion to identity
     */
    void setIdentity();

    /*!
     * \brief Sets this quaternion
     * \param angle new angle
     * \param x new X coordinate
     * \param y new Y coordinate
     * \param z new Z coordinate
     */
    void set(float angle, float x, float y, float z);

    /*!
     * \brief Normalize this quaternion
     */
    void normalize();

    /*!
     * \brief Set this quaternion
     * \param q will be copied into this quaternion
     */
    void copy(Quaternion q);

    /*!
     * \brief Sets matrix equivalent of this quaternion
     * \param m matrix in valid column order
     */
    void setByMatrix(float m[16]);

private:

    /*!
     * \brief Multiplies two quaternions
     * \param a first argument to multiplication
     * \param b second argument to multiplication
     * \returns resultant quaternion
     */
    static Quaternion multiply(Quaternion a, Quaternion b);

    /*!
     * \brief Divides B from A quaternion
     * \param a first argument to division
     * \param b second argument to division
     * \returns quotient quaternion
     */
    static Quaternion divide(Quaternion a, Quaternion b);

    /*!
     * \brief Adds A and B quaternions
     * \param a first argument to addition
     * \param b second argument to addition
     * \returns resultant quaternion
     */
    static Quaternion add(Quaternion a, Quaternion b);

    /*!
     * \brief Subtracts B from A quaternion
     * \param a first argument to subtraction
     * \param b second argument to subtraction
     * \returns resultant quaternion
     */
    static Quaternion subtract(Quaternion a, Quaternion b);

    float mW; //!< Quaternion, W part
    float mX; //!< Quaternion, X part
    float mY; //!< Quaternion, Y part
    float mZ; //!< Quaternion, Z part
};

#endif

