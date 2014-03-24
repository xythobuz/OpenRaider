/*!
 * \file src/SkeletalModel.cpp
 * \brief This is the factored out skeletal model class
 *
 * \author Mongoose
 * \author xythobuz
 */

#include "SkeletalModel.h"

SkeletalModel::SkeletalModel() {
    model = NULL;
    flags = 0;
    mBoneFrame = 0;
    mAnimationFrame = 0;
    mIdleAnimation = 0;
    time = 0.0f;
    lastTime = 0.0f;
    rate = 0.0f;
}

SkeletalModel::~SkeletalModel() {
    //! \fixme Causes "freeing already freed pointer" exceptions or EXEC_BAD_ACCESS
    /* if (model) {
        for(std::vector<animation_frame_t>::size_type i = 0; i < model->animation.size(); i++) {
            animation_frame_t *af = model->animation[i];

            if (!af)
                continue;

            for(std::vector<bone_frame_t>::size_type j = 0; j < af->frame.size(); j++) {
                bone_frame_t *bf = af->frame[j];

                if (!bf)
                    continue;

                for(std::vector<bone_tag_t>::size_type k = 0; k < bf->tag.size(); k++) {
                    if (bf->tag[i])
                        delete bf->tag[i];
                }

                delete bf;
            }

            delete af;
        }

        delete model;
    } */
}

int SkeletalModel::getAnimation() {
    return mAnimationFrame;
}

int SkeletalModel::getFrame() {
    return mBoneFrame;
}

int SkeletalModel::getIdleAnimation() {
    return mIdleAnimation;
}

void SkeletalModel::setModel(skeletal_model_t *mdl) {
    if (mdl)
        model = mdl;
}

void SkeletalModel::setAnimation(int index) {
    if (!model) // index > (int)model->animation.size())
        return;

    animation_frame_t *a = model->animation[index];

    if (a) {
        mAnimationFrame = index;
        mBoneFrame = 0;
        rate = a->rate;
    }
}

void SkeletalModel::setFrame(int index) {
    if (!model)
        return;

    animation_frame_t *a = model->animation[mAnimationFrame];

    if (a) { // index > (int)a->frame.size())
        bone_frame_t *b = a->frame[index];

        if (b)
            mBoneFrame = index;
    }
}

void SkeletalModel::setIdleAnimation(int index) {
    if (!model)
        return;

    if (model->animation[index])
        mIdleAnimation = index;
}

