/*!
 * \file src/loader/LoaderTR2.cpp
 * \brief TR2 level file loader
 *
 * \author xythobuz
 */

#include <array>
#include <cstdint>
#include <vector>

#include "global.h"
#include "Log.h"
#include "Mesh.h"
#include "Room.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "World.h"
#include "system/Sound.h"
#include "utils/pixel.h"
#include "loader/LoaderTR2.h"

LoaderTR2::LoaderTR2() {
}

LoaderTR2::~LoaderTR2() {
}

int LoaderTR2::load(std::string f) {
    if (file.open(f) != 0) {
        return 1; // Could not open file
    }

    if (file.readU32() != 0x2D) {
        return 2; // Not a TR2 level?!
    }

    loadPaletteTextiles();

    file.seek(file.tell() + 4); // Unused value?

    loadRooms();
    loadFloorData();
    loadMeshes();
    loadMoveables();
    loadStaticMeshes();
    loadTextures();
    loadSprites();
    loadCameras();
    loadSoundSources();
    loadBoxesOverlapsZones();
    loadAnimatedTextures();
    loadItems();

    file.seek(file.tell() + 8192); // Skip Light map, only for 8bit coloring

    loadCinematicFrames();
    loadDemoData();
    loadSoundMap();
    loadSoundDetails();
    loadSampleIndices();

    loadExternalSoundFile(f);

    return 0; // TODO Not finished with implementation!
}

// ---- Textures ----

void LoaderTR2::loadPaletteTextiles() {
    file.seek(file.tell() + 768); // Skip 8bit palette, 256 * 3 bytes

    // Read the 16bit palette, 256 * 4 bytes, RGBA, A unused
    std::array<uint32_t, 256> palette; //!< RGBA, A unused
    for (auto& x : palette)
        x = file.readU32();

    // TODO store palette somewhere

    uint32_t numTextiles = file.readU32();

    file.seek(file.tell() + (numTextiles * 256 * 256)); // Skip 8bit textiles

    // Read the 16bit textiles, numTextiles * 256 * 256 * 2 bytes
    for (unsigned int i = 0; i < numTextiles; i++) {
        std::array<uint8_t, 256 * 256 * 2> arr;
        for (auto& x : arr) {
            x = file.readU8();
        }

        // Convert 16bit textile to 32bit textile
        unsigned char* img = argb16to32(&arr[0], 256, 256);
        int r = getTextureManager().loadBufferSlot(img, 256, 256, ARGB, 32,
                TextureManager::TextureStorage::GAME, i);
        assert(r >= 0); //! \fixme properly handle error when texture could not be loaded!
        delete [] img;
    }
}

void LoaderTR2::loadTextures() {
    uint32_t numObjectTextures = file.readU32();
    for (unsigned int o = 0; o < numObjectTextures; o++) {
        // 0 means that a texture is all-opaque, and that transparency
        // information is ignored.
        // 1 means that transparency information is used. In 8-bit color,
        // index 0 is the transparent color, while in 16-bit color, the
        // top bit (0x8000) is the alpha channel (1 = opaque, 0 = transparent)
        uint16_t attribute = file.readU16();

        // Index into the textile list
        uint16_t tile = file.readU16();

        TextureTile* t = new TextureTile(attribute, tile);

        // The four corner vertices of the texture
        // The Pixel values are the actual coordinates of the vertexs pixel
        // The Coordinate values depend on where the other vertices are in
        // the object texture. And if the object texture is used to specify
        // a triangle, then the fourth vertexs values will all be zero
        // Coordinate is 1 if Pixel is the low val, 255 if high val in object texture
        for (int i = 0; i < 4; i++) {
            uint8_t xCoordinate = file.readU8();
            uint8_t xPixel = file.readU8();
            uint8_t yCoordinate = file.readU8();
            uint8_t yPixel = file.readU8();

            assert((xCoordinate != 1) || (xCoordinate != 255));
            assert((yCoordinate != 1) || (yCoordinate != 255));

            t->add(new TextureTileVertex(xCoordinate, xPixel, yCoordinate, yPixel));
        }

        getTextureManager().addTile(t);
    }
}

