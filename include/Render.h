/*!
 * \file include/Render.h
 * \brief OpenRaider Renderer class
 *
 * \author Mongoose
 */

#ifndef _RENDER_H_
#define _RENDER_H_

#include "Config.h"
#include "Matrix.h"
#include "ViewVolume.h"
#include "World.h"
#include "SkeletalModel.h"
#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"
#include "GLString.h"
#include "templates/Vector.h"

#ifdef USING_EMITTER
#include "Emitter.h"
#endif

/*!
 * \brief The GL light class
 */
class Light {
public:
    /*!
     * \brief Type a light can be of
     */
    typedef enum {
        typePoint       = 1, //!< Point light
        typeSpot        = 2, //!< Spot light
        typeDirectional = 3  //!< Directional light
    } LightType;

    vec4_t mPos; //! Light position in 3 space
    vec3_t mDir; //! Light direction
    float mAtt;
    vec4_t mColor; //! Color of light
    vec_t mCutoff; //! Fade out distance
    LightType mType; //! Type of light
};

/*!
 * \brief RenderRoom used by Renderer
 */
class RenderRoom {
public:

    /*!
     * \brief Constructs an object of RenderRoom
     */
    RenderRoom() {
        room = 0x0;
        dist = 0.0f;
    }

    /*!
     * \brief Deconstructs an object of RenderRoom
     */
    ~RenderRoom() {
        //! \fixme Hangs when erasing - might be shared pointers somewhere
        //lights.erase();
    }

    vec_t dist;             //!< Distance to near plane, move to room?
    Vector<Light *> lights; //!< List of lights in this room
    Mesh mesh;              //!< OpenGL mesh that represents this room

    //! \fixme very dangerous as allocated and used
    room_mesh_t *room;      //!< Physical room stored in World
};

/*!
 * \brief OpenRaider Renderer class
 */
class Render {
public:

    typedef enum {
        modeDisabled,
        modeLoadScreen,
        modeVertexLight,
        modeSolid,
        modeWireframe,
        modeTexture
    } RenderMode;

    typedef enum {
        fParticles              = (1 << 0),
        fPortals                = (1 << 1),
        fRoomAlpha              = (1 << 2),
        fViewModel              = (1 << 3),
        fSprites                = (1 << 4),
        fRoomModels             = (1 << 5),
        fEntityModels           = (1 << 6),
        fFog                    = (1 << 7),
        fUsePortals             = (1 << 8),
        fGL_Lights              = (1 << 9),
        fOneRoom                = (1 << 10),
        fRenderPonytail         = (1 << 11),
        fMultiTexture           = (1 << 12),
        fUpdateRoomListPerFrame = (1 << 13),
        fAnimateAllModels       = (1 << 14),
        fAllRooms               = (1 << 15)
    } RenderFlags;

    typedef enum {
        roomMesh,
        skeletalMesh
    } RenderMeshType;

    /*!
     * \brief Constructs an object of Render
     */
    Render();

    /*!
     * \brief Deconstructs an object of Render
     */
    ~Render();

    /*!
     * \brief Makes a screenshot, writes to disk
     * \param filenameBase basename of file to be written
     */
    void screenShot(char *filenameBase);

    /*!
     * \brief Gets current rendering mode
     * \returns current RenderMode
     * \fixme Don't return enum as int?!
     */
    int getMode();

    void addRoom(RenderRoom *rRoom);

    /*!
     * \brief Starts and sets up OpenGL target
     * \param width width of window
     * \param height height of window
     */
    void Init(int width, int height);

    /*!
     * \brief Loads textures in a certain id slot
     * \param image Image to load
     * \param width width of image
     * \param height height of image
     * \param id id for texture
     * \sa Texture::loadBufferSlot()
     */
    void loadTexture(unsigned char *image,
                          unsigned int width, unsigned int height,
                          unsigned int id);

    /*!
     * \brief Sets up textures for OpenRaider.
     * \param textureDir Is valid and exists with textures
     * \param numLoaded returns number of loaded textures and will update
     * number of external textures loaded
     * \param nextId will update next level texture id
     */
    void initTextures(char *textureDir, unsigned int *numLoaded,
                            unsigned int *nextId);

    /*!
     * \brief Initializes Emitter.
     *
     * Emitter is set up for rendering with 2 textures in
     * a overhead rain or snow like pattern.
     * Textures have to be initialized!
     * \param name valid C-String name
     * \param size valid size
     * \param snowTex1 valid first texture
     * \param snowTex2 valid second texture
     */
    void initEmitter(const char *name, unsigned int size,
                          unsigned int snowTex1, unsigned int snowTex2);

    /*!
     * Removes current world/entity/etc geometry
     */
    void ClearWorld();

