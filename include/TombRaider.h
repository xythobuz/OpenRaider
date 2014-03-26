/*!
 * \file include/TombRaider.h
 * \brief Loads maps, meshes, textures...
 *
 * \todo WARNING: No endian routines as of yet
 * \author Mongoose
 */

#ifndef _TOMBRAIDER_H_
#define _TOMBRAIDER_H_

#include <cstdint>

#include "TombRaiderData.h"

/*!
 * \brief Loads maps, meshes, textures...
 */
class TombRaider {
public:

    /*!
     * \brief Constructs an object of TombRaider
     */
    TombRaider();

    /*!
     * \brief Deconstructs an object of TombRaider
     */
    ~TombRaider();

    ////////////////////////////////////////
    // Wash me -- not part of cleaned API //
    ////////////////////////////////////////

    int NumRooms();

    int NumMoveables();

    int NumTextures();

    /*!
     * \brief Get number of _special_ textures/images
     * \returns number of special textures/images
     */
    int NumSpecialTextures();

    int NumAnimations();

    unsigned int NumFrames();

    int NumStaticMeshes();

    int NumSprites();

    int NumSpriteSequences();

    int NumItems();

    tr2_version_type Engine();

    unsigned short *Frame();

    tr2_animation_t *Animation();

    tr2_item_t *Item();

    tr2_object_texture_t *ObjectTextures();

    /*!
     * \brief Get number of boxes
     * \returns number of boxes
     */
    unsigned int getNumBoxes();

    tr2_box_t *Box();

    tr2_mesh_t *Mesh();

    /*!
     * \brief Get number of animations for a moveable
     * \param moveable_index valid moveable id
     * \returns number of animations for specified moveable
     */
    int getNumAnimsForMoveable(int moveable_index);

    tr2_staticmesh_t *StaticMesh();

    tr2_moveable_t *Moveable();

    tr2_meshtree_t *MeshTree();

    /*!
     * \brief Get the sprites
     * \returns the sprite array
     */
    tr2_sprite_texture_t *Sprite();

    tr2_sprite_sequence_t *SpriteSequence();

    /*!
     * \brief Makes a 32bit RGBA image from a stexture/bmap
     * \param texture valid index into tex_special list
     * \returns 32bit RGBA image or NULL on error
     */
    unsigned char *SpecialTexTile(int texture);

    /*!
     * \brief Get copies of texture and it's bumpmap
     * \param texture valid textile index
     * \param image will be set to texture if found, or NULL
     * \param bumpmap will be set to bumpmap if found, or NULL
     */
    void Texture(int texture, unsigned char **image, unsigned char **bumpmap);

    unsigned int *Palette16();

    unsigned char *Palette8();

    tr2_room_t *Room();

    /*!
     * \brief Check if a file is a TombRaider pak
     * \param filename file to check
     * \returns 0 if it is a TombRaider pak
     */
    static int checkMime(char *filename);

    /*!
     * \brief Loads TombRaider 1-5 pak into memory
     * and does some processing.
     * \param filename points to valid TombRaider pak
     * \returns 0 on success, < 0 on error
     * \sa TombRaider::loadTR5()
     */
    int Load(char *filename);

    /*!
     * \brief Makes a clamped 0.0 to 1.0 texel from coord pair
     * \param texel texel value, is modified, divided by 255.0 and returned
     * \param offset if offset is negative, texel is decreased by one, else increased
     * \returns modified texel divided by 255.0
     */
    float adjustTexel(unsigned char texel, char offset);

    /*!
     * \brief Compute rotation angles from moveable animation data
     * \param frame moveable animation data
     * \param frame_offset moveable animation data
     * \param angle_offset moveable animation data
     * \param x will be set to computed angle
     * \param y will be set to computed angle
     * \param z will be set to computed angle
     */
    void computeRotationAngles(unsigned short **frame,
                                unsigned int *frame_offset,
                                unsigned int *angle_offset,
                                float *x, float *y, float *z);

