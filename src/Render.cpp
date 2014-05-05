/*!
 * \file src/Render.cpp
 * \brief OpenRaider Renderer class
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <algorithm>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "global.h"
#include "main.h"
#include "Game.h"
#include "OpenRaider.h"
#include "Render.h"

bool compareEntites(const void *voidA, const void *voidB)
{
    entity_t *a = (entity_t *)voidA, *b = (entity_t *)voidB;
    vec_t distA, distB;

    if (!a || !b)
        return false; // error really

    distA = getRender().mViewVolume.getDistToSphereFromNear(a->pos[0],
            a->pos[1],
            a->pos[2],
            1.0f);
    distB = getRender().mViewVolume.getDistToSphereFromNear(b->pos[0],
            b->pos[1],
            b->pos[2],
            1.0f);

    return (distA < distB);
}

////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

Render::Render()
{
    mSkyMesh = -1;
    mSkyMeshRotation = false;
    mMode = Render::modeDisabled;
    mLock = 0;
    mFlags = (fRoomAlpha | fViewModel | fSprites |
            fRoomModels | fEntityModels | fRenderPonytail |
            fUsePortals | fUpdateRoomListPerFrame);
}


Render::~Render()
{
    ClearWorld();
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////

void Render::screenShot(char *filenameBase)
{
    mTexture.glScreenShot(filenameBase, getWindow().mWidth, getWindow().mHeight);
}

unsigned int Render::getFlags() {
    return mFlags;
}

////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////


void Render::loadTexture(unsigned char *image,
        unsigned int width, unsigned int height,
        unsigned int id)
{
    glColor3fv(WHITE);
    mTexture.loadBufferSlot(image, width, height, Texture::RGBA, 32, id);
}


int Render::initTextures(char *textureDir) {
    char filename[128];
    unsigned int numTextures = 0;
    unsigned char color[4];

    mTexture.reset();
    mTexture.setMaxTextureCount(128);  /* TR never needs more than 32 iirc
                                          However, color texturegen is a lot */

    mTexture.setFlag(Texture::fUseMipmaps);

    color[0] = 0xff;
    color[1] = 0xff;
    color[2] = 0xff;
    color[3] = 0xff;

    if (mTexture.loadColorTexture(color, 32, 32) > -1)
        numTextures++;

    //! \fixme Error Checking. Return negative error code, check in calling place too
    snprintf(filename, 126, "%s/%s", textureDir, "splash.tga");
    filename[127] = 0;
    if (mTexture.loadTGA(filename) > -1)
        numTextures++;

    return numTextures;
}


void Render::ClearWorld() {
    mRoomRenderList.clear();

    for (unsigned int i = 0; i < mModels.size(); i++) {
        if (mModels[i])
            delete mModels[i];
    }
    mModels.clear();
}


// Texture must be set to WHITE solid color texture
void renderTrace(int color, vec3_t start, vec3_t end)
{
    const float widthStart = 10.0f; //5.0f;
    const float widthEnd = 10.0f;
    float delta = randomNum(0.01f, 0.16f); // for flicker fx


    // Draw two long quads that skrink and fade the they go further out
    glBegin(GL_QUADS);

    switch (color)
    {
        case 0:
            glColor3f(0.9f - delta, 0.2f, 0.2f);
            break;
        case 1:
            glColor3f(0.2f, 0.9f - delta, 0.2f);
            break;
        case 2:
        default:
            glColor3f(0.2f, 0.2f, 0.9f - delta);
    }

    glVertex3f(start[0], start[1], start[2]);
    glVertex3f(start[0], start[1] + widthStart, start[2] + widthStart);
    glVertex3f(end[0], end[1] + widthEnd, end[2] + widthEnd);
    glVertex3f(end[0], end[1], end[2]);

    glVertex3f(start[0],  start[1], start[2]);
    glVertex3f(start[0],  start[1] + widthStart, start[2] - widthStart);
    glVertex3f(end[0], end[1] + widthEnd, end[2] - widthEnd);
    glVertex3f(end[0], end[1], end[2]);

    glVertex3f(start[0],  start[1] + widthStart, start[2] + widthStart);
    glVertex3f(start[0],  start[1] + widthStart, start[2] - widthStart);
    glVertex3f(end[0], end[1] + widthEnd, end[2] - widthEnd);
    glVertex3f(end[0], end[1] + widthEnd, end[2] + widthEnd);
    glEnd();
}


void setLighting(bool on)
{
    if (on)
    {
        glEnable(GL_LIGHTING);
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, WHITE);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, WHITE);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, DIM_WHITE);
    }
    else
    {
        glDisable(GL_LIGHTING);
    }
}