void LoaderTR2::loadAnimatedTextures() {
    uint32_t numWords = file.readU32() - 1;
    uint16_t numAnimatedTextures = file.readU16();
    std::vector<uint16_t> animatedTextures;
    for (unsigned int a = 0; a < numWords; a++) {
        animatedTextures.push_back(file.readU16());
    }

    int pos = 0;
    for (unsigned int a = 0; a < numAnimatedTextures; a++) {
        int count = animatedTextures.at(pos) + 1;
        if ((pos + count) >= numWords) {
            getLog() << "LoaderTR2: Invalid AnimatedTextures ("
                     << pos + count << " >= " << numWords << ")!" << Log::endl;
            return;
        }

        for (int i = 0; i < count; i++) {
            getTextureManager().addAnimatedTile(a, animatedTextures.at(pos + i + 1));
        }

        pos += count + 1;
    }

    if (pos != numWords)
        getLog() << "LoaderTR2: Extra bytes at end of AnimatedTextures?" << Log::endl;
}

// ---- Rooms ----

void LoaderTR2::loadRooms() {
    uint16_t numRooms = file.readU16();

    for (unsigned int i = 0; i < numRooms; i++) {
        // Room Header
        int32_t xOffset = file.read32();
        int32_t zOffset = file.read32();
        int32_t yBottom = file.read32(); // lowest point == largest y value
        int32_t yTop = file.read32(); // highest point == smallest y value

        // Number of data words (2 bytes) to follow
        uint32_t dataToFollow = file.readU32();


        std::vector<struct vertex_t> vertices;

        uint16_t numVertices = file.readU16();
        for (unsigned int v = 0; v < numVertices; v++) {
            struct vertex_t vert;
            // Vertex coordinates, relative to x/zOffset
            vert.x = file.read16();
            vert.y = file.read16();
            vert.z = file.read16();

            vert.light1 = file.read16();

            // Set of flags for special rendering effects
            // 0x8000 - Something to do with water surface?
            // 0x4000 - Underwater lighting modulation/movement if seen from above
            // 0x2000 - Water/Quicksand surface movement
            // 0x0010 - Normal?
            vert.attributes = file.readU16();

            vert.light2 = file.read16(); // Almost always equal to light1

            vertices.push_back(vert);
        }

        Room* room = new Room();

        uint16_t numRectangles = file.readU16();
        for (unsigned int r = 0; r < numRectangles; r++) {
            // Indices into the vertex list read just before
            uint16_t vertex1 = file.readU16();
            uint16_t vertex2 = file.readU16();
            uint16_t vertex3 = file.readU16();
            uint16_t vertex4 = file.readU16();

            // Index into the object-texture list
            uint16_t texture = file.readU16();

            room->getMesh().addTexturedRectangle(vertices.at(vertex1), vertices.at(vertex2),
                                                 vertices.at(vertex3), vertices.at(vertex4), texture);
        }

        uint16_t numTriangles = file.readU16();
        for (unsigned int t = 0; t < numTriangles; t++) {
            // Indices into the room vertex list
            uint16_t vertex1 = file.readU16();
            uint16_t vertex2 = file.readU16();
            uint16_t vertex3 = file.readU16();

            // Index into the object-texture list
            uint16_t texture = file.readU16();

            room->getMesh().addTexturedTriangle(vertices.at(vertex1), vertices.at(vertex2),
                                                vertices.at(vertex3), texture);
        }

        uint16_t numSprites = file.readU16();
        for (unsigned int s = 0; s < numSprites; s++) {
            uint16_t vertex = file.readU16(); // Index into vertex list
            uint16_t sprite = file.readU16(); // Index into sprite list

            // TODO store sprites somewhere
        }

        uint16_t numPortals = file.readU16();
        for (unsigned int p = 0; p < numPortals; p++) {
            // Which room this portal leads to
            uint16_t adjoiningRoom = file.readU16();

            // Which way the portal faces
            // The normal points away from the adjacent room
            // To be seen through, it must point toward the viewpoint
            int16_t xNormal = file.read16();
            int16_t yNormal = file.read16();
            int16_t zNormal = file.read16();

            // The corners of this portal
            // The right-hand rule applies with respect to the normal
            int16_t xCorner1 = file.read16();
            int16_t yCorner1 = file.read16();
            int16_t zCorner1 = file.read16();
            int16_t xCorner2 = file.read16();
            int16_t yCorner2 = file.read16();
            int16_t zCorner2 = file.read16();
            int16_t xCorner3 = file.read16();
            int16_t yCorner3 = file.read16();
            int16_t zCorner3 = file.read16();
            int16_t xCorner4 = file.read16();
            int16_t yCorner4 = file.read16();
            int16_t zCorner4 = file.read16();

            // TODO store portals somewhere
        }

        uint16_t numZSectors = file.readU16();
        uint16_t numXSectors = file.readU16();
        for (unsigned int s = 0; s < (numZSectors * numXSectors); s++) {
            // Sectors are 1024*1024 world coordinates. Floor and Ceiling are
            // signed numbers of 256 units of height.
            // Floor/Ceiling value of 0x81 is used to indicate impenetrable
            // walls around the sector.
            // Floor values are used by the original engine to determine
            // what objects can be traversed and how. Relative steps of 1 (256)
            // can be walked up, 2..7 must be jumped up, larger than 7 is too high
            // If RoomAbove/Below is not none, the Ceiling/Floor is a collisional
            // portal to that room
            uint16_t indexFloorData = file.readU16();
            uint16_t indexBox = file.readU16(); // 0xFFFF if none
            uint8_t roomBelow = file.readU8(); // 0xFF if none
            int8_t floor = file.read8(); // Absolute height of floor (divided by 256)
            uint8_t roomAbove = file.readU8(); // 0xFF if none
            int8_t ceiling = file.read8(); // Absolute height of ceiling (/ 256)

            // TODO store sectors somewhere
        }

        int16_t intensity1 = file.read16();
        int16_t intensity2 = file.read16();
        int16_t lightMode = file.read16();

        uint16_t numLights = file.readU16();
        for (unsigned int l = 0; l < numLights; l++) {
            // Position of light, in world coordinates
            int32_t x = file.read32();
            int32_t y = file.read32();
            int32_t z = file.read32();

            uint16_t intensity1 = file.readU16();
            uint16_t intensity2 = file.readU16(); // Almost always equal to intensity1

            uint32_t fade1 = file.readU32(); // Falloff value?
            uint32_t fade2 = file.readU32(); // Falloff value?

            // TODO store light somewhere
        }

        uint16_t numStaticMeshes = file.readU16();
        for (unsigned int s = 0; s < numStaticMeshes; s++) {
            // Absolute position in world coordinates
            int32_t x = file.read32();
            int32_t y = file.read32();
            int32_t z = file.read32();

            // High two bits (0xC000) indicate steps of
            // 90 degrees (eg. (rotation >> 14) * 90)
            uint16_t rotation = file.readU16();

            // Constant lighting, 0xFFFF means use mesh lighting
            uint16_t intensity1 = file.readU16();
            uint16_t intensity2 = file.readU16();

            // Which StaticMesh item to draw
            uint16_t objectID = file.readU16();

            // TODO store static meshes somewhere
        }

        int16_t alternateRoom = file.read16();

        uint16_t flags = file.readU16();

        getWorld().addRoom(room);
    }
}

