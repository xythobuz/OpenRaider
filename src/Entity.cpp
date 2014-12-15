/*!
 * \file src/Entity.cpp
 * \brief World Entities
 *
 * \author xythobuz
 */

#include "glm/glm.hpp"

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
    const float moved = 180.0f;
    const float testd = 220.0f;
    const float camHeight = 8.0f;
    float x, y, z, pitch, h, floor, ceiling;
    long roomNew, sector;
    bool wall;
    unsigned int roomFlags;

    switch (moveType) {
        case MoveTypeWalkNoSwim:
        case MoveTypeWalk:
            pitch = 0.0f; // in the future pitch could control jump up blocks here
            break;

        case MoveTypeNoClipping:
        case MoveTypeFly:
        case MoveTypeSwim:
            pitch = angles[2];
            break;
    }

    switch (movement) {
        case 'f':
            x = pos[0] + (testd * sinf(angles[1]));
            y = pos[1] + (testd * sinf(pitch));
            z = pos[2] + (testd * cosf(angles[1]));
            break;
        case 'b':
            x = pos[0] - (testd * sinf(angles[1]));
            y = pos[1] - (testd * sinf(pitch));
            z = pos[2] - (testd * cosf(angles[1]));
            break;
        case 'l':
            x = pos[0] - (testd * sinf(angles[1] + 90.0f));
            y = pos[1];
            z = pos[2] - (testd * cosf(angles[1] + 90.0f));
            break;
        case 'r':
            x = pos[0] + (testd * sinf(angles[1] + 90.0f));
            y = pos[1];
            z = pos[2] + (testd * cosf(angles[1] + 90.0f));
            break;
        default:
            return;
    }

    roomNew = getWorld().getRoomByLocation(room, x, y, z);

    if (roomNew == -1) { // Will we hit a portal?
        roomNew = getWorld().getRoom(room).getAdjoiningRoom(pos[0], pos[1], pos[2],
                  x, y, z);

        if (roomNew > -1)
            getLog() << "Crossing from room " << room << " to " << roomNew << Log::endl;
        else
            //! \fixme mRooms, sectors, ... are now std::vector, but often upper bound checks are missing
            return;
    }

    roomFlags = getWorld().getRoom(roomNew).getFlags();
    sector = getWorld().getRoom(roomNew).getSector(x, z, &floor, &ceiling);
    wall = getWorld().getRoom(roomNew).isWall(sector);

    // If you're underwater you may want to swim  =)
    // ...if you're worldMoveType_walkNoSwim, you better hope it's shallow
    if ((roomFlags & RoomFlagUnderWater) && (moveType == MoveTypeWalk))
        moveType = MoveTypeSwim;

    // Don't swim on land
    if (!(roomFlags & RoomFlagUnderWater) && (moveType == MoveTypeSwim))
        moveType = MoveTypeWalk;

    // Mongoose 2002.09.02, Add check for room -> room transition
    // (Only allow by movement between rooms by using portals)
    if (((moveType == MoveTypeNoClipping) ||
         (moveType == MoveTypeFly) ||
         (moveType == MoveTypeSwim)) ||
        ((roomNew > -1) && (!wall))) {
        room = roomNew;

        switch (movement) {
            case 'f':
                x = pos[0] + (moved * sinf(angles[1]));
                y = pos[1] + (moved * sinf(pitch));
                z = pos[2] + (moved * cosf(angles[1]));
                break;
            case 'b':
                x = pos[0] - (moved * sinf(angles[1]));
                y = pos[1] - (moved * sinf(pitch));
                z = pos[2] - (moved * cosf(angles[1]));
                break;
            case 'l':
                x = pos[0] - (moved * sinf(angles[1] + 90.0f));
                z = pos[2] - (moved * cosf(angles[1] + 90.0f));
                break;
            case 'r':
                x = pos[0] + (moved * sinf(angles[1] + 90.0f));
                z = pos[2] + (moved * cosf(angles[1] + 90.0f));
                break;
        }

        /*! \fixme Test for vector (move vector) / plane (portal) collision here
         * to see if we need to switch rooms... man...
         */

        h = y;
        getWorld().getRoom(room).getHeightAtPosition(x, &h, z);

        switch (moveType) {
            case MoveTypeFly:
            case MoveTypeSwim:
                // Don't fall out of world, avoid a movement that does
                if (h > y - camHeight) {
                    pos[0] = x;
                    pos[1] = y;
                    pos[2] = z;
                }
                break;

            case MoveTypeWalk:
            case MoveTypeWalkNoSwim:
                y = pos[1];  // Override vector movement walking ( er, not pretty )

                // Now fake gravity
                // Mongoose 2002.08.14, Remember TR is upside down ( you fall 'up' )

                //ddist = h - pos[1];

                // This is to force false gravity, by making camera stay on ground
                pos[1] = h; //roomFloor->bbox_min[1];

                // Check for camera below terrian and correct
                if (pos[1] < h - camHeight)
                    pos[1] = h - camHeight;

                pos[0] = x;
                pos[2] = z;
                break;

            case MoveTypeNoClipping:
                pos[0] = x;
                pos[1] = y;
                pos[2] = z;
                break;
        }
    }
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

