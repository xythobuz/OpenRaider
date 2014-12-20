/*!
 * \file src/loader/LoaderTR2.cpp
 * \brief TR2 level file loader
 *
 * \author xythobuz
 */

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "global.h"
#include "Game.h"
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
    for (auto& x : palette)
        x = file.readU32();

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
        int r = getTextureManager().loadBufferSlot(img, 256, 256,
                TextureManager::ColorMode::ARGB, 32,
                TextureManager::TextureStorage::GAME, i);
        assert(r >= 0); //! \fixme properly handle error when texture could not be loaded!
        delete [] img;
    }

    if (numTextiles > 0)
        getLog() << "LoaderTR2: Found " << numTextiles << " Textures!" << Log::endl;
    else
        getLog() << "LoaderTR2: No Textures in this level?!" << Log::endl;
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

            assert((xCoordinate == 1) || (xCoordinate == 255) || (xCoordinate == 0));
            assert((yCoordinate == 1) || (yCoordinate == 255) || (yCoordinate == 0));

            t->add(TextureTileVertex(xCoordinate, xPixel, yCoordinate, yPixel));
        }

        getTextureManager().addTile(t);
    }

    if (numObjectTextures > 0)
        getLog() << "LoaderTR2: Found " << numObjectTextures << " Textiles!" << Log::endl;
    else
        getLog() << "LoaderTR2: No Textiles in this level?!" << Log::endl;
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

    if ((numAnimatedTextures > 0) || (numWords > 0))
        getLog() << "LoaderTR2: Found " << numAnimatedTextures << " Animated Textures!" << Log::endl;
    else
        getLog() << "LoaderTR2: No Animated Textures in this level?!" << Log::endl;

    if (pos != numWords)
        getLog() << "LoaderTR2: Extra bytes at end of AnimatedTextures?!" << Log::endl;
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

        glm::vec3 pos(xOffset, 0.0f, zOffset);

        // Number of data words (2 bytes) to follow
        uint32_t dataToFollow = file.readU32();

        glm::vec3 bbox[2] = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f)
        };

        uint16_t numVertices = file.readU16();
        std::vector<RoomVertexTR2> vertices;
        for (unsigned int v = 0; v < numVertices; v++) {
            RoomVertexTR2 vert;
            vert.x = file.read16();
            vert.y = file.read16();
            vert.z = file.read16();
            vert.light1 = file.read16();
            vert.attributes = file.readU16();
            vert.light2 = file.read16();
            vertices.push_back(vert);

            // Fill bounding box
            if (v == 0) {
                for (int i = 0; i < 2; i++) {
                    bbox[i].x = vert.x;
                    bbox[i].y = vert.y;
                    bbox[i].z = vert.z;
                }
            } else {
                if (vert.x < bbox[0].x)
                    bbox[0].x = vert.x;
                if (vert.x > bbox[1].x)
                    bbox[1].x = vert.x;
                if (vert.y < bbox[0].y)
                    bbox[0].y = vert.y;
                if (vert.y > bbox[1].y)
                    bbox[1].y = vert.y;
                if (vert.z < bbox[0].z)
                    bbox[0].z = vert.z;
                if (vert.z > bbox[1].z)
                    bbox[1].z = vert.z;
            }
        }

        bbox[0] += pos;
        bbox[1] += pos;

        uint16_t numRectangles = file.readU16();
        std::vector<IndexedRectangle> rectangles;
        for (unsigned int r = 0; r < numRectangles; r++) {
            // Indices into the vertex list read just before
            uint16_t vertex1 = file.readU16();
            uint16_t vertex2 = file.readU16();
            uint16_t vertex3 = file.readU16();
            uint16_t vertex4 = file.readU16();

            // Index into the object-texture list
            uint16_t texture = file.readU16();

            rectangles.emplace_back(texture, vertex1, vertex2, vertex3, vertex4);
        }

        uint16_t numTriangles = file.readU16();
        std::vector<IndexedRectangle> triangles;
        for (unsigned int t = 0; t < numTriangles; t++) {
            // Indices into the room vertex list
            uint16_t vertex1 = file.readU16();
            uint16_t vertex2 = file.readU16();
            uint16_t vertex3 = file.readU16();

            // Index into the object-texture list
            uint16_t texture = file.readU16();

            triangles.emplace_back(texture, vertex1, vertex2, vertex3);
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

            // TODO store portals
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

            bool wall = false;
            if ((((uint8_t)floor) == 0x81) || (((uint8_t)ceiling) == 0x81)) {
                wall = true;
            }

            //room->addSector(new Sector(floor * 256.0f, ceiling * 256.0f, wall));
            // TODO store sectors
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
        std::vector<StaticModel*> staticModels;
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

            staticModels.push_back(new StaticModel(glm::vec3(x, y, z),
                                                   glm::radians((rotation >> 14) * 90.0f),
                                                   objectID));
        }

        int16_t alternateRoom = file.read16();

        uint16_t flags = file.readU16();
        unsigned int roomFlags = 0;
        if (flags & 0x0001) {
            roomFlags |= RoomFlagUnderWater;
        }

        BoundingBox* boundingbox = new BoundingBox(bbox[0], bbox[1]);
        RoomMesh* mesh = new RoomMesh(vertices, rectangles, triangles);
        Room* room = new Room(pos, boundingbox, mesh, roomFlags, alternateRoom,
                              numXSectors, numZSectors);

        for (auto m : staticModels)
            room->addModel(m);

        getWorld().addRoom(room);

        // Sanity check
        if ((numPortals == 0) && (numVertices == 0)
            && (numRectangles == 0) && (numTriangles == 0))
            getLog() << "LoaderTR2: Room " << i << " seems invalid: " << numPortals << "p "
                     << numRectangles << "r " << numTriangles << "t " << numVertices
                     << "v" << Log::endl;
    }

    if (numRooms > 0)
        getLog() << "LoaderTR2: Found " << numRooms << " Rooms!" << Log::endl;
    else
        getLog() << "LoaderTR2: No Rooms in this Level?!" << Log::endl;
}