    /*!
     * \brief Returns computed UV in u and v
     * \param st object texture vert, its coordinates are added to the pixels and divided by 255.0
     * \param u will contain calculated x value
     * \param v will contain calculated y value
     */
    void computeUV(tr2_object_texture_vert_t *st, float *u, float *v);

    /*!
     * \brief Get number of bump maps in loaded pak
     * \returns number of bump maps
     */
    int getBumpMapCount();

    void getColor(int index, float color[4]);

    tr2_version_type getEngine();

    /*!
     * \brief Get the collision sphere for a mesh
     * \param meshIndex mesh index
     * \param center will be filled with center coordinates, not NULL
     * \param radius will be filled with radius, not NULL
     */
    void getMeshCollisionInfo(unsigned int meshIndex,
                                float center[3], float *radius);

    /*!
     * \brief Get SIGNED mesh count (TR encoded < 0 errors)
     * \returns signed mesh count
     */
    int getMeshCount();

    /*!
     * \brief This method is made to let you fill arrays or
     * create single faces. There may be an allocatin version that
     * passes back arrays later.
     * Returns Quads/Rects as 2 triangles,
     * because returning quads would be too trivial :)
     * \param meshIndex mesh index
     * \param faceIndex face index
     * \param index allocated RGBA
     * \param color allocated RGBA
     * \fixme This method interface may change later
     */
    void getMeshColoredRectangle(unsigned int meshIndex,
                                    unsigned int faceIndex,
                                    int index[6], float color[4]);

    /*!
     * \brief This method is made to let you fill arrays or
     * create single faces. There may be an allocating version that
     * passes back arrays later.
     * \param meshIndex mesh index
     * \param faceIndex face index
     * \param index allocated RGBA
     * \param color allocated RGBA
     * \fixme This method interface may change later
     */
    void getMeshColoredTriangle(unsigned int meshIndex,
                                unsigned int faceIndex,
                                int index[3], float color[4]);

    /*!
     * \brief This method is made to let you fill arrays or
     * create single faces. There may be an allocatin version that
     * passes back arrays later.
     * Returns Quads/Rects as 2 triangles,
     * because returning quads would be too trivial :)
     * \param meshIndex mesh index
     * \param faceIndex face index
     * \param index allocated
     * \param st allocated
     * \param texture allocated
     * \param transparency allocated
     * \fixme This method interface may change later
     */
    void getMeshTexturedRectangle(unsigned int meshIndex,
                                    unsigned int faceIndex,
                                    int index[6], float st[12], int *texture,
                                    unsigned short *transparency);

    /*!
     * \brief This method is made to let you fill arrays or
     * create single faces. There may be an allocatin version that
     * passes back arrays later.
     * \param meshIndex mesh index
     * \param faceIndex face index
     * \param index allocated
     * \param st allocated
     * \param texture allocated
     * \param transparency allocated
     * \fixme This method interface may change later
     */
    void getMeshTexturedTriangle(unsigned int meshIndex,
                                    unsigned int faceIndex,
                                    int index[3], float st[6], int *texture,
                                    unsigned short *transparency);

    /*!
     * \brief Get face counts for a given mesh.
     *
     * \todo This method interface may change later...
     * \param meshIndex mesh index
     * \returns number of textured triangles in mesh
     */
    int getMeshTexturedTriangleCount(unsigned int meshIndex);

    /*!
     * \brief Get face counts for a given mesh.
     * \param meshIndex mesh index
     * \returns number of colored triangles in mesh
     */
    int getMeshColoredTriangleCount(unsigned int meshIndex);

    /*!
     * \brief Get face counts for a given mesh.
     * \param meshIndex mesh index
     * \returns number of textured rectangles in mesh
     */
    int getMeshTexturedRectangleCount(unsigned int meshIndex);

