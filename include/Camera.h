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
     * \brief Sends interactive command to camera
     * \param cmd valid camera command
     */
    void command(enum camera_command cmd);

private:

    void rotate(vec_t angle, vec_t x, vec_t y, vec_t z);

    Quaternion mQ;         //!< Quaternion for rotation
    vec4_t mPos;           //!< Location in 3 space (aka eye)
    vec4_t mTarget;        //!< Postition we're looking at
    vec_t mViewDistance;   //!< Distance from target
    vec_t mTheta;          //!< View angle Y
    vec_t mTheta2;         //!< View angle Z
    vec_t mRotationDeltaX; //!< Horizontal mouse sensitivity
    vec_t mRotationDeltaY; //!< Vertical mouse sensitivity
};

Camera &getCamera();

#endif
