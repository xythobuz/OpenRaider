/*!
 * \file src/Render.cpp
 * \brief OpenRaider Renderer class
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <algorithm>
#include <sstream>

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "global.h"
#include "Camera.h"
#include "Game.h"
#include "Render.h"
#include "TextureManager.h"
#include "utils/strings.h"
#include "utils/tga.h"
#include "Window.h"
#include "World.h"

Render::Render() {
    mSkyMesh = -1;
    mSkyMeshRotation = false;
    mMode = Render::modeDisabled;
    mLock = 0;
    mFlags = (fRoomAlpha | fEntityModels | fRenderPonytail);
}

Render::~Render() {
    ClearWorld();
}

void Render::ClearWorld() {
    mRoomRenderList.clear();
}

void Render::screenShot(const char *filenameBase)
{
    int sz = getWindow().getWidth() * getWindow().getHeight();
    unsigned char *image = new unsigned char[sz * 3];
    static int count = 0;
    bool done = false;

    assert(filenameBase != nullptr);

    // Don't overwrite files
    std::ostringstream filename;
    while (!done) {
        filename << filenameBase << "-" << count++ << ".tga";

        FILE *f = fopen(filename.str().c_str(), "rb");
        if (f) {
            fclose(f);
        } else {
            done = true;
        }
    }

    glReadPixels(0, 0, getWindow().getWidth(), getWindow().getHeight(), GL_BGR_EXT, GL_UNSIGNED_BYTE, image);
    tgaSave(filename.str().c_str(), image, getWindow().getWidth(), getWindow().getHeight(), 0);
    delete [] image;
}

unsigned int Render::getFlags() {
    return mFlags;
}

void Render::lightRoom(Room &room)
{
    for (unsigned int i = 0; i < room.sizeLights(); ++i)
    {
        Light &light = room.getLight(i);
        float pos[4], color[4];
        float dir[3];
        light.getPos(pos);
        light.getColor(color);
        light.getDir(dir);

        glEnable(GL_LIGHT0 + i);

        switch (light.getType())
        {
            case Light::typeSpot:
                glLightf(GL_LIGHT0 + i,  GL_SPOT_CUTOFF,    light.getCutoff());
                glLightfv(GL_LIGHT0 + i, GL_POSITION,       pos);
                glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, dir);
                glLightfv(GL_LIGHT0 + i, GL_DIFFUSE,        color);
                break;
            case Light::typePoint:
            case Light::typeDirectional:
                glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 1.0); // 1.0

                // GL_QUADRATIC_ATTENUATION
                // GL_LINEAR_ATTENUATION
                glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, light.getAtt());

                glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, color); // GL_DIFFUSE
                glLightfv(GL_LIGHT0 + i, GL_POSITION, pos);
                break;
        }
    }
}

void Render::clearFlags(unsigned int flags)
{
    mFlags &= ~flags;

    if (flags & Render::fFog)
    {
        if (glIsEnabled(GL_FOG))
        {
            glDisable(GL_FOG);
        }
    }

    if (flags & Render::fGL_Lights)
    {
        glDisable(GL_LIGHTING);
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

        float color[4];
        color[0] = BLACK[0] * 256.0f;
        color[1] = BLACK[1] * 256.0f;
        color[2] = BLACK[2] * 256.0f;
        color[3] = BLACK[3] * 256.0f;
        glFogfv(GL_FOG_COLOR, color);
    }

    if (flags & Render::fGL_Lights)
    {
        float color[4];
        color[0] = WHITE[0] * 256.0f;
        color[1] = WHITE[1] * 256.0f;
        color[2] = WHITE[2] * 256.0f;
        color[3] = WHITE[3] * 256.0f;

        glEnable(GL_LIGHTING);
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);

        color[0] = GREY[0] * 256.0f;
        color[1] = GREY[1] * 256.0f;
        color[2] = GREY[2] * 256.0f;
        color[3] = GREY[3] * 256.0f;
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, color);
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
            glClearColor(PURPLE[0] * 256.0f, PURPLE[1] * 256.0f,
                    PURPLE[2] * 256.0f, PURPLE[3] * 256.0f);
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

void Render::display()
{
    switch (mMode)
    {
        case Render::modeDisabled:
            return;
        case Render::modeLoadScreen:
            //! \fixme entry for seperate main drawing method -- Mongoose 2002.01.01
            drawLoadScreen();
            return;
        default:
            break;
    }

    if (mMode == Render::modeWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    float camOffsetH = 0.0f;

    switch (getGame().getLara().getMoveType())
    {
        case Entity::MoveTypeFly:
        case Entity::MoveTypeNoClipping:
        case Entity::MoveTypeSwim:
            //camOffsetH = 64.0f;
            camOffsetH = 512.0f;
            break;
        case Entity::MoveTypeWalk:
        case Entity::MoveTypeWalkNoSwim:
            camOffsetH = 512.0f;
            break;
    }

    float curPos[3], camPos[3], atPos[3];

    curPos[0] = getGame().getLara().getPos(0);
    curPos[1] = getGame().getLara().getPos(1);
    curPos[2] = getGame().getLara().getPos(2);
    float yaw = getGame().getLara().getAngle(1);

    // Mongoose 2002.08.24, New 3rd person camera hack
    camPos[0] = curPos[0] - (1024.0f * sinf(yaw));
    camPos[1] = curPos[1] - camOffsetH; // UP is lower val
    camPos[2] = curPos[2] - (1024.0f * cosf(yaw));

    long index = getGame().getLara().getRoom();
    long sector = getWorld().getRoom(index).getSector(camPos[0], camPos[2]);

    // Handle camera out of world
    if ((sector < 0) ||
            ((unsigned int)sector >= getWorld().getRoom(index).sizeSectors()) ||
            getWorld().getRoom(index).isWall(sector)) {
        camPos[0] = curPos[0] + (64.0f * sinf(yaw));
        camPos[1] -= 64.0f;
        camPos[2] = curPos[2] + (64.0f * cosf(yaw));
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Setup view in OpenGL with camera
    getCamera().setPosition(camPos);
    getCamera().update();
    getCamera().getTarget(atPos);
    // Mongoose 2002.08.13, Quick fix to render OpenRaider upside down
    getWindow().lookAt(camPos[0], camPos[1], camPos[2], atPos[0], atPos[1], atPos[2], 0.0f, -1.0f, 0.0f);

    // Update view volume for vising
    updateViewVolume();

    // Render world
    glColor3ubv(GREY); // was WHITE
    drawSkyMesh(96.0);

    // Figure out how much of the map to render
    newRoomRenderList(index);

    // Room solid pass, need to do depth sorting to avoid 2 pass render
    for (unsigned int i = 0; i < mRoomRenderList.size(); i++) {
        Room &room = *mRoomRenderList[i];

        if (mFlags & Render::fGL_Lights)
            lightRoom(room);

        room.display(false);
    }

    // Draw all visible enities
    if (mFlags & Render::fEntityModels) {
        std::vector<Entity *> entityRenderList;

        for (unsigned int i = 0; i < getWorld().sizeEntity(); i++) {
            Entity &e = getWorld().getEntity(i);

            // Don't show Lara to the player
            if (&e == &getGame().getLara())
                continue;

            // Mongoose 2002.08.15, Nothing to draw, skip
            // Mongoose 2002.12.24, Some entities have no animation =p
            if (e.getModel().size() == 0)
                    continue;

            // Is it in view volume? ( Hack to use sphere )
            if (!isVisible(e.getPos(0), e.getPos(1), e.getPos(2), 512.0f))
                continue;

            //! \fixme Is it in a room we're rendering?
            //if (mRoomRenderList[e->room] == 0x0)
            //{
            //  continue;
            //}

            entityRenderList.push_back(&e);
        }

        // Draw objects not tied to rooms
        glPushMatrix();

        // Draw lara or other player model ( move to entity rendering method )
        getGame().getLara().display();

        // Draw sprites after player to handle alpha
        for (unsigned int i = 0; i < getWorld().sizeSprite(); i++) {
            SpriteSequence &sprite = getWorld().getSprite(i);
            for (unsigned int j = 0; j < sprite.size(); j++)
                sprite.get(j).display();
        }

        glPopMatrix();

        // Depth sort entityRenderList and display each entity
        std::sort(entityRenderList.begin(), entityRenderList.end(), Entity::compare);
        for (unsigned int i = 0; i < entityRenderList.size(); i++) {
            entityRenderList[i]->display();
        }
    }

    // Room alpha pass
    // Skip room alpha pass for modes w/o texture
    if (!(mMode == Render::modeSolid || mMode == Render::modeWireframe)) {
        for (unsigned int i = 0; i < mRoomRenderList.size(); i++)
            mRoomRenderList[i]->display(true);
    }

    if (mMode == Render::modeWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glFlush();
}

void Render::drawLoadScreen()
{
    float x = 0.0f, y = 0.0f, z = 0.0f;
    float w = getWindow().getWidth(), h = getWindow().getHeight();

    if (getTextureManager().getTextureCount() <= 0)
        return;

    // Mongoose 2002.01.01, Rendered while game is loading...
    //! \fixme seperate logo/particle coor later
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glColor3ubv(WHITE);

    if (mFlags & Render::fGL_Lights)
        glDisable(GL_LIGHTING);

    // Mongoose 2002.01.01, Draw logo/load screen
    glTranslatef(0.0f, 0.0f, -2000.0f);
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);

    if (getTextureManager().getTextureCount() < 2)
        getTextureManager().bindTextureId(0); //! \fixme store texture id somewhere
    else
        getTextureManager().bindTextureId(1);

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

    if (index == -1) // -1 is error, so draw room 0, for the hell of it
    {
        mRoomRenderList.clear();
        mRoomRenderList.push_back(&getWorld().getRoom(0));
    }
    else
    {
        // Update room render list if needed
        if (currentRoomId != index)
        {
            buildRoomRenderList(getWorld().getRoom(index));
        }
    }

    currentRoomId = index;
}

void Render::buildRoomRenderList(Room &room)
{

    // Must be visible
    //! \fixme Add depth sorting here - remove multipass
    if (!isVisible(room.getBoundingBox()))
        return;

    // Must not already be cached
    for (unsigned int i = 0; i < mRoomRenderList.size(); i++)
    {
        Room *room2 = mRoomRenderList[i];

        if (room2 == &room)
            return;
    }

    /* Add current room to list */
    mRoomRenderList.push_back(&room);

    // Try to add adj rooms and their adj rooms, skip this room
    for (unsigned int i = 1; i < room.sizeAdjacentRooms(); i++)
    {
        if (room.getAdjacentRoom(i) < 0)
            continue;

        Room &room2 = getWorld().getRoom(room.getAdjacentRoom(i));

        //! \fixme Add portal visibility check here

        if (&room2 != &room)
            buildRoomRenderList(room2);
    }
}

