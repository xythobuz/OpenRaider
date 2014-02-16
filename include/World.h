/*!
 * \file include/World.h
 * \brief The game world (model)
 *
 * \author Mongoose
 */

#ifndef _WORLD_H_
#define _WORLD_H_

#define BAD_BLOOD  //!< \todo For temp rendering use

#ifdef BAD_BLOOD
#include <SkeletalModel.h>
#endif

#include <List.h>
#include <Vector.h>
#include <MatMath.h>

// Mirrors TombRaider class' room flags really
typedef enum {
    roomFlag_underWater    = 0x0001
} room_flags_t;

typedef enum {
    worldMoveType_walkNoSwim   = -1,
    worldMoveType_walk         = 0,
    worldMoveType_noClipping   = 1,
    worldMoveType_fly          = 2,
    worldMoveType_swim         = 3
} worldMoveType;

typedef struct {
    vec3_t pos;
} vertex_t;

/*
typedef struct {
    vec2_t uv;
} uv_t;

typedef struct {
    vec4_t rgba;
} color_t;
*/

typedef struct {
    vec2_t st;
} texel_t;

typedef struct {
    int num_verts;      //!< 4 == Quad, 3 == Triangle, rendered as triangles
    vertex_t vertex[4];
    texel_t texel[4];
    float pos[3];
    float radius;       //!< \fixme yeah, I know
    int texture;
} sprite_t;

typedef struct {
    int num_sprites;
    sprite_t *sprite;
} sprite_seq_t;

/*! \fixme For now shaders are textures on tex objects
 * and materials on color objects. If -1
 * then it doesn't have that information yet.
 */
typedef struct {
    int index[3];
    vec_t st[6];
    int texture;
    unsigned short transparency;
} texture_tri_t;

typedef struct {
    Vector<texture_tri_t *> texturedTriangles;
    Vector<texture_tri_t *> coloredTriangles;
    Vector<texture_tri_t *> texturedRectangles;
    Vector<texture_tri_t *> coloredRectangles;

    vec3_t center;
    float radius;

    unsigned int vertexCount;
    vec_t *vertices;

    unsigned int colorCount;
    vec_t *colors;

    unsigned int normalCount;
    vec_t *normals;
} model_mesh_t;

typedef struct entity_s {
    int id;                  //!< Unique identifier
    float pos[3];            //!< World position
    float angles[3];         //!< Euler angles (pitch, yaw, roll)
    int type;                //!< {(0x00, item), (0x01, ai), (0x02, player)}
    int room;                //!< Current room entity is in
    worldMoveType moveType;  //!< Type of motion/clipping
    bool moving;             //!< In motion?
    struct entity_s *master; //!< Part of entity chain?

    int state;               //!< State of the Player, AI, or object
    int objectId;            //!< What kind of entity?

    int modelId;             //!< Animation model
    void *tmpHook;
    bool animate;

    /*
    float time, lastTime;
    int state, lastState;
    int event, lastEvent;
    int goal;
    */
} entity_t;

typedef struct {
    int index;    //!< model_mesh index
    float yaw;    //!< angle of rotation on Y
    float pos[3]; //!< position

    //vec3_t bboxMax;
    //vec3_t bboxMin;
} static_model_t;

typedef struct {
    float vertices[4][3];
    float normal[3];
    int adjoining_room;
} portal_t;

typedef struct {
    vertex_t a;
    vertex_t b;
    vertex_t c;
    vertex_t d;
} box_t;

typedef struct {
    vec_t floor;
    vec_t ceiling;

    bool wall;
} sector_t;

//! \fixme No room mesh list or sprites and etc
typedef struct {
    Vector<int> adjacentRooms;
    Vector<portal_t *> portals;
    Vector<static_model_t *> models;
    Vector<sprite_t *> sprites;
    Vector<box_t *> boxes;
    Vector<sector_t *> sectors;

    int id;
    unsigned int flags;
    unsigned int numXSectors;
    unsigned int numZSectors;
    float pos[3];
    vec3_t bbox_min;
    vec3_t bbox_max;
} room_mesh_t;

// Workout generic entity and a client class from these entities
typedef struct world_entity_s {
    vec3_t pos;
    vec3_t lastPos;
    vec3_t angle;
    vec_t ttl;

    int type;
    int state;

    //struct world_entity_s *master;

} world_entity_t;

typedef struct {
    vec3_t pos;
    vec3_t lastPos;
    vec3_t angle;
    char clipping;
    float time, eventTime, eventTimer;
    int state, nextState;
    float health;

    // Client
    unsigned int uid;
    char name[32];
    int actor, enemy;

    // Render
    unsigned int model;
    unsigned int skin;
    unsigned int animFrame;

} actor_entity_t;

