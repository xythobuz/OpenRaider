/*!
 * \file src/Render.cpp
 * \brief OpenRaider Renderer class
 *
 * \author Mongoose
 */

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifdef USING_EMITTER
#include <Emitter.h>
#endif

#ifdef DEBUG_MEMORY
#include <memory_test.h>
#endif

#include <Render.h>

extern entity_t *LARA;
extern World gWorld;

// Colors
const float BLACK[]        = {  0.0f,  0.0f,  0.0f, 1.0f };
const float DIM_WHITE[]    = {  0.5f,  0.5f,  0.5f, 1.0f };
const float WHITE[]        = {  1.0f,  1.0f,  1.0f, 1.0f };
const float RED[]          = {  1.0f,  0.0f,  0.0f, 1.0f };
const float GREEN[]        = {  0.0f,  1.0f,  0.0f, 1.0f };
const float NEXT_PURPLE[]  = {  0.3f,  0.3f,  0.5f, 1.0f };
const float OR_BLUE[]      = {  0.5f,  0.7f,  1.0f, 1.0f };
const float PINK[]         = {  1.0f,  0.0f,  1.0f, 1.0f };
const float YELLOW[]       = {  1.0f,  1.0f,  0.0f, 1.0f };
const float CYAN[]         = {  0.0f,  1.0f,  1.0f, 1.0f };

ViewVolume gViewVolume; /* View volume for frustum culling */


int compareEntites(const void *voidA, const void *voidB)
{
    entity_t *a = (entity_t *)voidA, *b = (entity_t *)voidB;
    vec_t distA, distB;

    if (!a || !b)
        return -1; // error really

    distA = gViewVolume.getDistToSphereFromNear(a->pos[0],
            a->pos[1],
            a->pos[2],
            1.0f);
    distB = gViewVolume.getDistToSphereFromNear(b->pos[0],
            b->pos[1],
            b->pos[2],
            1.0f);

    // less than
    if (distA < distB)
        return -1;

    // greater than ( no need for equal )
    return 1;
}


int compareStaticModels(const void *voidA, const void *voidB)
{
    static_model_t *a = (static_model_t *)voidA, *b = (static_model_t *)voidB;
    vec_t distA, distB;

    if (!a || !b)
        return -1; // error really

    distA = gViewVolume.getDistToSphereFromNear(a->pos[0],
            a->pos[1],
            a->pos[2],
            128.0f);
    distB = gViewVolume.getDistToSphereFromNear(b->pos[0],
            b->pos[1],
            b->pos[2],
            128.0f);

    // less than
    if (distA < distB)
        return -1;

    // greater than ( no need for equal )
    return 1;
}


int compareRoomDist(const void *voidA, const void *voidB)
{
    const RenderRoom *a = static_cast<const RenderRoom *>(voidA);
    const RenderRoom *b = static_cast<const RenderRoom *>(voidB);


    if (!a || !b || !a->room || !b->room)
        return -1; // error really

    // less than
    if (a->dist < b->dist)
        return -1;

    // greater than ( no need for equal )
    return 1;
}


////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

Render::Render()
{
#ifdef USING_EMITTER
    mEmitter = 0x0;
#endif
    mCamera = 0x0;
    mSkyMesh = -1;
    mSkyMeshRotation = false;
    mMode = Render::modeDisabled;
    mLock = 0;
    mFlags = (Render::fRoomAlpha | Render::fViewModel | Render::fSprites |
            Render::fRoomModels | Render::fEntityModels |
            Render::fUsePortals | fUpdateRoomListPerFrame);

    mModels.setError(0x0);
    mRooms.setError(0x0);
    mRoomRenderList.setError(0x0);

    mNextTextureId = NULL;
    mNumTexturesLoaded = NULL;

    mWidth = 640;
    mHeight = 480;
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
    mTexture.glScreenShot(filenameBase, mWidth, mHeight);
}


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

void Render::addRoom(RenderRoom *room)
{
    mRooms.pushBack(room);
}


void Render::loadTexture(unsigned char *image,
        unsigned int width, unsigned int height,
        unsigned int id)
{
    glColor3fv(WHITE);
    mTexture.loadBufferSlot(image, width, height, Texture::RGBA, 32, id);
}