void lightRoom(Room &room)
{
    for (unsigned int i = 0; i < room.sizeLights(); ++i)
    {
        Light &light = room.getLight(i);

        glEnable(GL_LIGHT0 + i);

        switch (light.mType)
        {
            case Light::typeSpot:
                glLightf(GL_LIGHT0 + i,  GL_SPOT_CUTOFF,    light.mCutoff);
                glLightfv(GL_LIGHT0 + i, GL_POSITION,       light.mPos);
                glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, light.mDir);
                glLightfv(GL_LIGHT0 + i, GL_DIFFUSE,        light.mColor);
                break;
            case Light::typePoint:
            case Light::typeDirectional:
                glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 1.0); // 1.0

                // GL_QUADRATIC_ATTENUATION
                // GL_LINEAR_ATTENUATION
                glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, light.mAtt);

                glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light.mColor); // GL_DIFFUSE
                glLightfv(GL_LIGHT0 + i, GL_POSITION, light.mPos);
                break;
        }
    }
}


void Render::clearFlags(unsigned int flags)
{
    // _defaults |= flags; // Force removal if it wasn't set
    mFlags ^= flags;

    if (flags & Render::fFog)
    {
        if (glIsEnabled(GL_FOG))
        {
            glDisable(GL_FOG);
        }
    }

    if (flags & Render::fGL_Lights)
    {
        setLighting(false);
    }
}


void Render::setFlags(unsigned int flags)
{
    mFlags |= flags;

    if (flags & Render::fFog)
    {
        glEnable(GL_FOG);
        glFogf(GL_FOG_MODE, GL_EXP2);
        glFogf(GL_FOG_DENSITY, 0.00008f);
        glFogf(GL_FOG_START, 30000.0f);
        glFogf(GL_FOG_END, 50000.0f);
        glFogfv(GL_FOG_COLOR, BLACK);
    }

    if (flags & Render::fGL_Lights)
    {
        setLighting(true);
    }
}


int Render::getMode()
{
    return mMode;
}


void Render::setMode(int n)
{
    mMode = n;

    switch (mMode)
    {
        case Render::modeDisabled:
            break;
        case Render::modeSolid:
        case Render::modeWireframe:
            glClearColor(NEXT_PURPLE[0], NEXT_PURPLE[1],
                    NEXT_PURPLE[2], NEXT_PURPLE[3]);
            glDisable(GL_TEXTURE_2D);
            break;
        default:
            if (mMode == Render::modeLoadScreen)
            {
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            }
            else
            {
                glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            }

            glClearColor(BLACK[0], BLACK[1], BLACK[2], BLACK[3]);

            glEnable(GL_TEXTURE_2D);
    }
}

// Replaced the deprecated gluLookAt with slightly modified code from here:
// http://www.khronos.org/message_boards/showthread.php/4991
void gluLookAt(float eyeX, float eyeY, float eyeZ,
                            float lookAtX, float lookAtY, float lookAtZ,
                            float upX, float upY, float upZ) {
    float f[3];

    // calculating the viewing vector
    f[0] = lookAtX - eyeX;
    f[1] = lookAtY - eyeY;
    f[2] = lookAtZ - eyeZ;
    float fMag, upMag;
    fMag = sqrtf(f[0] * f[0] + f[1] * f[1] + f[2] * f[2]);
    upMag = sqrtf(upX * upX + upY * upY + upZ * upZ);

    // normalizing the viewing vector
    f[0] = f[0] / fMag;
    f[1] = f[1] / fMag;
    f[2] = f[2] / fMag;

    float s[3] = {
        f[1] * upZ - upY * f[2],
        upX * f[2] - f[0] * upZ,
        f[0] * upY - upX * f[1]
    };

    float u[3] = {
        s[1] * f[2] - f[1] * s[2],
        f[0] * s[2] - s[0] * f[2],
        s[0] * f[1] - f[0] * s[1]
    };

    float m[16] = {
        s[0], u[0], -f[0], 0,
        s[1], u[1], -f[1], 0,
        s[2], u[2], -f[2], 0,
           0,    0,     0, 1
    };
    glMultMatrixf(m);
    glTranslatef(-eyeX, -eyeY, -eyeZ);
}

