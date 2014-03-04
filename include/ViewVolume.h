/*!
 * \file include/ViewVolume.h
 * \brief Viewing Volume for culling use
 *
 * \author Mongoose
 */

#ifndef _VIEWVOLUME_H_
#define _VIEWVOLUME_H_

#include "Matrix.h"
#include "BoundingVolume.h"

/*!
 * \brief Viewing Volume for culling use
 */
class ViewVolume {
public:

    /*!
     * \brief Sides of the view volume
     */
    enum ViewVolumeSide {
        rightSide  = 0, //!< Right
        leftSide   = 1, //!< Left
        bottomSide = 2, //!< Bottom
        topSide    = 3, //!< Top
        farSide    = 4, //!< Back
        nearSide   = 5  //!< Front
    };

    /*!
     * \brief Planes of the view volume
     */
    enum ViewVolumePlane {
        planeA = 0, //!< X value of normal
        planeB = 1, //!< Y value of normal
        planeC = 2, //!< Z value of normal
        planeD = 3  //!< Distance to origin
    };

    /*!
     * \brief Constructs an object of ViewVolume
     */
    ViewVolume();

    /*!
     * \brief Deconstructs an object of ViewVolume
     */
    ~ViewVolume();

    /*!
     * \brief Check if bounding volume is in view volume
     * \param bvol bounding volume to check
     * \returns true if frustum contains the given bounding volume
     */
    bool isBoundingVolumeInFrustum(BoundingVolume bvol);

    /*!
     * \brief Check if bounding sphere is in view volume
     * \param bvol bounding sphere to check
     * \returns true if frustum contains the given bounding volume
     */
    bool isBoundingSphereInFrustum(BoundingSphere bvol);

    /*!
     * \brief Check if bounding box is in view volume
     * \param bvol bounding box to check
     * \returns true if frustum contains the given bounding volume
     */
    bool isBoundingBoxInFrustum(BoundingBox bvol);

    /*!
     * \brief Check if point is in view volume
     * \param x X coordinate of point
     * \param y Y coordinate of point
     * \param z Z coordinate of point
     * \returns true if point in view volume
     */
    bool isPointInFrustum(vec_t x, vec_t y, vec_t z);

    /*!
     * \brief Check if bounding sphere is in view volume
     * \param x X coordinate of a valid abstract sphere
     * \param y Y coordinate of a valid abstract sphere
     * \param z Z coordinate of a valid abstract sphere
     * \param radius radius of a valid abstract sphere
     * \returns true if abstract sphere in view volume
     */
    bool isSphereInFrustum(vec_t x, vec_t y, vec_t z, vec_t radius);

    /*!
     * \brief Check if bounding box is in view volume
     * \param min minimum point of valid abstract bounding box
     * \param max maximum point of valid abstract bounding box
     * \returns true if abstract bounding box in view volume
     */
    bool isBboxInFrustum(vec3_t min, vec3_t max);

    /*!
     * \brief Distance to Bounding sphere
     * \param x X coordinate of a valid abstract sphere
     * \param y Y coordinate of a valid abstract sphere
     * \param z Z coordinate of a valid abstract sphere
     * \param radius radius of a valid abstract sphere
     * \returns distance to abstract sphere bounding volume
     */
    vec_t getDistToSphereFromNear(vec_t x, vec_t y, vec_t z, vec_t radius);

    /*!
     * \brief Distance to Bounding box
     * \param min minimum point of a valid abstract bounding box
     * \param max maximum point of a valid abstract bounding box
     * \returns distance to abstract box bounding volume
     */
    vec_t getDistToBboxFromNear(vec3_t min, vec3_t max);

    /*!
     * \brief Get a copy of the view volume
     * \param frustum where frustum will be stored
     */
    void getFrustum(vec_t frustum[6][4]);

    /*!
     * \brief Get a copy of a given plane in view volume
     * \param p side
     * \param plane wher plane will be stored
     */
    void getPlane(ViewVolumeSide p, vec4_t plane);

    /*!
     * \brief Updates view volume for this frame.
     * \param proj new projection matrix
     * \param mdl new model matrix
     */
    void updateFrame(matrix_t proj, matrix_t mdl);

    /*!
     * \brief Updates view volume for this frame.
     *
     * Model & Projection Matrices must be set.
     */
    void updateFrame();

    /*!
     * \brief Set this class' model matrix
     * \param mdl new model matrix
     */
    void setModel(matrix_t mdl);

    /*!
     * \brief Set this class' projection matrix
     * \param proj new projection matrix
     */
    void setProjection(matrix_t proj);

private:

    /*!
     * \brief Computes clipping matrix.
     *
     * Model & Projection matrices must be set!
     */
    void updateClip();

    /*!
     * \brief Computes planes of frustum.
     *
     * Model, Projection & Clip matrices must be set!
     */
    void updateFrustum();

    Matrix mProjection;   //!< Projection matrix
    Matrix mModel;        //!< Model matrix
    Matrix mClip;         //!< Clipping matrix
    vec_t mFrustum[6][4]; //!< View volume
};

#endif