void Render::initTextures(char *textureDir, unsigned int *numLoaded,
        unsigned int *nextId)
{
    char filename[128];
    const char *console = "Toggle the Console with [`] key";
    const char *menu = "Press <esc> for menu";
    int font_id;
    int snow1_id;
    int snow2_id;
    int bg_id;
    int err;
    unsigned int numTextures = 0;
    unsigned char color[4];


    // We want to update as needed later
    mNumTexturesLoaded = numLoaded;
    mNextTextureId = nextId;

    mTexture.reset();
    mTexture.setMaxTextureCount(128);  /* TR never needs more than 32 iirc
                                          However, color texturegen is a lot */

    mTexture.setFlag(Texture::fUseMipmaps);

    printf("Processing Textures:\n");

    color[0] = 0xff;
    color[1] = 0xff;
    color[2] = 0xff;
    color[3] = 0xff;

    if ((font_id = mTexture.loadColorTexture(color, 32, 32)) > -1)
    {
        ++numTextures;
    }

    snprintf(filename, 126, "%s%s", textureDir, "splash.tga");
    filename[127] = 0;

    if ((bg_id = mTexture.loadTGA(filename)) > -1)
    {
        ++numTextures;
    }

    snprintf(filename, 126, "%s%s", textureDir, "snow.tga");
    filename[127] = 0;

    if ((snow1_id = mTexture.loadTGA(filename)) > -1)
    {
        ++numTextures;
    }

    snprintf(filename, 126, "%s%s", textureDir, "snow2.tga");
    filename[127] = 0;

    if ((snow2_id = mTexture.loadTGA(filename)) > -1)
    {
        ++numTextures;
    }

    extern char *gFontFilename;
    if ((font_id = mTexture.loadFontTTF(gFontFilename,
                    //0x303f, 0x3093-0x303f)) // Hiragana
            32, 126 - 32))  // ASCII
                > -1)
                {
                    ++numTextures;
                }

    // Weird that it isn't linear, must be some storage deal in Texture
    // I forgot about Id allocation
    *nextId = font_id;

    // Setup particle system test
    initEmitter("Snow test", 650, snow1_id, snow2_id);

    mString.Init(5);

    // String 0: OpenRaider version in lower right corner
    mString.Scale(1.00);
    err = mString.glPrintf(mWidth - 15 * strlen(VERSION),
            mHeight-35, "%s", VERSION);
    mString.SetString(0, "%s", VERSION);

    if (err)
    {
        printf("\n*** GLPrint test: ERROR %i\n", err);
    }

    // String 1: Used for FPS in game text output
    mString.Scale(0.75);
    err = mString.glPrintf(8, mHeight - 25, "%s", "                ");
    mString.SetString(1, "%s", "                ");

    if (err)
    {
        printf("\n*** GLPrint test: ERROR %i\n", err);
    }

    // String 2: Used for game console
    mString.Scale(1.0);
    err = mString.glPrintf(8, 25, "%s", console);
    mString.SetString(2, "%s", console);

    if (err)
    {
        printf("\n*** GLPrint test: ERROR %i\n", err);
    }

    // String 3: Used for one line map select menu
    mString.Scale(1.75);
    err = mString.glPrintf(mWidth/2-235, mHeight/2-24, "%s", menu);
    mString.SetString(3, "%s", menu);

    if (err)
    {
        printf("\n*** GLPrint test: ERROR %i\n", err);
    }

    // String 4: Used for one line in game text output
    mString.Scale(1.0);
    err = mString.glPrintf(8, 55, "%s", "                    ");
    mString.SetString(4, "%s", "                    ");

    if (err)
    {
        printf("\n*** GLPrint test: ERROR %i\n", err);
    }

    printf("\n");

    *numLoaded = numTextures;
}


void Render::initEmitter(const char *name, unsigned int size,
        unsigned int snowTex1, unsigned int snowTex2)
{
#ifdef USING_EMITTER
    if (mEmitter)
        delete mEmitter; // Public, so avoid possible leak

    // Mongoose 2002.01.01, Screwing around with particle emitter test
    //   note this is backwards b/c load screen is rendered upsidedown
    //mEmitter = new Emitter(/*name*/"snow", size);
    mEmitter = new Emitter(name, size);
    mEmitter->SetTextureId(snowTex1);
    mEmitter->TextureId(120, 280, snowTex2);
    mEmitter->TextureId(400, 450, snowTex2);
    mEmitter->TextureId(500, 550, snowTex2);

    // Mongoose 2002.01.01, Varing force and speed should look
    //   like varing mass/SA in wind, maybe
    mEmitter->Speed(0, 150, 3500, 3000, 3500);
    mEmitter->Speed(150, 350, 3000, 4000, 3000);
    mEmitter->Speed(400, 500, 2000, 5000, 2000);
    mEmitter->Speed(400, 500, 2000, 5000, 2000);
    mEmitter->Force(100, 200, 0.0, 7.0, 0.0);
    mEmitter->Force(200, 300, 0.0, 5.0, 0.0);
    mEmitter->Force(300, 500, 0.0, 10.0, 0.0);
    mEmitter->Force(500, 650, 0.0, 9.0, 0.0);
#endif
}


void Render::ClearWorld()
{
    LARA = NULL;

    mRoomRenderList.clear();

    mRooms.erase();

    mModels.erase();

#ifdef USING_EMITTER
    if (mEmitter)
    {
        delete mEmitter;
        mEmitter = 0x0;
    }
#endif
}


