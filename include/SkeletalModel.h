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

#include "TombRaider.h"

class BoneTag {
public:
    BoneTag(TombRaider &tr, unsigned int index, unsigned int j, unsigned int *l, unsigned int frame_offset);
    void display();

    void getOffset(float o[3]);
    void getRotation(float r[3]);
    char getFlag();

private:
    int mesh;
    float off[3];
    float rot[3];
    char flag;
};

class BoneFrame {
public:
    BoneFrame(TombRaider &tr, unsigned int index, unsigned int frame_offset);
    ~BoneFrame();

    void getPosition(float p[3]);

    unsigned int size();
    BoneTag &get(unsigned int i);

private:
    float pos[3];
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
    void setPonyPos(float x, float y, float z, float angle);

    unsigned int size();
    AnimationFrame &get(unsigned int i);

private:
    int id;
    bool tr4Overlay;
    bool pigtails;
    int ponytailId;
    float ponytail[3];
    int ponytailMeshId;
    unsigned int ponytailNumMeshes;
    float ponytailAngle;
    float ponyOff;
    float ponyOff2;
    std::vector<AnimationFrame *> animation;
};

#endif
