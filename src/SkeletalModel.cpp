/*!
 * \file src/SkeletalModel.cpp
 * \brief This is the factored out skeletal model class
 *
 * \author Mongoose
 * \author xythobuz
 */

#include "global.h"
#include "Log.h"
#include "SkeletalModel.h"
#include "World.h"

BoneTag::BoneTag(int m, float o[3], float r[3], char f) {
    mesh = m;
    flag = f;
    for (int i = 0; i < 3; i++) {
        off[i] = o[i];
        rot[i] = r[i];
    }
}

void BoneTag::display() {
    /*
    if (getWorld().sizeMesh() > 0)
        getWorld().getMesh(mesh).drawSolid(); // TODO ?
    else
        getWorld().getStaticMesh(mesh).display();
    */
}

void BoneTag::getOffset(float o[3]) {
    o[0] = off[0];
    o[1] = off[1];
    o[2] = off[2];
}

void BoneTag::getRotation(float r[3]) {
    r[0] = rot[0];
    r[1] = rot[1];
    r[2] = rot[2];
}

char BoneTag::getFlag() {
    return flag;
}

// ----------------------------------------------------------------------------

BoneFrame::BoneFrame(float p[3]) {
    for (int i = 0; i < 3; i++)
        pos[i] = p[i];
}

BoneFrame::~BoneFrame() {
    for (unsigned long i = 0; i < tag.size(); i++)
        delete tag[i];
}

unsigned long BoneFrame::size() {
    return tag.size();
}

BoneTag& BoneFrame::get(unsigned long i) {
    assert(i < tag.size());
    return *tag.at(i);
}

void BoneFrame::add(BoneTag* t) {
    tag.push_back(t);
}

void BoneFrame::getPosition(float p[3]) {
    p[0] = pos[0];
    p[1] = pos[1];
    p[2] = pos[2];
}

// ----------------------------------------------------------------------------

AnimationFrame::AnimationFrame(char r) {
    rate = r;
}

AnimationFrame::~AnimationFrame() {
    for (unsigned long i = 0; i < frame.size(); i++)
        delete frame[i];
}

unsigned long AnimationFrame::size() {
    return frame.size();
}

BoneFrame& AnimationFrame::get(unsigned long i) {
    assert(i < frame.size());
    return *frame.at(i);
}

void AnimationFrame::add(BoneFrame* f) {
    frame.push_back(f);
}

// ----------------------------------------------------------------------------

SkeletalModel::~SkeletalModel() {
    for (unsigned long i = 0; i < animation.size(); i++)
        delete animation[i];
}

void SkeletalModel::display(glm::mat4 MVP, int aframe, int bframe) {
    /*
    assert(aframe < size());
    assert(bframe < get(aframe).size());

    AnimationFrame& anim = get(aframe);
    BoneFrame& boneframe = anim.get(bframe);

    float pos[3];
    boneframe.getPosition(pos);
    glTranslatef(pos[0], pos[1], pos[2]);

    for (unsigned int a = 0; a < boneframe.size(); a++) {
        BoneTag& tag = boneframe.get(a);
        float rot[3], off[3];

        tag.getRotation(rot);
        tag.getOffset(off);

        if (a == 0) {
            glRotatef(rot[1], 0, 1, 0);
            glRotatef(rot[0], 1, 0, 0);
            glRotatef(rot[2], 0, 0, 1);
        } else {
            if (tag.getFlag() & 0x01)
                glPopMatrix();

            if (tag.getFlag() & 0x02)
                glPushMatrix();

            glTranslatef(off[0], off[1], off[2]);
            glRotatef(rot[1], 0, 1, 0);
            glRotatef(rot[0], 1, 0, 0);
            glRotatef(rot[2], 0, 0, 1);
        }

        tag.display();
    }
    */
}

unsigned long SkeletalModel::size() {
    return animation.size();
}

AnimationFrame& SkeletalModel::get(unsigned long i) {
    assert(i < animation.size());
    return *animation.at(i);
}

void SkeletalModel::add(AnimationFrame* f) {
    animation.push_back(f);
}

