/*!
 * \file include/SkeletalModel.h
 * \brief This is the factored out skeletal model class
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _SKELETALMODEL_H_
#define _SKELETALMODEL_H_

#include <vector>

#include "math/math.h"
#include "TombRaider.h"

class BoneTag {
public:
    BoneTag(TombRaider &tr, unsigned int index, unsigned int j, unsigned int *l, unsigned int frame_offset);
    void display();

    void getOffset(vec3_t o);
    void getRotation(vec3_t r);
    char getFlag();

private:
    int mesh;
    vec3_t off;
    vec3_t rot;
    char flag;
};

class BoneFrame {
public:
    BoneFrame(TombRaider &tr, unsigned int index, unsigned int frame_offset);
    ~BoneFrame();

    void getPosition(vec3_t p);

    unsigned int size();
    BoneTag &get(unsigned int i);

private:
    vec3_t pos;
    std::vector<BoneTag *> tag;
};

class AnimationFrame {
public:
    AnimationFrame(TombRaider &tr, unsigned int index, int a, unsigned int *frame_offset, int frame_step);
    ~AnimationFrame();

    unsigned int size();
    BoneFrame &get(unsigned int i);

private:
    char rate;
    std::vector<BoneFrame *> frame;
};

class SkeletalModel {
public:
    SkeletalModel(TombRaider &tr, unsigned int index, int objectId);
    ~SkeletalModel();
    void display(unsigned int aframe, unsigned int bframe);

    int getId();
    void setPigTail(bool b);
    void setPonyPos(vec_t x, vec_t y, vec_t z, vec_t angle);

    unsigned int size();
    AnimationFrame &get(unsigned int i);

private:
    int id;
    bool tr4Overlay;
    bool pigtails;
    int ponytailId;
    vec3_t ponytail;
    int ponytailMeshId;
    unsigned int ponytailNumMeshes;
    vec_t ponytailAngle;
    vec_t ponyOff;
    vec_t ponyOff2;
    std::vector<AnimationFrame *> animation;
};

#endif