void Render::display()
{
    vec3_t curPos;
    vec3_t camPos;
    vec3_t atPos;
    int index;

    switch (mMode)
    {
        case Render::modeDisabled:
            return;
        case Render::modeLoadScreen:
            //! \fixme entry for seperate main drawing method -- Mongoose 2002.01.01
            drawLoadScreen();
            return;
        default:
            ;
    }

    if (mMode == Render::modeWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    index = -1;

    if (getGame().mLara)
    {
        float yaw;
        int sector;
        float camOffsetH = 0.0f;


        switch (getGame().mLara->moveType)
        {
            case worldMoveType_fly:
            case worldMoveType_noClipping:
            case worldMoveType_swim:
                camOffsetH = 64.0f;
                break;
            case worldMoveType_walk:
            case worldMoveType_walkNoSwim:
                camOffsetH = 512.0f;
                break;
        }

        curPos[0] = getGame().mLara->pos[0];
        curPos[1] = getGame().mLara->pos[1];
        curPos[2] = getGame().mLara->pos[2];
        yaw = getGame().mLara->angles[1];

        index = getGame().mLara->room;

        // Mongoose 2002.08.24, New 3rd person camera hack
        camPos[0] = curPos[0];
        camPos[1] = curPos[1] - camOffsetH; // UP is lower val
        camPos[2] = curPos[2];

        camPos[0] -= (1024.0f * sinf(yaw));
        camPos[2] -= (1024.0f * cosf(yaw));

        sector = getWorld().getSector(index, camPos[0], camPos[2]);

        // Handle camera out of world
        if (sector < 0 || getWorld().isWall(index, sector))
        {
            camPos[0] = curPos[0] + (64.0f * sinf(yaw));
            camPos[1] -= 64.0f;
            camPos[2] = curPos[2] + (64.0f * cosf(yaw));
        }

        getCamera().setPosition(camPos);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Setup view in OpenGL with camera
    getCamera().update();
    getCamera().getPosition(camPos);
    getCamera().getTarget(atPos);
    // Mongoose 2002.08.13, Quick fix to render OpenRaider upside down

    gluLookAt(camPos[0], camPos[1], camPos[2], atPos[0], atPos[1], atPos[2], 0.0f, -1.0f, 0.0f);

    // Update view volume for vising
    updateViewVolume();

    // Let's see the LoS -- should be event controled
    if (getGame().mLara)
    {
        // SkeletalModel *mdl = getGame().mLara->tmpHook;

        // Draw in solid colors
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_CULL_FACE);

        if (getGame().mLara->state == 64) // eWeaponFire
        {
            vec3_t u, v; //, s, t;

            // Center of getGame().mLara
            u[0] = curPos[0];
            u[1] = curPos[1] - 512.0f;
            u[2] = curPos[2];

            // Location getGame().mLara is aiming at?  ( Not finished yet )
            v[0] = u[0] + (9000.0f * sinf(getGame().mLara->angles[1]));
            v[1] = u[1] + (9000.0f * sinf(getGame().mLara->angles[2]));
            v[2] = u[2] + (9000.0f * cosf(getGame().mLara->angles[1]));

            // Test tracing of aim
            renderTrace(0, u, v); // v = target
        }

        entity_t *route = getGame().mLara->master;

        while (route)
        {
            if (route->master)
            {
                renderTrace(1, route->pos, route->master->pos);
            }

            route = route->master;
        }

        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
    }

    // Render world
    glColor3fv(DIM_WHITE); // was WHITE
    drawSkyMesh(96.0);

    // Figure out how much of the map to render
    newRoomRenderList(index);

    // Room solid pass, need to do depth sorting to avoid 2 pass render
    for (unsigned int i = 0; i < mRoomRenderList.size(); i++)
    {
        Room &room = *mRoomRenderList[i];

        if (mFlags & Render::fGL_Lights)
        {
            lightRoom(room);
        }

        drawRoom(room, false);
    }

    // Draw all visible enities
    if (mFlags & Render::fEntityModels)
    {
        entity_t *e;
        std::vector<entity_t *> entityRenderList;
        std::vector<entity_t *> *entities = getWorld().getEntities();

        for (unsigned int i = 0; i < entities->size(); i++)
        {
            e = entities->at(i);

            // Mongoose 2002.03.26, Don't show lara to it's own player
            if (!e || e == getGame().mLara)
            {
                continue;
            }

            // Mongoose 2002.08.15, Nothing to draw, skip
            // Mongoose 2002.12.24, Some entities have no animation  =p
            if (e->tmpHook)
                if (e->tmpHook->model->animation.empty())
                    continue;

            // Is it in view volume? ( Hack to use sphere )
            if (!isVisible(e->pos[0], e->pos[1], e->pos[2], 512.0f))
                continue;

            //! \fixme Is it in a room we're rendering?
            //if (mRoomRenderList[e->room] == 0x0)
            //{
            //  continue;
            //}

            entityRenderList.push_back(e);
        }

        // Draw objects not tied to rooms
        glPushMatrix();
        drawObjects();
        glPopMatrix();

        // Depth sort entityRenderList with qsort
        std::sort(entityRenderList.begin(), entityRenderList.end(), compareEntites);

        for (unsigned int i = 0; i < entityRenderList.size(); i++)
        {
            e = entityRenderList[i];

            glPushMatrix();
            glTranslatef(e->pos[0], e->pos[1], e->pos[2]);
            glRotatef(e->angles[1], 0, 1, 0);
            drawModel(e->tmpHook);
            glPopMatrix();
        }
    }

    // Room alpha pass
    // Skip room alpha pass for modes w/o texture
    if (!(mMode == Render::modeSolid || mMode == Render::modeWireframe))
    {
        for (unsigned int i = 0; i < mRoomRenderList.size(); i++)
        {
            drawRoom(*mRoomRenderList[i], true);
        }
    }

    if (mMode == Render::modeWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glFlush();
}

void Render::drawLoadScreen()
{
    float x = 0.0f, y = 0.0f, z = -160.0f;
    float w, h;

    if (getWindow().mWidth < getWindow().mHeight)
        w = h = (float)getWindow().mWidth;
    else
        w = h = (float)getWindow().mHeight;


    if (mTexture.getTextureCount() <= 0)
        return;

    // Mongoose 2002.01.01, Rendered while game is loading...
    //! \fixme seperate logo/particle coor later
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glColor3fv(WHITE);

    if (mFlags & Render::fGL_Lights)
        glDisable(GL_LIGHTING);

    // Mongoose 2002.01.01, Draw logo/load screen
    glTranslatef(0.0f, 0.0f, -2000.0f);
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);

    mTexture.bindTextureId(1); //! \fixme store texture id somewhere

    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(x + w, y + h, z);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(x - w, y + h, z);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(x + w, y - h, z);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(x - w, y - h, z);
    glEnd();

    if (mFlags & Render::fGL_Lights)
        glEnable(GL_LIGHTING);

    glFlush();
}

void Render::newRoomRenderList(int index)
{
    static int currentRoomId = -1;

    if (mFlags & Render::fUsePortals)
    {
        if (index == -1) // -1 is error, so draw room 0, for the hell of it
        {
            mRoomRenderList.clear();
            mRoomRenderList.push_back(&getWorld().getRoom(0));
        }
        else
        {
            // Update room render list if needed
            if (mFlags & Render::fUpdateRoomListPerFrame ||
                    currentRoomId != index)
            {
                buildRoomRenderList(getWorld().getRoom(index));
            }
        }
    }
    else // Render all rooms pretty much
    {
        if (currentRoomId != index || index == -1)
        {
            printf("*** Room render list -> %i\n", index);
            mRoomRenderList.clear();

            for (unsigned int i = 0; i < getWorld().sizeRoom(); i++)
            {
                vec3_t bbox[2];
                getWorld().getRoom(i).getBoundingBox(bbox);
                if (!isVisible(bbox[0], bbox[1]))
                    continue;

                mRoomRenderList.push_back(&getWorld().getRoom(i));
            }
        }
    }

    currentRoomId = index;
}


void Render::buildRoomRenderList(Room &room)
{

    // Must be visible
    //! \fixme Add depth sorting here - remove multipass
    vec3_t bbox[2];
    room.getBoundingBox(bbox);
    if (!isVisible(bbox[0], bbox[1]))
        return;

    // Must not already be cached
    for (unsigned int i = 0; i < mRoomRenderList.size(); i++)
    {
        Room &room2 = *mRoomRenderList[i];

        if (&room2 == &room)
            return;
    }

    /* Add current room to list */
    mRoomRenderList.push_back(&room);

    if (mFlags & Render::fOneRoom)
    {
        return;
    }
    else if (mFlags & Render::fAllRooms) /* Are you serious? */
    {
        for (unsigned int i = 0; i < getWorld().sizeRoom(); i++)
        {
            Room &room2 = getWorld().getRoom(i);

            if (&room2 != &room)
            {
                buildRoomRenderList(room2);
            }
        }

        return;
    }

    // Try to add adj rooms and their adj rooms, skip this room
    for (unsigned int i = 1; i < room.sizeAdjacentRooms(); i++)
    {
        if (room.getAdjacentRoom(i) < 0)
            continue;

        Room &room2 = getWorld().getRoom(room.getAdjacentRoom(i));

        // Mongoose 2002.03.22, Add portal visibility check here

        if (&room2 != &room)
        {
            buildRoomRenderList(room2);
        }
    }
}


void Render::drawSkyMesh(float scale)
{
    skeletal_model_t *model = getWorld().getModel(mSkyMesh);


    if (!model)
        return;

    glDisable(GL_DEPTH_TEST);
    glPushMatrix();

    if (mSkyMeshRotation)
    {
        glRotated(90.0, 1, 0, 0);
    }

    glTranslated(0.0, 1000.0, 0.0);
    glScaled(scale, scale, scale);
    //drawModel(model);
    //drawModelMesh(getWorld().getMesh(mSkyMesh), );
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
}


void Render::drawObjects()
{
    // Draw lara or other player model ( move to entity rendering method )
    if (mFlags & Render::fViewModel && getGame().mLara && getGame().mLara->tmpHook)
    {
        SkeletalModel *mdl = getGame().mLara->tmpHook;

        if (mdl)
        {
            int frame;

            // Mongoose 2002.03.22, Test 'idle' aniamtions
            if (!getGame().mLara->moving)
            {
                frame = mdl->getIdleAnimation();

                // Mongoose 2002.08.15, Stop flickering of idle lara here
                if (frame == 11)
                {
                    mdl->setFrame(0);
                }
            }
            else
            {
                frame = mdl->getAnimation();
            }

            animation_frame_t *animation = mdl->model->animation[frame];

            if (animation && mdl->getFrame() > (int)animation->frame.size()-1)
            {
                mdl->setFrame(0);
            }
        }

        glPushMatrix();

        glTranslated(getGame().mLara->pos[0], getGame().mLara->pos[1], getGame().mLara->pos[2]);
        glRotated(OR_RAD_TO_DEG(getCamera().getRadianYaw()), 0, 1, 0);

        drawModel(getGame().mLara->tmpHook);
        glPopMatrix();
    }

    // Mongoose 2002.03.22, Draw sprites after player to handle alpha
    if (mFlags & Render::fSprites) {
        for (unsigned int i = 0; i < getWorld().sizeSprite(); i++) {
            SpriteSequence &sprite = getWorld().getSprite(i);
            for (unsigned int j = 0; j < sprite.size(); j++)
                sprite.get(j).display();
        }
    }
}


void Render::drawModel(SkeletalModel *model)
{
    animation_frame_t *animation;
    bone_frame_t *boneframe;
    bone_frame_t *boneframe2 = 0x0;
    bone_tag_t *tag;
    bone_tag_t *tag2;
    int bframe, aframe;
    skeletal_model_t *mdl;


    if (!model || !model->model)
        return;

    mdl = model->model;
    aframe = model->getAnimation();
    bframe = model->getFrame();
    animation = mdl->animation[aframe];

    if (!animation)
    {
#ifdef DEBUG
        printf("ERROR: No animation for model[%i].aframe[%i] %lu\n",
                mdl->id, aframe, mdl->animation.size());
#endif
        return;
    }

    if (animation->frame.empty())
    {
#ifdef DEBUG
        printf("ERROR: No boneframes?!?!  *** %i:%i ***\n",
                mdl->id, bframe);
#endif
        return;
    }

    boneframe = animation->frame[bframe];

    if (!boneframe)
        return;

    if (boneframe->tag.empty())
    {
        printf("Empty bone frame?!?!\n");
        return;
    }

    glTranslatef(boneframe->pos[0], boneframe->pos[1], boneframe->pos[2]);

    for (unsigned int a = 0; a < boneframe->tag.size(); a++)
    {
        tag = boneframe->tag[a];

        if (!tag)
            continue;

        if (a == 0)
        {
            if (!equalEpsilon(tag->rot[1], 0.0f)) // was just if (tag->rot[1])
                glRotatef(tag->rot[1], 0, 1, 0);

            if (!equalEpsilon(tag->rot[0], 0.0f))
                glRotatef(tag->rot[0], 1, 0, 0);

            if (!equalEpsilon(tag->rot[2], 0.0f))
                glRotatef(tag->rot[2], 0, 0, 1);
        }
        else
        {
            if (tag->flag & 0x01)
                glPopMatrix();

            if (tag->flag & 0x02)
                glPushMatrix();

            glTranslatef(tag->off[0], tag->off[1], tag->off[2]);

            if (!equalEpsilon(tag->rot[1], 0.0f))
                glRotatef(tag->rot[1], 0, 1, 0);

            if (!equalEpsilon(tag->rot[0], 0.0f))
                glRotatef(tag->rot[0], 1, 0, 0);

            if (!equalEpsilon(tag->rot[2], 0.0f))
                glRotatef(tag->rot[2], 0, 0, 1);
        }

        // Draw layered lara in TR4 ( 2 meshes per tag )
        if (mdl->tr4Overlay == 1)
        {
            boneframe2 = (mdl->animation[0])->frame[0];

            if (boneframe2)
            {
                tag2 = boneframe2->tag[a];

                if (tag2)
                {
                    drawModelMesh(getWorld().getMesh(tag2->mesh), Render::skeletalMesh);
                }
            }
        }

        if (mFlags & Render::fRenderPonytail)
        {
            if (mdl->ponytailId > 0 &&
                    a == 14)
            {
                glPushMatrix();

                // Mongoose 2002.08.30, TEST to align offset
                glTranslatef(mdl->ponytail[0], mdl->ponytail[1], mdl->ponytail[2]);
                glRotatef(mdl->ponytailAngle, 1, 0, 0);

                // HACK: To fill TR4 void between ponytail/head
                //   since no vertex welds are implemented yet
                if (mdl->tr4Overlay == 1)
                {
                    glScalef(1.20f, 1.20f, 1.20f);
                }

#ifdef EXPERIMENTAL_NON_ITEM_RENDER
                drawModel(mModels[mdl->ponytail], 0, 0);
#else
                for (unsigned int i = 0; i < mdl->ponytailNumMeshes; ++i)
                {
                    glPushMatrix();

                    if (i > 0)
                    {
                        glRotatef(randomNum(-8.0f, -10.0f), 1, 0, 0);
                        glRotatef(randomNum(-5.0f, 5.0f), 0, 1, 0);
                        glRotatef(randomNum(-5.0f, 5.0f), 0, 0, 1);

                        glTranslatef(0.0, 0.0, mdl->ponyOff);
                    }

                    if (mdl->pigtails)
                    {
                        glPushMatrix();
                        glTranslatef(mdl->ponyOff2, 0.0, 0.0);
                        drawModelMesh(getWorld().getMesh(mdl->ponytailMeshId + i),
                                Render::skeletalMesh);
                        glPopMatrix();

                        glPushMatrix();
                        glTranslatef(-mdl->ponyOff2, 0.0, 0.0);
                        drawModelMesh(getWorld().getMesh(mdl->ponytailMeshId + i),
                                Render::skeletalMesh);
                        glPopMatrix();
                    }
                    else
                    {
                        drawModelMesh(getWorld().getMesh(mdl->ponytailMeshId + i),
                                Render::skeletalMesh);
                    }
                }

                for (unsigned int i = 0; i < mdl->ponytailNumMeshes; ++i)
                {
                    glPopMatrix();
                }
#endif
                glPopMatrix();
            }
        }

        drawModelMesh(getWorld().getMesh(tag->mesh), Render::skeletalMesh);
    }

    // Cycle frames ( cheap hack from old ent state based system )
    if (mFlags & fAnimateAllModels)
    {
        if (model->getFrame() + 1 > (int)animation->frame.size()-1)
        {
            model->setFrame(0);
        }
        else
        {
            model->setFrame(model->getFrame()+1);
        }
    }
}


void draw_bbox(vec3_t min, vec3_t max, bool draw_points,
        const vec4_t c1, const vec4_t c2)
{
    // Bind before entering now
    //glBindTexture(GL_TEXTURE_2D, 1);
    glPointSize(4.0);
    //glLineWidth(1.25);

    //! \fixme Need to make custom color key for this
    glColor3fv(c1);

    glBegin(GL_POINTS);
    glVertex3f(max[0], max[1], max[2]);
    glVertex3f(min[0], min[1], min[2]);

    if (draw_points)
    {
        glVertex3f(max[0], min[1], max[2]);
        glVertex3f(min[0], max[1], max[2]);
        glVertex3f(max[0], max[1], min[2]);
        glVertex3f(min[0], min[1], max[2]);
        glVertex3f(min[0], max[1], min[2]);
        glVertex3f(max[0], min[1], min[2]);
    }

    glEnd();

    glColor3fv(c2);

    glBegin(GL_LINES);
    // max, top quad
    glVertex3f(max[0], max[1], max[2]);
    glVertex3f(max[0], min[1], max[2]);

    glVertex3f(max[0], max[1], max[2]);
    glVertex3f(min[0], max[1], max[2]);

    glVertex3f(max[0], max[1], max[2]);
    glVertex3f(max[0], max[1], min[2]);

    // max-min, vertical quads
    glVertex3f(min[0], max[1], max[2]);
    glVertex3f(min[0], max[1], min[2]);

    glVertex3f(max[0], min[1], max[2]);
    glVertex3f(max[0], min[1], min[2]);

    glVertex3f(max[0], min[1], max[2]);
    glVertex3f(min[0], min[1], max[2]);

    // min-max, vertical quads
    glVertex3f(max[0], max[1], min[2]);
    glVertex3f(max[0], min[1], min[2]);

    glVertex3f(max[0], max[1], min[2]);
    glVertex3f(min[0], max[1], min[2]);

    glVertex3f(min[0], max[1], max[2]);
    glVertex3f(min[0], min[1], max[2]);


    // min, bottom quad
    glVertex3f(min[0], min[1], min[2]);
    glVertex3f(min[0], max[1], min[2]);

    glVertex3f(min[0], min[1], min[2]);
    glVertex3f(max[0], min[1], min[2]);

    glVertex3f(min[0], min[1], min[2]);
    glVertex3f(min[0], min[1], max[2]);
    glEnd();

    glPointSize(1.0);
    //glLineWidth(1.0);
}


void Render::drawRoom(Room &room, bool draw_alpha)
{
    if (!(mFlags & Render::fRoomAlpha) && draw_alpha)
        return;

    glPushMatrix();
    //LightingSetup();

    mTexture.bindTextureId(0); // WHITE texture

    if (!draw_alpha &&
            (mFlags & Render::fPortals || mMode == Render::modeWireframe)) {
        glLineWidth(2.0);
        glColor3fv(RED);

        for (unsigned int i = 0; i < room.sizePortals(); i++) {
            Portal &portal = room.getPortal(i);
            vec3_t vertices[4];
            portal.getVertices(vertices);

            glBegin(GL_LINE_LOOP);
            glVertex3fv(vertices[0]);
            glVertex3fv(vertices[1]);
            glVertex3fv(vertices[2]);
            glVertex3fv(vertices[3]);
            glEnd();
        }

        glLineWidth(1.0);
    }

    if (mMode == Render::modeWireframe && !draw_alpha) {
        vec3_t bbox[2];
        room.getBoundingBox(bbox);
        draw_bbox(bbox[0], bbox[1], true, RED, GREEN);
    }

    vec3_t pos;
    room.getPos(pos);
    glTranslated(pos[0], pos[1], pos[2]);

    // Reset since GL_MODULATE used, reset to WHITE
    glColor3fv(WHITE);

    switch (mMode)
    {
        case modeWireframe:
            room.getMesh().mMode = Mesh::MeshModeWireframe;
            break;
        case modeSolid:
            room.getMesh().mMode = Mesh::MeshModeSolid;
            break;
        default:
            room.getMesh().mMode = Mesh::MeshModeTexture;
            break;
    }

    if (draw_alpha)
        room.getMesh().drawAlpha();
    else
        room.getMesh().drawSolid();

    glPopMatrix();

    //mTexture.bindTextureId(0);

    // Draw other room meshes and sprites
    if (draw_alpha || mMode == modeWireframe || mMode == modeSolid) {
        if (mFlags & Render::fRoomModels) {
            room.sortModels();
            for (unsigned int i = 0; i < room.sizeModels(); i++) {
                room.getModel(i).display();
            }
        }

        // Draw other room alpha polygon objects
        if (mFlags & Render::fSprites) {
            for (unsigned int i = 0; i < room.sizeSprites(); i++) {
                room.getSprite(i).display();
            }
        }
    }
}


void Render::tmpRenderModelMesh(model_mesh_t *r_mesh, texture_tri_t *ttri)
{
    glBegin(GL_TRIANGLES);

    switch (mMode)
    {
        case modeSolid:
        case modeVertexLight:
            if (r_mesh->colors)
            {
                glColor3fv(r_mesh->colors+ttri->index[0]);
                glTexCoord2fv(ttri->st);
                glVertex3fv(r_mesh->vertices+ttri->index[0]*3);

                glColor3fv(r_mesh->colors+ttri->index[1]);
                glTexCoord2fv(ttri->st+2);
                glVertex3fv(r_mesh->vertices+ttri->index[1]*3);

                glColor3fv(r_mesh->colors+ttri->index[2]);
                glTexCoord2fv(ttri->st+4);
                glVertex3fv(r_mesh->vertices+ttri->index[2]*3);
            }
            else if (r_mesh->normals)
            {
                glNormal3fv(r_mesh->normals+ttri->index[0]*3);
                glTexCoord2fv(ttri->st);
                glVertex3fv(r_mesh->vertices+ttri->index[0]*3);

                glNormal3fv(r_mesh->normals+ttri->index[1]*3);
                glTexCoord2fv(ttri->st+2);
                glVertex3fv(r_mesh->vertices+ttri->index[1]*3);

                glNormal3fv(r_mesh->normals+ttri->index[2]*3);
                glTexCoord2fv(ttri->st+4);
                glVertex3fv(r_mesh->vertices+ttri->index[2]*3);
            }
            else
            {
                glTexCoord2fv(ttri->st);
                glVertex3fv(r_mesh->vertices+ttri->index[0]*3);
                glTexCoord2fv(ttri->st+2);
                glVertex3fv(r_mesh->vertices+ttri->index[1]*3);
                glTexCoord2fv(ttri->st+4);
                glVertex3fv(r_mesh->vertices+ttri->index[2]*3);
            }
            break;
        case modeWireframe:
            glVertex3fv(r_mesh->vertices+ttri->index[0]*3);
            glVertex3fv(r_mesh->vertices+ttri->index[1]*3);
            glVertex3fv(r_mesh->vertices+ttri->index[2]*3);
            break;
        default:
            glTexCoord2fv(ttri->st);
            glVertex3fv(r_mesh->vertices+ttri->index[0]*3);
            glTexCoord2fv(ttri->st+2);
            glVertex3fv(r_mesh->vertices+ttri->index[1]*3);
            glTexCoord2fv(ttri->st+4);
            glVertex3fv(r_mesh->vertices+ttri->index[2]*3);
    }

    glEnd();
}


void Render::drawModelMesh(model_mesh_t *r_mesh, RenderMeshType type)
{
    texture_tri_t *ttri;
    int lastTexture = -1;


    // If they pass NULL structs let it hang up - this is tmp

    //! \fixme Duh, vis tests need to be put back
    //if (!isVisible(r_mesh->center,    r_mesh->radius, r_mesh->bbox))
    //{
    //   return;
    //}

#ifdef USE_GL_ARRAYS
    // Setup Arrays ( move these to another method depends on mMode )
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, r_mesh->vertices);

    if (r_mesh->normals)
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(3, GL_FLOAT, 0, r_mesh->normals);
    }

    if (r_mesh->colors)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, 0, r_mesh->colors);
    }

    //glTexCoordPointer(2, GL_FLOAT, 0, ttri->st);
    //glDrawArrays(GL_TRIANGLES, i * 3, 3 * j);

    glBegin(GL_TRIANGLES);

    for (unsigned int i = 0; i < r_mesh->texturedTriangles.size(); i++)
    {
        ttri = r_mesh->texturedTriangles[i];

        if (!ttri)
            continue;

        for (k = 0; k < 4; ++k)
        {
            index = mQuads[i].quads[j*4+k];
            glTexCoord2fv(mQuads[i].texcoors[j*4+k]);
            glArrayElement(mVertices[index]);
        }
    }

    glEnd();