// Texture must be set to WHITE solid color texture
void renderTrace(int color, vec3_t start, vec3_t end)
{
    const float widthStart = 10.0f; //5.0f;
    const float widthEnd = 10.0f;
    float delta = helRandomNum(0.01f, 0.16f); // for flicker fx


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


void Render::Init(int width, int height)
{
    char *s;

    mWidth = width;
    mHeight = height;

    // Print driver support information
    printf("\n## GL Driver Info ##\n");
    printf("Vendor     : %s\n", glGetString(GL_VENDOR));
    printf("Renderer   : %s\n", glGetString(GL_RENDERER));
    printf("Version    : %s\n\n", glGetString(GL_VERSION));
    //printf("\tExtensions : %s\n\n\n", (char*)glGetString(GL_EXTENSIONS));

    // Testing for goodies
    // Mongoose 2001.12.31, Fixed string use to check for bad strings
    s = (char*)glGetString(GL_EXTENSIONS);

    if (s && s[0])
    {
        printf("\tGL_ARB_multitexture       \t\t");

        if (strstr(s, "GL_ARB_multitexture"))
        {
            mFlags |= Render::fMultiTexture;
            printf("YES\n");
        }
        else
        {
            printf("NO\n");
        }

        printf("\tGL_EXT_texture_env_combine\t\t");

        if (strstr(s, "GL_EXT_texture_env_combine"))
        {
            printf("YES\n");
        }
        else
        {
            printf("NO\n");
        }
    }

    // Set up Z buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set up culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    //glFrontFace(GL_CCW);
    //glCullFace(GL_FRONT);

    // Set background to black
    glClearColor(BLACK[0], BLACK[1], BLACK[2], BLACK[3]);

    // Disable lighting
    glDisable(GL_LIGHTING);

    // Set up alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    //glEnable(GL_ALPHA_TEST); // Disable per pixel alpha blending
    glAlphaFunc(GL_GREATER, 0);

    glPointSize(5.0);

    // Setup shading
    glShadeModel(GL_SMOOTH);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_FOG_HINT, GL_NICEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DITHER);

    // AA polygon edges
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_AUTO_NORMAL);
    glDisable(GL_LOGIC_OP);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_FOG);

    glDisable(GL_NORMALIZE);

    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_EDGE_FLAG_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glPolygonMode(GL_FRONT, GL_FILL);

    glMatrixMode(GL_MODELVIEW);
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


void lightRoom(RenderRoom *room)
{
    unsigned int i;
    Light *light;


    for (i = 0; i < room->lights.size(); ++i)
    {
        light = room->lights[i];

        if (!light)
            continue;

        glEnable(GL_LIGHT0 + i);

        switch (light->mType)
        {
            case Light::typeSpot:
                glLightf(GL_LIGHT0 + i,  GL_SPOT_CUTOFF,    light->mCutoff);
                glLightfv(GL_LIGHT0 + i, GL_POSITION,       light->mPos);
                glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, light->mDir);
                glLightfv(GL_LIGHT0 + i, GL_DIFFUSE,        light->mColor);
                break;
            case Light::typePoint:
            case Light::typeDirectional:
                glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 1.0); // 1.0

                // GL_QUADRATIC_ATTENUATION
                // GL_LINEAR_ATTENUATION
                glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, light->mAtt);

                glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light->mColor); // GL_DIFFUSE
                glLightfv(GL_LIGHT0 + i, GL_POSITION, light->mPos);
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


void Render::Update(int width, int height)
{
    mWidth = width;
    mHeight = height;
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
void CrossProd(float x1, float y1, float z1, float x2, float y2, float z2, float res[3])
{
    res[0] = y1*z2 - y2*z1;
    res[1] = x2*z1 - x1*z2;
    res[2] = x1*y2 - x2*y1;
}
void deprecated_gluLookAt(float eyeX, float eyeY, float eyeZ, float lookAtX, float lookAtY, float lookAtZ, float upX, float upY, float upZ)
{
    float f[3];
    // calculating the viewing vector
    f[0] = lookAtX - eyeX;
    f[1] = lookAtY - eyeY;
    f[2] = lookAtZ - eyeZ;
    float fMag, upMag;
    fMag = sqrtf(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]);
    upMag = sqrtf(upX*upX + upY*upY + upZ*upZ);
    // normalizing the viewing vector
    f[0] = f[0]/fMag;
    f[1] = f[1]/fMag;
    f[2] = f[2]/fMag;
    // normalising the up vector. no need for this here if you have your
    // up vector already normalised, which is mostly the case.
    upX = upX/upMag;
    upY = upY/upMag;
    upZ = upZ/upMag;
    float s[3], u[3];
    CrossProd(f[0], f[1], f[2], upX, upY, upZ, s);
    CrossProd(s[0], s[1], s[2], f[0], f[1], f[2], u);
    float M[]=
    {
        s[0], u[0], -f[0], 0,
        s[1], u[1], -f[1], 0,
        s[2], u[2], -f[2], 0,
        0, 0, 0, 1
    };
    glMultMatrixf(M);
    glTranslatef (-eyeX, -eyeY, -eyeZ);
}