void Render::drawSkyMesh(float scale)
{
    //skeletal_model_t *model = getWorld().getModel(mSkyMesh);
    //if (!model)
    //    return;

    glDisable(GL_DEPTH_TEST);
    glPushMatrix();

    if (mSkyMeshRotation)
        glRotated(90.0, 1, 0, 0);

    glTranslated(0.0, 1000.0, 0.0);
    glScaled(scale, scale, scale);
    //drawModel(model);
    //drawModelMesh(getWorld().getMesh(mSkyMesh), );
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
}

void Render::setSkyMesh(int index, bool rot)
{
    mSkyMesh = index;
    mSkyMeshRotation = rot;
}

void Render::updateViewVolume()
{
    float proj[16], mdl[16];

    glGetFloatv(GL_PROJECTION_MATRIX, proj);
    glGetFloatv(GL_MODELVIEW_MATRIX, mdl);
    mViewVolume.updateFrame(proj, mdl);
}

bool Render::isVisible(BoundingBox &box) {
    float bbox[2][3];
    box.getBoundingBox(bbox);

    // For debugging purposes
    if (mMode == Render::modeWireframe)
        box.display(true, PINK, RED);

    return mViewVolume.isBboxInFrustum(bbox[0], bbox[1]);
}

bool Render::isVisible(float x, float y, float z)
{
    // For debugging purposes
    if (mMode == Render::modeWireframe)
    {
        glPointSize(5.0);
        glColor3ubv(PINK);
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
        glColor3ubv(PINK);
        glBegin(GL_POINTS);
        glVertex3f(x, y, z);
        glEnd();
    }

    return (mViewVolume.isSphereInFrustum(x, y, z, radius));
}

