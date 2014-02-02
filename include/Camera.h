/*!
 * \file include/Camera.h
 * \brief OpenGL camera class
 *
 * \author Mongoose
 */
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <MatMath.h>
#include <Matrix.h>
#include <Quaternion.h>

/*!
 * \brief Commands for interactive camera control
 */
enum camera_command {
  CAMERA_MOVE_FORWARD = 1,
  CAMERA_MOVE_BACKWARD,
  CAMERA_MOVE_UP,
  CAMERA_MOVE_DOWN,
  CAMERA_ROTATE_RIGHT,
  CAMERA_ROTATE_LEFT,
  CAMERA_SPEED_UP,
  CAMERA_SPEED_DOWN,
  CAMERA_ROTATE_UP,
  CAMERA_ROTATE_DOWN,
  CAMERA_MOVE_LEFT,
  CAMERA_MOVE_RIGHT
};

/*!
 * \brief Flags a camera can have
 */
enum CameraFlags {
    Camera_FlyMode = (1 << 0) //!< Camera is flying free?
};

/*!
 * \brief OpenGL camera class
 *
 * 2002.12.16:
 * Mongoose - Removed perspective setting and OpenGL dependency
 *            API changes to reflect new direction of this object:
 *              Removing outdated algorithms and code
 *              And refactoring the class in general
 *
 * 2001.06.06:
 * Mongoose - Moving GLU code into here to setup break up
 *            into Camera base class, DynamicCamera,
 *            and GLUCamera child classes
 *
 * 2001.06.04:
 * Mongoose - Quaternion based compile option
 *
 * 2001.05.18:
 * Mongoose - Created, based on my old GL camera code
 *            that has been used in GooseEgg since alpha
 *            and algorithms from Yuri Zhivago's trview
 */
class Camera {
public:
    /*!
     * \brief Constructs an object of Camera
     */
    Camera();

    /*!
     * \brief Deconstructs an object of Camera
     */
    ~Camera();

    /*!
     * \brief Get this cameras ID
     * \returns id
     */
    unsigned int getId();

    /*!
     * \brief Returns the current position
     * \param pos where the position will be stored
     */
    void getPosition(vec3_t pos);

    /*!
     * \brief Returns the up vector
     * \param up where the up vector will be stored
     */
    void getUp(vec3_t up);

    /*!
     * \brief Get the target currently looked at
     * \param target where the target will be stored
     */
    void getTarget(vec3_t target);

    /*!
     * \brief Get current yaw in degrees
     * \returns yaw in degrees
     */
    float getYaw();

    /*!
     * \brief Get angle/yaw of camera
     * \returns theta angle/yaw of camera
     */
    vec_t getRadianYaw();
    /*------------------------------------------------------
     * Pre  :
     * Post : Returns theta angle/yaw of camera
     *
     *-- History ------------------------------------------
     *
     * 2001.05.26:
     * Mongoose - Created
     ------------------------------------------------------*/

    /*!
     * \brief Get current angle/pitch
     * \returns current pitch in degrees
     */
    float getPitch();

    /*!
     * \brief Get angle/pitch of camera
     * \returns phi angle/pitch of camera
     */
    vec_t getRadianPitch();

    /*!
     * \brief Check if the coordinate is behind camera eye
     * \param x X coordinate to check
     * \param z Z coordinate to check
     * \returns true if (x, z) is behind camera eye
     */
    bool isBehind(int x, int z);

    /*!
     * \brief Rotate the camera
     * \param angle angle in radians
     * \param x X coordinate of axis
     * \param y Y coordinate of axis
     * \param z Z coordinate of axis
     */
    void rotate(float angle, float x, float y, float z);

    /*!
     * \brief Set Camera position
     * \param x new X coordinate
     * \param y new Y coordinate
     * \param z new Z coordinate
     */
    void translate(float x, float y, float z);

    /*!
     * \brief Set the Camera to its initial state
     */
    void reset();

    /*!
     * \brief Sets the X rotation delta
     * \param angle thetas rotation delta in degrees
     */
    void setSensitivityX(float angle);

    /*!
     * \brief Sets the Y rotation delta
     * \param angle thetas rotation delta in degrees
     */
    void setSensitivityY(float angle);

    /*!
     * \brief Sends interactive command to camera
     * \param cmd valid camera command
     */
    void command(enum camera_command cmd);

    /*!
     * \brief Sets speed
     * \param s new speed, is 256 or greater in general
     */
    void setSpeed(float s);

    /*!
     * \brief Updates view target
     */
    void update();

    /*!
     * \brief Set current position
     * \param pos new position
     */
    void setPosition(vec3_t pos);

    /*!
     * \brief Sets the up vector
     * \param up new up vector
     */
    void setUp(vec3_t up);

    /*!
     * \brief Sets target (look at pos)
     * \param target new target
     */
    void setTarget(vec3_t target);

private:
    unsigned int mId;             //!< Unique id
    Quaternion mQ;                //!< Quaternion for rotation
    unsigned int mFlags;          //!< For testing with flags
    vec_t mPos[4];                //!< Location in 3 space (aka eye)
    vec_t mTarget[4];             //!< Postition we're looking at
    vec_t mUp[4];                 //!< Up vector
    vec_t mSide[4];               //!< Side vector
    vec_t mViewDistance;          //!< Distance from target
    vec_t mTranslateDelta;        //!< Step size to move
    vec_t mRotateDelta;           //!< Radians to rotate Y
    vec_t mTheta;                 //!< View angle Y
    vec_t mRotateDelta2;          //!< Radians to rotate Z
    vec_t mTheta2;                //!< View angle Z
    static unsigned int mCounter; //!< Id system use
};

#endif