void LoaderTR2::loadFloorData() {
    uint32_t numFloorData = file.readU32();
    for (unsigned int f = 0; f < numFloorData; f++) {
        uint16_t unused = file.readU16();

        // TODO store floor data somewhere
    }

    if (numFloorData > 0)
        getLog() << "LoaderTR2: Found " << numFloorData << " words FloorData, unimplemented!" << Log::endl;
}

void LoaderTR2::loadSprites() {
    uint32_t numSpriteTextures = file.readU32();
    std::vector<Sprite> sprites;
    for (unsigned int s = 0; s < numSpriteTextures; s++) {
        uint16_t tile = file.readU16();
        uint8_t x = file.readU8();
        uint8_t y = file.readU8();
        uint16_t width = file.readU16(); // Actually (width * 256) + 255
        uint16_t height = file.readU16(); // Actually (height * 256) + 255

        // Required for what?
        int16_t leftSide = file.read16();
        int16_t topSide = file.read16();
        int16_t rightSide = file.read16();
        int16_t bottomSide = file.read16();

        sprites.emplace_back(tile, x, y, width, height);
    }

    uint32_t numSpriteSequences = file.readU32();
    for (unsigned int s = 0; s < numSpriteSequences; s++) {
        int32_t objectID = file.read32(); // Item identifier, matched in Items[]
        int16_t negativeLength = file.read16(); // Negative sprite count
        int16_t offset = file.read16(); // Where sequence starts in sprite texture list

        assert(negativeLength < 0);
        assert(offset >= 0);
        assert((offset + (negativeLength * -1)) <= numSpriteTextures);

        SpriteSequence* ss = new SpriteSequence(objectID);
        for (int i = 0; i < (negativeLength * -1); i++) {
            ss->add(sprites.at(offset + i));
        }
        getWorld().addSprite(ss);
    }
}