    /*!
     * \brief Get face counts for a given mesh.
     * \returns number if colored rectangles in mesh
     */
    int getMeshColoredRectangleCount(unsigned int meshIndex);

    /*!
     * \brief Get vertex, normal and color arrays for a mesh
     * \param meshIndex mesh index
     * \param vertexCount will be set to length of vertex array
     * \param verts will be set to allocated vertex array (XYX)
     * \param normalCount will be set to length of normal array
     * \param norms will be set to allocated normal array (IJK)
     * \param colorCount will be set to length of color array
     * \param colors will be set to allocated color array (I)
     */
    void getMeshVertexArrays(unsigned int meshIndex,
                                unsigned int *vertexCount, float **verts,
                                unsigned int *normalCount, float **norms,
                                unsigned int *colorCount, float **colors);

    /*!
     * \brief Get a single collision box from room (unified)
     * \param roomIndex room index
     * \param index index of box in room
     * \param xyzA will contain first corner of box
     * \param xyzB will contain second corner of box
     * \param xyzC will contain third corner of box
     * \param xyzD will contain fourth corner of box
     * \returns 0 on success, < 0 on error
     */
    int getRoomBox(unsigned int roomIndex, unsigned int index,
                    float *xyzA, float *xyzB, float *xyzC, float *xyzD);

    /*!
     * \brief Get number of collision boxes in room (unified)
     * \param roomIndex room index
     * \returns number of collision boxes in room
     */
    unsigned int getRoomBoxCount(unsigned int roomIndex);

    void getRoomInfo(unsigned int index,
                        unsigned int *flags, float pos[3],
                        float bboxMin[3], float bboxMax[3]);

    /*!
     * \brief Get a single light from a room (unified)
     * \param roomIndex valid room index
     * \param index valid light index in room
     * \param pos allocated, will contain position
     * \param color allocated, will contain color
     * \param dir allocated, will contain direction
     * \param attenuation will contain attenuation
     * \param cutoffAngle will contain cutoff angle
     * \param type will contain type
     * \param flags will contain flags
     * \returns 0 on success, < 0 on error
     */
    int getRoomLight(unsigned int roomIndex, unsigned int index,
                        float pos[4], float color[4], float dir[3],
                        float *attenuation, float *cutoffAngle,
                        unsigned int *type, unsigned int *flags);

    /*!
     * \brief Get number of lights in room (unified)
     * \param roomIndex room index
     * \returns number of lights in room
     */
    unsigned int getRoomLightCount(unsigned int roomIndex);

    /*!
     * \brief Get a single model info from a room
     * \param roomIndex valid room index
     * \param index valid model index in room
     * \param modelIndex will contain starting mesh
     * \param pos will contain pos
     * \param yaw will contain yaw angle
     * \returns 0 on success, < 0 on error
     */
    int getRoomModel(unsigned int roomIndex, unsigned int index,
                        int *modelIndex, float pos[3], float *yaw);

    /*!
     * \brief Get number of room models in room (unified)
     * \param roomIndex room index
     * \returns number of room models in room
     */
    unsigned int getRoomModelCount(unsigned int roomIndex);

    /*!
     * \brief Get a single portal from room (unified)
     * \param roomIndex valid room index
     * \param index valid portal index in room
     * \param adjoiningRoom will contain adjoining room index
     * \param normal allocated, will contain normal vector
     * \param vertices allocated, will contain vertices
     * \returns 0 on success, < 0 on error
     */
    int getRoomPortal(unsigned int roomIndex, unsigned int index,
                        int *adjoiningRoom, float normal[3], float vertices[12]);

    /*!
     * \brief Get number of portals from room (unified)
     * \param roomIndex room index
     * \returns number of portals from room
     */
    unsigned int getRoomPortalCount(unsigned int roomIndex);