void Render::Display()
{
    vec3_t curPos;
    vec3_t camPos;
    vec3_t atPos;
    RenderRoom *room;
    int index;


#ifdef DEBUG_MATRIX
    gl_test_reset();
#endif

    // Assertion: Rendering is disabled without texture or camera
    if (!mCamera)
    {
        fprintf(stderr, "Render::Display> ERROR: No camera is registered\n");
        return;
    }

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

    if (LARA)
    {
        float yaw;
        int sector;
        float camOffsetH = 0.0f;


        switch (LARA->moveType)
        {
            case worldMoveType_fly:
            case worldMoveType_noClipping:
            case worldMoveType_swim:
                camOffsetH = 64.0f;
                break;
            default:
            case worldMoveType_walk:
            case worldMoveType_walkNoSwim:
                camOffsetH = 512.0f;
        }

        curPos[0] = LARA->pos[0];
        curPos[1] = LARA->pos[1];
        curPos[2] = LARA->pos[2];
        yaw = LARA->angles[1];

        index = LARA->room;

        // Mongoose 2002.08.24, New 3rd person camera hack
        camPos[0] = curPos[0];
        camPos[1] = curPos[1] - camOffsetH; // UP is lower val
        camPos[2] = curPos[2];

        camPos[0] -= (1024.0f * sinf(yaw));
        camPos[2] -= (1024.0f * cosf(yaw));

        sector = gWorld.getSector(index, camPos[0], camPos[2]);

        // Handle camera out of world
        if (sector < 0 || gWorld.isWall(index, sector))
        {
            camPos[0] = curPos[0] + (64.0f * sinf(yaw));
            camPos[1] -= 64.0f;
            camPos[2] = curPos[2] + (64.0f * cosf(yaw));
        }

        mCamera->setPosition(camPos);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Setup view in OpenGL with camera
    mCamera->update();
    mCamera->getPosition(camPos);
    mCamera->getTarget(atPos);
    // Mongoose 2002.08.13, Quick fix to render OpenRaider upside down
    // gluLookAt(camPos[0], camPos[1], camPos[2], atPos[0], atPos[1], atPos[2], 0.0, -1.0, 0.0);

    deprecated_gluLookAt(camPos[0], camPos[1], camPos[2], atPos[0], atPos[1], atPos[2], 0.0f, -1.0f, 0.0f);

    // Update view volume for vising
    updateViewVolume();

    // Let's see the LoS -- should be event controled
    if (LARA)
    {
        //      SkeletalModel *mdl = (SkeletalModel *)LARA->tmpHook;

        // Draw in solid colors
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_CULL_FACE);

        if (LARA->state == 64) // eWeaponFire
        {
            vec3_t u, v; //, s, t;

            // Center of LARA
            u[0] = curPos[0];
            u[1] = curPos[1] - 512.0f;
            u[2] = curPos[2];

            // Location LARA is aiming at?  ( Not finished yet )
            v[0] = u[0] + (9000.0f * sinf(LARA->angles[1]));
            v[1] = u[1] + (9000.0f * sinf(LARA->angles[2]));
            v[2] = u[2] + (9000.0f * cosf(LARA->angles[1]));

            //mtkVectorSubtract(u, v, t);
            //mtkVectorSubtract(u, curPos, s);
            //printf("* %f rad\n", LARA->angles[1]);

            // Test tracing of aim
            renderTrace(0, u, v); // v = target
        }

        entity_t *route = LARA->master;

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
    for (mRoomRenderList.start(); mRoomRenderList.forward();
            mRoomRenderList.next())
    {
        room = mRoomRenderList.current();

        if (room)
        {
            if (mFlags & Render::fGL_Lights)
            {
                lightRoom(room);
            }

            drawRoom(room, false);
        }
    }

    // Draw all visible enities
    if (mFlags & Render::fEntityModels)
    {
        entity_t *e;
        Vector<entity_t *> entityRenderList;
        Vector<entity_t *> *entities = gWorld.getEntities();

        for (entities->start(); entities->forward(); entities->next())
        {
            e = entities->current();

            // Mongoose 2002.03.26, Don't show lara to it's own player
            if (!e || e == LARA)
            {
                continue;
            }

            // Mongoose 2002.08.15, Nothing to draw, skip
            // Mongoose 2002.12.24, Some entities have no animation  =p
            if (e->tmpHook)
            {
                SkeletalModel *mdl = static_cast<SkeletalModel *>(e->tmpHook);

                if (mdl->model->animation.empty())
                    continue;
            }

            // Is it in view volume? ( Hack to use sphere )
            if (!isVisible(e->pos[0], e->pos[1], e->pos[2], 512.0f))
                continue;

            // Is it in a room we're rendering?
            //if (mRoomRenderList[e->room] == 0x0)
            //{
            //  continue;
            //}

            entityRenderList.pushBack(e);
        }

        // Draw objects not tied to rooms
        glPushMatrix();
        drawObjects();
        glPopMatrix();

        // Depth sort entityRenderList with qsort
        entityRenderList.qSort(compareEntites);

        for (entityRenderList.start(); entityRenderList.forward();
                entityRenderList.next())
        {
            e = entityRenderList.current();

            glPushMatrix();
            glTranslatef(e->pos[0], e->pos[1], e->pos[2]);
            glRotatef(e->angles[1], 0, 1, 0);
            drawModel(static_cast<SkeletalModel *>(e->tmpHook));
            glPopMatrix();
        }
    }

    // Room alpha pass
    // Skip room alpha pass for modes w/o texture
    if (!(mMode == Render::modeSolid || mMode == Render::modeWireframe))
    {
        for (mRoomRenderList.start(); mRoomRenderList.forward();
                mRoomRenderList.next())
        {
            room = mRoomRenderList.current();

            if (room)
            {
                drawRoom(room, true);
            }
        }
    }

#ifdef USING_EMITTER_IN_GAME
    // Mongoose 2002.01.01, Test particle prototype in game
    if (EMIT && mFlags & RENDER_F_PARTICLES)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glPushMatrix();
        glLoadIdentity();

        glEnable(GL_BLEND);
        glRotatef(180.0, 1.0, 0.0, 0.0);
        glTranslatef(0.0, -820.0, 575.0);
        glScalef(80.0, 80.0, 80.0);

        EMIT->Draw();

        glPopMatrix();

        // Mongoose 2002.03.26, Account for particle system
        //   not using new binds by setting WHITE texture here
        mTexture.bindTextureId(0);
    }
#endif

    if (mMode == Render::modeWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnterMode2d(mWidth, mHeight);
    glColor3fv(OR_BLUE);
    mString.Render();
    glExitMode2d();

#ifdef USING_EMITTER
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
#endif

    // Mongoose 2002.01.01, Test matrix ops
#ifdef DEBUG_MATRIX
    if (gl_test_val())
    {
        printf("ERROR in matrix stack %i\n", gl_test_val());
    }
#endif

    glFlush();
}


void Render::newRoomRenderList(int index)
{
    static int currentRoomId = -1;
    RenderRoom *room;


    if (mFlags & Render::fUsePortals)
    {
        if (index == -1) // -1 is error, so draw room 0, for the hell of it
        {
            room = mRooms[0];

            mRoomRenderList.clear();

            if (room)
            {
                mRoomRenderList.pushBack(room);
            }
        }
        else
        {
            // Update room render list if needed
            if (mFlags & Render::fUpdateRoomListPerFrame ||
                    currentRoomId != index)
            {
                mRoomRenderList.clear();
                room = mRooms[index];
                buildRoomRenderList(room);
            }
        }
    }
    else // Render all rooms pretty much
    {
        if (currentRoomId != index || index == -1)
        {
            printf("*** Room render list -> %i\n", index);
            mRoomRenderList.clear();

            for (mRooms.start(); mRooms.forward(); mRooms.next())
            {
                room = mRooms.current();

                if (!room || !room->room)
                    continue;

                if (!isVisible(room->room->bbox_min, room->room->bbox_max))
                    continue;

                //room->dist =
                //gViewVolume.getDistToBboxFromNear(room->room->bbox_min,
                //                                           room->room->bbox_max);

                mRoomRenderList.pushBack(room);
            }
        }
    }

    // Depth Sort roomRenderList ( no use in that, work on portals first )
    mRoomRenderList.qSort(compareRoomDist);

    currentRoomId = index;
}


void Render::buildRoomRenderList(RenderRoom *rRoom)
{
    RenderRoom *rRoom2;


    // Must exist
    if (!rRoom || !rRoom->room)
        return;

    // Must be visible
    //! \fixme Add depth sorting here - remove multipass
    if (!isVisible(rRoom->room->bbox_min, rRoom->room->bbox_max))
        return;

    // Must not already be cached
    for (mRoomRenderList.start(); mRoomRenderList.forward();
            mRoomRenderList.next())
    {
        rRoom2 = mRoomRenderList.current();

        if (rRoom2 == rRoom)
            return;
    }

    //rRoom->dist =
    //gViewVolume.getDistToBboxFromNear(rRoom->room->bbox_min,
    //                                           rRoom->room->bbox_max);

    /* Add current room to list */
    mRoomRenderList.pushBack(rRoom);

    if (mFlags & Render::fOneRoom)
    {
        return;
    }
    else if (mFlags & Render::fAllRooms) /* Are you serious? */
    {
        for (mRooms.start(); mRooms.forward(); mRooms.next())
        {
            rRoom2 = mRooms.current();

            if (rRoom2 && rRoom2 != rRoom)
            {
                buildRoomRenderList(rRoom2);
            }
        }

        return;
    }

    // Try to add adj rooms and their adj rooms, skip this room
    for (rRoom->room->adjacentRooms.start(), rRoom->room->adjacentRooms.next();
            rRoom->room->adjacentRooms.forward(); rRoom->room->adjacentRooms.next())
    {
        if (rRoom->room->adjacentRooms.current() < 0)
            continue;

        rRoom2 = mRooms[rRoom->room->adjacentRooms.current()];

        // Mongoose 2002.03.22, Add portal visibility check here

        if (rRoom2 && rRoom2 != rRoom)
        {
            buildRoomRenderList(rRoom2);
        }
    }
}


void Render::drawSkyMesh(float scale)
{
    skeletal_model_t *model = gWorld.getModel(mSkyMesh);


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
    //drawModelMesh(gWorld.getMesh(mSkyMesh), );
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
}


void Render::drawLoadScreen()
{
    static float wrap = 0.0001f;
    float x = 0.0f, y = 0.0f, z = -160.0f;
    float w = 500.0f, h = 500.0f;


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

    // Mongoose 2002.03.26, Account for particle system not using new binds
    //   by not using them here either
    //mTexture.Bind(1);  // Second loaded texture ( index + 1 = 2 )
    if (mFlags & Render::fMultiTexture && wrap < 1.121096f)
    {
        mTexture.bindMultiTexture(1, 3);

        glBegin(GL_TRIANGLE_STRIP);
        mTexture.useMultiTexture(1.0f, 1.0f, 0.5f - wrap, 1.0f);
        glColor3fv(WHITE);
        glVertex3f(x + w, y + h, z);
        mTexture.useMultiTexture(0.0f, 1.0f, 0.0f - wrap, 1.0f);
        glColor3fv(WHITE);
        glVertex3f(x - w, y + h, z);
        mTexture.useMultiTexture(1.0f, 0.0f, 0.5f - wrap, 0.5f);
        glColor3fv(WHITE);
        glVertex3f(x + w, y - h, z);
        mTexture.useMultiTexture(0.0f, 0.0f, 0.0f - wrap, 0.5f);
        glColor3fv(WHITE);
        glVertex3f(x - w, y - h, z);
        glEnd();

        // wrap += 0.0012f;
        // The Loading Screen sat around for 25s, doing nothing.
        // Incrementing wrap by a much bigger number speeds up the animation
        // thus greatly reducing startup time?! -- xythobuz
        wrap += 0.075f;

        if (wrap > 1.121096f)
            mTexture.disableMultiTexture();
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, 2);

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
    }

    if (mFlags & Render::fGL_Lights)
        glEnable(GL_LIGHTING);

#ifdef USING_EMITTER
    // Mongoose 2002.01.01, Test particle prototype on load screen
    if (mEmitter && mFlags & Render::fParticles)
    {
        glPushMatrix();
        glLoadIdentity();

        glEnable(GL_BLEND);
        glRotatef(180.0, 1.0, 0.0, 0.0);
        glTranslatef(0.0, -820.0, 575.0);
        glScalef(80.0, 80.0, 80.0);

        // Update view volume for vising
        updateViewVolume();
        gViewVolume.getFrustum(mEmitter->mFrustum);
        mEmitter->Flags(Emitter::fUseDepthSorting, true);
        mEmitter->Draw();

        glPopMatrix();
    }
#endif

    glEnterMode2d(mWidth, mHeight);
    glColor3fv(OR_BLUE);
    mString.Render();
    glExitMode2d();

    glFlush();
}


