/*!
 * \file src/loader/LoaderTR1.cpp
 * \brief TR1 level file loader
 *
 * \author xythobuz
 */

#include "global.h"
#include "Game.h"
#include "Log.h"
#include "SoundManager.h"
#include "World.h"
#include "utils/strings.h"
#include "loader/LoaderTR1.h"

int LoaderTR1::load(std::string f) {
    if (file.open(f) != 0) {
        return 1; // Could not open file
    }

    uint32_t version = file.readU32();
    if (version != 0x20) {
        return 2; // Not a TR1 level?!
    }

    bool unfinishedBusiness = stringEndsWith(f, ".tub");
    if (unfinishedBusiness)
        Log::get(LOG_INFO) << "LoaderTR1: Detected Unfinished Business level!" << Log::endl;

    loadTextures();

    file.seek(file.tell() + 4); // Unused value?

    loadRooms();
    loadFloorData();
    loadMeshes();
    loadMoveables();
    loadStaticMeshes();
    loadTextiles();
    loadSprites();

    if (unfinishedBusiness)
        loadPalette();

    loadCameras();
    loadSoundSources();
    loadBoxesOverlapsZones();
    loadAnimatedTextures();
    loadItems();

    file.seek(file.tell() + 8192); // TODO light map!

    if (!unfinishedBusiness)
        loadPalette();

    loadCinematicFrames();
    loadDemoData();
    loadSoundMap();
    loadSoundDetails();
    loadSoundSamples();

    return 0;
}

void LoaderTR1::loadPalette() {
    // Read the 8bit palette, 256 * 3 bytes, RGB
    for (int i = 0; i < 256; i++) {
        uint8_t r = file.readU8();
        uint8_t g = file.readU8();
        uint8_t b = file.readU8();

        // Color values range from 0 to 63, so multiply by 4
        static const uint8_t lightFactor = 4;
        r *= lightFactor;
        g *= lightFactor;
        b *= lightFactor;

        glm::vec4 c(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
        TextureManager::setPalette(i, c);
    }
}

void LoaderTR1::loadTextures() {
    uint32_t numTextures = file.readU32();
    for (unsigned int i = 0; i < numTextures; i++) {
        std::array<uint8_t, 256 * 256> arr;
        for (auto& x : arr) {
            x = file.readU8(); // Palette index
        }

        TextureManager::addIndexedTexture(&arr[0], 256, 256);
    }

    if (numTextures > 0)
        Log::get(LOG_INFO) << "LoaderTR1: Found " << numTextures << " Textures!" << Log::endl;
    else
        Log::get(LOG_INFO) << "LoaderTR1: No Textures in this level?!" << Log::endl;
}

void LoaderTR1::loadRoomLights() {
    int16_t intensity = file.read16();

    uint16_t numLights = file.readU16();
    for (unsigned int l = 0; l < numLights; l++) {
        // Position of light, in world coordinates
        int32_t x = file.read32();
        int32_t y = file.read32();
        int32_t z = file.read32();

        uint16_t intensity1 = file.readU16();
        uint32_t fade = file.readU32(); // Falloff value?

        // TODO store light somewhere
    }
}

void LoaderTR1::loadRoomStaticMeshes(std::vector<StaticModel*>& staticModels) {
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
        //! \fixme Use static mesh lighting information
        uint16_t intensity1 = file.readU16();

        // Which StaticMesh item to draw
        uint16_t objectID = file.readU16();

        staticModels.push_back(new StaticModel(glm::vec3(x, y, z),
                                               glm::radians((rotation >> 14) * 90.0f),
                                               objectID));
    }
}

void LoaderTR1::loadRoomVertex(RoomVertexTR2& vert) {
    vert.x = file.read16();
    vert.y = file.read16();
    vert.z = file.read16();
    vert.light1 = file.read16();
    vert.attributes = 0;
    vert.light2 = vert.light1;
}

void LoaderTR1::loadItems() {
    uint32_t numItems = file.readU32();
    for (unsigned int i = 0; i < numItems; i++) {
        int16_t objectID = file.read16();
        int16_t room = file.read16();

        // Item position in world coordinates
        int32_t x = file.read32();
        int32_t y = file.read32();
        int32_t z = file.read32();

        uint16_t angle = file.readU16(); // (0xC000 >> 14) * 90deg
        int16_t intensity = file.read16(); // Constant lighting; -1 means mesh lighting

        // 0x0100 - Initially visible
        // 0x3E00 - Activation mask, open, can be XORed with related FloorData list fields.
        uint16_t flags = file.readU16();

        glm::vec3 pos(
            static_cast<float>(x),
            static_cast<float>(y),
            static_cast<float>(z)
        );

        glm::vec3 rot(
            0.0f,
            glm::radians(((angle >> 14) & 0x03) * 90.0f),
            0.0f
        );

        Entity* e = new Entity(objectID, room, pos, rot);
        getWorld().addEntity(e);

        if (objectID == 0) {
            Game::setLara(getWorld().sizeEntity() - 1);
        }
    }

    if (numItems > 0)
        Log::get(LOG_INFO) << "LoaderTR1: Found " << numItems << " Items!" << Log::endl;
    else
        Log::get(LOG_INFO) << "LoaderTR1: No Items in this level?!" << Log::endl;
}