    /*!
     * \brief Get rectangle data with texCoords for non-matching
     * vertex/uv for each vertex in TombRaider room (unified)
     * \param roomIndex valid room index
     * \param rectangleIndex rectangle index in room
     * \param indices will contain indices
     * \param texCoords will contain texCoords
     * \param texture will contain texture
     * \param flags will contain flags
     */
    void getRoomRectangle(unsigned int roomIndex, unsigned int rectangleIndex,
                            unsigned int *indices, float *texCoords, int *texture,
                            unsigned int *flags);

    /*!
     * \brief Get number of rectangles from room (unified)
     * \param roomIndex room index
     * \returns number of rectangles from room
     */
    unsigned int getRoomRectangleCount(unsigned int roomIndex);

    /*!
     * \brief Get a single sector from room (unified)
     * \param roomIndex room index
     * \param index sector index
     * \param flags will contain flags
     * \param ceiling will contain ceiling
     * \param floor will contain floor
     * \param floorDataIndex will contain floor data index
     * \param boxIndex will contain boxIndex
     * \param roomBelow will contain roomBelow
     * \param roomAbove will contain roomAbove
     * \return 0 on success, < 0 on error
     */
    int getRoomSector(unsigned int roomIndex, unsigned int index,
                        unsigned int *flags, float *ceiling, float *floor,
                        int *floorDataIndex, int *boxIndex,
                        int *roomBelow, int *roomAbove);

    /*!
     * \brief Get number of sectors in room (unified)
     * \param roomIndex room index
     * \param zSectorsCount will contain width of sector list
     * \param xSectorsCount will contain height of sector list
     * \returns number of sectors in room
     */
    unsigned int getRoomSectorCount(unsigned int roomIndex,
                                    unsigned int *zSectorsCount,
                                    unsigned int *xSectorsCount);

    void getRoomSprite(unsigned int roomIndex, unsigned int index,
                        float scale, int *texture,
                        float *pos, float *vertices, float *texcoords);

    /*!
     * \brief Get number of sprites in room (unified)
     * \param roomIndex room index
     * \returns number of sprites in room
     */
    unsigned int getRoomSpriteCount(unsigned int roomIndex);

    /*!
     * \brief Gets triangle data with texCoords for non-matching
     * vertex/uv for each vertex in TombRaider room (unified)
     * \param roomIndex room index
     * \param triangleIndex triangle index
     * \param indices will contain indices
     * \param texCoords will contain texCoords
     * \param texture will contain texture
     * \param flags will contain flags
     */
    void getRoomTriangle(unsigned int roomIndex, unsigned int triangleIndex,
                            unsigned int *indices, float *texCoords, int *texture,
                            unsigned int *flags);

    /*!
     * \brief Gets triangle data with texCoords for non-matching
     * vertex/uv for each vertex in TombRaider room.
     *
     * This is used with vertices, colors, etc. to do partial array
     * rendering, since the texcoords will never match vertives
     * (Tomb Raider is textile based).
     * \param index room index
     * \param textureOffset texture offset
     * \param count will contain count
     * \param indices will contain indices
     * \param texCoords will contain texCoords
     * \param textures will contain textures
     * \param flags will contain flags
     */
    void getRoomTriangles(unsigned int index, int textureOffset,
                            unsigned int *count, unsigned int **indices,
                            float **texCoords, int **textures,
                            unsigned int **flags);

    /*!
     * \brief Gets triangle data with duplicated vertex/color/normal
     * data for each face vertex to match the textile based texcoords.
     *
     * This uses more memory, but lets you do direct array rendering
     * with OpenGL, D3D, etc.
     * \param roomIndex room index
     * \param textureOffset texture offset
     * \param count will contain count
     * \param indices will contain indices
     * \param vertices will contain vertices
     * \param texCoords will contain texCoords
     * \param colors will contain colors
     * \param textures will contain textures
     * \param flags will contain flags
     */
    void getRoomTriangles(unsigned int roomIndex, int textureOffset,
                            unsigned int *count,
                            unsigned int **indices, float **vertices,
                            float **texCoords, float **colors,
                            int **textures, unsigned int **flags);