// ---- Meshes ----

void LoaderTR2::loadMeshes() {
    // Number of bitu16s of mesh data to follow
    // Read all the mesh data into a buffer, because
    // only afterward we can read the number of meshes
    // in this data block
    uint32_t numMeshData = file.readU32();
    std::vector<uint16_t> buffer;
    for (unsigned int i = 0; i < numMeshData; i++) {
        buffer.push_back(file.readU16());
    }

    uint32_t numMeshPointers = file.readU32();

    if ((numMeshData == 0) || (numMeshPointers == 0)) {
        getLog() << "LoaderTR2: No mesh data found!" << Log::endl;
        return;
    }

    for (unsigned int i = 0; i < numMeshPointers; i++) {
        uint32_t meshPointer = file.readU32();

        char* tmpPtr = reinterpret_cast<char*>(&buffer[meshPointer / 2]);
        BinaryMemory mem(tmpPtr, (numMeshData * 2) - meshPointer);

        int16_t mx = mem.read16();
        int16_t my = mem.read16();
        int16_t mz = mem.read16();

        int32_t collisionSize = mem.read32();

        uint16_t numVertices = mem.readU16();
        for (int v = 0; v < numVertices; v++) {
            int16_t x = mem.read16();
            int16_t y = mem.read16();
            int16_t z = mem.read16();

        }

        int16_t numNormals = mem.read16();
        if (numNormals > 0) {
            // External vertex lighting is used, with the lighting calculated
            // from the rooms ambient and point-source lighting values. The
            // latter appears to use a simple Lambert law for directionality:
            // intensity is proportional to
            //      max((normal direction).(direction to source), 0)
            for (int n = 0; n < numNormals; n++) {
                int16_t x = mem.read16();
                int16_t y = mem.read16();
                int16_t z = mem.read16();

            }
        } else if (numNormals < 0) {
            // Internal vertex lighting is used,
            // using the data included with the mesh
            for (int l = 0; l < (numNormals * -1); l++) {
                int16_t light = mem.read16();

            }
        }

        int16_t numTexturedRectangles = mem.read16();
        for (int r = 0; r < numTexturedRectangles; r++) {
            uint16_t vertex1 = mem.readU16();
            uint16_t vertex2 = mem.readU16();
            uint16_t vertex3 = mem.readU16();
            uint16_t vertex4 = mem.readU16();
            uint16_t texture = mem.readU16();

        }

        int16_t numTexturedTriangles = mem.read16();
        for (int t = 0; t < numTexturedTriangles; t++) {
            uint16_t vertex1 = mem.readU16();
            uint16_t vertex2 = mem.readU16();
            uint16_t vertex3 = mem.readU16();
            uint16_t texture = mem.readU16();

        }

        int16_t numColoredRectangles = mem.read16();
        for (int r = 0; r < numColoredRectangles; r++) {
            uint16_t vertex1 = mem.readU16();
            uint16_t vertex2 = mem.readU16();
            uint16_t vertex3 = mem.readU16();
            uint16_t vertex4 = mem.readU16();
            uint16_t texture = mem.readU16();

        }

        int16_t numColoredTriangles = mem.read16();
        for (int t = 0; t < numColoredTriangles; t++) {
            uint16_t vertex1 = mem.readU16();
            uint16_t vertex2 = mem.readU16();
            uint16_t vertex3 = mem.readU16();
            uint16_t texture = mem.readU16();

        }

        // TODO store mesh data somewhere
    }
}