enum OpenRaiderEvent {
    eNone = 0,
    eWeaponDischarge,
    eDying,
    eDead,
    eWounded,
    eRunForward,
    eRunBackward,
    eJump,
    eCrouchWalk,
    eIdle,
    eTaunt,
    eTurn,
    eRespawn,
    eLand
};

/*!
 * \brief The game world (model)
 */
class World {
public:

    enum WorldFlag {
        fEnableHopping = 1
    };

    /*!
     * \brief Constructs an object of World
     */
    World();

    /*!
     * \brief Deconstructs an object of World
     */
    ~World();

    /*!
     * \brief Find room a location is in.
     *
     * If it fails to be in a room it gives closest overlapping room.
     * \param index Guessed room index
     * \param x X coordinate
     * \param y Y coordinate
     * \param z Z coordinate
     * \returns correct room index or -1 for unknown
     */
    int getRoomByLocation(int index, float x, float y, float z);

    /*!
     * \brief Find room a location is in.
     *
     * If it fails to be in a room it gives closest overlapping room.
     * \param x X coordinate
     * \param y Y coordinate
     * \param z Z coordinate
     * \returns correct room index or -1 for unknown
     */
    int getRoomByLocation(float x, float y, float z);

    /*!
     * \brief Looks for portal crossings from xyz to xyz2 segment
     * from room[index]
     * \param index valid room index
     * \param x X coordinate of first point
     * \param y Y coordinate of first point
     * \param z Z coordinate of first point
     * \param x2 X coordinate of second point
     * \param y2 Y coordinate of second point
     * \param z2 Z coordinate of second point
     * \returns index of adjoined room or -1
     */
    int getAdjoiningRoom(int index,
                            float x, float y, float z,
                            float x2, float y2, float z2);

    /*!
     * \brief Gets the sector index of the position in room
     * \param room valid room index
     * \param x X coordinate in room
     * \param z Z coordinate in room
     * \returns sector index of position in room
     */
    int getSector(int room, float x, float z);
    int getSector(int room, float x, float z, float *floor, float *ceiling);

    unsigned int getRoomInfo(int room);

    /*!
     * \brief Check if sector is a wall
     * \param room valid room index
     * \param sector valid sector index
     * \returns true if this sector is a wall
     */
    bool isWall(int room, int sector);

    /*!
     * \brief Get the world height at a position
     * \param index valid room index
     * \param x X coordinate
     * \param y will be set to world height in that room
     * \param z Z coordinate
     * \returns true if position is in a room
     */
    bool getHeightAtPosition(int index, float x, float *y, float z);

#ifdef BAD_BLOOD
    //! \todo Temp methods for rendering use until more refactoring is done
    model_mesh_t *getMesh(int index);
    skeletal_model_t *getModel(int index);
    room_mesh_t *getRoom(int index);
    Vector<entity_t *> *getEntities();
    Vector<sprite_seq_t *> *getSprites();
    Vector<room_mesh_t *> *getRooms();
#endif

    /*!
     * \brief Set an option flag
     * \param flag flag to set
     */
    void setFlag(WorldFlag flag);

    /*!
     * \brief Clear an option flag
     * \param flag flag to clear
     */
    void clearFlag(WorldFlag flag);

    /*!
     * \brief Clears all data in world
     * \todo in future will check if data is in use before clearing
     */
    void destroy();

    /*!
     * \brief Adds room to world
     * \param room room to add
     */
    void addRoom(room_mesh_t *room);

    /*!
     * \brief ADds mesh to world
     * \param model mesh to add
     */
    void addMesh(model_mesh_t *model);

    /*!
     * \brief Adds entity to world
     * \param e entity to add
     */
    void addEntity(entity_t *e);

    /*!
     * \brief Adds model to world.
     * \param model model to add
     * \returns next model ID or -1 on error
     */
    int addModel(skeletal_model_t *model);

    /*!
     * \brief Adds sprite to world
     * \param sprite sprite to add
     */
    void addSprite(sprite_seq_t *sprite);

    /*!
     * \brief Move entity in given direction unless collision occurs
     * \param e entity to move
     * \param movement direction of movement ('f', 'b', 'l' or 'r')
     */
    void moveEntity(entity_t *e, char movement);

private:

    /*!
     * \brief Clears all data in world
     */
    void clear();

    bool mClearLock;
    unsigned int mFlags;                //!< World flags
    Vector<entity_t *> mEntities;       //!< World entities
    Vector<room_mesh_t *> mRooms;       //!< Map data and meshes
    Vector<model_mesh_t *> mMeshes;     //!< Unanimated meshes
    Vector<sprite_seq_t *> mSprites;    //!< Sprites
    Vector<skeletal_model_t *> mModels; //!< Skeletal animation models
};

#endif