#endif


    //! \fixme 'AMBIENT' -- Mongoose 2002.01.08
    glColor3fv(WHITE);

    if (mMode == modeWireframe)
    {
        switch (type)
        {
            case roomMesh:
                glColor3fv(YELLOW);
                break;
            case skeletalMesh:
                glColor3fv(WHITE);
                break;
        }
    }

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, 1);  // White texture for colors

    // Colored Triagles
    for (unsigned int i = 0; i < r_mesh->coloredTriangles.size(); i++)
    {
        ttri = r_mesh->coloredTriangles[i];

        if (!ttri)
            continue;

        if (mMode != modeWireframe && mMode != modeSolid &&
                ttri->texture != lastTexture)
        {
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glBindTexture(GL_TEXTURE_2D, ttri->texture+1);
            lastTexture = ttri->texture;
        }

        tmpRenderModelMesh(r_mesh, ttri);
    }

    // Colored Rectagles
    for (unsigned int i = 0; i < r_mesh->coloredRectangles.size(); i++)
    {
        ttri = r_mesh->coloredRectangles[i];

        if (!ttri)
            continue;

        if (mMode != modeWireframe && mMode != modeSolid &&
                ttri->texture != lastTexture)
        {
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glBindTexture(GL_TEXTURE_2D, ttri->texture+1);
            lastTexture = ttri->texture;
        }

        tmpRenderModelMesh(r_mesh, ttri);
    }

    // Textured Tris
    for (unsigned int i = 0; i < r_mesh->texturedTriangles.size(); i++)
    {
        ttri = r_mesh->texturedTriangles[i];

        if (!ttri)
            continue;

        if (mMode != modeWireframe && mMode != modeSolid &&
                ttri->texture != lastTexture)
        {
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glBindTexture(GL_TEXTURE_2D, ttri->texture+1);
            lastTexture = ttri->texture;
        }

        tmpRenderModelMesh(r_mesh, ttri);
    }

    // Textured Quads
    for (unsigned int i = 0; i < r_mesh->texturedRectangles.size(); i++)
    {
        ttri = r_mesh->texturedRectangles[i];

        if (!ttri)
            continue;

        if (mMode != modeWireframe && mMode != modeSolid &&
                ttri->texture != lastTexture)
        {
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glBindTexture(GL_TEXTURE_2D, ttri->texture+1);
            lastTexture = ttri->texture;
        }

        tmpRenderModelMesh(r_mesh, ttri);
    }
}


