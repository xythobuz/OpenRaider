/*!
 * \file include/Camera.h
 * \brief Camera class
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _CAMERA_H_
#define _CAMERA_H_

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
    void getTarget(float target[3]);

    /*!
     * \brief Get angle/yaw of camera
     * \returns theta angle/yaw of camera
     */
    float getRadianYaw();

    /*!
     * \brief Get angle/pitch of camera
     * \returns phi angle/pitch of camera
     */
    float getRadianPitch();

    /*!
     * \brief Set current position
     * \param pos new position
     */
    void setPosition(float pos[3]);

    void setSensitivityX(float sens);

    void setSensitivityY(float sens);

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

    void rotate(float angle, float x, float y, float z);

    Quaternion mQ;         //!< Quaternion for rotation
    float mPos[4];         //!< Location in 3 space (aka eye)
    float mTarget[4];      //!< Postition we're looking at
    float mViewDistance;   //!< Distance from target
    float mTheta;          //!< View angle Y
    float mTheta2;         //!< View angle Z
    float mRotationDeltaX; //!< Horizontal mouse sensitivity
    float mRotationDeltaY; //!< Vertical mouse sensitivity
};

Camera& getCamera();

#endif