void LoaderTR2::loadStaticMeshes() {
    uint32_t numStaticMeshes = file.readU32();
    for (unsigned int s = 0; s < numStaticMeshes; s++) {
        uint32_t objectID = file.readU32(); // Matched in Items[]
        uint16_t mesh = file.readU16(); // Offset into MeshPointers[]

        // tr2_vertex BoundingBox[2][2];
        // First index is which one, second index is opposite corners
        int16_t x11 = file.read16();
        int16_t y11 = file.read16();
        int16_t z11 = file.read16();

        int16_t x12 = file.read16();
        int16_t y12 = file.read16();
        int16_t z12 = file.read16();

        int16_t x21 = file.read16();
        int16_t y21 = file.read16();
        int16_t z21 = file.read16();

        int16_t x22 = file.read16();
        int16_t y22 = file.read16();
        int16_t z22 = file.read16();

        // Meaning uncertain. Usually 2, and 3 for objects Lara can
        // travel through, like TR2s skeletons and underwater plants
        uint16_t flags = file.readU16();

        // TODO store static meshes somewhere
    }

    if (numStaticMeshes > 0)
        getLog() << "LoaderTR2: Found " << numStaticMeshes << " StaticMeshes, unimplemented!" << Log::endl;
}

// ---- Moveables ----

void LoaderTR2::loadMoveables() {
    uint32_t numAnimations = file.readU32();
    for (unsigned int a = 0; a < numAnimations; a++) {
        // *Byte* Offset into Frames[] (so divide by 2!)
        uint32_t frameOffset = file.readU32();
        uint8_t frameRate = file.readU8(); // Engine ticks per frame

        // Number of bit16s in Frames[] used by this animation
        // Be careful when parsing frames using the FrameSize value
        // as the size of each frame, since an animations frame range
        // may extend into the next animations frame range, and that
        // may have a different FrameSize value.
        uint8_t frameSize = file.readU8();

        uint16_t stateID = file.readU16();

        file.seek(file.tell() + 8); // Skip 8 unknown bytes

        uint16_t frameStart = file.readU16(); // First frame in this animation
        uint16_t frameEnd = file.readU16(); // Last frame in this animation
        uint16_t nextAnimation = file.readU16();
        uint16_t nextFrame = file.readU16();
        uint16_t numStateChanges = file.readU16();
        uint16_t stateChangeOffset = file.readU16(); // Index into StateChanges[]
        uint16_t numAnimCommands = file.readU16(); // How many animation commands to use
        uint16_t animCommandOffset = file.readU16(); // Index into AnimCommand[]

        // TODO store animations somewhere
    }

    uint32_t numStateChanges = file.readU32();
    for (unsigned int s = 0; s < numStateChanges; s++) {
        uint16_t stateID = file.readU16();
        uint16_t numAnimDispatches = file.readU16();
        uint16_t animDispatchOffset = file.readU16(); // Index into AnimDispatches[]

        // TODO store state changes somewhere
    }

    uint32_t numAnimDispatches = file.readU32();
    for (unsigned int a = 0; a < numAnimDispatches; a++) {
        int16_t low = file.read16(); // Lowest frame that uses this range
        int16_t high = file.read16(); // Highest frame (+1?) that uses this range
        int16_t nextAnimation = file.read16(); // Animation to go to
        int16_t nextFrame = file.read16(); // Frame offset to go to

        // TODO store animation dispatches somewhere
    }

    uint32_t numAnimCommands = file.readU32();
    std::vector<int16_t> animCommands;
    for (unsigned int a = 0; a < numAnimCommands; a++) {
        animCommands.push_back(file.read16());
    }

    uint32_t numMeshTrees = file.readU32();
    for (unsigned int m = 0; m < numMeshTrees; m++) {
        // 0x0002 - Put parent mesh on the mesh stack
        // 0x0001 - Pop mesh from stack, use as parent mesh
        // When both are not set, use previous mesh as parent mesh
        // When both are set, do 0x0001 first, then 0x0002, thereby
        // reading the stack but not changing it
        uint32_t flags = file.readU32();


        // Offset of mesh origin from the parent mesh origin
        //int32_t x = file.read32();
        //int32_t y = file.read32();
        //int32_t z = file.read32();
        // Does not appear to be true...?

        // TODO store mesh trees somewhere
    }

    uint32_t numFrames = file.readU32();
    std::vector<uint16_t> frames;
    for (unsigned int f = 0; f < numFrames; f++) {
        frames.push_back(file.readU16());
    }

    uint32_t numMoveables = file.readU32();
    for (unsigned int m = 0; m < numMoveables; m++) {
        // Item identifier, matched in Items[]
        uint32_t objectID = file.readU32();
        uint16_t numMeshes = file.readU16();
        uint16_t startingMesh = file.readU16(); // Offset into MeshPointers[]
        uint32_t meshTree = file.readU32(); // Offset into MeshTree[]
        // *Byte* offset into Frames[] (divide by 2 for Frames[i])
        uint32_t frameOffset = file.readU32();

        // If animation index is 0xFFFF, the object is stationary or
        // animated by the engine (ponytail)
        uint16_t animation = file.readU16();

        // TODO store moveables somewhere
    }

    // TODO combine all this into moveables with their animations
}