void LoaderTR2::loadFloorData() {
    uint32_t numFloorData = file.readU32();
    for (unsigned int f = 0; f < numFloorData; f++) {
        uint16_t unused = file.readU16();

        // TODO store floor data somewhere
    }

    if (numFloorData > 0)
        getLog() << "LoaderTR2: Found " << numFloorData << " words FloorData, unimplemented!" << Log::endl;
    else
        getLog() << "LoaderTR2: No FloorData in this level?!" << Log::endl;
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

    if ((numSpriteTextures > 0) || (numSpriteSequences > 0))
        getLog() << "LoaderTR2: Found " << numSpriteTextures << " Sprites in " << numSpriteSequences <<
                 " Sequences!" << Log::endl;
    else
        getLog() << "LoaderTR2: No Sprites in this level?!" << Log::endl;
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

    for (unsigned int i = 0; i < numMeshPointers; i++) {
        uint32_t meshPointer = file.readU32();

        if (numMeshData < (meshPointer / 2)) {
            getLog() << "LoaderTR2: Invalid Mesh: "
                     << (meshPointer / 2) << " > " << numMeshData << Log::endl;
            continue;
        }

        char* tmpPtr = reinterpret_cast<char*>(&buffer[meshPointer / 2]);
        BinaryMemory mem(tmpPtr, (numMeshData * 2) - meshPointer);

        int16_t mx = mem.read16();
        int16_t my = mem.read16();
        int16_t mz = mem.read16();
        int32_t collisionSize = mem.read32();
        // TODO store mesh collision info somewhere

        uint16_t numVertices = mem.readU16();
        std::vector<glm::vec3> vertices;
        for (int v = 0; v < numVertices; v++) {
            int16_t x = mem.read16();
            int16_t y = mem.read16();
            int16_t z = mem.read16();
            vertices.emplace_back(x, y, z);
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

                //mesh->addNormal(glm::vec3(x, y, z));
            }
        } else if (numNormals < 0) {
            // Internal vertex lighting is used,
            // using the data included with the mesh
            for (int l = 0; l < (numNormals * -1); l++) {
                int16_t light = mem.read16();
                // TODO store lights somewhere
            }
        }

        int16_t numTexturedRectangles = mem.read16();
        std::vector<IndexedRectangle> texturedRectangles;
        for (int r = 0; r < numTexturedRectangles; r++) {
            uint16_t vertex1 = mem.readU16();
            uint16_t vertex2 = mem.readU16();
            uint16_t vertex3 = mem.readU16();
            uint16_t vertex4 = mem.readU16();
            uint16_t texture = mem.readU16();

            texturedRectangles.emplace_back(texture, vertex1, vertex2, vertex3, vertex4);
        }

        int16_t numTexturedTriangles = mem.read16();
        std::vector<IndexedRectangle> texturedTriangles;
        for (int t = 0; t < numTexturedTriangles; t++) {
            uint16_t vertex1 = mem.readU16();
            uint16_t vertex2 = mem.readU16();
            uint16_t vertex3 = mem.readU16();
            uint16_t texture = mem.readU16();

            texturedTriangles.emplace_back(texture, vertex1, vertex2, vertex3);
        }

        int16_t numColoredRectangles = mem.read16();
        std::vector<IndexedColoredRectangle> coloredRectangles;
        for (int r = 0; r < numColoredRectangles; r++) {
            uint16_t vertex1 = mem.readU16();
            uint16_t vertex2 = mem.readU16();
            uint16_t vertex3 = mem.readU16();
            uint16_t vertex4 = mem.readU16();
            uint16_t texture = mem.readU16();

            int index = (texture & 0xFF00) >> 8;
            uint8_t red = (palette.at(index) & 0xFF000000) >> 24,
                    green = (palette.at(index) & 0x00FF0000) >> 16,
                    blue = (palette.at(index) & 0x0000FF00) >> 8;

            coloredRectangles.emplace_back(red, green, blue, vertex1, vertex2, vertex3, vertex4);
        }

        int16_t numColoredTriangles = mem.read16();
        std::vector<IndexedColoredRectangle> coloredTriangles;
        for (int t = 0; t < numColoredTriangles; t++) {
            uint16_t vertex1 = mem.readU16();
            uint16_t vertex2 = mem.readU16();
            uint16_t vertex3 = mem.readU16();
            uint16_t texture = mem.readU16();

            int index = (texture & 0xFF00) >> 8;
            uint8_t red = (palette.at(index) & 0xFF000000) >> 24,
                    green = (palette.at(index) & 0x00FF0000) >> 16,
                    blue = (palette.at(index) & 0x0000FF00) >> 8;

            coloredTriangles.emplace_back(red, green, blue, vertex1, vertex2, vertex3);
        }

        Mesh* mesh = new Mesh(vertices, texturedRectangles, texturedTriangles,
                              coloredRectangles, coloredTriangles);
        getWorld().addMesh(mesh);
    }

    if (numMeshPointers > 0)
        getLog() << "LoaderTR2: Found " << numMeshPointers << " Meshes!" << Log::endl;
    else
        getLog() << "LoaderTR2: No Meshes in this level?!" << Log::endl;
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

        getWorld().addStaticMesh(new StaticMesh(objectID, mesh));
    }

    if (numStaticMeshes > 0)
        getLog() << "LoaderTR2: Found " << numStaticMeshes << " StaticMeshes!" << Log::endl;
    else
        getLog() << "LoaderTR2: No StaticMeshes in this level?!" << Log::endl;
}