void Render::setSkyMesh(int index, bool rot)
{
    mSkyMesh = index;
    mSkyMeshRotation = rot;
}


void Render::ViewModel(entity_t *ent, int index)
{
    skeletal_model_t *model;


    if (!ent)
    {
        return;
    }

    model = getWorld().getModel(index);

    if (model)
    {
        ent->modelId = index;
        printf("Viewmodel skeletal model %i\n", model->id);
    }
}


void Render::addSkeletalModel(SkeletalModel *mdl)
{
    mModels.push_back(mdl);
}


void Render::updateViewVolume()
{
    matrix_t proj;
    matrix_t mdl;


    glGetFloatv(GL_PROJECTION_MATRIX, proj);
    glGetFloatv(GL_MODELVIEW_MATRIX, mdl);
    mViewVolume.updateFrame(proj, mdl);
}


bool Render::isVisible(float bbox_min[3], float bbox_max[3])
{
    // For debugging purposes
    if (mMode == Render::modeWireframe)
    {
        //glPointSize(5.0);
        //glColor3fv(PINK);
        //glBegin(GL_POINTS);
        //glVertex3fv(bbox_min);
        //glVertex3fv(bbox_max);
        //glEnd();

        draw_bbox(bbox_min, bbox_max, true, PINK, RED);
    }

    return mViewVolume.isBboxInFrustum(bbox_min, bbox_max);
}


bool Render::isVisible(float x, float y, float z)
{
    // For debugging purposes
    if (mMode == Render::modeWireframe)
    {
        glPointSize(5.0);
        glColor3fv(PINK);
        glBegin(GL_POINTS);
        glVertex3f(x, y, z);
        glEnd();
    }

    return (mViewVolume.isPointInFrustum(x, y, z));
}


bool Render::isVisible(float x, float y, float z, float radius)
{
    // For debugging purposes
    if (mMode == Render::modeWireframe)
    {
        glPointSize(5.0);
        glColor3fv(PINK);
        glBegin(GL_POINTS);
        glVertex3f(x, y, z);
        glEnd();
    }

    return (mViewVolume.isSphereInFrustum(x, y, z, radius));
}