void LoaderTR1::loadBoxesOverlapsZones() {
    uint32_t numBoxes = file.readU32();
    for (unsigned int b = 0; b < numBoxes; b++) {
        // Sectors (not scaled!)
        int32_t zMin = file.read32();
        int32_t zMax = file.read32();
        int32_t xMin = file.read32();
        int32_t xMax = file.read32();

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
        int16_t fly = file.read16();

        // Alternate room state
        int16_t ground1alt = file.read16();
        int16_t ground2alt = file.read16();
        int16_t flyAlt = file.read16();

        // TODO store zones somewhere
    }

    if ((numBoxes > 0) || (numOverlaps > 0))
        Log::get(LOG_INFO) << "LoaderTR1: Found NPC NavigationHints (" << numBoxes
                           << ", " << numOverlaps << ", " << list << "), unimplemented!" << Log::endl;
    else
        Log::get(LOG_INFO) << "LoaderTR1: No NPC NavigationHints in this level?!" << Log::endl;
}

void LoaderTR1::loadSoundMap() {
    for (int i = 0; i < 256; i++) {
        SoundManager::addSoundMapEntry(file.read16());
    }
}

void LoaderTR1::loadSoundSamples() {
    uint32_t soundSampleSize = file.readU32();
    std::vector<uint8_t> buffer;
    for (int i = 0; i < soundSampleSize; i++) {
        buffer.push_back(file.readU8());
    }

    uint32_t numSampleIndices = file.readU32();
    for (unsigned int i = 0; i < numSampleIndices; i++) {
        SoundManager::addSampleIndex(i);
        uint32_t sampleOffset = file.readU32();
        assertLessThan(sampleOffset, soundSampleSize);
        char* tmpPtr = reinterpret_cast<char*>(&buffer[sampleOffset]);
        BinaryMemory sample(tmpPtr, soundSampleSize - sampleOffset);
        int ret = loadSoundFiles(sample, 1);
        assertEqual(ret, 1);
    }

    if (numSampleIndices > 0)
        Log::get(LOG_INFO) << "LoaderTR1: Found " << numSampleIndices << " SoundSamples" << Log::endl;
    else
        Log::get(LOG_INFO) << "LoaderTR1: No SoundSamples in this level?!" << Log::endl;
}

int LoaderTR1::getPaletteIndex(uint16_t index) {
    return index;
}

void LoaderTR1::loadAngleSet(BoneFrame* bf, BinaryReader& frame, uint16_t numMeshes,
                             uint16_t startingMesh, uint32_t meshTree,
                             uint32_t numMeshTrees, std::vector<int32_t> meshTrees) {
    /*! \fixme
     * The TR Rosetta Stone documentation says:
     *     number of angle sets to follow;
     *     these start with the first mesh, and meshes
     *     without angles get zero angles.
     * I'm not sure what this means. This code may be wrong.
     */
    uint16_t numValues = frame.readU16();

    for (int i = 0; i < numValues; i++) {
        int mesh = startingMesh + i;
        glm::vec3 offset(0.0f, 0.0f, 0.0f);
        float rotation[3] = { 0.0f, 0.0f, 0.0f };
        char flag = (i == 0) ? 2 : 0;

        // Nonprimary tag - positioned relative to first tag
        if (i != 0) {
            char* tmp = reinterpret_cast<char*>(&meshTrees[0]) + meshTree; // TODO (meshTree * 4)?
            tmp += (i - 1) * 16; // TODO ?
            BinaryMemory tree(tmp, (numMeshTrees * 4) - meshTree - ((i - 1) * 16));

            flag = (char)tree.readU32();
            offset.x = tree.read32();
            offset.y = tree.read32();
            offset.z = tree.read32();

            uint16_t b = frame.readU16();
            uint16_t a = frame.readU16();
            rotation[0] = (a & 0x3FF0) >> 4;
            rotation[1] = ((a & 0x000F) << 6) | ((b & 0xFC00) >> 10);
            rotation[2] = b & 0x03FF;
            for (int i = 0; i < 3; i++)
                rotation[i] = rotation[i] * 360.0f / 1024.0f;
        }

        glm::vec3 rot(rotation[0], rotation[1], rotation[2]);
        BoneTag* bt = new BoneTag(mesh, offset, rot, flag);
        bf->add(bt);
    }
}

