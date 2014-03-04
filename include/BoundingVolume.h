/*!
 * \file include/BoundingVolume.h
 * \brief Bounding volume class for world/vis/culling/clipping/collision stuff.
 *
 * \author Mongoose
 */

#ifndef _BOUNDINGVOLUME_H_
#define _BOUNDINGVOLUME_H_

#include "MatMath.h"

/*!
 * \brief Defines a 3D sphere.
 */
class BoundingSphere {
public:
    vec3_t mCenter; //!< Center of bounding sphere
    vec_t mRadius; //!< Raduis of bounding sphere
};

/*!
 * \brief Defines a 3D rectangle.
 */
class BoundingBox {
public:
    vec3_t mMin; //!< Bounding box MIN point
    vec3_t mMax; //!< Bounding box MAX point
};

/*!
 * \brief Defines a 3D sphere and/or rectangle.
 */
class BoundingVolume {
public:
    BoundingSphere mSphere; //!< Bounding sphere of this volume
    BoundingBox mBox; //!< Bounding box of this volume
};

#endif
