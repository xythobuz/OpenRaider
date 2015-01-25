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

class BoneTag {
  public:
    BoneTag(int m, float o[3], float r[3], char f);
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
    BoneFrame(float p[3]);
    ~BoneFrame();

    void getPosition(float p[3]);

    unsigned long size();
    BoneTag& get(unsigned long i);
    void add(BoneTag* t);

  private:
    float pos[3];
    std::vector<BoneTag*> tag;
};

class AnimationFrame {
  public:
    AnimationFrame(char r);
    ~AnimationFrame();

    unsigned long size();
    BoneFrame& get(unsigned long i);
    void add(BoneFrame* f);

  private:
    char rate;
    std::vector<BoneFrame*> frame;
};

class SkeletalModel {
  public:
    SkeletalModel(int i) : id(i) { }
    ~SkeletalModel();
    void display(glm::mat4 MVP, int aframe, int bframe);

    int getID() { return id; }

    unsigned long size();
    AnimationFrame& get(unsigned long i);
    void add(AnimationFrame* f);

  private:
    int id;
    std::vector<AnimationFrame*> animation;
};

#endif