    /*!
     * \brief Get number of triangles from room (unified)
     * \param roomIndex room index
     * \returns number of triangles from room
     */
    unsigned int getRoomTriangleCount(unsigned int roomIndex);

    /*!
     * \brief Gets vertex position and color
     * \param roomIndex room index
     * \param vertexIndex vertex index
     * \param xyz will contain vertex position, has to be allocated
     * \param rgba will contain vertex color, has to be allocated
     */
    void getRoomVertex(unsigned int roomIndex, unsigned int vertexIndex,
                        float *xyz, float *rgba);

    /*!
     * \brief Get allocated vertex and color arrays and their element counts (unified)
     * \param roomIndex valid room index
     * \param vertexCount will contain vertex array length
     * \param vertices will contain vertex array
     * \param normalCount will contain normal array length
     * \param normals will contain normal array
     * \param colorCount will contain color array length
     * \param colors will contain color array
     */
    void getRoomVertexArrays(unsigned int roomIndex,
                                unsigned int *vertexCount, float **vertices,
                                unsigned int *normalCount, float **normals,
                                unsigned int *colorCount, float **colors);

    /*!
     * \brief Get number of lights in room (unified)
     * \param roomIndex room index
     * \returns number of lights in room
     */
    unsigned int getRoomVertexCount(unsigned int roomIndex);

    /*!
     * \brief Get sky mesh ID
     * \returns moveable id of sky mesh or -1 if none
     */
    int getSkyModelId();

    void getSprites();

    /*!
     * \brief Get a copy of a sound sample and its byte size
     * \param index sound sample index
     * \param bytes will contain byte size of sound sample
     * \param data will contain sound sample
     */
    void getSoundSample(unsigned int index,
                        unsigned int *bytes, unsigned char **data);

    /*!
     * \brief Get number of loaded sound samples
     * \returns number of sound samples loaded
     */
    unsigned int getSoundSamplesCount();

    /*!
     * \brief Check if a mesh is valid
     * \param index mesh index (?)
     * \returns true if mesh is valid
     */
    bool isMeshValid(int index);

    /*!
     * \brief Check if a room is valid (TRC support)
     * \param index room index
     * \returns true if room is valid
     */
    bool isRoomValid(int index);

    /*!
     * \brief Load an external sound pak for TR2 and TR3
     * \param filename pak to load
     * \returns < 0 on error
     */
    int loadSFX(char *filename);

    void reset();

    void setDebug(bool toggle);

    /*!
     * \brief Sets lighting factor for each vertex color per room in TR3 paks
     * \param f new lighting factor
     */
    void setRoomVertexLightingFactor(float f);

    /*!
     * \brief Set scaling for sprite texel alignment, etc.
     * \param f new scaling factor
     */
    void setTexelScalingFactor(float f);

private:

    void extractMeshes(unsigned char *mesh_data,
                        unsigned int num_mesh_pointers,
                        unsigned int *mesh_pointers);

    int Fread(void *buffer, size_t size, size_t count, FILE *f);

    /*!
     * \brief Get a copy of the sound samples
     * \param bytes will contain size of sound samples
     * \param data will contain sound samples themselves
     */
    void getRiffData(unsigned int *bytes, unsigned char **data);

    /*!
     * \brief Get a copy of a TR4 sound sample
     * \param index sound sample index
     * \param bytes will contain length of sound sample
     * \param data will contain sound sample itself
     */
    void getRiffDataTR4(unsigned int index,
                        unsigned int *bytes, unsigned char **data);

