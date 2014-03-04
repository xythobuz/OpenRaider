/*!
 * \file include/Vector3d.h
 * \brief 3D Math vector
 *
 * \author Mongoose
 */

#ifndef _VECTOR3D_H_
#define _VECTOR3D_H_

#include "MatMath.h"

/*!
 * \brief 3D Math Vector
 */
class Vector3d {
public:

    /*!
     * \brief Constructs an object of Vector3d
     */
    Vector3d();

    /*!
     * \brief Constructs an object of Vector3d
     * \param v data to load into new Vector3d
     */
    Vector3d(vec3_t v);

    /*!
     * \brief Constructs an object of Vector3d
     * \param x X part of new Vector3d
     * \param y Y part of new Vector3d
     * \param z Z part of new Vector3d
     */
    Vector3d(vec_t x, vec_t y, vec_t z);

    /*!
     * \brief Deconstructs an object of Vector3d
     */
    ~Vector3d();

    /*!
     * \brief Calculate dot product
     * \param u first argument
     * \param v second argument
     * \returns dot product of u and v vectors
     */
    static vec_t dot(const Vector3d &u, const Vector3d &v);

    /*!
     * \brief Calculate cross product
     * \param u first argument
     * \param v second argument
     * \returns cross product of u and v vectors
     */
    static Vector3d cross(const Vector3d &u, const Vector3d &v);

    /*!
     * \brief Get Magnitude
     * \returns magnitude of this vector
     */
    vec_t magnitude();

    /*!
     * \brief Normalize
     * \returns normalized copy of this vector
     */
    Vector3d unit();

    /*!
     * \brief Get the Zero vector
     * \returns (0, 0, 0) vector
     */
    static Vector3d zeroVector();

    /*!
     * \brief Add to this vector
     * \param v addend
     * \returns a vector = this vector + v
     */
    Vector3d operator +(const Vector3d &v);

    /*!
     * \brief Subtract from this vector
     * \param v subtrahend
     * \returns a vector = this vector - v
     */
    Vector3d operator -(const Vector3d &v);

    /*!
     * \brief Negate this vector
     * \returns a copy of this vector, negated
     */
    Vector3d operator -();

    /*!
     * \brief Scale this vector
     * \param s scaling factor
     * \returns this vector multiplied with s
     */
    Vector3d operator *(vec_t s);

    /*!
     * \brief Scale this vactor
     * \param s inverse scaling factor
     * \returns this vector divided by s
     */
    Vector3d operator /(vec_t s);

    /*!
     * \brief Dot product this vector
     * \param v second vector for dot product
     * \returns dot product of V by this vector
     */
    vec_t operator *(const Vector3d &v);

    /*!
     * \brief Normalizes this vector
     */
    void normalize();

    /*!
     * \brief Set this vector to Zero (0, 0, 0)
     */
    void zero();

    /*!
     * \brief Set this vector
     * \param v what this vector will be set to
     * \returns this vector, now equal to v
     */
    Vector3d &operator =(const Vector3d &v);

    /*!
     * \brief Add to this vector, in place
     * \param v what will be added to this vector
     * \returns this vector, with v added
     */
    Vector3d &operator +=(const Vector3d &v);

    /*!
     * \brief Subtract from this vector, in place
     * \param v what will be subtracted from this vector
     * \returns this vector, with v subtracted
     */
    Vector3d &operator -=(const Vector3d &v);

    /*!
     * \brief Scale this vector, in place
     * \param s scaling factor
     * \returns this vactor multiplied by s
     */
    Vector3d &operator *=(vec_t s);

    vec3_t mVec; //!< Vector data
};

#endif