    /*!
     * \brief Clears bitflags, changes state of renderer in some way
     * \param flags RenderFlags to clear (ORed)
     * \fixme use enum not integer as parameter?!
     */
    void clearFlags(unsigned int flags);

    /*!
     * \brief Sets bitflags, changes state of renderer in some way
     * \param flags RenderFlags to set (ORed)
     * \fixme use enum not integer as parameter?!
     */
    void setFlags(unsigned int flags);

    void setMode(int n);

    void Update(int width, int height);

    /*!
     * \brief Renders a single game frame
     */
    void Display();

    void setSkyMesh(int index, bool rot);

    void ViewModel(entity_t *ent, int index);

    void RegisterCamera(Camera *camera);

    /*!
     * \brief Get the GL text output agent
     * \returns Used GLString instance
     */
    GLString *GetString();

    /*!
     * \brief Renders load screen.
     *
     * Textures must be initialized.
     * \fixme Should be private!
     */
    void drawLoadScreen();

    void addSkeletalModel(SkeletalModel *mdl);

private:

    /*!
     * \brief Check if a bounding box is in the View Volume
     * \param bboxMin Start coordinates of a valid bounding box
     * \param bboxMax End coordinates of a valid bounding box
     * \returns true if bounding box is visible
     */
    bool isVisible(float bboxMin[3], float bboxMax[3]);

    /*!
     * \brief Check if a point is in the View Volume
     * \param x X coordinate
     * \param y Y coordinate
     * \param z Z coordinate
     * \returns true if point is visible
     */
    bool isVisible(float x, float y, float z);

    /*!
     * \brief Check if a sphere is in the View Volume
     * \param x X coordinate of center of sphere
     * \param y Y coordinate of center of sphere
     * \param z Z coordinate of center of sphere
     * \param radius radius of sphere
     * \returns true if sphere is visible
     */
    bool isVisible(float x, float y, float z, float radius);

    /*!
     * \brief Build a visible room list starting at index
     * \param index valid room index where to start the list
     */
    void newRoomRenderList(int index);

    /*!
     * \brief Build a visible room list starting from room and
     * only considers its linked rooms and their linked rooms.
     * \param room First room in list
     */
    void buildRoomRenderList(RenderRoom *room);

    /*!
     * \brief Renders visible world object.
     *
     * Texture must be initialized.
     */
    void drawObjects();

    /*!
     * \brief Renders Sky domes/boxes/etc by scaling factor.
     *
     * Texture must be initialized.
     * \param scale correct scale for map size
     */
    void drawSkyMesh(float scale);

    /*!
     * \brief Renders a skeletal model.
     *
     * Texture must be initialized!
     * \param model model to render
     */
    void drawModel(SkeletalModel *model);

    /*!
     * Renders a room in 2 seperate passes to handle alpha.
     *
     * Currently doesnt sort alpha but looks pretty good.
     * Texture must be initialized.
     * Draw all rooms with alpha false, then again with alpha true.
     * \param rRoom room to render
     * \param draw_alpha once false, once true
     */
    void drawRoom(RenderRoom *rRoom, bool draw_alpha);

    /*!
     * \brief Renders static room model.
     *
     * Texture must be initialized.
     * \param mesh Static model to render
     */
    void drawRoomModel(static_model_t *mesh);

    /*!
     * \brief Renders a mesh.
     *
     * Texture must be initialized.
     * \param r_mesh Mesh to render.
     * \param type Must be object containing mesh
     */
    void drawModelMesh(model_mesh_t *r_mesh, RenderMeshType type);

    /*!
     * \brief Renders a sprite.
     *
     * Texture must be initialized.
     * \param sprite sprite to render
     */
    void drawSprite(sprite_t *sprite);

    /*!
     * \brief Updates View Volume. Call once per render frame.
     */
    void updateViewVolume();

    //! \fixme Let them eat cake...? O.o
    void tmpRenderModelMesh(model_mesh_t *r_mesh, texture_tri_t *ttri);

    Texture mTexture;                     //!< Texture subsystem
    Camera *mCamera;                      //!< Camera subsystem
    GLString mString;                     //!< GL Text subsystem
    Vector<RenderRoom *> mRoomRenderList;
    Vector<RenderRoom *> mRooms;
    Vector<SkeletalModel *> mModels;
    unsigned int mFlags;                  //!< Rendering flags
    unsigned int mWidth;                  //!< Viewport width
    unsigned int mHeight;                 //!< Viewport height
    unsigned int mMode;                   //!< Rendering mode
    unsigned int *mNumTexturesLoaded;
    unsigned int *mNextTextureId;
    int mLock;
    int mSkyMesh;                         //!< Skymesh model id
    bool mSkyMeshRotation;                //!< Should Skymesh be rotated?
#ifdef USING_EMITTER
    Emitter *mEmitter;                    //!< Particle emitter test
#endif
};

#endif