    /*!
     * \brief Get an array of offsets for a contiguous RIFF data stream in chunks.
     *
     * Offsets will be allocated with enough space to hold expected
     * number of offsets. (Should be known number, otherwise not all RIFFs
     * will be parsed.)
     * \param riffData riff data
     * \param riffDataBytes length of riff data
     * \param offsets will contain offsets
     * \param numOffsets known number
     * \returns number of RIFFs found
     */
    int getRiffOffsets(unsigned char *riffData, unsigned int riffDataBytes,
                        unsigned int **offsets, unsigned int numOffsets);

    /*!
     * \brief Makes a 32bit RGBA image from a textile.
     *
     * This handles all selection and conversion, including
     * alpha layering flags, now.
     * \param texture valid index into textile list
     * \returns 32bit RGBA image or NULL on error
     */
    unsigned char *getTexTile(int texture);

    /*!
     * \brief Loads a TR5 pak into memory.
     * \param f valid FILE
     * \returns 0 on success, < 0 on error
     */
    int loadTR5(FILE *f);

    static void print(const char *methodName, const char *s, ...) __attribute__((format(printf, 2, 3)));

    void printDebug(const char *methodName, const char *s, ...) __attribute__((format(printf, 3, 4)));

    bool mReset;                           //!< Guard multiple calls to reset()
    bool mDebug;                           //!< Debug output toggle
    unsigned int mPakVersion;              //!< TombRaider pak file header version
    tr2_version_type mEngineVersion;       //!< TombRaider engine version
    tr2_colour_t _palette8[256];           //!< 8-bit palette
    unsigned int _palette16[256];          //!< 16-bit palette
    unsigned int _num_textiles;            //!< Total number of texture tiles
    unsigned short  _num_room_textures;    //!< Num textures only for room use?
    unsigned short _num_misc_textures;     //!< Num of textures for misc use?
    unsigned short _num_bump_map_textures; //!< Num of textures that are bump map, texture pairs
    tr2_textile8_t *_textile8;             //!< 8-bit (palettised) textiles
    tr2_textile16_t *_textile16;           //!< 16-bit (ARGB) textiles
    tr2_textile32_t *_textile32;           //!< 32-bit (BGRA) textiles
    unsigned int _num_tex_special;         //!< Special textures and bump maps count
    unsigned char *_tex_special;           //!< Special textures and bump maps
    unsigned int _unknown_t;               //!< 32-bit unknown (always 0 in real TR2 levels)
    unsigned short _num_rooms;             //!< Number of rooms in this level
    tr2_room_t *_rooms;                    //!< List of rooms (TR1,TR2,TR3,TR4)
    tr5_room_t *mRoomsTR5;                 //!< Rooms ( TR5 / TRC ) Only
    unsigned int _num_floor_data;          //!< Num of words of floor data this level
    unsigned short *_floor_data;           //!< Floor data
    int mMeshCount;                        //!< Number of meshes this level
    tr2_mesh_t *mMeshes;                   //!< list of meshes
    unsigned int _num_animations;          //!< number of animations this level
    tr2_animation_t *_animations;          //!< list of animations
    unsigned int _num_state_changes;       //!< number of structures(?) this level
    tr2_state_change_t *_state_changes;    //!< list of structures
    unsigned int _num_anim_dispatches;     //!< number of ranges(?) this level
    tr2_anim_dispatch_t *_anim_dispatches; //!< list of ranges
    unsigned int _num_anim_commands;       //!< number of Bone1s this level
    tr2_anim_command_t *_anim_commands;    //!< list of Bone1s
    unsigned int _num_mesh_trees;          //!< number of Bone2s this level
    tr2_meshtree_t *_mesh_trees;           //!< list of Bone2s
    unsigned int _num_frames;              //!< num of words of frame data this level
    unsigned short *_frames;               //!< frame data
    unsigned int _num_moveables;           //!< number of moveables this level
    tr2_moveable_t *_moveables;            //!< list of moveables

    uint32_t numMoveablesTR5;
    tr5_moveable_t *moveablesTR5;

