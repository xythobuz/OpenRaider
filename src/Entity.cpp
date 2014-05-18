/*!
 * \file src/Entity.cpp
 * \brief World Entities
 *
 * \author xythobuz
 */

#include "global.h"
#include "Console.h"
#include "Entity.h"
#include "Render.h"
#include "World.h"

#include "games/TombRaider1.h"

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
}

bool Entity::operator<(Entity &o) {
    vec_t distA = getRender().mViewVolume.getDistToSphereFromNear(pos[0], pos[1], pos[2], 1.0f);
    vec_t distB = getRender().mViewVolume.getDistToSphereFromNear(o.pos[0], o.pos[1], o.pos[2], 1.0f);
    return (distA < distB);
}

void Entity::display() {
    glPushMatrix();
    glTranslatef(pos[0], pos[1], pos[2]);
    glRotatef(OR_RAD_TO_DEG(angles[1]), 0, 1, 0);
    getWorld().getSkeletalModel(skeletalModel).display(animationFrame, boneFrame);
    glPopMatrix();

    // Cycle frames
    if (getRender().getFlags() & Render::fAnimateAllModels) {
        if (boneFrame < (getModel().get(animationFrame).size() - 1))
            boneFrame++;
        else
            boneFrame = 0;
    }
}

void Entity::move(char movement) {
    const float moved = 180.0f;
    const float testd = 220.0f;
    const float camHeight = 8.0f;
    float x, y, z, pitch, h, floor, ceiling;
    int roomNew, sector;
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

    //room = getRoomByLocation(x, y, z);
    roomNew = getWorld().getRoomByLocation(room, x, y, z);

    if (roomNew == -1) { // Will we hit a portal?
        roomNew = getWorld().getAdjoiningRoom(room,
                pos[0],  pos[1], pos[2],
                x, y, z);

        if (roomNew > -1)
            getConsole().print("Crossing from room %i to %i", room, roomNew);
        else
            //! \fixme mRooms, sectors, ... are now std::vector, but often upper bound checks are missing
            return;
    }

    roomFlags = getWorld().getRoomInfo(roomNew);
    sector = getWorld().getSector(roomNew, x, z, &floor, &ceiling);
    wall = getWorld().isWall(roomNew, sector);

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
        getWorld().getHeightAtPosition(room, x, &h, z);

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
    getConsole().print("Entity %d:", objectId);
    getConsole().print("  Room %i (0x%X)", room, getWorld().getRoomInfo(room));
    getConsole().print("  %.1fx %.1fy %.1fz", pos[0], pos[1], pos[2]);
    getConsole().print("  %.1f Yaw", OR_RAD_TO_DEG(angles[1]));
}

SkeletalModel &Entity::getModel() {
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

void Entity::setAngle(vec_t yaw) {
    angles[1] = yaw;
}

vec_t Entity::getPos(unsigned int i) {
    return pos[i];
}

vec_t Entity::getAngle(unsigned int i) {
    return angles[i];
}

int Entity::getRoom() {
    return room;
}

unsigned int Entity::getAnimation() {
    return animationFrame;
}

void Entity::setAnimation(unsigned int index) {
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

