/*!
 * \file src/Entity.cpp
 * \brief World Entities
 *
 * \author xythobuz
 */

#include "Entity.h"

Entity::Entity(TombRaider &tr) {

}

unsigned int Entity::getAnimationFrame() {
    return animationFrame;
}

void Entity::setAnimationFrame(unsigned int index) {
    animationFrame = index;
}

unsigned int Entity::getBoneFrame() {
    return boneFrame;
}

void Entity::setBoneFrame(unsigned int index) {
    boneFrame = index;
}

unsigned int Entity::getIdleAnimation() {
    return idleAnimation;
}

void Entity::setIdleAnimation(unsigned int index) {
    idleAnimation = index;
}