// ---- Moveables ----

struct Animation_t {
    uint32_t frameOffset;
    uint8_t frameRate, frameSize;
    uint16_t stateID, frameStart, frameEnd, nextAnimation;
    uint16_t nextFrame, numStateChanges, stateChangeOffset;
    uint16_t numAnimCommands, animCommandOffset;

    Animation_t(uint32_t fo, uint8_t fr, uint8_t fs, uint16_t si,
                uint16_t fst, uint16_t fe, uint16_t na, uint16_t nf,
                uint16_t ns, uint16_t so, uint16_t nac, uint16_t ao)
        : frameOffset(fo), frameRate(fr), frameSize(fs),
          stateID(si), frameStart(fst), frameEnd(fe), nextAnimation(na),
          nextFrame(nf), numStateChanges(ns), stateChangeOffset(so),
          numAnimCommands(nac), animCommandOffset(ao) { }
};

struct StateChange_t {
    uint16_t stateID, numAnimDispatches, animDispatchOffset;

    StateChange_t(uint16_t s, uint16_t n, uint16_t a)
        : stateID(s), numAnimDispatches(n), animDispatchOffset(a) { }
};

struct AnimDispatch_t {
    int16_t low, high, nextAnimation, nextFrame;

    AnimDispatch_t(int16_t l, int16_t h, int16_t na, int16_t nf)
        : low(l), high(h), nextAnimation(na), nextFrame(nf) { }
};

