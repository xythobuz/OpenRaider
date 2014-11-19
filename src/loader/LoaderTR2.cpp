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
#include "loader/LoaderTR2.h"

LoaderTR2::LoaderTR2() {
}

LoaderTR2::~LoaderTR2() {
}

int LoaderTR2::load(std::string f) {
    if (file.open(f.c_str()) != 0) {
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

    return 0;
}

void LoaderTR2::loadPaletteTextiles() {
    file.seek(file.tell() + 768); // Skip 8bit palette, 256 * 3 bytes

    // Read the 16bit palette, 256 * 4 bytes, RGBA, A unused
    std::array<uint32_t, 256> palette; //!< RGBA, A unused
    for (auto& x : palette)
        x = file.readU32();

    uint32_t numTextiles = file.readU32();

    file.seek(file.tell() + (numTextiles * 256 * 256)); // Skip 8bit textiles

    // Read the 16bit textiles, numTextiles * 256 * 256 * 2 bytes
    std::vector<std::array<uint16_t, 256 * 256>> textiles;
    for (unsigned int i = 0; i < numTextiles; i++) {
        std::array<uint16_t, 256 * 256> arr;
        for (auto& x : arr) {
            x = file.readU16();
        }
        textiles.push_back(arr);
    }

    // TODO store palette and textiles somewhere
}

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

        uint16_t numVertices = file.readU16();
        for (unsigned int v = 0; v < numVertices; v++) {
            // Vertex coordinates, relative to x/zOffset
            int16_t relativeX = file.read16();
            int16_t relativeY = file.read16();
            int16_t relativeZ = file.read16();

            int16_t lighting1 = file.read16();

            // Set of flags for special rendering effects
            // 0x8000 - Something to do with water surface?
            // 0x4000 - Underwater lighting modulation/movement if seen from above
            // 0x2000 - Water/Quicksand surface movement
            // 0x0010 - Normal?
            uint16_t attributes = file.readU16();

            int16_t lighting2 = file.read16(); // Almost always equal to lighting1

            // TODO store vertex somewhere
        }

        uint16_t numRectangles = file.readU16();
        for (unsigned int r = 0; r < numRectangles; r++) {
            // Indices into the vertex list read just before
            uint16_t vertex1 = file.readU16();
            uint16_t vertex2 = file.readU16();
            uint16_t vertex3 = file.readU16();
            uint16_t vertex4 = file.readU16();

            // Index into the object-texture list
            uint16_t texture = file.readU16();

            // TODO store rectangles somewhere
        }

        uint16_t numTriangles = file.readU16();
        for (unsigned int t = 0; t < numTriangles; t++) {
            // Indices into the room vertex list
            uint16_t vertex1 = file.readU16();
            uint16_t vertex2 = file.readU16();
            uint16_t vertex3 = file.readU16();

            // Index into the object-texture list
            uint16_t texture = file.readU16();

            // TODO store triangles somewhere
        }

        uint16_t numSprites = file.readU16();
        for (unsigned int s = 0; s < numSprites; s++) {
            uint16_t vertex = file.readU16(); // Index into vertex list
            uint16_t texture = file.readU16(); // Index into object-texture list

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
    }
}

