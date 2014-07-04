/*!
 * \file include/Entity.h
 * \brief World Entities
 *
 * \author xythobuz
 */

#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "SkeletalModel.h"
#include "TombRaider.h"

class Entity {
public:
    typedef enum {
        MoveTypeWalkNoSwim = -1,
        MoveTypeWalk       = 0,
        MoveTypeNoClipping = 1,
        MoveTypeFly        = 2,
        MoveTypeSwim       = 3
    } MoveType;

    Entity(TombRaider &tr, unsigned int index, unsigned int i, unsigned int model);

    bool operator<(Entity &o);
    void display();
    void move(char movement);
    void print();

    SkeletalModel &getModel();
    void setSkeletalModel(unsigned int model);
    MoveType getMoveType();
    void setMoveType(MoveType m);
    int getObjectId();
    void setAngles(float a[3]);
    float getPos(unsigned int i);
    float getAngle(unsigned int i);
    int getRoom();

    // Animation State
    unsigned int getAnimation();
    void setAnimation(unsigned int index);
    unsigned int getBoneFrame();
    void setBoneFrame(unsigned int index);
    unsigned int getIdleAnimation();
    void setIdleAnimation(unsigned int index);

private:
    float pos[3];
    float angles[3];
    int room;

    unsigned int skeletalModel;
    MoveType moveType;

    int state;
    int objectId;

    // Animation State
    unsigned int boneFrame;
    unsigned int animationFrame;
    unsigned int idleAnimation;
};

#endif

