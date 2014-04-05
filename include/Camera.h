/*!
 * \file include/Camera.h
 * \brief Camera class
 *
 * \author Mongoose
 * \author xythobuz
 */
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "math/math.h"
#include "math/Matrix.h"
#include "math/Quaternion.h"

/*!
 * \brief Commands for interactive camera control
 */
enum camera_command {
    CAMERA_ROTATE_RIGHT,
    CAMERA_ROTATE_LEFT,
    CAMERA_ROTATE_UP,
    CAMERA_ROTATE_DOWN
};

/*!
 * \brief Camera class
 */
class Camera {
public:
    /*!
     * \brief Constructs an object of Camera
     */
    Camera();

    /*!
     * \brief Returns the current position
     * \param pos where the position will be stored
     */
    void getPosition(vec3_t pos);

    /*!
     * \brief Get the target currently looked at
     * \param target where the target will be stored
     */
    void getTarget(vec3_t target);

    /*!
     * \brief Get angle/yaw of camera
     * \returns theta angle/yaw of camera
     */
    vec_t getRadianYaw();

    /*!
     * \brief Get angle/pitch of camera
     * \returns phi angle/pitch of camera
     */
    vec_t getRadianPitch();

    /*!
     * \brief Set current position
     * \param pos new position
     */
    void setPosition(vec3_t pos);

    void setSensitivityX(vec_t sens);

    void setSensitivityY(vec_t sens);

    /*!
     * \brief Updates view target
     */
    void update();

    /*!
     * \brief Rotate the camera
     * \param angle angle in radians
     * \param x X coordinate of axis
     * \param y Y coordinate of axis
     * \param z Z coordinate of axis
     */
    void rotate(vec_t angle, vec_t x, vec_t y, vec_t z);

    /*!
     * \brief Sends interactive command to camera
     * \param cmd valid camera command
     */
    void command(enum camera_command cmd);

private:
    Quaternion mQ;                //!< Quaternion for rotation
    vec_t mPos[4];                //!< Location in 3 space (aka eye)
    vec_t mTarget[4];             //!< Postition we're looking at
    vec_t mUp[4];                 //!< Up vector
    vec_t mSide[4];               //!< Side vector
    vec_t mViewDistance;          //!< Distance from target
    vec_t mTheta;                 //!< View angle Y
    vec_t mTheta2;                //!< View angle Z
    vec_t mRotationDeltaX;
    vec_t mRotationDeltaY;
};

#endif