void Render::drawObjects()
{
#ifdef USING_FPS_CAMERA
    vec3_t curPos;
#endif
    sprite_seq_t *sprite;
    int frame;


    // Draw lara or other player model ( move to entity rendering method )
    if (mFlags & Render::fViewModel && LARA && LARA->tmpHook)
    {
        SkeletalModel *mdl = static_cast<SkeletalModel *>(LARA->tmpHook);

        if (mdl)
        {

            // Mongoose 2002.03.22, Test 'idle' aniamtions
            if (!LARA->moving)
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

#ifdef USING_FPS_CAMERA
        mCamera->getPosition(curPos);
        glTranslated(curPos[0], curPos[1], curPos[2]);
        glRotated(mCamera->getYaw(), 0, 1, 0);
        glTranslated(0, 500, 1200);
#else
        glTranslated(LARA->pos[0], LARA->pos[1], LARA->pos[2]);
        glRotated(mCamera->getYaw(), 0, 1, 0);
#endif

        drawModel(static_cast<SkeletalModel *>(LARA->tmpHook));
        glPopMatrix();
    }

    // Mongoose 2002.03.22, Draw sprites after player to handle alpha
    if (mFlags & Render::fSprites)
    {
        Vector<sprite_seq_t *> *sprites;

        sprites = gWorld.getSprites();

        for (sprites->start(); sprites->forward(); sprites->next())
        {
            sprite = sprites->current();

            if (!sprite)
                continue;

            if (sprite->sprite && sprite->num_sprites)
            {
                for (int i = 0; i < sprite->num_sprites; i++)
                {
                    drawSprite((sprite_t *)(sprite->sprite+i));
                }
            }
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
        printf("ERROR: No animation for model[%i].aframe[%i] %u\n",
                mdl->id, aframe, mdl->animation.size());
#endif
        return;
    }

    if (animation->frame.empty())
    {
#ifdef DEBUG_RENDER
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

    for (boneframe->tag.start(); boneframe->tag.forward(); boneframe->tag.next())
    {
        tag = boneframe->tag.current();

        if (!tag)
            continue;

        if (boneframe->tag.getCurrentIndex() == 0)
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
                tag2 = boneframe2->tag[boneframe->tag.getCurrentIndex()];

                if (tag2)
                {
                    drawModelMesh(gWorld.getMesh(tag2->mesh), Render::skeletalMesh);
                }
            }
        }

        if (mFlags & Render::fRenderPonytail)
        {
            if (mdl->ponytailId > 0 &&
                    boneframe->tag.getCurrentIndex() == 14)
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
                        glRotatef(helRandomNum(-8.0f, -10.0f), 1, 0, 0);
                        glRotatef(helRandomNum(-5.0f, 5.0f), 0, 1, 0);
                        glRotatef(helRandomNum(-5.0f, 5.0f), 0, 0, 1);

                        glTranslatef(0.0, 0.0, mdl->ponyOff);
                    }

                    if (mdl->pigtails)
                    {
                        glPushMatrix();
                        glTranslatef(mdl->ponyOff2, 0.0, 0.0);
                        drawModelMesh(gWorld.getMesh(mdl->ponytailMeshId + i),
                                Render::skeletalMesh);
                        glPopMatrix();

                        glPushMatrix();
                        glTranslatef(-mdl->ponyOff2, 0.0, 0.0);
                        drawModelMesh(gWorld.getMesh(mdl->ponytailMeshId + i),
                                Render::skeletalMesh);
                        glPopMatrix();
                    }
                    else
                    {
                        drawModelMesh(gWorld.getMesh(mdl->ponytailMeshId + i),
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

        drawModelMesh(gWorld.getMesh(tag->mesh), Render::skeletalMesh);
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


void draw_bbox(vec3_t min, vec3_t max, bool draw_points)
{
    // Bind before entering now
    //glBindTexture(GL_TEXTURE_2D, 1);
    glPointSize(4.0);
    //glLineWidth(1.25);

    //! \fixme Need to make custom color key for this
    glColor3fv(RED);

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

    glColor3fv(GREEN);

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


void draw_bbox_color(vec3_t min, vec3_t max, bool draw_points,
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


void Render::drawRoom(RenderRoom *rRoom, bool draw_alpha)
{
    room_mesh_t *room;


    if (!rRoom || !rRoom->room)
        return;

    room = rRoom->room;

    if (!(mFlags & Render::fRoomAlpha) && draw_alpha)
        return;

    glPushMatrix();
    //LightingSetup();

    glBindTexture(GL_TEXTURE_2D, 1);  // WHITE texture

    if (!draw_alpha &&
            (mFlags & Render::fPortals || mMode == Render::modeWireframe))
    {
        portal_t *portal;

        glLineWidth(2.0);
        glColor3fv(RED);

        for (room->portals.start(); room->portals.forward(); room->portals.next())
        {
            portal = room->portals.current();

            if (!portal)
                continue;

            glBegin(GL_LINE_LOOP);
            glVertex3fv(portal->vertices[0]);
            glVertex3fv(portal->vertices[1]);
            glVertex3fv(portal->vertices[2]);
            glVertex3fv(portal->vertices[3]);
            glEnd();
        }

        glLineWidth(1.0);

#ifdef OBSOLETE
        glColor3fv(RED);

        for (i = 0; i < (int)room->num_boxes; ++i)
        {
            // Mongoose 2002.08.14, This is a simple test -
            //   these like portals are really planes
            glBegin(GL_QUADS);
            glVertex3fv(room->boxes[i].a.pos);
            glVertex3fv(room->boxes[i].b.pos);
            glVertex3fv(room->boxes[i].c.pos);
            glVertex3fv(room->boxes[i].d.pos);
            glEnd();
        }
#endif
    }

    if (mMode == Render::modeWireframe && !draw_alpha)
    {
        draw_bbox(room->bbox_min, room->bbox_max, true);
    }

    glTranslated(room->pos[0], room->pos[1], room->pos[2]);

    // Reset since GL_MODULATE used, reset to WHITE
    glColor3fv(WHITE);

    switch (mMode)
    {
        case modeWireframe:
            rRoom->mesh.mMode = Mesh::MeshModeWireframe;
            break;
        case modeSolid:
            rRoom->mesh.mMode = Mesh::MeshModeSolid;
            break;
        default:
            rRoom->mesh.mMode = Mesh::MeshModeTexture;
            break;
    }

    if (draw_alpha)
    {
        rRoom->mesh.drawAlpha();
    }
    else
    {
        rRoom->mesh.drawSolid();
    }

    glPopMatrix();

    //mTexture.bindTextureId(0);

    // Draw other room meshes and sprites
    if (draw_alpha || mMode == modeWireframe || mMode == modeSolid)
    {
        if (mFlags & Render::fRoomModels)
        {
            static_model_t *mdl;

            for (room->models.start(); room->models.forward();
                    room->models.next())
            {
                mdl = room->models.current();

                if (!mdl)
                    continue;

                mdl->pos[0] += room->pos[0];
                mdl->pos[1] += room->pos[1];
                mdl->pos[2] += room->pos[2];

                // Depth sort room model render list with qsort
                room->models.qSort(compareStaticModels);

                mdl->pos[0] -= room->pos[0];
                mdl->pos[1] -= room->pos[1];
                mdl->pos[2] -= room->pos[2];
            }

            for (room->models.start(); room->models.forward();
                    room->models.next())
            {
                drawRoomModel(room->models.current());
            }
        }

        // Draw other room alpha polygon objects
        if (mFlags & Render::fSprites)
        {
            for (room->sprites.start(); room->sprites.forward(); room->sprites.next())
            {
                drawSprite(room->sprites.current());
            }
        }
    }
}


void Render::drawSprite(sprite_t *sprite)
{
    if (!sprite)
        return;

    if (!isVisible(sprite->pos[0], sprite->pos[1], sprite->pos[2],
                sprite->radius))
        return;

    glPushMatrix();
    glTranslated(sprite->pos[0], sprite->pos[1], sprite->pos[2]);

    // Sprites must always face camera, because they have no depth  =)
    glRotated(mCamera->getYaw(), 0, 1, 0);

    switch (mMode)
    {
        // No vertex lighting on sprites, as far as I see in specs
        // So just draw normal texture, no case 2
        case Render::modeSolid:
            glBegin(GL_TRIANGLE_STRIP);
            glColor3f(sprite->texel[0].st[0], sprite->texel[0].st[1], 0.5);
            glVertex3fv(sprite->vertex[0].pos);

            glColor3f(sprite->texel[1].st[0], sprite->texel[1].st[1], 0.5);
            glVertex3fv(sprite->vertex[1].pos);

            glColor3f(sprite->texel[3].st[0], sprite->texel[3].st[1], 0.5);
            glVertex3fv(sprite->vertex[3].pos);

            glColor3f(sprite->texel[2].st[0], sprite->texel[2].st[1], 0.5);
            glVertex3fv(sprite->vertex[2].pos);
            glEnd();
            break;
        case Render::modeWireframe:
            glColor3fv(CYAN);
            glBegin(GL_LINE_LOOP);
            glVertex3fv(sprite->vertex[0].pos);
            glVertex3fv(sprite->vertex[1].pos);
            glVertex3fv(sprite->vertex[2].pos);
            glVertex3fv(sprite->vertex[3].pos);
            glEnd();
            glColor3fv(WHITE);
            break;
        default:
            glBindTexture(GL_TEXTURE_2D, sprite->texture+1);

            glBegin(GL_TRIANGLE_STRIP);
            glTexCoord2fv(sprite->texel[0].st);
            glVertex3fv(sprite->vertex[0].pos);
            glTexCoord2fv(sprite->texel[1].st);
            glVertex3fv(sprite->vertex[1].pos);
            glTexCoord2fv(sprite->texel[3].st);
            glVertex3fv(sprite->vertex[3].pos);
            glTexCoord2fv(sprite->texel[2].st);
            glVertex3fv(sprite->vertex[2].pos);
            glEnd();
    }

    glPopMatrix();
}


void Render::drawRoomModel(static_model_t *mesh)
{
    model_mesh_t *r_mesh;


    if (!mesh)
        return;

    r_mesh = gWorld.getMesh(mesh->index);

    if (!r_mesh)
        return;

    if (!isVisible(mesh->pos[0], mesh->pos[1], mesh->pos[2], r_mesh->radius))
        return;

    glPushMatrix();
    glTranslated(mesh->pos[0], mesh->pos[1], mesh->pos[2]);
    glRotated(mesh->yaw, 0, 1, 0);

    drawModelMesh(r_mesh, roomMesh);
    glPopMatrix();
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

    for (r_mesh->texturedTriangles.start();
            r_mesh->texturedTriangles.forward();
            r_mesh->texturedTriangles.next())
    {
        ttri = r_mesh->texturedTriangles.current();

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
    for (r_mesh->coloredTriangles.start();
            r_mesh->coloredTriangles.forward();
            r_mesh->coloredTriangles.next())
    {
        ttri = r_mesh->coloredTriangles.current();

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
    for (r_mesh->coloredRectangles.start();
            r_mesh->coloredRectangles.forward();
            r_mesh->coloredRectangles.next())
    {
        ttri = r_mesh->coloredRectangles.current();

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
    for (r_mesh->texturedTriangles.start();
            r_mesh->texturedTriangles.forward();
            r_mesh->texturedTriangles.next())
    {
        ttri = r_mesh->texturedTriangles.current();

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
    for (r_mesh->texturedRectangles.start();
            r_mesh->texturedRectangles.forward();
            r_mesh->texturedRectangles.next())
    {
        ttri = r_mesh->texturedRectangles.current();

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

    model = gWorld.getModel(index);

    if (model)
    {
        ent->modelId = index;
        printf("Viewmodel skeletal model %i\n", model->id);
    }
}


void Render::RegisterCamera(Camera *camera)
{
    if (camera)
    {
        mCamera = camera;
    }
}


GLString *Render::GetString()
{
    return &mString;
}


void Render::addSkeletalModel(SkeletalModel *mdl)
{
    mModels.pushBack(mdl);
}


void Render::updateViewVolume()
{
    matrix_t proj;
    matrix_t mdl;


    glGetFloatv(GL_PROJECTION_MATRIX, proj);
    glGetFloatv(GL_MODELVIEW_MATRIX, mdl);
    gViewVolume.updateFrame(proj, mdl);
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

        draw_bbox_color(bbox_min, bbox_max, true, PINK, RED);
    }

    return gViewVolume.isBboxInFrustum(bbox_min, bbox_max);
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

    return (gViewVolume.isPointInFrustum(x, y, z));
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

    return (gViewVolume.isSphereInFrustum(x, y, z, radius));
}
