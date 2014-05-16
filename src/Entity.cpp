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

Entity::Entity(TombRaider &tr, unsigned int index, unsigned int i, unsigned int model) {
    tr2_moveable_t *moveable = tr.Moveable();
    tr2_item_t *item = tr.Item();

    vec_t yaw = ((item[i].angle >> 14) & 0x03);
    yaw *= 90;

    pos[0] = item[i].x;
    pos[1] = item[i].y;
    pos[2] = item[i].z;
    angles[0] = 0;
    angles[1] = yaw;
    angles[2] = 0;
    objectId = moveable[index].object_id;
    moveType = MoveTypeWalk;
    room = getWorld().getRoomByLocation(pos[0], pos[1], pos[2]);
    skeletalModel = model;
    boneFrame = 0;
    animationFrame = 0;
    idleAnimation = 0;
    state = 0;

    if (tr.Engine() == TR_VERSION_1) {
        switch (objectId) {
            case TombRaider1::Wolf:
                state = TombRaider1::WolfState_Lying;
                animationFrame = 3;
                break;
        }
    }

    // Gather more info if this is lara
    if (objectId == 0) {
        animationFrame = TR_ANIAMTION_RUN;
        idleAnimation = TR_ANIAMTION_STAND;
    }
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

