/*!
 * \file include/OpenRaider.h
 * \brief Main Game Singleton
 *
 * \author Mongoose
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>

#ifdef DEBUG_MEMORY
#include <memory_test.h>
#endif

#include <World.h>
#include <SkeletalModel.h>
#include <TombRaider1.h> // tmp stop-gap
#include <OpenRaider.h>

enum OpenRaiderText { textConsole = 2, textMenu = 3, textOutput = 4 };

const unsigned int TextureLimit       = 24;
const float TexelScale                = 256.0f;

World gWorld;
Map<int, int> gMapTex2Bump;
Vector <unsigned int> gColorTextureHACK;
int gTextureOffset;
entity_t *LARA = 0x0;
entity_t *gClientPlayer[32];
unsigned int gNetTicks = 0;
unsigned int gNetLastTicks = 0;
bool gStartServer = false;
skeletal_model_t *gLaraModel = 0x0;
char *gFontFilename = 0x0;

unsigned int getTicks()
{
    OpenRaider *game = OpenRaider::Instance();

    return game->getTicks();
}


////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

OpenRaider *OpenRaider::mInstance = 0x0;


OpenRaider *OpenRaider::Instance()
{
    if (mInstance == 0x0)
    {
        mInstance = new OpenRaider();
    }

    return mInstance;
}


void killOpenRaiderSingleton()
{
    killNetworkSingleton();

    printf("Shutting down Game...\n");

    // Requires public deconstructor
    // Causes pointer-being-freed-not-allocated error!
    //delete OpenRaider::Instance();
#warning "Can't delete OpenRaider::Instance() without a not-allocated-free error. Something is fishy here..."

#ifdef DEBUG_MEMORY
    printf("\n[Mongoose MEMORY_DEBUG]\nMemory leak table:\n");
    dump_memory_report();
#endif

    printf("\nThanks for testing %s\n", VERSION);
    printf("Please file bug reports\n\n");
    printf("Build date: %s @ %s\n", __DATE__, __TIME__);
    printf("Build host: %s\n", BUILD_HOST);
    printf("Web site  : http://github.com/xythobuz/OpenRaider\n");
}


OpenRaider::OpenRaider() : SDLSystem()
{
    Network::Instance();

    for (unsigned int i = 0; i < 32; ++i)
    {
        gClientPlayer[i] = 0x0;
    }

    m_pakDir = 0x0;
    m_audioDir = 0x0;
    m_homeDir = 0x0;

    m_mouseX = 2.5;
    m_mouseY = 0.5;
    m_texOffset = 0;
    mLevelTextureOffset = 0;
    m_testSFX = -1;

    mText = NULL;
    m_flags = 0;
    m_mapName[0] = '\0';

    /*! \todo Replace numbers with enum modes.
     * Only do this when you know the amount of commands + 1 (0 reserved)
     */
    mMode[addCommandMode("[OpenRaider.Engine]")] = 2;
    mMode[addCommandMode("[Network.Server]")] = 5;
    mMode[addCommandMode("[Video.OpenGL]")] = 0;
    mMode[addCommandMode("[Audio.OpenAL]")] = 1;
    mMode[addCommandMode("[OpenRaider.Console]")] = 4;
    mMode[addCommandMode("[Input.Mouse]")] = 3;
}


