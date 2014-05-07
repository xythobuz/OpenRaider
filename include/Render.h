/*!
 * \file include/Render.h
 * \brief OpenRaider Renderer class
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _RENDER_H_
#define _RENDER_H_

#include <vector>

#include "config.h"
#include "math/Matrix.h"
#include "ViewVolume.h"
#include "World.h"
#include "SkeletalModel.h"
#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"
#include "RoomData.h"

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
        fRoomAlpha              = (1 << 0),
        fViewModel              = (1 << 1),
        fEntityModels           = (1 << 2),
        fFog                    = (1 << 3),
        fUsePortals             = (1 << 4),
        fGL_Lights              = (1 << 5),
        fOneRoom                = (1 << 6),
        fRenderPonytail         = (1 << 7),
        // fMultiTexture           = (1 << 8), //! \todo Whats up with Multitexture stuff? Where is it needed?
        fUpdateRoomListPerFrame = (1 << 9),
        fAnimateAllModels       = (1 << 10),
        fAllRooms               = (1 << 11)
    } RenderFlags;

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
     * \brief Sets up textures for OpenRaider
     * \param textureDir Is valid and exists with textures
     * \returns number of loaded textures
     */
    int initTextures(char *textureDir);

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

    /*!
     * \brief Renders a single game frame
     */
    void display();

    void setSkyMesh(int index, bool rot);

    void ViewModel(entity_t *ent, int index);

    void addSkeletalModel(SkeletalModel *mdl);

    unsigned int getFlags();

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

    bool isVisible(BoundingBox &box);

    /*!
     * \brief Renders a mesh.
     *
     * Texture must be initialized.
     * \param r_mesh Mesh to render.
     */
    void drawModelMesh(model_mesh_t *r_mesh);

    //! \fixme should be private

    ViewVolume mViewVolume; //!< View Volume for frustum culling
    std::vector<SkeletalModel *> mModels;
    Texture mTexture;                     //!< Texture subsystem

private:

    void drawLoadScreen();

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
    void buildRoomRenderList(Room &room);

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
     * \brief Updates View Volume. Call once per render frame.
     */
    void updateViewVolume();

    //! \fixme Let them eat cake...? O.o
    void tmpRenderModelMesh(model_mesh_t *r_mesh, texture_tri_t *ttri);

    std::vector<Room *> mRoomRenderList;

    unsigned int mFlags;                  //!< Rendering flags
    unsigned int mMode;                   //!< Rendering mode
    int mLock;
    int mSkyMesh;                         //!< Skymesh model id
    bool mSkyMeshRotation;                //!< Should Skymesh be rotated?
};

#endif