void LoaderTR2::loadMoveables() {
    uint32_t numAnimations = file.readU32();
    std::vector<Animation_t> animations;
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

        animations.emplace_back(frameOffset, frameRate, frameSize,
                                stateID, frameStart, frameEnd, nextAnimation, nextFrame, numStateChanges,
                                stateChangeOffset, numAnimCommands, animCommandOffset);
    }

    if (numAnimations > 0)
        getLog() << "LoaderTR2: Found " << numAnimations << " Animations!" << Log::endl;
    else
        getLog() << "LoaderTR2: No Animations in this level?!" << Log::endl;

    uint32_t numStateChanges = file.readU32();
    std::vector<StateChange_t> stateChanges;
    for (unsigned int s = 0; s < numStateChanges; s++) {
        uint16_t stateID = file.readU16();
        uint16_t numAnimDispatches = file.readU16(); // Number of ranges (always 1..5?)
        uint16_t animDispatchOffset = file.readU16(); // Index into AnimDispatches[]

        stateChanges.emplace_back(stateID, numAnimDispatches, animDispatchOffset);
    }

    if (numStateChanges > 0)
        getLog() << "LoaderTR2: Found " << numStateChanges << " StateChanges!" << Log::endl;
    else
        getLog() << "LoaderTR2: No StateChanges in this level?!" << Log::endl;

    uint32_t numAnimDispatches = file.readU32();
    std::vector<AnimDispatch_t> animDispatches;
    for (unsigned int a = 0; a < numAnimDispatches; a++) {
        int16_t low = file.read16(); // Lowest frame that uses this range
        int16_t high = file.read16(); // Highest frame (+1?) that uses this range
        int16_t nextAnimation = file.read16(); // Animation to go to
        int16_t nextFrame = file.read16(); // Frame offset to go to

        animDispatches.emplace_back(low, high, nextAnimation, nextFrame);
    }

    if (numAnimDispatches > 0)
        getLog() << "LoaderTR2: Found " << numAnimDispatches << " AnimationDispatches!" << Log::endl;
    else
        getLog() << "LoaderTR2: No AnimationDispatches in this level?!" << Log::endl;

    uint32_t numAnimCommands = file.readU32();
    std::vector<int16_t> animCommands;
    for (unsigned int a = 0; a < numAnimCommands; a++) {
        // A list of Opcodes with zero or more operands each,
        // some referring to the whole animation (jump/grab points),
        // some to specific frames (sound, bubbles, ...).
        animCommands.push_back(file.read16());
    }

    if (numAnimCommands > 0)
        getLog() << "LoaderTR2: Found " << numAnimCommands << " AnimationCommands!" << Log::endl;
    else
        getLog() << "LoaderTR2: No AnimationCommands in this level?!" << Log::endl;

    // This is really one uint32_t flags, followed by
    // three int32_t x, y, z. However, we're given the number
    // of 32bits, as well as byte indices later, so we store
    // it as a single list of int32_t.
    uint32_t numMeshTrees = file.readU32();
    std::vector<int32_t> meshTrees;
    for (unsigned int m = 0; m < numMeshTrees; m++) {
        // 0x0002 - Put parent mesh on the mesh stack
        // 0x0001 - Pop mesh from stack, use as parent mesh
        // When both are not set, use previous mesh as parent mesh
        // When both are set, do 0x0001 first, then 0x0002, thereby
        // reading the stack but not changing it
        //uint32_t flags = file.readU32();

        // Offset of mesh origin from the parent mesh origin
        //int32_t x = file.read32();
        //int32_t y = file.read32();
        //int32_t z = file.read32();

        meshTrees.push_back(file.read32());
    }

    if (numMeshTrees > 0)
        getLog() << "LoaderTR2: Found " << numMeshTrees << " MeshTrees!" << Log::endl;
    else
        getLog() << "LoaderTR2: No MeshTrees in this level?!" << Log::endl;

    uint32_t numFrames = file.readU32();
    std::vector<uint16_t> frames;
    for (unsigned int f = 0; f < numFrames; f++) {
        // int16 bb1x, bb1y, bb1z
        // int16 bb2x, bb2y, bb2z
        // int16 offsetX, offsetY, offsetZ
        // What follows next is a list of angles with numMeshes (from Moveable) entries.
        // If the top bit (0x8000) of the first uint16 is set, a single X angle follows,
        // if the second bit (0x4000) is set, a Y angle follows, both are a Z angle.
        // If none is set, it's a three-axis rotation. The next 10 bits (0x3FF0) are
        // the X rotation, the next 10 (0x000F 0xFC00) are Y, the next (0x03FF) are
        // the Z rotation. The scaling is always 0x100->90deg.
        // Rotation order: Y, X, Z!
        frames.push_back(file.readU16());
    }

    if (numFrames > 0)
        getLog() << "LoaderTR2: Found " << numFrames << " Frames!" << Log::endl;
    else
        getLog() << "LoaderTR2: No Frames in this level?!" << Log::endl;

    uint32_t numMoveables = file.readU32();
    for (unsigned int m = 0; m < numMoveables; m++) {
        // Item identifier, matched in Items[]
        uint32_t objectID = file.readU32();
        uint16_t numMeshes = file.readU16();
        uint16_t startingMesh = file.readU16(); // Offset into MeshPointers[]
        uint32_t meshTree = file.readU32(); // Offset into MeshTree[]
        // *Byte* offset into Frames[] (divide by 2 for Frames[i])
        uint32_t frameOffset = file.readU32(); // Only needed if no animation

        // If animation index is 0xFFFF, the object is stationary or
        // animated by the engine (ponytail)
        uint16_t animation = file.readU16();

        // TODO load all animations, not only the first frame!
        //if (animation == 0xFFFF) {

        // Just add the frame indicated in frameOffset, nothing else
        char* tmp = reinterpret_cast<char*>(&frames[0]) + frameOffset;
        BinaryMemory frame(tmp + 12, (numFrames * 2) - frameOffset - 12); // skip two BBs
        float pos[3];
        pos[0] = frame.read16();
        pos[1] = frame.read16();
        pos[2] = frame.read16();
        BoneFrame* bf = new BoneFrame(pos);

        for (int i = 0; i < numMeshes; i++) {
            int mesh = startingMesh + i;
            float offset[3] = { 0.0f, 0.0f, 0.0f };
            float rotation[3] = { 0.0f, 0.0f, 0.0f };
            char flag = (i == 0) ? 2 : 0;

            // Nonprimary tag - positioned relative to first tag
            if (i != 0) {
                tmp = reinterpret_cast<char*>(&meshTrees[0]) + meshTree; // TODO (meshTree * 4)?
                tmp += (i - 1) * 16; // TODO ?
                BinaryMemory tree(tmp, (numMeshTrees * 4) - meshTree - ((i - 1) * 16));
                flag = (char)tree.readU32();
                offset[0] = tree.read32();
                offset[1] = tree.read32();
                offset[2] = tree.read32();

                uint16_t a = frame.readU16();
                if (a & 0xC000) {
                    // Single angle
                    int index = 0;
                    if ((a & 0x8000) && (a & 0x4000))
                        index = 2;
                    else if (a & 0x4000)
                        index = 1;
                    rotation[index] = ((float)(a & 0x03FF)) * 360.0f / 1024.0f;
                } else {
                    // Three angles
                    uint16_t b = frame.readU16();
                    rotation[0] = (a & 0x3FF0) >> 4;
                    rotation[1] = ((a & 0x000F) << 6) | ((b & 0xFC00) >> 10);
                    rotation[2] = b & 0x03FF;
                    for (int i = 0; i < 3; i++)
                        rotation[i] = rotation[i] * 360.0f / 1024.0f;
                }
            }

            BoneTag* bt = new BoneTag(mesh, offset, rotation, flag);
            bf->add(bt);
        }

        AnimationFrame* af = new AnimationFrame(0);
        af->add(bf);

        SkeletalModel* sm = new SkeletalModel(objectID);
        sm->add(af);
        getWorld().addSkeletalModel(sm);

        //} else {
        // Add the whole animation hierarchy
        //}
    }

    if (numMoveables > 0)
        getLog() << "LoaderTR2: Found " << numMoveables << " Moveables!" << Log::endl;
    else
        getLog() << "LoaderTR2: No Moveables in this level?!" << Log::endl;
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

        uint16_t angle = file.readU16(); // (0xC000 >> 14) * 90deg
        int16_t intensity1 = file.read16(); // Constant lighting; -1 means mesh lighting
        int16_t intensity2 = file.read16(); // Almost always like intensity1

        // 0x0100 - Initially visible
        // 0x3E00 - Activation mask, open, can be XORed with related FloorData list fields.
        uint16_t flags = file.readU16();

        // TODO for now we're only creating Entities for each Moveable Item
        for (int m = 0; m < getWorld().sizeSkeletalModel(); m++) {
            if (getWorld().getSkeletalModel(m).getId() == objectID) {
                float pos[3] = {
                    static_cast<float>(x),
                    static_cast<float>(y),
                    static_cast<float>(z)
                };

                float rot[3] = {
                    0.0f,
                    glm::radians(((angle >> 14) & 0x03) * 90.0f),
                    0.0f
                };

                Entity* e = new Entity(pos, rot, objectID, room, m);
                getWorld().addEntity(e);

                if (objectID == 0) {
                    getGame().setLara(getWorld().sizeEntity() - 1);
                }
            }
        }
    }

    if (numItems > 0)
        getLog() << "LoaderTR2: Found " << numItems << " Items!" << Log::endl;
    else
        getLog() << "LoaderTR2: No Items in this level?!" << Log::endl;
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
        uint16_t overlapIndex = file.readU16();

        // TODO store boxes somewhere
    }

    uint32_t numOverlaps = file.readU32();
    std::vector<std::vector<uint16_t>> overlaps;
    overlaps.emplace_back();
    unsigned int list = 0;
    for (unsigned int o = 0; o < numOverlaps; o++) {
        // Apparently used by NPCs to decide where to go next.
        // List of neighboring boxes for each box.
        // Each entry is a uint16, 0x8000 set marks end of list.
        uint16_t e = file.readU16();
        overlaps.at(list).push_back(e);
        if (e & 0x8000) {
            overlaps.emplace_back();
            list++;
        }
    }

    // TODO store overlaps somewhere

    for (unsigned int z = 0; z < numBoxes; z++) {
        // Normal room state
        int16_t ground1 = file.read16();
        int16_t ground2 = file.read16();
        int16_t ground3 = file.read16();
        int16_t ground4 = file.read16();
        int16_t fly = file.read16();

        // Alternate room state
        int16_t ground1alt = file.read16();
        int16_t ground2alt = file.read16();
        int16_t ground3alt = file.read16();
        int16_t ground4alt = file.read16();
        int16_t flyAlt = file.read16();

        // TODO store zones somewhere
    }

    if ((numBoxes > 0) || (numOverlaps > 0))
        getLog() << "LoaderTR2: Found NPC NavigationHints (" << numBoxes
                 << ", " << numOverlaps << ", " << list << "), unimplemented!" << Log::endl;
    else
        getLog() << "LoaderTR2: No NPC NavigationHints in this level?!" << Log::endl;
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
    else
        getLog() << "LoaderTR2: No SoundSources in this level?!" << Log::endl;
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
    else
        getLog() << "LoaderTR2: No SoundDetails in this level?!" << Log::endl;
}

