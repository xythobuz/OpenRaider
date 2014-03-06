/*!
 * \file include/SkeletalModel.h
 * \brief This is the factored out skeletal model class
 *
 * \author Mongoose
 *
 * \todo Start cutting off old hacks by simple force use of method interface.
 * Also move the publicly exposed attributes out  =)
 * Better animation system in general - this is memory wasteful
 */

#ifndef _SKELETALMODEL_H_
#define _SKELETALMODEL_H_

#include "utils/math.h"
#include "templates/Vector.h"

typedef struct {
    int mesh;
    vec3_t off;
    vec3_t rot;
    char flag;
} bone_tag_t;

typedef struct {
    Vector<bone_tag_t *> tag;
    vec3_t pos;
    float yaw;
} bone_frame_t;

typedef struct {
    int id;
    char rate;
    Vector<bone_frame_t *> frame;
} animation_frame_t;

typedef struct {
    int id;
    bool tr4Overlay;
    bool pigtails;
    int ponytailId;
    vec3_t ponytail;
    int ponytailMeshId;
    unsigned int ponytailNumMeshes;
    float ponytailAngle;
    float ponyOff;
    float ponyOff2;
    Vector<animation_frame_t *> animation;
} skeletal_model_t;

/*!
 * \brief This is the factored out skeletal model class
 */
class SkeletalModel {
public:
    /*!
     * \brief Constructs an object of SkeletalModel
     */
    SkeletalModel();

     /*!
     * \brief Deconstructs an object of SkeletalModel
     */
    ~SkeletalModel();

    int getAnimation();

    int getFrame();

    int getIdleAnimation();

    void setModel(skeletal_model_t *mdl);

    void setAnimation(int index);

    void setFrame(int index);

    void setIdleAnimation(int index);

    unsigned int flags;
    skeletal_model_t *model; //!< World render model
    float time;              //!< Interpolation use
    float lastTime;
    float rate;              //!< \fixme temp cache this here for old animation system use

private:
    int mBoneFrame;      //!< Bone frame
    int mAnimationFrame; //!< Animation frame
    int mIdleAnimation;  //!< Idle animation
};

#endif
