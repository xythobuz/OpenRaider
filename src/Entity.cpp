/*!
 * \file src/Entity.cpp
 * \brief World Entities
 *
 * \author xythobuz
 */

#include "global.h"
#include "Log.h"
#include "World.h"
#include "Entity.h"

Entity::Entity(float p[3], float a[3], int id, long r, unsigned int model) {
    for (int i = 0; i < 3; i++) {
        pos[i] = p[i];
        angles[i] = a[i];
    }
    objectId = id;
    moveType = MoveTypeWalk;
    room = r;
    skeletalModel = model;
    boneFrame = 0;
    animationFrame = 0;
    idleAnimation = 0;
    state = 0;
}

void Entity::display() {
    /*
    glPushMatrix();
    glTranslatef(pos[0], pos[1], pos[2]);
    glRotatef(glm::degrees(angles[1]), 0, 1, 0);
    glRotatef(glm::degrees(angles[0]), 1, 0, 0);
    //glRotatef(glm::degrees(angles[2]), 0, 0, 1);
    getWorld().getSkeletalModel(skeletalModel).display(animationFrame, boneFrame);
    glPopMatrix();
    */
}

void Entity::move(char movement) {

}

void Entity::print() {
    getLog() << "Entity " << objectId << ":" << Log::endl
             << "  Room " << room << " (" << getWorld().getRoom(room).getFlags()
             << ")" << Log::endl
             << "  " << pos[0] << "x " << pos[1] << "y " << pos[2] << "z"
             << Log::endl
             << "  " << glm::degrees(angles[1]) << " Yaw" << Log::endl;
}

SkeletalModel& Entity::getModel() {
    return getWorld().getSkeletalModel(skeletalModel);
}

void Entity::setSkeletalModel(unsigned int model) {
    skeletalModel = model;
    animationFrame = 0;
    boneFrame = 0;
    idleAnimation = 0;
}

Entity::MoveType Entity::getMoveType() {
    return moveType;
}

void Entity::setMoveType(MoveType m) {
    moveType = m;
}

int Entity::getObjectId() {
    return objectId;
}

void Entity::setAngles(float a[3]) {
    for (unsigned int i = 0; i < 3; i++)
        angles[i] = a[i];
}

float Entity::getPos(unsigned int i) {
    return pos[i];
}

float Entity::getAngle(unsigned int i) {
    return angles[i];
}

long Entity::getRoom() {
    return room;
}

unsigned long Entity::getAnimation() {
    return animationFrame;
}

void Entity::setAnimation(unsigned long index) {
    animationFrame = index;
    boneFrame = 0;
}

unsigned long Entity::getBoneFrame() {
    return boneFrame;
}

void Entity::setBoneFrame(unsigned long index) {
    boneFrame = index;
}

unsigned long Entity::getIdleAnimation() {
    return idleAnimation;
}

void Entity::setIdleAnimation(unsigned long index) {
    idleAnimation = index;
}