void LoaderTR2::loadItems() {
    uint32_t numItems = file.readU32();
    for (unsigned int i = 0; i < numItems; i++) {
        int16_t objectID = file.read16();
        int16_t room = file.read16();

        // Item position in world coordinates
        int32_t x = file.read32();
        int32_t y = file.read32();
        int32_t z = file.read32();

        int16_t angle = file.read16(); // (0xC000 >> 14) * 90deg
        int16_t intensity1 = file.read16(); // Constant lighting; -1 means mesh lighting
        int16_t intensity2 = file.read16(); // Almost always like intensity1

        // 0x0100 - Initially visible
        // 0x3E00 - Activation mask, open, can be XORed with related FloorData list fields.
        uint16_t flags = file.readU16();

        // TODO store items somewhere
    }

    if (numItems > 0)
        getLog() << "LoaderTR2: Found " << numItems << " Items, unimplemented!" << Log::endl;
}

void LoaderTR2::loadBoxesOverlapsZones() {
    uint32_t numBoxes = file.readU32();
    for (unsigned int b = 0; b < numBoxes; b++) {
        // Sectors (* 1024 units)
        uint8_t zMin = file.readU8();
        uint8_t zMax = file.readU8();
        uint8_t xMin = file.readU8();
        uint8_t xMax = file.readU8();

        int16_t trueFloor = file.read16(); // Y value (no scaling)

        // Index into overlaps[]. The high bit is sometimes set
        // this occurs in front of swinging doors and the like
        int16_t overlapIndex = file.read16();

        // TODO store boxes somewhere
    }

    uint32_t numOverlaps = file.readU32();
    std::vector<uint16_t> overlaps;
    for (unsigned int o = 0; o < numOverlaps; o++) {
        overlaps.push_back(file.readU16());
    }

    // TODO store overlaps somewhere

    std::vector<int16_t> zones;
    for (unsigned int z = 0; z < numBoxes; z++) {
        for (unsigned int i = 0; i < 10; i++) {
            zones.push_back(file.read16());
        }
    }

    // TODO store zones somewhere

    if ((numBoxes > 0) || (numOverlaps > 0))
        getLog() << "LoaderTR2: Found NPC NavigationHints, not yet implemented!" << Log::endl;
}

// ---- Sound ----

void LoaderTR2::loadSoundSources() {
    uint32_t numSoundSources = file.readU32();
    for (unsigned int s = 0; s < numSoundSources; s++) {
        // Absolute world coordinate positions of sound source
        int32_t x = file.read32();
        int32_t y = file.read32();
        int32_t z = file.read32();

        // Internal sound index
        uint16_t soundID = file.readU16();

        // Unknown, 0x40, 0x80 or 0xC0
        uint16_t flags = file.readU16();

        SoundManager::addSoundSource(x, y, z, soundID, flags);
    }

    if (numSoundSources > 0)
        getLog() << "LoaderTR2: Found " << numSoundSources << " SoundSources" << Log::endl;
}

void LoaderTR2::loadSoundMap() {
    for (int i = 0; i < 370; i++) {
        SoundManager::addSoundMapEntry(file.read16());
    }
}