OpenRaider::~OpenRaider()
{
    /*! \fixme GL call to critical section,
     * needs mutex really -- Mongoose 2002.01.02
     */
    m_render.setMode(Render::modeDisabled);
    sleep(1);

    printf("Removing World...\n");
    gWorld.destroy();

    printf("Cleaning up...\n");

    for (mMapList.start(); mMapList.forward(); mMapList.next())
    {
        if (mMapList.current())
        {
            delete [] mMapList.current();
        }
    }

    for (mMusicList.start(); mMusicList.forward(); mMusicList.next())
    {
        if (mMusicList.current())
        {
            delete [] mMusicList.current();
        }
    }

    if (m_pakDir)
        delete [] m_pakDir;

    if (m_audioDir)
        delete [] m_audioDir;

    if (m_homeDir)
        delete [] m_homeDir;

    if (gFontFilename)
        delete [] gFontFilename;
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

void eventAnimTest(int anim)
{
    if (LARA)
    {
        SkeletalModel *mdl = static_cast<SkeletalModel *>(LARA->tmpHook);

        if (mdl)
        {
            mdl->setAnimation(anim);
        }
    }
}


void percent_callback(int p)
{
    OpenRaider &game = *OpenRaider::Instance();

    switch (p)
    {
        case 10:
            game.print(true, "Level textures loaded");
            break;
        default:
            // %% has problems with OpenRaider::print it seems
            game.print(false, "Level pak %i/100 loaded", p);
    }
}


void openraider_warning(const char *func_name, const char *warning,
        const char *filename, int line)
{
    printf("%s> WARNING %s %s:%i\n", func_name, warning, filename, line);
}

void openraider_error(const char *func_name, const char *error,
        const char *filename, int line)
{
    printf("%s> ERROR %s %s:%i\n", func_name, error, filename, line);
}


void from_network_layer(network_packet_t *p, unsigned int *last_id)
{
    static unsigned int i = 0;


    *last_id = i++;

    if (gNetTicks > gNetLastTicks + 200) // 200ms
    {
        gNetLastTicks = gNetTicks;
        p->send = 1;
    }

    if (LARA)
    {
        p->pos[0] = LARA->pos[0];
        p->pos[1] = LARA->pos[1];
        p->pos[2] = LARA->pos[2];
        p->yaw = LARA->angles[1];
        p->pitch = LARA->angles[1];
        p->view_model = LARA->modelId;

        SkeletalModel *mdl = static_cast<SkeletalModel *>(LARA->tmpHook);

        if (mdl)
        {
            p->aframe = mdl->getAnimation();
            p->bframe = mdl->getFrame();
        }
    }

#ifdef DEBUG_NETWORK
    printf("<> Sending data\n");
#endif
}


void to_network_layer(network_packet_t p)
{
    OpenRaider &game = *OpenRaider::Instance();


#ifdef DEBUG_NETWORK
    printf("<> Recieved data\n");
#endif

    if (gClientPlayer[p.cid])
    {
        gClientPlayer[p.cid]->pos[0] = p.pos[0];
        gClientPlayer[p.cid]->pos[1] = p.pos[1];
        gClientPlayer[p.cid]->pos[2] = p.pos[2];
        gClientPlayer[p.cid]->angles[1] = p.yaw;
        gClientPlayer[p.cid]->angles[2] = p.pitch;
    }
    else if (LARA)
    {
        gClientPlayer[p.cid] = new entity_t;

        gClientPlayer[p.cid]->pos[0] = p.pos[0];
        gClientPlayer[p.cid]->pos[1] = p.pos[1];
        gClientPlayer[p.cid]->pos[2] = p.pos[2];
        gClientPlayer[p.cid]->angles[1] = p.yaw;
        gClientPlayer[p.cid]->angles[2] = p.pitch;
        gClientPlayer[p.cid]->moving = true;
        gClientPlayer[p.cid]->objectId = 0;
        gClientPlayer[p.cid]->id = LARA->id+1000+p.cid;
        gClientPlayer[p.cid]->type = 0x02;

        SkeletalModel *mdl = new SkeletalModel();
        gClientPlayer[p.cid]->tmpHook = mdl;
        mdl->setAnimation(p.aframe);
        mdl->setFrame(p.bframe);
        mdl->setIdleAnimation(TR_ANIAMTION_STAND);
        gClientPlayer[p.cid]->modelId = LARA->modelId; //p.view_model;
        gClientPlayer[p.cid]->animate = true;

        gWorld.addEntity(gClientPlayer[p.cid]);
        game.print(true, "A new player (%u) connected", p.cid);
    }

    if (LARA)
    {
        Network &network = *Network::Instance();
        network_frame_t &gPiggyBack = network.getPiggyBack();


        if (gNetTicks > gNetLastTicks + 200)
        {
            gNetLastTicks = gNetTicks;
            gPiggyBack.data.send = 1;
        }

        gPiggyBack.data.pos[0] = LARA->pos[0];
        gPiggyBack.data.pos[1] = LARA->pos[1];
        gPiggyBack.data.pos[2] = LARA->pos[2];
        gPiggyBack.data.yaw = LARA->angles[1];
        gPiggyBack.data.pitch = LARA->angles[2];
        gPiggyBack.data.view_model = LARA->modelId;

        SkeletalModel *mdl = static_cast<SkeletalModel *>(LARA->tmpHook);

        if (mdl)
        {
            gPiggyBack.data.aframe = mdl->getAnimation();
            gPiggyBack.data.bframe = mdl->getFrame();
        }
    }
}


void OpenRaider::handleMouseMotionEvent(float x, float y)
{
    if (x > 0)
    {
        while (x-- > 0) // Tried to fix very slow camera movements
            m_camera.command(CAMERA_ROTATE_RIGHT);

        if (LARA)
        {
            switch (LARA->moveType)
            {
                case worldMoveType_swim:
                case worldMoveType_fly:
                case worldMoveType_noClipping:
                    eventAnimTest(TR_ANIAMTION_SWIM_R);
                    break;
                default:
                case worldMoveType_walk:
                case worldMoveType_walkNoSwim:
                    eventAnimTest(TR_ANIAMTION_TURN_L); // TURN left always for now
            }

            LARA->moving = true;
        }
    }
    else if (x < 0)
    {
        while (x++ < 0) // Tried to fix very slow camera movements
            m_camera.command(CAMERA_ROTATE_LEFT);

        if (LARA)
        {
            switch (LARA->moveType)
            {
                case worldMoveType_swim:
                case worldMoveType_fly:
                case worldMoveType_noClipping:
                    eventAnimTest(TR_ANIAMTION_SWIM_L);
                    break;
                default:
                case worldMoveType_walk:
                case worldMoveType_walkNoSwim:
                    eventAnimTest(TR_ANIAMTION_TURN_L);
            }

            LARA->moving = true;
        }
    }

    if (y > 0)
        while (y-- > 0) // Tried to fix very slow camera movements
            m_camera.command(CAMERA_ROTATE_UP);
    else if (y < 0)
        while (y++ < 0) // Tried to fix very slow camera movements
            m_camera.command(CAMERA_ROTATE_DOWN);

    if (LARA)
    {
        LARA->angles[1] = m_camera.getRadianYaw();
        LARA->angles[2] = m_camera.getRadianPitch();
    }
}


void OpenRaider::handleKeyReleaseEvent(unsigned int key, unsigned int mod)
{
}


void OpenRaider::handleBoundKeyPressEvent(unsigned int key)
{
    switch (key)
    {
        case OpenRaiderKey_jump:
            m_camera.command(CAMERA_MOVE_UP);

            if (LARA)
            {
                LARA->moving = true;
            }
            break;
        case OpenRaiderKey_crouch:
            m_camera.command(CAMERA_MOVE_DOWN);

            if (LARA)
            {
                LARA->moving = true;
            }
            break;
        case OpenRaiderKey_forward:
            gWorld.moveEntity(LARA, 'f');

            if (LARA && LARA->moving)
            {
                switch (LARA->moveType)
                {
                    case worldMoveType_swim:
                    case worldMoveType_fly:
                    case worldMoveType_noClipping:
                        eventAnimTest(TR_ANIAMTION_SWIM);
                        break;
                    default:
                    case worldMoveType_walk:
                    case worldMoveType_walkNoSwim:
                        eventAnimTest(TR_ANIAMTION_RUN);
                }
            }
            else if (LARA)
            {
                switch (LARA->moveType)
                {
                    case worldMoveType_swim:
                    case worldMoveType_fly:
                    case worldMoveType_noClipping:
                        eventAnimTest(TR_ANIAMTION_SWIM_IDLE);
                        break;
                    default:
                    case worldMoveType_walk:
                    case worldMoveType_walkNoSwim:
                        eventAnimTest(TR_ANIAMTION_HIT_WALL_FRONT);
                }
            }
            else
            {
                m_camera.command(CAMERA_MOVE_FORWARD);
            }
            break;
        case OpenRaiderKey_backward:
            gWorld.moveEntity(LARA, 'b');

            if (LARA && LARA->moving)
            {
                switch (LARA->moveType)
                {
                    case worldMoveType_swim:
                    case worldMoveType_fly:
                    case worldMoveType_noClipping:
                        eventAnimTest(TR_ANIAMTION_SWIM);
                        break;
                    default:
                    case worldMoveType_walk:
                    case worldMoveType_walkNoSwim:
                        eventAnimTest(TR_ANIAMTION_RUN);
                }
            }
            break;
        case OpenRaiderKey_left:
            gWorld.moveEntity(LARA, 'l');

            if (LARA && LARA->moving)
            {
                switch (LARA->moveType)
                {
                    case worldMoveType_swim:
                    case worldMoveType_fly:
                    case worldMoveType_noClipping:
                        eventAnimTest(TR_ANIAMTION_SWIM);
                        break;
                    default:
                    case worldMoveType_walk:
                    case worldMoveType_walkNoSwim:
                        eventAnimTest(TR_ANIAMTION_RUN);
                }
            }
            else if (!LARA)
            {
                m_camera.command(CAMERA_MOVE_LEFT);
            }
            break;
        case OpenRaiderKey_right:
            gWorld.moveEntity(LARA, 'r');

            if (LARA && LARA->moving)
            {
                switch (LARA->moveType)
                {
                    case worldMoveType_swim:
                    case worldMoveType_fly:
                    case worldMoveType_noClipping:
                        eventAnimTest(TR_ANIAMTION_SWIM);
                        break;
                    default:
                    case worldMoveType_walk:
                    case worldMoveType_walkNoSwim:
                        eventAnimTest(TR_ANIAMTION_RUN);
                }
            }
            else if (!LARA)
            {
                m_camera.command(CAMERA_MOVE_RIGHT);
            }
            break;
        case OpenRaiderKey_panRight:
            m_camera.command(CAMERA_ROTATE_RIGHT);
            break;
        case OpenRaiderKey_panLeft:
            m_camera.command(CAMERA_ROTATE_LEFT);
            break;
        case OpenRaiderKey_tiltDown:
            m_camera.command(CAMERA_ROTATE_DOWN);
            break;
        case OpenRaiderKey_tiltUp:
            m_camera.command(CAMERA_ROTATE_UP);
            break;
    }
}


void OpenRaider::handleBoundKeyReleaseEvent(unsigned int key)
{
    switch (key)
    {
        case '`': // Temp hack also until console is rolled back
            //i = 2;

            if (mConsoleMode)
            {
                //buffer[0] = '>';
                //buffer[1] = ' ';
                //buffer[2] = 0;

                mText->setActive(textConsole, true);
            }
            else
            {
                mText->setActive(textConsole, false);
                //buffer[0] = 0;
            }
            break;
        case OpenRaiderKey_forward:
        case OpenRaiderKey_backward:
        case OpenRaiderKey_right:
        case OpenRaiderKey_left:
            if (LARA)
            {
                LARA->moving = false;
            }
    }
}


void OpenRaider::handleConsoleKeyPressEvent(unsigned int key,unsigned int mod)
{
    static char lastCmd[64] = "2  ";
    static unsigned int i = 0;
    char *buffer;
    unsigned int len;


    // Cheap console test
    len = mText->GetStringLen(textConsole);
    buffer = mText->GetBuffer(textConsole);

    //printf("- 0x%x\n", key);

    if (key == mConsoleKey)
    {
        i = 2;

        if (mConsoleMode)
        {
            buffer[0] = '>';
            buffer[1] = ' ';
            buffer[2] = 0;

            mText->setActive(textConsole, true);
        }
        else
        {
            mText->setActive(textConsole, false);
            buffer[0] = 0;
        }
    }
    else if (mConsoleMode && len > i+1)
    {
        switch (key)
        {
            case SYS_KEY_UP:
                i = lastCmd[0];

                lastCmd[0] = '>';
                mText->SetString(textConsole, lastCmd);
                break;
            case 0:
                break;
            case '\b':
                if (i > 2)
                {
                    buffer[--i] = 0;
                }
                break;
            case '\r':
            case '\n':
                strncpy(lastCmd, buffer, 63);
                lastCmd[0] = (char)i;

                command("[OpenRaider.Console]");
                command(buffer);

                i = 2;
                buffer[0] = '>';
                buffer[1] = ' ';
                buffer[2] = 0;
                break;
            default:
                if (mod & (SYS_MOD_KEY_RSHIFT | SYS_MOD_KEY_LSHIFT) &&
                        key > 96 && key < 122)
                {
                    buffer[i++] = (char)(key - 32);
                }
                else
                {
                    if (mod & (SYS_MOD_KEY_RSHIFT | SYS_MOD_KEY_LSHIFT))
                    {
                        if (key == '-')
                            key = '_';
                    }

                    buffer[i++] = (char)key;
                }

                buffer[i] = 0;
        }
    }
}


void OpenRaider::handleKeyPressEvent(unsigned int key, unsigned int mod)
{
    static bool menu = false;

    // Cheap menu test
    if (menu)
    {
        switch (key)
        {
            case SYS_KEY_ESC:
                menu = false;
                mText->SetString(textMenu, " ");
                mText->setActive(textMenu, false);
                break;
            case SYS_KEY_ENTER:
                loadLevel(mMapList.current());
                mText->SetString(textMenu, " ");
                mText->setActive(textMenu, false);

                menu = false;
                break;
            case SYS_KEY_DOWN:
                mMapList.prev();

                if (!mMapList.backward())
                {
                    mMapList.finish();
                }

                if (mMapList.current())
                {
                    mText->SetString(textMenu, "Load %s?", mMapList.current());
                }
                else
                {
                    mText->SetString(textMenu, "See README for map install");
                }
                break;
            case SYS_KEY_UP:
                mMapList.next();

                if (!mMapList.forward())
                {
                    mMapList.start();
                }

                if (mMapList.current())
                {
                    mText->SetString(textMenu, "Load %s?", mMapList.current());
                }
                else
                {
                    mText->SetString(textMenu, "See README for map install");
                }
        }

        return;
    }

    switch (key)
    {
        case SYS_KEY_ESC:
            menu = true;
            mText->SetString(textMenu, "Select a map UP/DOWN");
            mText->setActive(textMenu, true);
            break;
        case SYS_KEY_ENTER:
            if (mod & SYS_MOD_KEY_LALT)
            {
                toggleFullscreen();
            }
            break;
    }

    ////////////////////////////////////////////////////////////

    switch (key)
    {
        case SYS_MOUSE_LEFT:
            mSound.play(m_testSFX);

            if (LARA)
            {
                eventAnimTest(15);
            }
            break;
        case '/':
            if (LARA)
            {
                entity_t *e = new entity_t;

                e->pos[0] = LARA->pos[0];
                e->pos[1] = LARA->pos[1] - 256.0f; // crawling H
                e->pos[2] = LARA->pos[2];
                e->type = 61;
                e->master = LARA->master;
                LARA->master = e;
            }
            break;
        case '[':
            if (LARA)
            {
                SkeletalModel *mdl = static_cast<SkeletalModel *>(LARA->tmpHook);

                if (mdl)
                {
                    mdl->setIdleAnimation(mdl->getIdleAnimation() - 1);
                    mdl->setAnimation(mdl->getIdleAnimation());
                    print(false, "AFRAME %i\n", mdl->getIdleAnimation());
                }
            }
            break;
        case ']':
            if (LARA)
            {
                SkeletalModel *mdl = static_cast<SkeletalModel *>(LARA->tmpHook);

                if (mdl)
                {
                    mdl->setIdleAnimation(mdl->getIdleAnimation() + 1);
                    mdl->setAnimation(mdl->getIdleAnimation());
                    print(false, "AFRAME %i\n", mdl->getIdleAnimation());
                }
            }
            break;
        case 'r':
            mSound.play(m_testSFX);
            break;
    }
}


void OpenRaider::print(bool dump_stdout, const char *format, ...)
{
    static char buffer[128];
    va_list args;
    unsigned int l;


    va_start(args, format);
    vsnprintf(buffer, 128, format, args);

    l = strlen(buffer);

    if (!l || !buffer[0])
    {
        return;
    }

    // Strip message of an trailing carrage return
    if (buffer[l-1] == '\n')
    {
        buffer[l-1] = 0;
    }

    // Print to game 'console'
    if (mText)
    {
        mText->SetString(textOutput, buffer);
    }

    // Print to stdout
    if (dump_stdout)
    {
        fprintf(stdout, "> %s\n",  buffer);
    }

    va_end(args);

    // Mongoose 2002.08.14, Currently, you must request
    //   frame redraws in this mode
    if (m_flags & OpenRaider_Loading)
    {
        gameFrame();
    }
}


void OpenRaider::initGL()
{
    // Mongoose 2002.08.13, Handled in start() actually
}


void OpenRaider::start()
{
    vec3_t up;
    char *filename;


    printf("\n[Starting %s]\n\n", VERSION);


    filename = fullPath("~/.OpenRaider/OpenRaider.init", 0);

    if (loadResourceFile(filename) < 0)
    {

        printf("Could not find ~/.OpenRaider/OpenRaider.init\n");
        if (loadResourceFile("OpenRaider.init") < 0)
        {
            shutdown(-1);
        }
        printf("Loaded defaults...\n");
    }

    delete [] filename;

    m_camera.reset();
    m_camera.setSensitivityX(m_mouseX);
    m_camera.setSensitivityY(m_mouseY);
    m_camera.setSpeed(512);
    up[0] = 0.0;
    up[1] = -1.0;
    up[2] = 0.0;
    m_camera.setUp(up);

    if (mMapList.empty())
    {
        printf("No maps in map list, please edit your OpenRaider.init\n");
        shutdown(0);
    }

    // Mongoose 2002.08.13, System protected members abuse
    m_fovY = 40.0f;
    m_clipFar = 600000.0f;
    m_clipNear = 10.0f;

    // Mongoose 2002.08.13, This starts GL context
    printf("\n[Starting video subsystem...]\n");
    setFastCardPerformance(true);
    initVideo(m_width, m_height, false);

    // Mongoose 2002.01.02, Disable rendering to avoid GL call conflicts
    m_render.setMode(Render::modeDisabled);
    m_render.Init(m_width, m_height, m_fastCard);
    m_render.initTextures(m_homeDir, &m_texOffset, &mLevelTextureOffset);

    m_render.RegisterCamera(&m_camera);
    mText = m_render.GetString();

    // Mongoose 2002.01.03, Good idea?
    m_render.setMode(Render::modeLoadScreen);

    // Mongoose 2002.01.02, Load external audio tracks and fx
    //   General audio init
    initSound();

    // Old room movement
    gWorld.setFlag(World::fEnableHopping);
    // reenabled, what should be the new room movement? --xythobuz

    resetTicks();

    runGame();
}


void OpenRaider::initSound()
{
    if (!(m_flags & OpenRaider_EnableSound) || mSound.init() < 0)
    {
        print(true, "Disabling sound...\n");
        m_flags &= ~OpenRaider_EnableSound;
        return;
    }

#ifdef OBSOLETE
    char filename[128];
    int id;

    printf("Processing external sound files: ");

    // Mongoose 2001.12.31, Use music list from init script
    if (mMusicList.Empty())
    {
        print(true, "No music in map list, please edit your OpenRaider.init\n");
    }
    else
    {
        snprintf(filename, 126, "%s%s", m_audioDir, mMusicList[0]);
        filename[127] = 0;

        if (!mSound.addFile(filename, &id, mSound.SoundFlagsLoop))
        {
            mSound.play(id);

            printf("m");
            fflush(stdout);
        }
    }

    //snprintf(filename, 126, "%s%s", m_homeDir, "sample.wav");
    //filename[127] = 0;
    //mSound.addFile(filename, &m_testSFX, mSound.SoundFlagsNone);

    printf(".");
    fflush(stdout);

    printf("\n");
#endif
}


void OpenRaider::processPakSounds()
{
    unsigned char *riff;
    unsigned int riffSz;
    //tr2_sound_source_t *sound;
    //tr2_sound_details_t *detail;
    //float pos[3];
    unsigned int i;
    int id;

    /* detail
       short sample;
       short volume;
       short sound_range;
       short flags;         // bits 8-15: priority?, 2-7: number of sound samples
    // in this group, bits 0-1: channel number
    */

    if (m_flags & OpenRaider_EnableSound)
    {
        printf("Processing pak sound files: ");
    }
    else
    {
        return;
    }

    for (i = 0; i < m_tombraider.getSoundSamplesCount(); ++i)
    {
        print(false, "Processing pak sounds: %i / %i",
                i, m_tombraider.getSoundSamplesCount());

        m_tombraider.getSoundSample(i, &riffSz, &riff);

        mSound.addWave(riff, riffSz, &id, mSound.SoundFlagsNone);

        if (((i + 1) == TR_SOUND_F_PISTOL) && (id > 0))
        {
            m_testSFX = id;
        }

        delete [] riff;

        // sound[i].sound_id; // internal sound index
        // sound[i].flags;  // 0x40, 0x80, or 0xc0
        //pos[0] = sound[i].x;
        //pos[1] = sound[i].y;
        //pos[2] = sound[i].z;
        //mSound.SourceAt(id, pos);

        printf(".");
        fflush(stdout);
    }

    printf("\n");
}


void OpenRaider::loadLevel(char *mapname)
{
    int i, j, len;
    char filename[256];


    // Mongoose 2002.03.23, Clear last loaded 'world'
    m_render.setMode(Render::modeDisabled);
    m_render.ClearWorld();
    gWorld.destroy();

    // Mongoose 2002.01.02, Now draw load screen, since external
    //   GL textures are loaded and renderer is initialized
    m_render.setMode(Render::modeLoadScreen);
    gameFrame();

    /* 2003.07.05, Mongoose - No longer seeding, and looking for
       map to load -- load this map or fail and return */
    snprintf(filename, 254, "%s%s", m_pakDir, mapname);
    filename[255] = 0;
    len = strlen(filename);

    for (i = len, j = 0; i > 0; --i, ++j)
    {
        if (filename[i] == '/' || filename[i] == '\\')
            break;
    }

    j--;

    memset(m_mapName, 0, 32);

    for (i = 0; i < len - j && i < 30; ++i)
    {
        m_mapName[i] = filename[i + len - j];
    }

    m_tombraider.setDebug(m_flags & OpenRaider_DebugMap);

    m_flags |= OpenRaider_Loading;
    print(true, "Level %s loading", m_mapName);

    if (m_tombraider.Load(filename, percent_callback))
    {
        print(true, "ERROR: Failed to load level %s\n", m_mapName);
        return;
    }
    else
    {
        char altSfx[256];

        switch (m_tombraider.getEngine())
        {
            case TR_VERSION_2:
            case TR_VERSION_3:
                // Now loads eg. "tr3/MAIN.SFX" instead of "tr3/SHORE.TR2.sfx"
                filename[len - j] = '\0';
                snprintf(altSfx, 255, "%sMAIN.SFX", filename);
                print(true, "Loading alternate SFX for TR2 or TR3 pak at %sMAIN.SFX\n", filename);

                m_tombraider.loadSFX(altSfx);
                break;
            default:
            case TR_VERSION_1:
            case TR_VERSION_4:
            case TR_VERSION_5:
            case TR_VERSION_UNKNOWN:
                ;
        }
    }

    print(true, "Optimizing and translating pak data...");
    m_flags ^= OpenRaider_Loading;

    // Mongoose 2002.08.16, Used to cache TR4 textures at 10% of file load
    //   in a callback - then with other engines process them here
    m_render.setMode(Render::modeDisabled);
    //sleep(1); // Why should we sleep here? -- xythobuz
    processTextures();
    m_render.setMode(Render::modeLoadScreen);
    print(true, "Level textures cached\n");

    // Cache/process rooms
    printf("Processing rooms: ");

    for (i = 0; i < m_tombraider.NumRooms(); ++i)
    {
        processRoom(i);
        print(false, "Processing rooms: %i/%i", i, m_tombraider.NumRooms());
    }

    printf("\n");

    // Cache/process meshes
    printf("Processing meshes: ");

    for (i = 0; i < m_tombraider.getMeshCount(); ++i)
    {
        m_render.setMode(Render::modeDisabled);
        processModel(i);
        m_render.setMode(Render::modeLoadScreen);
        print(false, "Processing meshes: %i/%i", i, m_tombraider.getMeshCount());
    }

    printf("\n");

    // Cache/process sprites
    processSprites();

    // Cache/process skeletal models
    processMoveables();

    // Cache/process sound fx
    processPakSounds();

#ifdef DEBUG_MEMORY
    // Right before pak is cleared will be highest use or memory
    display_memory_usage();
#endif

    // Mongoose 2002.01.02, Go ahead and free the TR pak
    m_tombraider.reset();

    print(true, "Level pak freed from memory, Starting game...");

#ifdef DEBUG_MEMORY
    display_memory_usage();
#endif

    /*! \fixme GL call to critical section,
     * needs mutex really -- Mongoose 2002.01.02
     */
    m_render.setMode(Render::modeDisabled);
    sleep(1);

    // Draw game, level is loaded
    m_render.setMode(Render::modeVertexLight);

    m_flags ^= OpenRaider_Loading;
}


void OpenRaider::gameFrame()
{
    SkeletalModel *mdl = 0x0;
    unsigned int ticks;
    float time;


    // Remember: ticks in milliseconds, time in hundredths
    gNetTicks = ticks = getTicks();
    time = gNetTicks * 0.1f;

    switch (m_render.getMode())
    {
        case Render::modeDisabled:
            break;
        case Render::modeLoadScreen:
            if ((ticks % 10000) > 1)
            {
                m_render.drawLoadScreen();
                swapBuffersGL();
            }
            break;
        default:
            if (LARA)
            {
                mdl = static_cast<SkeletalModel *>(LARA->tmpHook);

                if (mdl)
                {
                    float elapsed = time - mdl->lastTime;


                    if (elapsed > mdl->rate)
                    {
                        if (mdl->getAnimation() == 0 &&
                                (mdl->getFrame() == 2 ||  mdl->getFrame() == 10))
                        {
                            if (m_flags & OpenRaider_EnableSound)
                            {
                                mSound.play(TR_SOUND_FOOTSTEP0);
                            }
                        }
                        else if (mdl->getAnimation() == 15)
                        {
                            eventAnimTest(TR_ANIAMTION_RUN);
                        }

                        mdl->setFrame(mdl->getFrame()+1);
                        mdl->lastTime = elapsed / mdl->rate;
                    }
                    else
                    {
                        mdl->lastTime = time;
                    }
                }
            }

            m_render.Display();
            swapBuffersGL();

            if (mdl && mdl->getAnimation() == 12)
            {
                switch (LARA->moveType)
                {
                    case worldMoveType_swim:
                    case worldMoveType_fly:
                    case worldMoveType_noClipping:
                        eventAnimTest(TR_ANIAMTION_SWIM_IDLE);
                        break;
                    default:
                    case worldMoveType_walk:
                    case worldMoveType_walkNoSwim:
                        eventAnimTest(TR_ANIAMTION_STAND);
                }

                LARA->moving = false;
            }
    }

    if (m_flags & OpenRaider_ShowFPS)
    {
        static unsigned int frames = 0;
        static float lastTime = 0.0f;


        ++frames;

        // Only update every second, so it can't be affected
        // by fast/slow cards it only counts frames instead
        // of estimating
        if (time - lastTime > 100.0f)
        {
            if (mText)
                mText->SetString(1, "%dFPS", frames);

            lastTime = time;
            frames = 0;
        }
    }
}


void OpenRaider::processTextures()
{
    unsigned char *image;
    unsigned char *bumpmap;
    int i;


    printf("Processing TR textures: ");

    //if ( m_tombraider.getNumBumpMaps())
    //  gBumpMapStart = m_tombraider.NumTextures();

    for (i = 0; i < m_tombraider.NumTextures(); ++i)
    {
        print(false, "Processing textures: %i/%i", i, m_tombraider.NumTextures());

        m_tombraider.Texture(i, &image, &bumpmap);

        // Overwrite any previous level textures on load
        m_render.loadTexture(image, 256, 256, mLevelTextureOffset + i);

        gMapTex2Bump.Add(mLevelTextureOffset + i, -1);

        if (bumpmap)
        {
            gMapTex2Bump.Add(mLevelTextureOffset + i, mLevelTextureOffset + i +
                    m_tombraider.NumTextures());
            m_render.loadTexture(bumpmap, 256, 256, mLevelTextureOffset + i +
                    m_tombraider.NumTextures());
        }

        if (image)
            delete [] image;

        if (bumpmap)
            delete [] bumpmap;

        printf(".");
        fflush(stdout);
    }

    gTextureOffset = mLevelTextureOffset + m_tombraider.NumTextures();

    printf("\n");
}


////////////////////////////////////////////////////////////
// Private Accessors
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Private Mutators
////////////////////////////////////////////////////////////

void OpenRaider::soundEvent(int type, int id, vec3_t pos, vec3_t angles)
{
    switch (type)
    {
        case 0: //  Reset listener position
            mSound.listenAt(pos, angles);
            break;
        default:
            mSound.sourceAt(id, pos);
            mSound.play(id);
    }
}


void OpenRaider::processSprites()
{
    int i, j, k, l, x, y, s_index, width, height;
    float scale, width2, height2;
    tr2_sprite_texture_t *sprite;
    tr2_sprite_texture_t *sprite_textures;
    tr2_sprite_sequence_t *sprite_sequence;
    sprite_seq_t *r_mesh;
    tr2_item_t *item;


    item = m_tombraider.Item();
    sprite_textures = m_tombraider.Sprite();
    sprite_sequence = m_tombraider.SpriteSequence();
    scale = 4.0;

    printf("Processing sprites: ");

    for (i = 0; i < m_tombraider.NumItems() - 1; ++i)
    {
        print(false, "Processing sprites in Items: %i/%i",
                i, m_tombraider.NumItems());

        // It's a mesh, skip it
        if (m_tombraider.Engine() == TR_VERSION_1 && item[i].intensity1 == -1)
            continue;

        k = item[i].object_id;

        // Search the SpriteSequence list
        //  (if we didn't already decide that it's a mesh)
        for (j = 0; j < (int)m_tombraider.NumSpriteSequences(); ++j)
        {
            if (sprite_sequence[j].object_id == k)
            {
                k = item[i].object_id;

                s_index = sprite_sequence[j].offset;

                r_mesh = new sprite_seq_t;
                gWorld.addSprite(r_mesh);
                r_mesh->num_sprites = -sprite_sequence[j].negative_length;
                r_mesh->sprite = new sprite_t[r_mesh->num_sprites];

                for (l = 0; l < r_mesh->num_sprites; ++l)
                {
                    sprite = &sprite_textures[s_index];

                    width = sprite->width >> 8;
                    height = sprite->height >> 8;
                    x = sprite->x;
                    y = sprite->y;
                    width2 = width * scale;
                    height2 = height * scale;

                    // For vising use
                    r_mesh->sprite[l].pos[0] = item[i].x;
                    r_mesh->sprite[l].pos[1] = item[i].y;
                    r_mesh->sprite[l].pos[2] = item[i].z;
                    r_mesh->sprite[l].texture = sprite->tile + m_texOffset;

                    r_mesh->sprite[l].radius = width2 / 2.0f;

                    r_mesh->sprite[l].vertex[0].pos[0] = -width2 / 2.0f;
                    r_mesh->sprite[l].vertex[1].pos[0] = -width2 / 2.0f;
                    r_mesh->sprite[l].vertex[2].pos[0] = width2 / 2.0f;
                    r_mesh->sprite[l].vertex[3].pos[0] = width2 / 2.0f;

                    r_mesh->sprite[l].vertex[0].pos[1] = 0;
                    r_mesh->sprite[l].vertex[1].pos[1] = -height2;
                    r_mesh->sprite[l].vertex[2].pos[1] = -height2;
                    r_mesh->sprite[l].vertex[3].pos[1] = 0;

                    r_mesh->sprite[l].vertex[0].pos[2] = 0;
                    r_mesh->sprite[l].vertex[1].pos[2] = 0;
                    r_mesh->sprite[l].vertex[2].pos[2] = 0;
                    r_mesh->sprite[l].vertex[3].pos[2] = 0;

                    r_mesh->sprite[l].texel[3].st[0] = (vec_t)(x+width)/TexelScale;
                    r_mesh->sprite[l].texel[3].st[1] = (vec_t)(y+height)/TexelScale;

                    r_mesh->sprite[l].texel[2].st[0] = (vec_t)(x+width)/TexelScale;
                    r_mesh->sprite[l].texel[2].st[1] = (vec_t)(y)/TexelScale;

                    r_mesh->sprite[l].texel[1].st[0] = (vec_t)(x) /TexelScale;
                    r_mesh->sprite[l].texel[1].st[1] = (vec_t)(y) /TexelScale;

                    r_mesh->sprite[l].texel[0].st[0] = (vec_t)(x) / TexelScale;
                    r_mesh->sprite[l].texel[0].st[1] = (vec_t)(y+height)/TexelScale;

                    printf(".");
                    fflush(stdout);
                }
            }
        }
    }

    printf("\n");
}


void OpenRaider::processMoveables()
{
    List <unsigned int> cache;
    List <skeletal_model_t *> cache2;
    tr2_mesh_t *mesh = NULL;
    tr2_moveable_t *moveable = NULL;
    tr2_meshtree_t *meshtree = NULL;
    tr2_item_t *item = NULL;
    tr2_animation_t *animation = NULL;
    unsigned short *frame = NULL;
    tr2_sprite_sequence_t *sprite_sequence = NULL;
    tr2_object_texture_t *object_texture = NULL;
    int i, j, object_id;
    int ent = 0;


    frame = m_tombraider.Frame();
    moveable = m_tombraider.Moveable();
    meshtree = m_tombraider.MeshTree();
    mesh = m_tombraider.Mesh();
    object_texture = m_tombraider.ObjectTextures();
    item = m_tombraider.Item();
    animation = m_tombraider.Animation();
    sprite_sequence = m_tombraider.SpriteSequence();

    printf("Processing skeletal models: ");

    for (i = 0; i < m_tombraider.NumItems(); ++i)
    {
        object_id = item[i].object_id;

        // It may not be a moveable, test for sprite
        if (!(m_tombraider.Engine() == TR_VERSION_1 && item[i].intensity1 == -1))
        {
            for (j = 0; j < (int)m_tombraider.NumSpriteSequences(); ++j)
            {
                if (sprite_sequence[j].object_id == object_id)
                    break;
            }

            // It's not a moveable, skip sprite
            if (j != (int)m_tombraider.NumSpriteSequences())
            {
                printf("s");
                fflush(stdout);
                continue;
            }
        }

        for (j = 0; j < (int)m_tombraider.NumMoveables(); ++j)
        {
            if ((int)moveable[j].object_id == object_id)
                break;
        }

        // It's not a moveable or even a sprite?, skip unknown
        if (j == (int)m_tombraider.NumMoveables())
        {
            printf("?"); // what the wolf?
            fflush(stdout);
            continue;
        }

        processMoveable(j, i, &ent, cache2, cache, object_id);
    }

    // Get models that aren't items
    for (i = 0; i < m_tombraider.NumMoveables(); ++i)
    {
        print(false, "Processing skeletal models: %i/%i",
                i, m_tombraider.NumMoveables());

        switch ((int)moveable[i].object_id)
        {
            case 30:
            case 2: // Which tr needs this as model agian?
                processMoveable(i, i, &ent, cache2, cache,
                        (int)moveable[i].object_id);
                break;
            default:
                switch (m_tombraider.Engine())
                {
                    case TR_VERSION_1:
                        switch ((int)moveable[i].object_id)
                        {
                            case TombRaider1::LaraMutant:
                                processMoveable(i, i, &ent, cache2, cache,
                                        (int)moveable[i].object_id);
                                break;
                        }
                        break;
                    case TR_VERSION_4:
                        switch ((int)moveable[i].object_id)
                        {
                            case TR4_PISTOLS_ANIM:
                            case TR4_UZI_ANIM:
                            case TR4_SHOTGUN_ANIM:
                            case TR4_CROSSBOW_ANIM:
                            case TR4_GRENADE_GUN_ANIM:
                            case TR4_SIXSHOOTER_ANIM:
                                processMoveable(i, i, &ent, cache2, cache,
                                        (int)moveable[i].object_id);
                                break;
                        }
                        break;
                    default:
                    case TR_VERSION_2:
                    case TR_VERSION_3:
                    case TR_VERSION_5:
                    case TR_VERSION_UNKNOWN:
                        ;
                }
        }
    }

    printf("\n");
}


void OpenRaider::processMoveable(int index, int i, int *ent,
        List <skeletal_model_t *> &cache2,
        List <unsigned int> &cache, int object_id)
{
    skeletal_model_t *r_model = NULL;
    skeletal_model_t *c_model = NULL;
    animation_frame_t *animation_frame = NULL;
    tr2_mesh_t *mesh = NULL;
    tr2_moveable_t *moveable = NULL;
    tr2_meshtree_t *meshtree = NULL;
    tr2_item_t *item = NULL;
    tr2_animation_t *animation = NULL;
    tr2_meshtree_t *mesh_tree = NULL;
    bone_frame_t *bone = NULL;
    bone_tag_t *tag = NULL;
    entity_t *thing = NULL;
    SkeletalModel *sModel = 0x0;
    unsigned short *frame;
    int j, k, a, frame_step;
    unsigned int l, frame_offset, frame_count, f;
    float pos[3];
    float yaw;
    bool lara = false;
    int skyMesh;


    skyMesh = m_tombraider.getSkyModelId();
    frame = m_tombraider.Frame();
    moveable = m_tombraider.Moveable();
    meshtree = m_tombraider.MeshTree();
    mesh = m_tombraider.Mesh();
    item = m_tombraider.Item();
    animation = m_tombraider.Animation();

    pos[0] = item[i].x;
    pos[1] = item[i].y;
    pos[2] = item[i].z;

    yaw = ((item[i].angle >> 14) & 0x03);
    yaw *= 90;

    thing = new entity_t;
    thing->id = (*ent)++;
    thing->type = 0x00;
    thing->pos[0] = item[i].x;
    thing->pos[1] = item[i].y;
    thing->pos[2] = item[i].z;
    thing->angles[1] = yaw;
    thing->objectId = moveable[index].object_id;
    thing->moving = false;
    thing->animate = false;

    sModel = new SkeletalModel();
    m_render.addSkeletalModel(sModel);
    thing->tmpHook = sModel; // temp hack to keep a running version during refactoring

    if (m_tombraider.Engine() == TR_VERSION_1)
    {
        switch (thing->objectId)
        {
            case TombRaider1::Wolf:
                thing->state = TombRaider1::WolfState_Lying;
                //thing->animate = true;
                sModel->setAnimation(3);
                sModel->setFrame(0);
                break;
        }
    }

    //! \fixme Check here and see if we already have one for object_id later
    // if (gWorld.isCachedSkeletalModel(moveable[index].object_id))
    // {
    //   thing->modelId = m_render.add(sModel);
    //   return;
    // }

    r_model = new skeletal_model_t;
    r_model->id = moveable[index].object_id;

    // Gather more info if this is lara
    if (moveable[index].object_id == 0)
    {
        gLaraModel = r_model; // hack to avoid broken system until new event sys

        lara = true;
        m_camera.translate(pos[0], pos[1] - 470, pos[2]);
        thing->type = 0x02;
        LARA = thing; // Mongoose 2002.03.22, Cheap hack for now
        LARA->master = 0x0;

        switch (m_tombraider.Engine())
        {
            case TR_VERSION_3:
                LARA->modelId = i;
                sModel->setAnimation(TR_ANIAMTION_RUN);
                sModel->setIdleAnimation(TR_ANIAMTION_STAND);
                r_model->tr4Overlay = false;
                break;
            case TR_VERSION_4:
                LARA->modelId = i;
                sModel->setAnimation(TR_ANIAMTION_RUN);
                sModel->setIdleAnimation(TR_ANIAMTION_STAND);
                // Only TR4 lara has 2 layer bone tags/meshes per bone frame
                r_model->tr4Overlay = true;
                break;
            default:
            case TR_VERSION_1:
            case TR_VERSION_2:
            case TR_VERSION_5:
            case TR_VERSION_UNKNOWN:
                LARA->modelId = index;
                sModel->setAnimation(TR_ANIAMTION_RUN);
                sModel->setIdleAnimation(TR_ANIAMTION_STAND);
                r_model->tr4Overlay = false;
        }

        r_model->ponytailId = 0;
    }
    else
    {
        lara = false;
        r_model->ponytailId = -1;
    }

    // Animation
    a = moveable[index].animation;

    frame_offset = animation[a].frame_offset / 2;
    frame_step = animation[a].frame_size;

    int frame_cycle = 0;

    if (a >= (int)m_tombraider.NumAnimations())
    {
        a = m_tombraider.NumFrames() - frame_offset;
    }
    else
    {
        a = (animation[a].frame_offset / 2) - frame_offset;
    }

    if (frame_step != 0)  // prevent divide-by-zero errors
        a /= frame_step;

    if (a != 0) // prevent divide-by-zero errors
        frame_offset += frame_step * (frame_cycle % a);

    if (a < 0)
    {
        //continue;
        return;
    }

    //! \fixme Might be better UID for each model, but this seems to work well
    j = object_id;

    // We only want one copy of the skeletal model in memory
    if (cache.Empty() || cache.SearchKey(j) == UINT_MAX)
    {
        sModel->model = r_model;
        gWorld.addEntity(thing);

        k = gWorld.addModel(r_model);

        cache.Add(j);
        cache2.Add(r_model);

        switch (m_tombraider.Engine())
        {
            case TR_VERSION_4:
                if (LARA && moveable[index].object_id == 30)
                {
                    r_model->ponytailId = k;
                    r_model->ponytailMeshId = moveable[index].starting_mesh;
                    r_model->ponytailNumMeshes = ((moveable[index].num_meshes > 0) ?
                            moveable[index].num_meshes : 0);
                    r_model->ponytailAngle = -90.0f;
                    r_model->ponytail[0] = -3;
                    r_model->ponytail[1] = -22;
                    r_model->ponytail[2] = -20;
                    r_model->ponyOff = 40;
                    r_model->ponyOff2 = 32;
                    r_model->pigtails = false;

                    // Try to guess pigtails by looking for certian num verts in head
                    if (mesh[moveable[0].starting_mesh].num_vertices > 80)
                    {
                        r_model->pigtails = true;
                        r_model->ponyOff -= 20;
                        r_model->ponytail[1] -= 32;
                    }

                    m_render.setFlags(Render::fRenderPonytail);
                    print(true, "Found known ponytail\n");
                }
                break; // ?
            default:
            case TR_VERSION_1:
            case TR_VERSION_2:
            case TR_VERSION_3:
            case TR_VERSION_5:
            case TR_VERSION_UNKNOWN:
                if (LARA && moveable[index].object_id == 2)
                {
                    r_model->ponytailId = k;
                    r_model->ponytailMeshId = moveable[index].starting_mesh;
                    r_model->ponytailNumMeshes = ((moveable[index].num_meshes > 0) ?
                            moveable[index].num_meshes : 0);
                    r_model->ponytailAngle = -90.0f;
                    r_model->ponytail[0] = 0;
                    r_model->ponytail[1] = -20;
                    r_model->ponytail[2] = -20;
                    r_model->ponyOff = 40;
                    r_model->ponyOff2 = 0;

                    m_render.setFlags(Render::fRenderPonytail);
                    print(true, "Found ponytail?\n");
                }
        }
    }
    else
    {
        delete r_model;

        c_model = cache2[cache.SearchKey(j)];
        sModel->model = c_model;
        gWorld.addEntity(thing);
        gWorld.addModel(c_model);
        printf("c"); // it's already cached
        fflush(stdout);

        //continue;
        return;
    }


    int aloop = m_tombraider.getNumAnimsForMoveable(index);

#ifdef DEBUG
    if (m_flags & OpenRaider_DebugModel)
    {
        printf("\nanimation = %i, num_animations = %i\n",
                moveable[index].animation, aloop);
        printf("\nitem[%i].flags = %i\nentity[%i]\n",
                i, item[i].flags, thing->id);
    }
#endif

    //a = moveable[index].animation;
    //frame_offset = animation[a].frame_offset / 2;
    //frame_step = animation[a].frame_size;

    for (; a < aloop; ++a,
            frame_offset = animation[a].frame_offset / 2,
            frame_step = animation[a].frame_size)
    {
        animation_frame = new animation_frame_t;
        r_model->animation.pushBack(animation_frame);

        frame_count = (animation[a].frame_end - animation[a].frame_start) + 1;
        animation_frame->rate = animation[a].frame_rate;

#ifdef DEBUG
        if (m_flags & OpenRaider_DebugModel)
        {
            printf("animation[%i] state and unknowns = %i, %i, %i, %i, %i\n",
                    a, animation[a].state_id, animation[a].unknown1,
                    animation[a].unknown2, animation[a].unknown3,
                    animation[a].unknown4);
            printf("animation[%i].frame_rate = %i\n",
                    a, animation[a].frame_rate);
            printf("animation[%i].next_animation = %i\n",
                    a, animation[a].next_animation);
            printf("animation[%i].frame_offset = %u\n",
                    a, animation[a].frame_offset);
            printf("animation[%i].anim_command = %i\n",
                    a, animation[a].anim_command);
            printf("animation[%i].num_anim_commands = %i\n",
                    a, animation[a].num_anim_commands);
            printf("animation[%i].state_change_offset = %i\n",
                    a, animation[a].state_change_offset);
            printf("              frame_offset = %u\n",
                    frame_offset);
        }
#endif

        // Get all the frames for aniamtion
        for (f = 0; f < frame_count; ++f, frame_offset += frame_step)
        {
            // HACK: Lara's ObjectID is 315, but her meshes start at 0, so make a
            // quick substitution (so she doesn't appear as a bunch of thighs)
            if (index == 0 && m_tombraider.Engine() == TR_VERSION_3)
            {
                for (j = 0; j < (int)m_tombraider.NumMoveables() && !index; ++j)
                {
                    if (moveable[j].object_id == 315)
                        index = j;
                }
            }

            // Fix Lara in TR4
            if (index == 0 && m_tombraider.Engine() == TR_VERSION_4)
            {
                for (j = 0; j < (int)m_tombraider.NumMoveables() && !index; ++j)
                {
                    // Body is ItemID 8, joints are ItemID 9
                    //  (TR4 demo: body is ItemID 10, joints are ItemID 11)
                    if (moveable[j].object_id == 8)
                        index = j;
                }
            }
            else if (moveable[index].object_id == 8 &&
                    m_tombraider.Engine() == TR_VERSION_4)
            {
                // KLUDGE to do "skinning"
                index = 0;

                for (j = 0; j < (int)m_tombraider.NumMoveables() && !index; ++j)
                {
                    // Body is ItemID 8, joints are ItemID 9
                    //  (TR4 demo: body is ItemID 10, joints are ItemID 11)
                    if (moveable[j].object_id == 9)
                        index = j;
                }
            }

#ifdef DEBUG
            if (m_flags & OpenRaider_DebugModel)
            {
                printf("animation[%i].boneframe[%u] = offset %u, step %i\n",
                        a, f, frame_offset, frame_step);
            }
#endif

            // Mongoose 2002.08.15, Was
            //   if (frame_offset + 8 > _tombraider.NumFrames())
            if (frame_offset > m_tombraider.NumFrames())
            {
                print(true, "WARNING: Bad animation frame %i > %i\n",
                        frame_offset, m_tombraider.NumFrames());

                // Mongoose 2002.08.15, Attempt to skip more likely bad animation data
                print(true, "WARNING: Handling bad animation data...");
                return; //continue;
            }

            // Generate bone frames and tags per frame ////////////
            bone = new bone_frame_t;
            animation_frame->frame.pushBack(bone);

            // Init translate for bone frame
            bone->pos[0] = (short)frame[frame_offset + 6];
            bone->pos[1] = (short)frame[frame_offset + 7];
            bone->pos[2] = (short)frame[frame_offset + 8];
            bone->yaw = yaw;

            //printf("%f %f %f\n", bone->pos[0], bone->pos[1], bone->pos[2]);

            l = 9;   // First angle offset in this Frame

            // Run through the tag and calculate the rotation and offset
            for (j = 0; j < (int)moveable[index].num_meshes; ++j)
            {
                tag = new bone_tag_t;
                bone->tag.pushBack(tag);
                tag->off[0] = 0.0;
                tag->off[1] = 0.0;
                tag->off[2] = 0.0;
                tag->flag = 0x00;
                tag->rot[0] = 0.0;
                tag->rot[1] = 0.0;
                tag->rot[2] = 0.0;
                tag->mesh = moveable[index].starting_mesh + j;


                // Setup offsets to produce skeletion
                if (j == 0)
                {
                    // Since we use bone's offset, these aren't used
                    tag->off[0] = 0.0;
                    tag->off[1] = 0.0;
                    tag->off[2] = 0.0;

                    // Always push tag[0], this isn't really used either
                    tag->flag = 0x02;
                }
                else  // Nonprimary tag - position relative to first tag
                {
                    int *tree;
                    // Hack: moveable[index].mesh_tree is a byte offset
                    //       into mesh_tree[], so we have to convert to index
                    tree = (int *)(void *)meshtree;
                    mesh_tree = (tr2_meshtree_t *)&tree[moveable[index].mesh_tree
                        + ((j - 1) * 4)];

                    tag->off[0] = mesh_tree->x;
                    tag->off[1] = mesh_tree->y;
                    tag->off[2] = mesh_tree->z;
                    tag->flag = (char)mesh_tree->flags;
                }

                // Setup tag rotations
                m_tombraider.computeRotationAngles(&frame, &frame_offset, &l,
                        tag->rot, tag->rot+1, tag->rot+2);
            }
        }
    }

    if (i == skyMesh)
    {
        m_render.setSkyMesh(i, //moveable[i].starting_mesh,
                (m_tombraider.Engine() == TR_VERSION_2));
    }

    printf(".");
    fflush(stdout);
}


int compareFaceTextureId(const void *voidA, const void *voidB)
{
    texture_tri_t *a = (texture_tri_t *)voidA, *b = (texture_tri_t *)voidB;


    if (!a || !b)
        return -1; // error really

    // less than
    if (a->texture < b->texture)
        return -1;

    // greater than ( no need for equal )
    return 1;
}


void setupTextureColor(texture_tri_t *r_tri, Render *render, float *colorf)
{
    unsigned char color[4];
    unsigned int colorI;


    color[0] = (unsigned char)(colorf[0]*255.0f);
    color[1] = (unsigned char)(colorf[1]*255.0f);
    color[2] = (unsigned char)(colorf[2]*255.0f);
    color[3] = (unsigned char)(colorf[3]*255.0f);

    ((unsigned char *)(&colorI))[3] = color[0];
    ((unsigned char *)(&colorI))[2] = color[1];
    ((unsigned char *)(&colorI))[1] = color[2];
    ((unsigned char *)(&colorI))[0] = color[3];

    if (!gColorTextureHACK.find(colorI))
    {
        gColorTextureHACK.pushBack(colorI);
        r_tri->texture = gTextureOffset + gColorTextureHACK.size();

        render->loadTexture(Texture::generateColorTexture(color, 32, 32),
                32, 32,
                r_tri->texture);

#ifdef DEBUG_COLOR_TEXTURE_GEN
        printf("Color 0x%02x%02x%02x%02x | 0x%08xto texture[%u]?\n",
                color[0], color[1], color[2], color[3], colorI,
                gColorTextureHACK.size());
#endif
    }
    else
    {
        //printf("Color already loaded %i -> 0x%08x\n",
        //       gColorTextureHACK.getCurrentIndex(),
        //       gColorTextureHACK.current());

        r_tri->texture = gTextureOffset + gColorTextureHACK.getCurrentIndex();
    }

    //r_tri->texture = white; // White texture
}


void OpenRaider::processModel(int index)
{
    int i, j, count, texture;
    int vertexIndices[6];
    float st[12];
    float color[4];
    unsigned short transparency;
    texture_tri_t *r_tri;


    // Assert common sense
    if (index < 0 || !m_tombraider.isMeshValid(index))
    {
        //! \fixme allow sparse lists with matching ids instead?
        gWorld.addMesh(NULL); // Filler, to make meshes array ids align
        printf("x");
        fflush(stdout);
        return;
    }

#ifndef EXPERIMENTAL
    // WHITE texture id
    int white = 0;
#endif

    model_mesh_t *mesh = new model_mesh_t;

    // Mongoose 2002.08.30, Testing support for 'shootable' models ( traceable )
    m_tombraider.getMeshCollisionInfo(index, mesh->center, &mesh->radius);

    //! \fixme Arrays don't work either  =)
    // Mesh geometery, colors, etc
    m_tombraider.getMeshVertexArrays(index,
            &mesh->vertexCount, &mesh->vertices,
            &mesh->normalCount, &mesh->normals,
            &mesh->colorCount,  &mesh->colors);


    // Textured Triangles
    count = m_tombraider.getMeshTexturedTriangleCount(index);
    mesh->texturedTriangles.reserve(count); // little faster

    for (i = 0; i < count; ++i)
    {
        r_tri = new texture_tri_t;

        m_tombraider.getMeshTexturedTriangle(index, i,
                r_tri->index,
                r_tri->st,
                &r_tri->texture,
                &r_tri->transparency);

        r_tri->texture += m_texOffset;

        // Add to face vector
        mesh->texturedTriangles.pushBack(r_tri);
    }


    // Coloured Triangles
    count = m_tombraider.getMeshColoredTriangleCount(index);
    mesh->coloredTriangles.reserve(count); // little faster

    for (i = 0; i < count; i++)
    {
        r_tri = new texture_tri_t;

        m_tombraider.getMeshColoredTriangle(index, i,
                r_tri->index,
                color);
        r_tri->st[0] = color[0];
        r_tri->st[1] = color[1];
        r_tri->st[2] = color[2];
        r_tri->st[3] = color[3];
        r_tri->st[4] = 1.0;
        r_tri->st[5] = 1.0;

#ifdef EXPERIMENTAL
        setupTextureColor(r_tri, &m_render, color);
#else
        r_tri->texture = white; // White texture
#endif
        r_tri->transparency = 0;

        // Add to face vector
        mesh->coloredTriangles.pushBack(r_tri);
    }

    // Textured Rectangles
    count = m_tombraider.getMeshTexturedRectangleCount(index);
    mesh->texturedRectangles.reserve(count*2); // little faster

    for (i = 0; i < count; ++i)
    {
        m_tombraider.getMeshTexturedRectangle(index, i,
                vertexIndices,
                st,
                &texture,
                &transparency);

        r_tri = new texture_tri_t;

        for (j = 0; j < 3; ++j)
            r_tri->index[j] = vertexIndices[j];

        for (j = 0; j < 6; ++j)
            r_tri->st[j] = st[j];

        r_tri->texture = texture + m_texOffset;
        r_tri->transparency = transparency;

        // Add to face vector
        mesh->texturedRectangles.pushBack(r_tri);

        r_tri = new texture_tri_t;

        for (j = 3; j < 6; ++j)
            r_tri->index[j-3] = vertexIndices[j];

        for (j = 6; j < 12; ++j)
            r_tri->st[j-6] = st[j];

        r_tri->texture = texture + m_texOffset;
        r_tri->transparency = transparency;

        // Add to face vector
        mesh->texturedRectangles.pushBack(r_tri);
    }


    // Coloured Rectangles
    count = m_tombraider.getMeshColoredRectangleCount(index);
    mesh->coloredRectangles.reserve(count*2); // little faster

    for (i = 0; i < count; ++i)
    {
        m_tombraider.getMeshColoredRectangle(index, i,
                vertexIndices,
                color);

        r_tri = new texture_tri_t;

        for (j = 0; j < 3; ++j)
            r_tri->index[j] = vertexIndices[j];

        //for (j = 0; j < 6; ++j)
        //  r_tri->st[j] = st[j];

        r_tri->st[0] = color[0];
        r_tri->st[1] = color[1];
        r_tri->st[2] = color[2];
        r_tri->st[3] = color[3];
        r_tri->st[4] = 1.0;
        r_tri->st[5] = 1.0;

#ifdef EXPERIMENTAL
        //for (j = 6; j < 12; ++j)
        //  r_tri->st[j-6] = st[j];
        setupTextureColor(r_tri, &m_render, color);
#else
        r_tri->texture = white; // White texture
#endif
        r_tri->transparency = 0;

        // Add to face vector
        mesh->coloredRectangles.pushBack(r_tri);

        r_tri = new texture_tri_t;

        for (j = 3; j < 6; ++j)
            r_tri->index[j-3] = vertexIndices[j];

        //for (j = 6; j < 12; ++j)
        //  r_tri->st[j-6] = st[j];

        r_tri->st[0] = color[0];
        r_tri->st[1] = color[1];
        r_tri->st[2] = color[2];
        r_tri->st[3] = color[3];
        r_tri->st[4] = 1.0;
        r_tri->st[5] = 1.0;

#ifdef EXPERIMENTAL
        setupTextureColor(r_tri, &m_render, color);
#else
        r_tri->texture = white; // White texture
#endif
        r_tri->transparency = 0;

        // Add to face vector
        mesh->coloredRectangles.pushBack(r_tri);
    }

    // Sort faces by texture
    mesh->texturedTriangles.qSort(compareFaceTextureId);
    mesh->coloredTriangles.qSort(compareFaceTextureId);
    mesh->texturedRectangles.qSort(compareFaceTextureId);
    mesh->coloredRectangles.qSort(compareFaceTextureId);

    gWorld.addMesh(mesh);
    printf(".");
    fflush(stdout);
}


#define MATRIX_TRANSFORMS
void OpenRaider::processRoom(int index)
{
    unsigned int i, j, count;
    room_mesh_t *r_mesh = NULL;
    RenderRoom *rRoom = NULL;
#ifdef MATRIX_TRANSFORMS
    //matrix_t transform;
    Matrix transform;
#endif


    if (!m_tombraider.isRoomValid(index))
    {
        openraider_error("OpenRaider::RoomSetup", "Invalid room index",
                __FILE__, __LINE__);

        print(false, "WARNING: Handling invalid vertex array in room");
        gWorld.addRoom(0x0);
        m_render.addRoom(0x0);

        printf("x");
        fflush(stdout);
        return;
    }

    rRoom = new RenderRoom();
    r_mesh = new room_mesh_t;
    r_mesh->id = index;

    m_tombraider.getRoomInfo(index, &r_mesh->flags, r_mesh->pos,
            r_mesh->bbox_min, r_mesh->bbox_max);

    // Adjust positioning for OR world coord translation
    r_mesh->bbox_min[0] += r_mesh->pos[0];
    r_mesh->bbox_max[0] += r_mesh->pos[0];
    r_mesh->bbox_min[2] += r_mesh->pos[2];
    r_mesh->bbox_max[2] += r_mesh->pos[2];

    // Mongoose 2002.04.03, Setup 3d transform
#ifdef MATRIX_TRANSFORMS
    //mtkMatrixIdentity(transform);
    //mtkMatrixTranslate(transform,
    //                       r_mesh->pos[0], r_mesh->pos[1], r_mesh->pos[2]);
    transform.setIdentity();
    transform.translate(r_mesh->pos);
#else
#endif

    // Setup portals
    float portalVertices[12];
    count = m_tombraider.getRoomPortalCount(index);

    //! \fixme OR wrongly uses a cached adj room list for rendering vis
    r_mesh->adjacentRooms.reserve(count + 1);
    r_mesh->adjacentRooms.setError(-1);

    // Current room is always first
    r_mesh->adjacentRooms.pushBack(index);

    for (i = 0; i < count; ++i)
    {
        portal_t *portal = new portal_t;

        m_tombraider.getRoomPortal(index, i,
                &portal->adjoining_room, portal->normal,
                portalVertices);

        for (j = 0; j < 4; ++j)
        {
            portal->vertices[j][0] = portalVertices[j*3];
            portal->vertices[j][1] = portalVertices[j*3+1];
            portal->vertices[j][2] = portalVertices[j*3+2];

            // Relative coors in vis portals
#ifdef MATRIX_TRANSFORMS
            //mtkMatrixTransform(transform, portal->vertices[j]);
            transform.multiply3v(portal->vertices[j], portal->vertices[j]);
#else
            portal->vertices[j][0] += r_mesh->pos[0];
            portal->vertices[j][1] += r_mesh->pos[1];
            portal->vertices[j][2] += r_mesh->pos[2];
#endif
        }

        r_mesh->adjacentRooms.pushBack(portal->adjoining_room);
        r_mesh->portals.pushBack(portal);
    }

    // Physics/gameplay use /////////////////////////////

    //! \fixme Use more of sector structure, boxes, and floordata

    // List of sectors in this room
    unsigned int sectorFlags;
    int floorDataIndex, boxIndex, roomBelow, roomAbove;
    count = m_tombraider.getRoomSectorCount(index, &r_mesh->numZSectors,
            &r_mesh->numXSectors);
    r_mesh->sectors.reserve(count);
    r_mesh->sectors.setError(0x0);

    for (i = 0; i < count; ++i)
    {
        sector_t *sector = new sector_t;

        m_tombraider.getRoomSector(index, i, &sectorFlags,
                &sector->ceiling, &sector->floor,
                &floorDataIndex, &boxIndex, &roomBelow,
                &roomAbove);

        if (sectorFlags & tombraiderSector_wall)
        {
            sector->wall = true;
        }
        else
        {
            sector->wall = false;
        }

        r_mesh->sectors.pushBack(sector);
    }

    // Setup collision boxes ( Should use sectors, but this is a test )
    count = m_tombraider.getRoomBoxCount(index);
    r_mesh->boxes.reserve(count);
    r_mesh->boxes.setError(0x0);

    //! fixme Only to be done only on room[0]?  I don't think so...
    for (i = 0; !index && i < count; ++i)
    {
        box_t *box = new box_t;

        m_tombraider.getRoomBox(index, i,
                box->a.pos, box->b.pos, box->c.pos, box->d.pos);

        r_mesh->boxes.pushBack(box);
    }


    // Setup room lights /////////////////////////////////////
    unsigned int lightFlags, lightType;
    count = m_tombraider.getRoomLightCount(index);
    rRoom->lights.reserve(count);
    rRoom->lights.setError(0x0);

    for (i = 0; i < count; ++i)
    {
        Light *light = new Light();

        m_tombraider.getRoomLight(index, i,
                light->mPos, light->mColor, light->mDir,
                &light->mAtt, &light->mCutoff,
                &lightType, &lightFlags);

        switch (lightType)
        {
            case tombraiderLight_typeDirectional:
                light->mType = Light::typeDirectional;
                break;
            case tombraiderLight_typeSpot:
                light->mType = Light::typeSpot;
                break;
            case tombraiderLight_typePoint:
            default:
                light->mType = Light::typePoint;
        }

        rRoom->lights.pushBack(light);
    }


    // Room geometery //////////////////////////////////

    //#define EXPERIMENTAL_UNFIFIED_ROOM_GEOMETERY
#ifdef EXPERIMENTAL_UNFIFIED_ROOM_GEOMETERY
    unsigned int vertexCount, normalCount, colorCount, triCount;
    vec_t *vertexArray;
    vec_t *normalArray;
    vec_t *colorArray;
    unsigned int *indices, *flags;
    float *texCoords;
    int *textures;


    m_tombraider.getRoomVertexArrays(index,
            &vertexCount, &vertexArray,
            &normalCount, &normalArray,
            &colorCount, &colorArray);

    rRoom->mesh.bufferVertexArray(vertexCount, (vec_t *)vertexArray);
    rRoom->mesh.bufferNormalArray(normalCount, (vec_t *)normalArray);
    rRoom->mesh.bufferColorArray(vertexCount, (vec_t *)colorArray);

    m_tombraider.getRoomTriangles(index, m_texOffset,
            &triCount, &indices, &texCoords, &textures,
            &flags);

    rRoom->mesh.bufferTriangles(triCount, indices, texCoords, textures, flags);
#else
    float rgba[4];
    float xyz[3];

    count = m_tombraider.getRoomVertexCount(index);
    rRoom->mesh.allocateVertices(count);
    rRoom->mesh.allocateNormals(0); // count
    rRoom->mesh.allocateColors(count);

    for (i = 0; i < count; ++i)
    {
        m_tombraider.getRoomVertex(index, i, xyz, rgba);

        rRoom->mesh.setVertex(i, xyz[0], xyz[1], xyz[2]);
        rRoom->mesh.setColor(i, rgba);
    }

    // Mongoose 2002.06.09, Setup allocation of meshes and polygons
    // Counters ( Textured polygon lists are allocated per texture)
    //          ( Textures are mapped to these meshes )
    int triangle_counter[TextureLimit];
    int triangle_counter_alpha[TextureLimit];
    int rectangle_counter[TextureLimit];
    int rectangle_counter_alpha[TextureLimit];
    int tris_mesh_map[TextureLimit];
    int rect_mesh_map[TextureLimit];


    for (i = 0; i < TextureLimit; ++i)
    {
        triangle_counter[i]        = 0;
        triangle_counter_alpha[i]  = 0;
        rectangle_counter[i]       = 0;
        rectangle_counter_alpha[i] = 0;

        tris_mesh_map[i] = -1;
        rect_mesh_map[i] = -1;
    }

    unsigned int numTris = 0;
    unsigned int numQuads = 0;

    int texture;
    unsigned int r, t, q, v, flags;
    unsigned int indices[4];
    float texCoords[8];

    count = m_tombraider.getRoomTriangleCount(index);

    // Mongoose 2002.08.15, Presort by alpha and texture and setup mapping
    for (t = 0; t < count; ++t)
    {
        m_tombraider.getRoomTriangle(index, t,
                indices, texCoords, &texture, &flags);

        texture += m_texOffset;

        if (texture > (int)TextureLimit)
        {
            print(true, "Handling bad room[%i].tris[%i].texture = %i",
                    index, t, texture);
            texture = TextureLimit - 1;
        }

        // Counters set up polygon allocation
        if (flags & tombraiderFace_Alpha ||
                flags & tombraiderFace_PartialAlpha)
        {
            triangle_counter_alpha[texture] += 1;
        }
        else
        {
            triangle_counter[texture] += 1;
        }

        // Counter sets up texture id to mesh id mapping
        if (tris_mesh_map[texture] == -1)
        {
            tris_mesh_map[texture] = ++numTris;
        }
    }

    count = m_tombraider.getRoomRectangleCount(index);

    for (r = 0; r < count; ++r)
    {
        m_tombraider.getRoomRectangle(index, r,
                indices, texCoords, &texture, &flags);

        texture += m_texOffset;

        if (texture > (int)TextureLimit)
        {
            print(true, "Handling bad room[%i].quad[%i].texture = %i",
                    index, r, texture);
            texture = TextureLimit - 1;
        }

        if (flags & tombraiderFace_Alpha ||
                flags & tombraiderFace_PartialAlpha)
        {
            rectangle_counter_alpha[texture] += 1;
        }
        else
        {
            rectangle_counter[texture] += 1;
        }

        if (rect_mesh_map[texture] == -1)
        {
            rect_mesh_map[texture] = ++numQuads;
        }
    }

    // Allocate indexed polygon meshes
    rRoom->mesh.allocateTriangles(numTris);
    rRoom->mesh.allocateRectangles(numQuads);

    for (i = 0, j = 0; i < TextureLimit; ++i)
    {
        if (tris_mesh_map[i] > 0)
        {
            j = tris_mesh_map[i] - 1;

            t = triangle_counter[i];

            rRoom->mesh.mTris[j].texture = i;
#ifdef MULTITEXTURE
            rRoom->mesh.mTris[j].bumpmap = gMapTex2Bump[i];
#endif
            rRoom->mesh.mTris[j].cnum_triangles = 0;
            rRoom->mesh.mTris[j].num_triangles = 0;
            rRoom->mesh.mTris[j].cnum_alpha_triangles = 0;
            rRoom->mesh.mTris[j].num_alpha_triangles = 0;
            rRoom->mesh.mTris[j].triangles = 0x0;
            rRoom->mesh.mTris[j].alpha_triangles = 0x0;
            rRoom->mesh.mTris[j].texcoors = 0x0;
            rRoom->mesh.mTris[j].texcoors2 = 0x0;

            if (t > 0)
            {
                rRoom->mesh.mTris[j].num_triangles = t;
                rRoom->mesh.mTris[j].triangles = new unsigned int[t*3];
                rRoom->mesh.mTris[j].num_texcoors = t * 3;
                rRoom->mesh.mTris[j].texcoors = new vec2_t[t * 3];
            }

            t = triangle_counter_alpha[i];

            if (t > 0)
            {
                rRoom->mesh.mTris[j].num_alpha_triangles = t;
                rRoom->mesh.mTris[j].alpha_triangles = new unsigned int[t*3];
                rRoom->mesh.mTris[j].num_texcoors2 = t * 3;
                rRoom->mesh.mTris[j].texcoors2 = new vec2_t[t * 3];
            }
        }

        ///////////////////////////////////////////

        if (rect_mesh_map[i] > 0)
        {
            j = rect_mesh_map[i] - 1;

            r = rectangle_counter[i];

            rRoom->mesh.mQuads[j].texture = i;
#ifdef MULTITEXTURE
            rRoom->mesh.mQuads[j].bumpmap = gMapTex2Bump[i];
#endif
            rRoom->mesh.mQuads[j].cnum_quads = 0;
            rRoom->mesh.mQuads[j].num_quads = 0;
            rRoom->mesh.mQuads[j].cnum_alpha_quads = 0;
            rRoom->mesh.mQuads[j].num_alpha_quads = 0;
            rRoom->mesh.mQuads[j].quads = 0x0;
            rRoom->mesh.mQuads[j].alpha_quads = 0x0;
            rRoom->mesh.mQuads[j].texcoors = 0x0;
            rRoom->mesh.mQuads[j].texcoors2 = 0x0;

            if (r > 0)
            {
                rRoom->mesh.mQuads[j].num_quads = r;
                rRoom->mesh.mQuads[j].quads = new unsigned int[r*4];
                rRoom->mesh.mQuads[j].num_texcoors = r * 4;
                rRoom->mesh.mQuads[j].texcoors = new vec2_t[r * 4];
            }

            r = rectangle_counter_alpha[i];

            if (r > 0)
            {
                rRoom->mesh.mQuads[j].num_alpha_quads = r;
                rRoom->mesh.mQuads[j].alpha_quads = new unsigned int[r*4];
                rRoom->mesh.mQuads[j].num_texcoors2 = r * 4;
                rRoom->mesh.mQuads[j].texcoors2 = new vec2_t[r * 4];
            }
        }
    }

    // Generate textured triangles
    count = m_tombraider.getRoomTriangleCount(index);

    for (t = 0; t < count; ++t)
    {
        m_tombraider.getRoomTriangle(index, t,
                indices, texCoords, &texture, &flags);

        // Adjust texture id using m_texOffset to map into
        // correct textures
        texture += m_texOffset;

        j = tris_mesh_map[texture] - 1;

        // Setup per vertex
        for (i = 0; i < 3; ++i)
        {
            // Get vertex index {(0, a), (1, b), (2, c)}
            v = indices[i];

            if ((flags & tombraiderFace_Alpha ||
                        flags & tombraiderFace_PartialAlpha) &&
                    rRoom->mesh.mTris[j].num_alpha_triangles > 0)
            {
                q = rRoom->mesh.mTris[j].cnum_alpha_triangles*3+i;

                rRoom->mesh.mTris[j].alpha_triangles[q] = v;

                rRoom->mesh.mTris[j].texcoors2[q][0] = texCoords[i*2];
                rRoom->mesh.mTris[j].texcoors2[q][1] = texCoords[i*2+1];
            }
            else if (rRoom->mesh.mTris[j].num_triangles > 0)
            {
                q = rRoom->mesh.mTris[j].cnum_triangles*3+i;

                rRoom->mesh.mTris[j].triangles[q] = v;

                rRoom->mesh.mTris[j].texcoors[q][0] = texCoords[i*2];
                rRoom->mesh.mTris[j].texcoors[q][1] = texCoords[i*2+1];
            }

            // Partial alpha hack
            if (flags & tombraiderFace_PartialAlpha)
            {
                //rRoom->mesh.colors[v].rgba[3] = 0.45;
            }
        }

        if (flags & tombraiderFace_Alpha ||
                flags & tombraiderFace_PartialAlpha)
        {
            rRoom->mesh.mTris[j].cnum_alpha_triangles++;
        }
        else
        {
            rRoom->mesh.mTris[j].cnum_triangles++;
        }
    }


    // Generate textured quads
    count = m_tombraider.getRoomRectangleCount(index);

    for (r = 0; r < count; ++r)
    {
        m_tombraider.getRoomRectangle(index, r,
                indices, texCoords, &texture, &flags);

        // Adjust texture id using m_texOffset to map into
        // correct textures
        texture += m_texOffset;

        if (texture > (int)TextureLimit)
        {
            texture = TextureLimit - 1;
        }

        j = rect_mesh_map[texture] - 1;

        if (rRoom->mesh.mQuads[j].num_quads <= 0 &&
                rRoom->mesh.mQuads[j].num_alpha_quads <= 0)
            continue;

        // Setup per vertex
        for (i = 0; i < 4; ++i)
        {
            // Get vertex index {(0, a), (1, b), (2, c), (3, d)}
            v = indices[i];

            if ((flags & tombraiderFace_Alpha ||
                        flags & tombraiderFace_PartialAlpha) &&
                    rRoom->mesh.mQuads[j].num_alpha_quads > 0)
            {
                q = rRoom->mesh.mQuads[j].cnum_alpha_quads*4+i;

                rRoom->mesh.mQuads[j].alpha_quads[q] = v;

                rRoom->mesh.mQuads[j].texcoors2[q][0] = texCoords[i*2];
                rRoom->mesh.mQuads[j].texcoors2[q][1] = texCoords[i*2+1];
            }
            else if (rRoom->mesh.mQuads[j].num_quads > 0)
            {
                q = rRoom->mesh.mQuads[j].cnum_quads*4+i;

                rRoom->mesh.mQuads[j].quads[q] = v;

                rRoom->mesh.mQuads[j].texcoors[q][0] = texCoords[i*2];
                rRoom->mesh.mQuads[j].texcoors[q][1] = texCoords[i*2+1];
            }

            // Partial alpha hack
            if (flags & tombraiderFace_PartialAlpha)
            {
                //rRoom->mesh.colors[v].rgba[3] = 0.45;
            }
        }

        if (flags & tombraiderFace_Alpha ||
                flags & tombraiderFace_PartialAlpha)
        {
            rRoom->mesh.mQuads[j].cnum_alpha_quads++;
        }
        else
        {
            rRoom->mesh.mQuads[j].cnum_quads++;
        }
    }
#endif

    // Room models
    count = m_tombraider.getRoomModelCount(index);
    r_mesh->models.reserve(count);
    r_mesh->models.setError(0x0);

    for (i = 0; i < count; ++i)
    {
        static_model_t *model = new static_model_t;

        m_tombraider.getRoomModel(index, i,
                &model->index, model->pos, &model->yaw);

        r_mesh->models.pushBack(model);
    }

    // Room sprites
    float spriteVertices[12];
    float spriteTexCoords[8];
    count = m_tombraider.getRoomSpriteCount(index);
    r_mesh->sprites.reserve(count);
    r_mesh->sprites.setError(0x0);

    for (i = 0; i < count; ++i)
    {
        sprite_t *sprite = new sprite_t;

        m_tombraider.getRoomSprite(index, i,
                10.0f, &sprite->texture, sprite->pos,
                spriteVertices, spriteTexCoords);

        sprite->texture += m_texOffset; // OpenRaider preloads some textures

        sprite->vertex[0].pos[0] = spriteVertices[0];
        sprite->vertex[0].pos[1] = spriteVertices[1];
        sprite->vertex[0].pos[2] = spriteVertices[2];
        sprite->vertex[1].pos[0] = spriteVertices[3];
        sprite->vertex[1].pos[1] = spriteVertices[4];
        sprite->vertex[1].pos[2] = spriteVertices[5];
        sprite->vertex[2].pos[0] = spriteVertices[6];
        sprite->vertex[2].pos[1] = spriteVertices[7];
        sprite->vertex[2].pos[2] = spriteVertices[8];
        sprite->vertex[3].pos[0] = spriteVertices[9];
        sprite->vertex[3].pos[1] = spriteVertices[10];
        sprite->vertex[3].pos[2] = spriteVertices[11];

        sprite->texel[0].st[0] = spriteTexCoords[0];
        sprite->texel[0].st[1] = spriteTexCoords[1];
        sprite->texel[1].st[0] = spriteTexCoords[2];
        sprite->texel[1].st[1] = spriteTexCoords[3];
        sprite->texel[2].st[0] = spriteTexCoords[4];
        sprite->texel[2].st[1] = spriteTexCoords[5];
        sprite->texel[3].st[0] = spriteTexCoords[6];
        sprite->texel[3].st[1] = spriteTexCoords[7];

        r_mesh->sprites.pushBack(sprite);
    }

    gWorld.addRoom(r_mesh);

    rRoom->room = r_mesh;
    m_render.addRoom(rRoom);

    printf(".");
    fflush(stdout);
}


void OpenRaider::consoleCommand(char *cmd)
{
    if (!cmd || !cmd[0])
        return;

    // Mongoose 2003.05.31, Strip off decp console prefix
    if (cmd[0] == '>')
    {
        rc_command(">", cmd);
    }

    if (rc_command("quit", cmd))
    {
        shutdown(0);
    }
    else if (rc_command("port", cmd))
    {
        Network &net = *Network::Instance();
        net.setPort(atoi(cmd));
    }
    else if (rc_command("killserver", cmd))
    {
        Network &net = *Network::Instance();


        print(true, "Stopping network server...");
        net.killServerThread();
    }
    else if (rc_command("server", cmd))
    {
        Network &net = *Network::Instance();


        print(true, "Starting network server...");
        net.spawnServerThread();
    }
    else if (rc_command("disconnect", cmd))
    {
        Network &net = *Network::Instance();


        print(true, "Diconnecting...");
        net.killClientThread();
    }
    else if (rc_command("connect", cmd))
    {
        Network &net = *Network::Instance();


        print(true, "Connecting to %s...", cmd);

        net.setRemoteHost(cmd);
        net.spawnClientThread();
    }
    else if (rc_command("fly", cmd))
    {
        if (LARA)
        {
            LARA->moveType = worldMoveType_fly;
        }

        print(false, "World clipping is [SPACEY]");
    }
    else if (rc_command("walk", cmd))
    {
        if (LARA)
        {
            LARA->moveType = worldMoveType_walk;
        }

        print(false, "World clipping is [ON]");
    }
    else if (rc_command("ghost", cmd))
    {
        if (LARA)
        {
            LARA->moveType = worldMoveType_noClipping;
        }

        print(false, "World clipping is [OFF]");
    }
    else if (rc_command("mem", cmd))
    {
#ifdef DEBUG_MEMORY
        if (rc_command("usage", cmd))
        {
            display_memory_usage();
        }
        else if (rc_command("report", cmd))
        {
            dump_memory_report();
        }
#else
        printf("This build isn't DEBUG_MEMORY enabled\n");
#endif
    }
    else if (rc_command("loadlevel", cmd))
    {
        loadLevel(cmd);
    }
    else if (rc_command("play", cmd))
    {
        if (m_flags & OpenRaider_EnableSound)
        {
            mSound.play(atoi(cmd));
        }
    }
    else if (rc_command("sensitivity.x", cmd))
    {
        m_mouseX = static_cast<float>(atof(cmd));
    }
    else if (rc_command("sensitivity.y", cmd))
    {
        m_mouseY = static_cast<float>(atof(cmd));
    }
    else if (rc_command("r_pigtails", cmd))
    {
        if (gLaraModel)
        {
            gLaraModel->pigtails = atoi(cmd);

            if (gLaraModel->pigtails)
            {
                gLaraModel->ponyOff -= 20;
                gLaraModel->ponytail[1] -= 32;
            }
            else
            {
                gLaraModel->ponyOff += 20;
                gLaraModel->ponytail[1] += 32;
            }
        }
    }
    else if (rc_command("r_ponyangle", cmd))
    {
        if (gLaraModel)
        {
            gLaraModel->ponytailAngle = atoi(cmd);
        }
    }
    else if (rc_command("r_ponyx", cmd))
    {
        if (gLaraModel)
        {
            gLaraModel->ponytail[0] = atoi(cmd);
        }
    }
    else if (rc_command("r_ponyy", cmd))
    {
        if (gLaraModel)
        {
            gLaraModel->ponytail[1] = atoi(cmd);
        }
    }
    else if (rc_command("r_ponyz", cmd))
    {
        if (gLaraModel)
        {
            gLaraModel->ponytail[2] = atoi(cmd);
        }
    }
    else if (rc_command("r_animate", cmd))
    {
        if (atoi(cmd))
        {
            m_render.setFlags(Render::fAnimateAllModels);
            print(false, "Animating all models");
        }
        else
        {
            m_render.clearFlags(Render::fAnimateAllModels);
            print(false, "No longer animating all models");
        }
    }
    else if (rc_command("r_ponytail", cmd))
    {
        if (atoi(cmd))
        {
            m_render.setFlags(Render::fRenderPonytail);
            print(false, "Rendering ponytail");
        }
        else
        {
            m_render.clearFlags(Render::fRenderPonytail);
            print(false, "No longer rendering ponytail");
        }
    }
    else if (rc_command("r_light", cmd))
    {
        if (atoi(cmd))
        {
            m_render.setFlags(Render::fGL_Lights);
        }
        else
        {
            m_render.clearFlags(Render::fGL_Lights);
        }
    }
    else if (rc_command("hop", cmd))
    {
        if (atoi(cmd))
        {
            gWorld.setFlag(World::fEnableHopping);
            print(true, "Room hopping is on");
        }
        else
        {
            gWorld.clearFlag(World::fEnableHopping);
            print(true, "Room hopping is off");
        }
    }
    else if (rc_command("r_fog", cmd))
    {
        if (atoi(cmd))
        {
            m_render.setFlags(Render::fFog);
        }
        else
        {
            m_render.clearFlags(Render::fFog);
        }
    }
    else if (rc_command("wireframe", cmd))
    {
        m_render.setMode(Render::modeWireframe);
        print(false, "wireframe mode");
    }
    else if (rc_command("solid", cmd))
    {
        m_render.setMode(Render::modeSolid);
        print(false, "solid mode");
    }
    else if (rc_command("texture", cmd))
    {
        m_render.setMode(Render::modeTexture);
        print(false, "texture mode");
    }
    else if (rc_command("vertexlight", cmd))
    {
        m_render.setMode(Render::modeVertexLight);
        print(false, "vertexlight mode");
    }
    else if (rc_command("titlescreen", cmd))
    {
        m_render.setMode(Render::modeLoadScreen);
        print(false, "titlescreen mode");
    }
    else if (rc_command("r_viewmodel", cmd))
    {
        if (LARA)
        {
            SkeletalModel *smdl = static_cast<SkeletalModel *>(LARA->tmpHook);
            skeletal_model_t *mdl = gWorld.getModel(atoi(cmd));

            if (smdl)
            {
                smdl->setModel(mdl);
            }
        }

        //m_render.ViewModel(LARA, atoi(cmd));
    }
    else if (rc_command("r_oneroom", cmd))
    {
        if (atoi(cmd))
        {
            m_render.setFlags(Render::fOneRoom);
        }
        else
        {
            m_render.clearFlags(Render::fOneRoom);
        }
    }
    else if (rc_command("r_allrooms", cmd))
    {
        if (atoi(cmd))
        {
            m_render.setFlags(Render::fAllRooms);
        }
        else
        {
            m_render.clearFlags(Render::fAllRooms);
        }

        print(true, "Rendering all rooms [%s]", (atoi(cmd) == 0) ? "off" : "on");
    }
    else if (rc_command("r_sprite", cmd))
    {
        if (atoi(cmd))
        {
            m_render.setFlags(Render::fSprites);
        }
        else
        {
            m_render.clearFlags(Render::fSprites);
        }
    }
    else if (rc_command("r_roommodel", cmd))
    {
        if (atoi(cmd))
        {
            m_render.setFlags(Render::fRoomModels);
        }
        else
        {
            m_render.clearFlags(Render::fRoomModels);
        }
    }
    else if (rc_command("r_entmodel", cmd))
    {
        if (atoi(cmd))
        {
            m_render.setFlags(Render::fEntityModels);
        }
        else
        {
            m_render.clearFlags(Render::fEntityModels);
        }
    }
    else if (rc_command("r_particle", cmd))
    {
        if (atoi(cmd))
        {
            m_render.setFlags(Render::fParticles);
        }
        else
        {
            m_render.clearFlags(Render::fParticles);
        }
    }
    else if (rc_command("r_vis", cmd))
    {
        if (atoi(cmd))
        {
            m_render.setFlags(Render::fUsePortals);
        }
        else
        {
            m_render.clearFlags(Render::fUsePortals);
        }
    }
    else if (rc_command("r_upf", cmd))
    {
        if (atoi(cmd))
        {
            m_render.setFlags(Render::fUpdateRoomListPerFrame);
        }
        else
        {
            m_render.clearFlags(Render::fUpdateRoomListPerFrame);
        }
    }
    else if (rc_command("r_portal", cmd))
    {
        if (atoi(cmd))
        {
            m_render.setFlags(Render::fPortals);
        }
        else
        {
            m_render.clearFlags(Render::fPortals);
        }
    }
    else if (rc_command("r_vmodel", cmd))
    {
        if (atoi(cmd))
        {
            m_render.setFlags(Render::fViewModel);
        }
        else
        {
            m_render.clearFlags(Render::fViewModel);
        }
    }
    else if (rc_command("r_ralpha", cmd))
    {
        if (atoi(cmd))
        {
            m_render.setFlags(Render::fRoomAlpha);
        }
        else
        {
            m_render.clearFlags(Render::fRoomAlpha);
        }
    }
    else if (rc_command("resize", cmd))
    {
        if (rc_command("xga", cmd))
        {
            resize(1024, 768);
            m_render.Update(1024, 768);
        }
        else if (rc_command("svga", cmd))
        {
            resize(800, 600);
            m_render.Update(800, 600);
        }
        else if (rc_command("vga", cmd))
        {
            resize(640, 460);
            m_render.Update(640, 460);
        }
    }
    else if (rc_command("sshot", cmd))
    {
        char sfilename[1024];
        char *tmp = fullPath("~/.OpenRaider/sshots/", '/');

        /* Not very pretty, but gets it done */
        snprintf(sfilename, 1024, "%s%s", tmp, VERSION);
        delete [] tmp;
        m_render.screenShot(sfilename);
        print(false, "Took screenshot");
    }
    else if (rc_command("fullscreen", cmd))
    {
        toggleFullscreen();
    }
    else if (rc_command("showfps", cmd))
    {
        m_flags |= OpenRaider_ShowFPS;

        if (!atoi(cmd))
        {
            m_flags ^= OpenRaider_ShowFPS;
        }
    }
    else if (rc_command("bind", cmd))
    {
        if (rc_command("+forward", cmd))
        {
            bindKeyCommand("+forward", atoi(cmd), OpenRaiderKey_forward);
        }
        else if (rc_command("+console", cmd))
        {
            bindKeyCommand("+console", atoi(cmd), OpenRaiderKey_console);
        }
        else if (rc_command("+backward", cmd))
        {
            bindKeyCommand("+backward", atoi(cmd), OpenRaiderKey_backward);
        }
        else if (rc_command("+jump", cmd))
        {
            bindKeyCommand("+jump", atoi(cmd), OpenRaiderKey_jump);
        }
        else if (rc_command("+crouch", cmd))
        {
            bindKeyCommand("+crouch", atoi(cmd), OpenRaiderKey_crouch);
        }
        else if (rc_command("+right", cmd))
        {
            bindKeyCommand("+right", atoi(cmd), OpenRaiderKey_right);
        }
        else if (rc_command("+left", cmd))
        {
            bindKeyCommand("+left", atoi(cmd), OpenRaiderKey_left);
        }
        else if (rc_command("+attack", cmd))
        {
            bindKeyCommand("+attack", atoi(cmd), OpenRaiderKey_attack);
        }
    }
    else if (rc_command("set", cmd))
    {
        if (rc_command("mousegrab", cmd))
        {
            setGrabMouse(atoi(cmd));
            print(true, "Mouse grabbing [%s]", atoi(cmd) ? "on" : "off");
        }
    }
    else if (rc_command("stat", cmd))
    {
        if (rc_command("fps", cmd))
        {
            m_flags ^= OpenRaider_ShowFPS;
        }
        else if (rc_command("pos", cmd))
        {
            if (LARA)
            {
                print(true, "Room %2i  Pos %.0f %.0f %.0f  Yaw %.0f  Pitch %.0f",
                        LARA->room,
                        LARA->pos[0], LARA->pos[1], LARA->pos[2],
                        HEL_RAD_TO_DEG(LARA->angles[1]),
                        HEL_RAD_TO_DEG(LARA->angles[2]));
            }
        }
        else if (rc_command("room", cmd))
        {
            if (rc_command("flags", cmd))
            {
                if (LARA)
                {
                    print(true, "Room[%i] flags: 0x%x",
                            LARA->room,
                            gWorld.getRoomInfo(LARA->room));
                }
            }
        }
    }
}


void OpenRaider::handleCommand(char *cmd, unsigned int mode)
{
    bool b;
    int i;


    // So we can use switch stmt, translate the Ids
    mode = mMode[mode];

    switch (mode)
    {
        case 0: // [Video.OpenGL]
            if (rc_command("Width", cmd))
            {
                m_width = atoi(cmd);
            }
            else if (rc_command("Height", cmd))
            {
                m_height = atoi(cmd);
            }
            else if (rc_command("FastCard", cmd))
            {
                rc_get_bool(cmd, &m_fastCard);
            }
            else if (rc_command("FullScreen", cmd))
            {
                rc_get_bool(cmd, &b);

                m_flags |= OpenRaider_FullScreen;

                if (!b)
                {
                    m_flags ^= OpenRaider_FullScreen;
                }
            }
            else if (rc_command("Font", cmd))
            {
                gFontFilename = fullPath(cmd, '/');
            }
            else if (rc_command("Driver", cmd))
            {
                if (cmd[0])
                {
                    setDriverGL(cmd);
                }
            }
            else
            {
                printf("Command> [Video.OpenGL] Unknown command '%s'\n", cmd);
            }
            break;
        case 1: // [Audio.OpenAL]
            if (rc_command("Enable", cmd))
            {
                rc_get_bool(cmd, &b);

                m_flags |= OpenRaider_EnableSound;

                if (!b)
                {
                    m_flags ^= OpenRaider_EnableSound;
                }
            }
            else
            {
                printf("Command> [Audio.OpenAL] Unknown command '%s'\n", cmd);
            }
            break;
        case 2: // [OpenRaider.Engine]
            if (rc_command("PakDir", cmd))
            {
                if (m_pakDir)
                {
                    delete [] m_pakDir;
                }

                m_pakDir = fullPath(cmd, '/');
            }
            else if (rc_command("HomeDir", cmd))
            {
                if (m_homeDir)
                {
                    delete [] m_homeDir;
                }

                i = strlen(cmd);

                m_homeDir = fullPath(cmd, '/');
            }
            else if (rc_command("AudioDir", cmd))
            {
                if (m_audioDir)
                {
                    delete [] m_audioDir;
                }

                m_audioDir = fullPath(cmd, '/');
            }
            else if (rc_command("MapDebug", cmd))
            {
                rc_get_bool(cmd, &b);

                m_flags |= OpenRaider_DebugMap;

                if (!b)
                {
                    m_flags ^= OpenRaider_DebugMap;
                }
            }
            else if (rc_command("Map", cmd))
            {
                if (cmd[0])
                {
                    char *fullPathMap;


                    fullPathMap = bufferString("%s%s", m_pakDir, cmd);

                    if (m_tombraider.checkMime(fullPathMap) == 0)
                    {
                        printf("Validated pak: '%s'\n",
                                fullPathMap);
                        delete [] fullPathMap;

                        /* Just load relative filename */
                        mMapList.pushBack(bufferString("%s", cmd));
                    }
                    else
                    {
                        printf("ERROR: pak file '%s' not found or invalid\n",
                                fullPathMap);

                        delete [] fullPathMap;
                    }
                }
            }
            // Mongoose 2001.12.31, Added music list back
            else if (rc_command("Music", cmd))
            {
                if (cmd[0])
                {
                    char *music;

                    i = strlen(cmd);
                    music = new char[i+1];
                    strncpy(music, cmd, i);
                    music[i] = 0;

                    mMusicList.pushBack(music);
                }
            }
            else if (rc_command("DisplayFPS", cmd))
            {
                rc_get_bool(cmd, &b);

                m_flags |= OpenRaider_ShowFPS;

                if (!b)
                {
                    m_flags ^= OpenRaider_ShowFPS;
                }
            }
            else if (rc_command("ModelDebug", cmd))
            {
                rc_get_bool(cmd, &b);

                m_flags |= OpenRaider_DebugModel;

                if (!b)
                {
                    m_flags ^= OpenRaider_DebugModel;
                }
            }
            else if (rc_command("DumpTexture", cmd))
            {
                rc_get_bool(cmd, &b);

                m_flags |= OpenRaider_DumpTexture;

                if (!b)
                {
                    m_flags ^= OpenRaider_DumpTexture;
                }
            }
            else
            {
                printf("Command> [OpenRaider.Engine] Unknown command '%s'\n", cmd);
            }
            break;
        case 3: // [Input.Mouse]
            if (rc_command("SensitivityX", cmd))
            {
                m_mouseX = static_cast<float>(atof(cmd));
            }
            else if (rc_command("SensitivityY", cmd))
            {
                m_mouseY = static_cast<float>(atof(cmd));
            }
            else
            {
                printf("Command> [Input.Mouse] Unknown command '%s'\n", cmd);
            }
            break;
        case 4:
            if (cmd[1] == '.')
            {
                cmd[0] = '>';
                cmd[1] = ' ';

                for (i = strlen(cmd) - 1; i >= 0; --i)
                {
                    if (cmd[i] == '.' || cmd[i] == '=')
                    {
                        cmd[i] = ' ';
                    }
                }
            }

            consoleCommand(cmd);
            break;
        case 5:
            if (rc_command("Enable", cmd))
            {
                rc_get_bool(cmd, &gStartServer);
            }
            else if (rc_command("Port", cmd))
            {
                if (gStartServer)
                {
                    Network &net = *Network::Instance();
                    int port = atoi(cmd);

                    net.setPort(port);
                    print(true, "Starting network server on port %i...", port);
                    net.spawnServerThread();
                }
            }
            else
            {
                printf("Command> [Network.Server] '%s' not implemented\n", cmd);
            }
            break;
    }
}


int main(int argc, char *argv[]) {
    if (argc != 1) {
        printf("Usage:\n\t%s\n", argv[0]);
        return 1;
    }

    OpenRaider *game = OpenRaider::Instance();
    atexit(killOpenRaiderSingleton);
    game->start();

    return 0;
}
