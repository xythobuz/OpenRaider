/*!
 * \file src/SkeletalModel.cpp
 * \brief Moveable Mesh Geometry
 *
 * \author Mongoose
 * \author xythobuz
 */

#include "global.h"
#include "Log.h"
#include "SkeletalModel.h"
#include "World.h"

#include <glm/gtc/matrix_transform.hpp>

void BoneTag::display(glm::mat4 MVP, ShaderTexture* shaderTexture) {
    World::getMesh(mesh).display(MVP, shaderTexture);
}

// ----------------------------------------------------------------------------

BoneFrame::~BoneFrame() {
    for (unsigned long i = 0; i < tag.size(); i++)
        delete tag[i];
}

unsigned long BoneFrame::size() {
    return tag.size();
}

BoneTag& BoneFrame::get(unsigned long i) {
    orAssertLessThan(i, tag.size());
    return *tag.at(i);
}

void BoneFrame::add(BoneTag* t) {
    tag.push_back(t);
}

// ----------------------------------------------------------------------------

AnimationFrame::~AnimationFrame() {
    for (unsigned long i = 0; i < frame.size(); i++)
        delete frame[i];
}

unsigned long AnimationFrame::size() {
    return frame.size();
}

BoneFrame& AnimationFrame::get(unsigned long i) {
    orAssertLessThan(i, frame.size());
    return *frame.at(i);
}

void AnimationFrame::add(BoneFrame* f) {
    frame.push_back(f);
}

// ----------------------------------------------------------------------------

class MatrixStack {
  public:
    explicit MatrixStack(glm::mat4 start) : startVal(start) { stack.push_back(startVal); }

    void push() {
        //orAssertGreaterThan(stack.size(), 0);
        if (stack.size() > 0)
            stack.push_back(stack.at(stack.size() - 1));
    }

    void pop() {
        //orAssertGreaterThan(stack.size(), 0);
        if (stack.size() > 0)
            stack.pop_back();
    }

    glm::mat4 get() {
        //orAssertGreaterThan(stack.size(), 0);
        if (stack.size() > 0)
            return stack.at(stack.size() - 1);
        return startVal;
    }

  private:
    std::vector<glm::mat4> stack;
    glm::mat4 startVal;
};

// ----------------------------------------------------------------------------

SkeletalModel::~SkeletalModel() {
    for (unsigned long i = 0; i < animation.size(); i++)
        delete animation[i];
}

//#define DEBUG_MODELS

#ifdef DEBUG_MODELS
#include <bitset>
#endif

void SkeletalModel::display(glm::mat4 MVP, int aframe, int bframe, ShaderTexture* shaderTexture) {
    orAssertLessThan(aframe, size());
    orAssertLessThan(bframe, get(aframe).size());

    AnimationFrame& anim = get(aframe);
    BoneFrame& boneframe = anim.get(bframe);

    glm::vec3 pos = boneframe.getPosition();
    glm::mat4 frameTrans = glm::translate(glm::mat4(1.0f), pos);

    MatrixStack stack(MVP * frameTrans);

#ifdef DEBUG_MODELS
    Log::get(LOG_DEBUG) << "Starting SkeletalModel:" << Log::endl;
    int cnt = 0;
#endif

    for (unsigned int a = 0; a < boneframe.size(); a++) {
        BoneTag& tag = boneframe.get(a);

        glm::mat4 translate(1.0f);

        if (a != 0) {
            if (tag.getFlag() & 0x01) {
                stack.pop();
#ifdef DEBUG_MODELS
                Log::get(LOG_DEBUG) << "  --> pop" << Log::endl;
                cnt--;
#endif
            }

            if (tag.getFlag() & 0x02) {
                stack.push();
#ifdef DEBUG_MODELS
                Log::get(LOG_DEBUG) << "  --> push" << Log::endl;
                cnt++;
#endif
            }

#ifdef DEBUG_MODELS
            if (tag.getFlag() & ~0x03) {
                std::bitset<8> bs(tag.getFlag());
                Log::get(LOG_DEBUG) << "  Unexpected flag " << bs << Log::endl;
            }
#endif

            glm::vec3 off = tag.getOffset();
            translate = glm::translate(glm::mat4(1.0f), off);
        }

        glm::vec3 rot = tag.getRotation();
        glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), rot[1], glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), rot[0], glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), rot[2], glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 rotate = rotZ * rotX * rotY;

        glm::mat4 mod = translate * rotate;
        tag.display(stack.get() * mod, shaderTexture);
#ifdef DEBUG_MODELS
        Log::get(LOG_DEBUG) << "  --> get (" << cnt << ")" << Log::endl;
#endif
    }

#ifdef DEBUG_MODELS
    Log::get(LOG_DEBUG) << "Done!" << Log::endl;
#endif
}

unsigned long SkeletalModel::size() {
    return animation.size();
}

AnimationFrame& SkeletalModel::get(unsigned long i) {
    orAssertLessThan(i, animation.size());
    return *animation.at(i);
}

void SkeletalModel::add(AnimationFrame* f) {
    animation.push_back(f);
}

