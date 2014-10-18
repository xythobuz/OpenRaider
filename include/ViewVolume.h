/*!
 * \file include/ViewVolume.h
 * \brief Viewing Volume for culling use
 *
 * \author Mongoose
 */

#ifndef _VIEWVOLUME_H_
#define _VIEWVOLUME_H_

#include "math/Matrix.h"

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
     * \brief Check if point is in view volume
     * \param x X coordinate of point
     * \param y Y coordinate of point
     * \param z Z coordinate of point
     * \returns true if point in view volume
     */
    bool isPointInFrustum(float x, float y, float z);

    /*!
     * \brief Check if bounding sphere is in view volume
     * \param x X coordinate of a valid abstract sphere
     * \param y Y coordinate of a valid abstract sphere
     * \param z Z coordinate of a valid abstract sphere
     * \param radius radius of a valid abstract sphere
     * \returns true if abstract sphere in view volume
     */
    bool isSphereInFrustum(float x, float y, float z, float radius);

    /*!
     * \brief Check if bounding box is in view volume
     * \param min minimum point of valid abstract bounding box
     * \param max maximum point of valid abstract bounding box
     * \returns true if abstract bounding box in view volume
     */
    bool isBboxInFrustum(float min[3], float max[3]);

    /*!
     * \brief Distance to Bounding sphere
     * \param x X coordinate of a valid abstract sphere
     * \param y Y coordinate of a valid abstract sphere
     * \param z Z coordinate of a valid abstract sphere
     * \param radius radius of a valid abstract sphere
     * \returns distance to abstract sphere bounding volume
     */
    float getDistToSphereFromNear(float x, float y, float z, float radius);

    /*!
     * \brief Distance to Bounding box
     * \param min minimum point of a valid abstract bounding box
     * \param max maximum point of a valid abstract bounding box
     * \returns distance to abstract box bounding volume
     */
    float getDistToBboxFromNear(const float min[3], const float max[3]);

    /*!
     * \brief Get a copy of the view volume
     * \param frustum where frustum will be stored
     */
    void getFrustum(float frustum[6][4]);

    /*!
     * \brief Get a copy of a given plane in view volume
     * \param p side
     * \param plane wher plane will be stored
     */
    void getPlane(ViewVolumeSide p, float plane[4]);

    /*!
     * \brief Updates view volume for this frame.
     * \param proj new projection matrix
     * \param mdl new model matrix
     */
    void updateFrame(float proj[16], float mdl[16]);

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
    void setModel(float mdl[16]);

    /*!
     * \brief Set this class' projection matrix
     * \param proj new projection matrix
     */
    void setProjection(float proj[16]);

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
    float mFrustum[6][4]; //!< View volume
};

#endif