void LoaderTR2::loadFloorData() {
    uint32_t numFloorData = file.readU32();
    for (unsigned int f = 0; f < numFloorData; f++) {
        uint16_t unused = file.readU16();

        // TODO store floor data somewhere
    }
}

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
    std::vector<uint32_t> meshPointers;
    for (unsigned int i = 0; i < numMeshPointers; i++) {
        meshPointers.push_back(file.readU32());
    }

    // TODO interpret the buffered mesh data
}

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

        /* Where the hell does this come from?!
        int32_t x = file.read32();
        int32_t y = file.read32();
        int32_t z = file.read32();
        */

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

        // The four corner vertices of the texture
        // The Pixel values are the actual coordinates of the vertexs pixel
        // The Coordinate values depend on where the other vertices are in
        // the object texture. And if the object texture is used to specify
        // a triangle, then the fourth vertexs values will all be zero
        // Coordinate is 1 if Pixel is the low val, 255 if high val in object texture
        uint8_t xCoordinate1 = file.readU8();
        uint8_t xPixel1 = file.readU8();
        uint8_t yCoordinate1 = file.readU8();
        uint8_t yPixel1 = file.readU8();
        uint8_t xCoordinate2 = file.readU8();
        uint8_t xPixel2 = file.readU8();
        uint8_t yCoordinate2 = file.readU8();
        uint8_t yPixel2 = file.readU8();
        uint8_t xCoordinate3 = file.readU8();
        uint8_t xPixel3 = file.readU8();
        uint8_t yCoordinate3 = file.readU8();
        uint8_t yPixel3 = file.readU8();
        uint8_t xCoordinate4 = file.readU8();
        uint8_t xPixel4 = file.readU8();
        uint8_t yCoordinate4 = file.readU8();
        uint8_t yPixel4 = file.readU8();

        // TODO store object textures somewhere
    }
}

void LoaderTR2::loadSprites() {
    uint32_t numSpriteTextures = file.readU32();
    for (unsigned int s = 0; s < numSpriteTextures; s++) {
        uint16_t tile = file.readU16();
        uint8_t x = file.readU8();
        uint8_t y = file.readU8();
        uint16_t width = file.readU16(); // Actually (width * 256) + 255
        uint16_t height = file.readU16(); // Actually (height * 256) + 255
        int16_t leftSide = file.read16();
        int16_t topSide = file.read16();
        int16_t rightSide = file.read16();
        int16_t bottomSide = file.read16();

        // TODO store sprite textures somewhere
    }

    uint32_t numSpriteSequences = file.readU32();
    for (unsigned int s = 0; s < numSpriteSequences; s++) {
        int32_t objectID = file.read32(); // Item identifier, matched in Items[]
        int16_t negativeLength = file.read16(); // Negative sprite count
        int16_t offset = file.read16(); // Where sequence starts in sprite texture list

        // TODO store sprite sequences somewhere
    }
}

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
}

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

        // TODO store sound sources somewhere
    }
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
}

void LoaderTR2::loadAnimatedTextures() {
    uint32_t numAnimatedTextures = file.readU32();
    std::vector<uint16_t> animatedTextures;
    for (unsigned int a = 0; a < numAnimatedTextures; a++) {
        animatedTextures.push_back(file.readU16());
    }

    // TODO store animated textures somewhere. Format?
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
}

void LoaderTR2::loadDemoData() {
    uint16_t numDemoData = file.readU16();
    for (unsigned int d = 0; d < numDemoData; d++)
        file.readU8();

    // TODO store demo data somewhere, find out meaning
}

void LoaderTR2::loadSoundMap() {
    std::array<int16_t, 370> soundMap;
    for (auto& x : soundMap) {
        x = file.read16();
    }

    // TODO store sound map somewhere
}

void LoaderTR2::loadSoundDetails() {
    uint32_t numSoundDetails = file.readU32();
    for (unsigned int s = 0; s < numSoundDetails; s++) {
        int16_t sample = file.read16(); // Index into SampleIndices[]
        int16_t volume = file.read16();

        // sound range? distance at which this sound can be heard?
        int16_t unknown1 = file.read16();

        // Bits 8-15: priority?
        // Bits 2-7: number of samples in this group
        // Bits 0-1: channel number?
        int16_t unknown2 = file.read16();

        // TODO store sound details somewhere
    }
}

void LoaderTR2::loadSampleIndices() {
    uint32_t numSampleIndices = file.readU32();
    std::vector<uint32_t> sampleIndices;
    for (unsigned int i = 0; i < numSampleIndices; i++) {
        sampleIndices.push_back(file.readU32());
    }

    // TODO store sample indices somewhere
}