void LoaderTR2::loadSampleIndices() {
    uint32_t numSampleIndices = file.readU32();
    for (unsigned int i = 0; i < numSampleIndices; i++) {
        SoundManager::addSampleIndex(file.readU32());
    }

    if (numSampleIndices > 0)
        getLog() << "LoaderTR2: Found " << numSampleIndices << " SampleIndices" << Log::endl;
    else
        getLog() << "LoaderTR2: No SampleIndices in this level?!" << Log::endl;
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

        int ret = Sound::loadBuffer(buff, riffSize + 8);
        assert(ret >= 0);

        riffCount++;
    }

    if (riffCount > 0)
        getLog() << "LoaderTR2: Found " << riffCount << " SoundSamples in SFX" << Log::endl;
    else
        getLog() << "LoaderTR2: No SoundSamples in SFX?!" << Log::endl;
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
                 << " CinematicFrames, unimplemented!" << Log::endl;
}

void LoaderTR2::loadDemoData() {
    uint16_t numDemoData = file.readU16();
    for (unsigned int d = 0; d < numDemoData; d++)
        file.readU8();

    // TODO store demo data somewhere, find out meaning
    if (numDemoData > 0)
        getLog() << "LoaderTR2: Found " << numDemoData << " bytes DemoData, unimplemented!" <<
                 Log::endl;
}

