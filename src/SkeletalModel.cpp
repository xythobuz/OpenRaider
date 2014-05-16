/*!
 * \file src/SkeletalModel.cpp
 * \brief This is the factored out skeletal model class
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <assert.h>

#include "SkeletalModel.h"

BoneTag::BoneTag() {

}

BoneFrame::BoneFrame() {

}

BoneFrame::~BoneFrame() {
    for (unsigned int i = 0; i < tag.size(); i++)
        delete tag[i];
}

unsigned int BoneFrame::size() {
    return tag.size();
}

BoneTag &BoneFrame::get(unsigned int i) {
    assert(i < tag.size());
    return *tag.at(i);
}

void BoneFrame::add(BoneTag &b) {
    tag.push_back(&b);
}

AnimationFrame::AnimationFrame() {

}

AnimationFrame::~AnimationFrame() {
    for (unsigned int i = 0; i < frame.size(); i++)
        delete frame[i];
}

unsigned int AnimationFrame::size() {
    return frame.size();
}

BoneFrame &AnimationFrame::get(unsigned int i) {
    assert(i < frame.size());
    return *frame.at(i);
}

void AnimationFrame::add(BoneFrame &b) {
    frame.push_back(&b);
}

SkeletalModel::SkeletalModel(TombRaider &tr, unsigned int index) {

}

SkeletalModel::~SkeletalModel() {
    for (unsigned int i = 0; i < animation.size(); i++)
        delete animation[i];
}

int SkeletalModel::getId() {
    return id;
}

unsigned int SkeletalModel::size() {
    return animation.size();
}

AnimationFrame &SkeletalModel::get(unsigned int i) {
    assert(i < animation.size());
    return *animation.at(i);
}

void SkeletalModel::add(AnimationFrame &a) {
    animation.push_back(&a);
}

