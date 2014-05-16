/*!
 * \file src/Entity.cpp
 * \brief World Entities
 *
 * \author xythobuz
 */

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#elif defined WIN32
#include <gl/glew.h>
#include <gl/wglew.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "Entity.h"
#include "main.h"

Entity::Entity(TombRaider &tr) {

}

bool Entity::operator<(Entity &o) {
    vec_t distA = getRender().mViewVolume.getDistToSphereFromNear(pos[0], pos[1], pos[2], 1.0f);
    vec_t distB = getRender().mViewVolume.getDistToSphereFromNear(o.pos[0], o.pos[1], o.pos[2], 1.0f);
    return (distA < distB);
}

void Entity::display() {
    /*
    glPushMatrix();
    glTranslatef(pos[0], pos[1], pos[2]);
    glRotatef(angles[1], 0, 1, 0);
    getWorld().getSkeletalModel(skeletalModel).display();
    glPopMatrix();
    */
}

void Entity::setSkeletalModel(unsigned int model) {
    skeletalModel = model;
    animationFrame = 0;
    boneFrame = 0;
    idleAnimation = 0;
}

unsigned int Entity::getAnimationFrame() {
    return animationFrame;
}

void Entity::setAnimationFrame(unsigned int index) {
    animationFrame = index;
    boneFrame = 0;
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