void LoaderTR2::loadSoundDetails() {
    uint32_t numSoundDetails = file.readU32();
    for (unsigned int s = 0; s < numSoundDetails; s++) {
        uint16_t sample = file.readU16(); // Index into SampleIndices[]
        uint16_t volume = file.readU16();

        // sound range? distance at which this sound can be heard?
        uint16_t unknown1 = file.readU16();

        // Bits 8-15: priority?
        // Bits 2-7: number of samples in this group
        // Bits 0-1: channel number?
        uint16_t unknown2 = file.readU16();

        SoundManager::addSoundDetail(sample, ((float)volume) / 32767.0f);
    }

    if (numSoundDetails > 0)
        getLog() << "LoaderTR2: Found " << numSoundDetails << " SoundDetails" << Log::endl;
}

void LoaderTR2::loadSampleIndices() {
    uint32_t numSampleIndices = file.readU32();
    for (unsigned int i = 0; i < numSampleIndices; i++) {
        SoundManager::addSampleIndex(file.readU32());
    }

    if (numSampleIndices > 0)
        getLog() << "LoaderTR2: Found " << numSampleIndices << " SampleIndices" << Log::endl;
}

void LoaderTR2::loadExternalSoundFile(std::string f) {
    size_t dir = f.find_last_of("/\\");
    if (dir != std::string::npos) {
        f.replace(dir + 1, std::string::npos, "MAIN.SFX");
    } else {
        f = "MAIN.SFX";
    }

    BinaryFile sfx;
    if (sfx.open(f) != 0) {
        getLog() << "LoaderTR2: Can't open \"" << f << "\"!" << Log::endl;
        return;
    }

    int riffCount = 0;
    while (!sfx.eof()) {
        char test[5];
        test[4] = '\0';
        for (int i = 0; i < 4; i++)
            test[i] = sfx.read8();

        if (std::string("RIFF") != std::string(test)) {
            getLog() << "LoaderTR2: External SFX invalid! (" << riffCount
                     << ", \"" << test << "\" != \"RIFF\")" << Log::endl;
            return;
        }

        // riffSize is (fileLength - 8)
        uint32_t riffSize = sfx.readU32();

        unsigned char buff[riffSize + 8];
        sfx.seek(sfx.tell() - 8);
        for (int i = 0; i < (riffSize + 8); i++)
            buff[i] = sfx.readU8();

        unsigned long src;
        int ret = Sound::loadBuffer(buff, riffSize + 8);
        assert(ret >= 0);

        riffCount++;
    }

    if (riffCount > 0)
        getLog() << "LoaderTR2: Found " << riffCount << " SoundSamples" << Log::endl;
}

// ---- Stuff ----

void LoaderTR2::loadCameras() {
    uint32_t numCameras = file.readU32();
    for (unsigned int c = 0; c < numCameras; c++) {
        int32_t x = file.read32();
        int32_t y = file.read32();
        int32_t z = file.read32();
        int16_t room = file.read16();

        file.seek(file.tell() + 2); // Unknown, correlates to Boxes? Zones?

        // TODO store cameras somewhere
    }

    if (numCameras > 0)
        getLog() << "LoaderTR2: Found " << numCameras << " Cameras, unimplemented!" << Log::endl;
}

void LoaderTR2::loadCinematicFrames() {
    uint16_t numCinematicFrames = file.readU16();
    for (unsigned int c = 0; c < numCinematicFrames; c++) {
        int16_t rotY = file.read16(); // Y rotation, +-32767 = +-180deg
        int16_t rotZ = file.read16(); // Z rotation, like rotY
        int16_t rotZ2 = file.read16(); // Like rotZ?
        int16_t posZ = file.read16(); // Camera pos relative to what?
        int16_t posY = file.read16();
        int16_t posX = file.read16();
        int16_t unknown = file.read16(); // Changing this can cause runtime error
        int16_t rotX = file.read16(); // X rotation, like rotY

        // TODO store cinematic frames somewhere
    }

    if (numCinematicFrames > 0)
        getLog() << "LoaderTR2: Found " << numCinematicFrames
                 << " CinematicFrames, not yet implemented!" << Log::endl;
}

void LoaderTR2::loadDemoData() {
    uint16_t numDemoData = file.readU16();
    for (unsigned int d = 0; d < numDemoData; d++)
        file.readU8();

    // TODO store demo data somewhere, find out meaning
    if (numDemoData > 0)
        getLog() << "LoaderTR2: Found " << numDemoData << " bytes DemoData, not yet implemented!" <<
                 Log::endl;
}

