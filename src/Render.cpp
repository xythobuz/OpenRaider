/*!
 * \file src/Render.cpp
 * \brief OpenRaider Renderer class
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <algorithm>

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "global.h"
#include "Game.h"
#include "OpenRaider.h"
#include "Render.h"
#include "utils/strings.h"
#include "utils/tga.h"
#include "Window.h"

Render::Render() {
    mSkyMesh = -1;
    mSkyMeshRotation = false;
    mMode = Render::modeDisabled;
    mLock = 0;
    mFlags = (fRoomAlpha | fViewModel |
            fEntityModels | fRenderPonytail |
            fUsePortals | fUpdateRoomListPerFrame);
}


Render::~Render() {
    ClearWorld();
}

void Render::ClearWorld() {
    mRoomRenderList.clear();
}


void Render::screenShot(char *filenameBase)
{
    int sz = getWindow().getWidth() * getWindow().getHeight();
    unsigned char *image = new unsigned char[sz * 3];
    char *filename = NULL;
    static int count = 0;
    bool done = false;

    assert(filenameBase != NULL);

    // Don't overwrite files
    while (!done) {
        filename = bufferString("%s-%04i.tga", filenameBase, count++);

        FILE *f = fopen(filename, "rb");

        if (f)
            fclose(f);
        else
            done = true;
    }

    // Capture frame buffer
    glReadPixels(0, 0, getWindow().getWidth(), getWindow().getHeight(), GL_BGR_EXT, GL_UNSIGNED_BYTE, image);

    tgaSaveFilename(image, getWindow().getWidth(), getWindow().getHeight(), 0, "%s", filename);
    printf("Took screenshot '%s'.\n", filename);

    delete [] filename;
    delete [] image;
}

unsigned int Render::getFlags() {
    return mFlags;
}


void Render::loadTexture(unsigned char *image,
        unsigned int width, unsigned int height,
        unsigned int id)
{
    glColor3fv(WHITE);
    mTexture.loadBufferSlot(image, width, height, Texture::RGBA, 32, id);
}


int Render::initTextures(char *textureDir) {
    char *filename;
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

    // Temporary
    filename = bufferString("%s/tr2/TITLE.PCX", getOpenRaider().mPakDir);
    if (mTexture.loadPCX(filename) > -1) {
        numTextures++;
        delete [] filename;
    } else {
        delete [] filename;
        //! \fixme Error Checking. Return negative error code, check in calling place too
        filename = bufferString("%s/%s", textureDir, "splash.tga");
        if (mTexture.loadTGA(filename) > -1)
            numTextures++;
        delete [] filename;
    }

    return numTextures;
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
        vec4_t pos, color;
        vec3_t dir;
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

    float fMag = sqrtf(f[0] * f[0] + f[1] * f[1] + f[2] * f[2]);
    //float upMag = sqrtf(upX * upX + upY * upY + upZ * upZ);

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

    vec3_t curPos;
    vec3_t camPos;
    vec3_t atPos;

    curPos[0] = getGame().getLara().getPos(0);
    curPos[1] = getGame().getLara().getPos(1);
    curPos[2] = getGame().getLara().getPos(2);
    vec_t yaw = getGame().getLara().getAngle(1);

    // Mongoose 2002.08.24, New 3rd person camera hack
    camPos[0] = curPos[0] - (1024.0f * sinf(yaw));
    camPos[1] = curPos[1] - camOffsetH; // UP is lower val
    camPos[2] = curPos[2] - (1024.0f * cosf(yaw));

    int index = getGame().getLara().getRoom();
    int sector = getWorld().getRoom(index).getSector(camPos[0], camPos[2]);

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
    gluLookAt(camPos[0], camPos[1], camPos[2], atPos[0], atPos[1], atPos[2], 0.0f, -1.0f, 0.0f);

    // Update view volume for vising
    updateViewVolume();

    // Render world
    glColor3fv(DIM_WHITE); // was WHITE
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
        if (mFlags & Render::fViewModel)
            getGame().getLara().display();

        // Draw sprites after player to handle alpha
        for (unsigned int i = 0; i < getWorld().sizeSprite(); i++) {
            SpriteSequence &sprite = getWorld().getSprite(i);
            for (unsigned int j = 0; j < sprite.size(); j++)
                sprite.get(j).display();
        }

        glPopMatrix();

        // Depth sort entityRenderList with qsort
        std::sort(entityRenderList.begin(), entityRenderList.end());

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
    float x = 0.0f, y = 0.0f, z = -160.0f;
    float w = getWindow().getWidth(), h = getWindow().getHeight();

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
                if (!isVisible(getWorld().getRoom(i).getBoundingBox()))
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
    if (!isVisible(room.getBoundingBox()))
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


void Render::drawModelMesh(model_mesh_t *r_mesh)
{
    texture_tri_t *ttri;
    int lastTexture = -1;

    // If they pass NULL structs let it hang up - this is tmp

    //! \fixme Duh, vis tests need to be put back
    //if (!isVisible(r_mesh->center,    r_mesh->radius, r_mesh->bbox))
    //{
    //   return;
    //}

    //! \fixme 'AMBIENT' -- Mongoose 2002.01.08
    glColor3fv(WHITE);

    if (mMode == modeWireframe)
        glColor3fv(WHITE);

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


void Render::updateViewVolume()
{
    matrix_t proj;
    matrix_t mdl;


    glGetFloatv(GL_PROJECTION_MATRIX, proj);
    glGetFloatv(GL_MODELVIEW_MATRIX, mdl);
    mViewVolume.updateFrame(proj, mdl);
}

bool Render::isVisible(BoundingBox &box) {
    vec3_t bbox[2];
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