    uint32_t numAnimationsTR5;
    tr5_animation_t *animationsTR5;

    uint32_t numObjectTexturesTR5;
    tr5_object_texture_t *objectTexturesTR5;

    uint32_t numCinematicFramesTR5;
    tr5_cinematic_frame_t *cinematicFramesTR5;

    uint32_t numFlyByCamerasTR5;
    tr5_flyby_camera_t *flyByCamerasTR5;

    unsigned int _num_static_meshes;        //!< number of static meshes this level
    tr2_staticmesh_t *_static_meshes;       //!< static meshes
    unsigned int _num_object_textures;      //!< number of object textures this level
    tr2_object_texture_t *_object_textures; //!< list of object textures

    unsigned int _num_sprite_textures;        //!< num of sprite textures this level
    tr2_sprite_texture_t *_sprite_textures;   //!< list of sprite textures
    unsigned int _num_sprite_sequences;       //!< num of sprite sequences this level
    tr2_sprite_sequence_t *_sprite_sequences; //!< sprite sequence data
    int  _num_cameras;                        //!< Number of Cameras
    tr2_camera_t  *_cameras;                  //!< cameras
    int  _num_sound_sources;                  //!< Number of Sounds
    tr2_sound_source_t *_sound_sources;       //!< sounds
    int  _num_boxes;                          //!< Number of Boxes
    tr2_box_t *_boxes;                        /*!< boxes - looks like
                                               * struct { unsigned short value[4]; }
                                               * - value[0..2] might be a vector;
                                               * value[3] seems to be index into
                                               * Overlaps[] */
    int  _num_overlaps;                       //!< Number of Overlaps
    short  *_overlaps;                        /*!< Overlaps -
                                               * looks like ushort; 0x8000 is flag
                                               * of some sort appears to be an
                                               * offset into Boxes[] and/or
                                               * Boxes2[] */
    short  *_zones;                           //!< Boxes2
    int  _num_animated_textures;              //!< Number of AnimTextures
    short  *_animated_textures;               //!< Animtextures
    int  _num_items;                          //!< Number of Items
    tr2_item_t *_items;                       //!< Items
    unsigned char  *_light_map;               //!< Colour-light maps
    unsigned int _num_cinematic_frames;       //!< Number of cut-scene frames
    tr2_cinematic_frame_t *_cinematic_frames; //!< Cut-scene frames
    short  _num_demo_data;                    //!< Number of Demo Data
    unsigned char  *_demo_data;               //!< Demo data
    float mRoomVertexLightingFactor;
    float mTexelScale;

    // Sound data
    short  *mSoundMap;                   //!< Sound map
    int  mNumSoundDetails;               //!< Number of SampleModifiers
    tr2_sound_details_t *mSoundDetails;  //!< Sample modifiers
    int  mNumSampleIndices;              //!< Number of Sample Indices
    int  *mSampleIndices;                //!< Sample indices
    unsigned int *mSampleIndicesTR5;
    bool mRiffAlternateLoaded;           //!< Is a TR2,TR3 SFX loaded?
    unsigned int *mRiffAlternateOffsets; //!< After parsing this will
                                         //!< hold byte offsets for TR2,TR3
                                         //!< RIFFs in the buffered SFX
    int mRiffDataSz;                     //!< Byte size of a loaded SFX
    unsigned char *mRiffData;            //!< SFX RIFF data in chunks
    unsigned int mNumTR4Samples;
    unsigned char **mTR4Samples;
    unsigned int *mTR4SamplesSz;

    // For packed Fread emu/wrapper
    unsigned char *mCompressedLevelData;     //!< Buffer used to emulate fread with uncompressed libz data
    unsigned int mCompressedLevelDataOffset; //!< Offset into buffer
    unsigned int mCompressedLevelSize;       //!< Size of buffer
    tr_fread_mode_t mFreadMode;              //!< Fread mode file|buffer
};

#endif

