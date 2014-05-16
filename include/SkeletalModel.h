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
    BoneTag();

private:
    int mesh;
    vec3_t off;
    vec3_t rot;
    char flag;
};

class BoneFrame {
public:
    BoneFrame();
    ~BoneFrame();

    unsigned int size();
    BoneTag &get(unsigned int i);
    void add(BoneTag &b);

private:
    vec3_t pos;
    vec_t yaw;
    std::vector<BoneTag *> tag;
};

class AnimationFrame {
public:
    AnimationFrame();
    ~AnimationFrame();

    unsigned int size();
    BoneFrame &get(unsigned int i);
    void add(BoneFrame &b);

private:
    char rate;
    std::vector<BoneFrame *> frame;
};

class SkeletalModel {
public:
    SkeletalModel(TombRaider &tr, );
    ~SkeletalModel();
    int getId();

    unsigned int size();
    AnimationFrame &get(unsigned int i);
    void add(AnimationFrame &a);

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
