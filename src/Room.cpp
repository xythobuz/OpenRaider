/*!
 * \file src/Room.cpp
 * \brief World Room Mesh
 *
 * \author xythobuz
 */

#include <algorithm>
#include <assert.h>

#include "main.h"
#include "Room.h"

Room::Room(TombRaider &tr, unsigned int index) {
    Matrix transform;

    if (!tr.isRoomValid(index)) {
        getConsole().print("WARNING: Handling invalid vertex array in room");
        //printf("x");
        //fflush(stdout);
        return;
    }

    tr.getRoomInfo(index, &flags, pos, bbox[0], bbox[1]);

    // Adjust positioning for OR world coordinate translation
    bbox[0][0] += pos[0];
    bbox[1][0] += pos[0];
    bbox[0][2] += pos[2];
    bbox[1][2] += pos[2];

    // Mongoose 2002.04.03, Setup 3D transform
    transform.setIdentity();
    transform.translate(pos);

    // Current room is always first
    adjacentRooms.push_back(index);

    // Setup portals
    unsigned int count = tr.getRoomPortalCount(index);
    for (unsigned int i = 0; i < count; i++) {
        portals.push_back(new Portal(tr, index, i, transform));
        adjacentRooms.push_back(portals.back()->getAdjoiningRoom());
    }

    //! \fixme Use more of sector structure, boxes, and floordata

    // List of sectors in this room
    count = tr.getRoomSectorCount(index, &numZSectors, &numXSectors);
    for (unsigned int i = 0; i < count; i++)
        sectors.push_back(new Sector(tr, index, i));

    // Setup collision boxes
    //! fixme Should use sectors, but this is a test
    count = tr.getRoomBoxCount(index);
    for (unsigned int i = 0; i < count; i++)
        boxes.push_back(new Box(tr, index, i));

    // Setup room lights
    count = tr.getRoomLightCount(index);
    for (unsigned int i = 0; i < count; i++)
        lights.push_back(new Light(tr, index, i));

    // Room models
    count = tr.getRoomModelCount(index);
    for (unsigned int i = 0; i < count; i++)
        models.push_back(new StaticModel(tr, index, i));

    // Room sprites
    count = tr.getRoomSpriteCount(index);
    for (unsigned int i = 0; i < count; i++)
        sprites.push_back(new Sprite(tr, index, i));

#define EXPERIMENTAL_UNIFIED_ROOM_GEOMETERY
#ifdef EXPERIMENTAL_UNIFIED_ROOM_GEOMETERY
    unsigned int vertexCount, normalCount, colorCount, triCount;
    vec_t *vertexArray;
    vec_t *normalArray;
    vec_t *colorArray;
    unsigned int *indices, *fflags;
    float *texCoords;
    int *textures;

    tr.getRoomVertexArrays(index,
            &vertexCount, &vertexArray,
            &normalCount, &normalArray,
            &colorCount, &colorArray);

    mesh.bufferVertexArray(vertexCount, vertexArray);
    mesh.bufferNormalArray(normalCount, normalArray);
    mesh.bufferColorArray(vertexCount, colorArray);

    tr.getRoomTriangles(index, getGame().getTextureStart(),
            &triCount, &indices, &texCoords, &textures,
            &fflags);

    mesh.bufferTriangles(triCount, indices, texCoords, textures, fflags);
#else
    const unsigned int TextureLimit = 24;
    float rgba[4];
    float xyz[3];

    count = tr.getRoomVertexCount(index);
    mesh.allocateVertices(count);
    mesh.allocateNormals(0); // count
    mesh.allocateColors(count);

    for (unsigned int i = 0; i < count; ++i)
    {
        tr.getRoomVertex(index, i, xyz, rgba);

        mesh.setVertex(i, xyz[0], xyz[1], xyz[2]);
        mesh.setColor(i, rgba);
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

    for (unsigned int i = 0; i < TextureLimit; ++i)
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
    unsigned int r, t, q, v;
    unsigned int indices[4];
    float texCoords[8];

    count = tr.getRoomTriangleCount(index);

    // Mongoose 2002.08.15, Presort by alpha and texture and setup mapping
    for (t = 0; t < count; ++t)
    {
        tr.getRoomTriangle(index, t,
                indices, texCoords, &texture, &flags);

        texture += getGame().getTextureStart();

        if (texture > (int)TextureLimit)
        {
            getConsole().print("Handling bad room[%i].tris[%i].texture = %i",
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

    count = tr.getRoomRectangleCount(index);

    for (r = 0; r < count; ++r)
    {
        tr.getRoomRectangle(index, r,
                indices, texCoords, &texture, &flags);

        texture += getGame().getTextureStart();

        if (texture > (int)TextureLimit)
        {
            getConsole().print("Handling bad room[%i].quad[%i].texture = %i",
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
    mesh.allocateTriangles(numTris);
    mesh.allocateRectangles(numQuads);

    for (unsigned int i = 0, j = 0; i < TextureLimit; ++i)
    {
        if (tris_mesh_map[i] > 0)
        {
            j = tris_mesh_map[i] - 1;

            t = triangle_counter[i];

            mesh.mTris[j].texture = i;
#ifdef MULTITEXTURE
            mesh.mTris[j].bumpmap = gMapTex2Bump[i];
#endif
            mesh.mTris[j].cnum_triangles = 0;
            mesh.mTris[j].num_triangles = 0;
            mesh.mTris[j].cnum_alpha_triangles = 0;
            mesh.mTris[j].num_alpha_triangles = 0;
            mesh.mTris[j].triangles = 0x0;
            mesh.mTris[j].alpha_triangles = 0x0;
            mesh.mTris[j].texcoors = 0x0;
            mesh.mTris[j].texcoors2 = 0x0;

            if (t > 0)
            {
                mesh.mTris[j].num_triangles = t;
                mesh.mTris[j].triangles = new unsigned int[t*3];
                mesh.mTris[j].num_texcoors = t * 3;
                mesh.mTris[j].texcoors = new vec2_t[t * 3];
            }

            t = triangle_counter_alpha[i];

            if (t > 0)
            {
                mesh.mTris[j].num_alpha_triangles = t;
                mesh.mTris[j].alpha_triangles = new unsigned int[t*3];
                mesh.mTris[j].num_texcoors2 = t * 3;
                mesh.mTris[j].texcoors2 = new vec2_t[t * 3];
            }
        }

        ///////////////////////////////////////////

        if (rect_mesh_map[i] > 0)
        {
            j = rect_mesh_map[i] - 1;

            r = rectangle_counter[i];

            mesh.mQuads[j].texture = i;
#ifdef MULTITEXTURE
            mesh.mQuads[j].bumpmap = gMapTex2Bump[i];
#endif
            mesh.mQuads[j].cnum_quads = 0;
            mesh.mQuads[j].num_quads = 0;
            mesh.mQuads[j].cnum_alpha_quads = 0;
            mesh.mQuads[j].num_alpha_quads = 0;
            mesh.mQuads[j].quads = 0x0;
            mesh.mQuads[j].alpha_quads = 0x0;
            mesh.mQuads[j].texcoors = 0x0;
            mesh.mQuads[j].texcoors2 = 0x0;

            if (r > 0)
            {
                mesh.mQuads[j].num_quads = r;
                mesh.mQuads[j].quads = new unsigned int[r*4];
                mesh.mQuads[j].num_texcoors = r * 4;
                mesh.mQuads[j].texcoors = new vec2_t[r * 4];
            }

            r = rectangle_counter_alpha[i];

            if (r > 0)
            {
                mesh.mQuads[j].num_alpha_quads = r;
                mesh.mQuads[j].alpha_quads = new unsigned int[r*4];
                mesh.mQuads[j].num_texcoors2 = r * 4;
                mesh.mQuads[j].texcoors2 = new vec2_t[r * 4];
            }
        }
    }

    // Generate textured triangles
    count = tr.getRoomTriangleCount(index);

    for (t = 0; t < count; ++t)
    {
        tr.getRoomTriangle(index, t,
                indices, texCoords, &texture, &flags);

        // Adjust texture id using getGame().getTextureStart() to map into
        // correct textures
        texture += getGame().getTextureStart();

        unsigned int j = tris_mesh_map[texture] - 1;

        // Setup per vertex
        for (unsigned int i = 0; i < 3; ++i)
        {
            // Get vertex index {(0, a), (1, b), (2, c)}
            v = indices[i];

            if ((flags & tombraiderFace_Alpha ||
                        flags & tombraiderFace_PartialAlpha) &&
                    mesh.mTris[j].num_alpha_triangles > 0)
            {
                q = mesh.mTris[j].cnum_alpha_triangles*3+i;

                mesh.mTris[j].alpha_triangles[q] = v;

                mesh.mTris[j].texcoors2[q][0] = texCoords[i*2];
                mesh.mTris[j].texcoors2[q][1] = texCoords[i*2+1];
            }
            else if (mesh.mTris[j].num_triangles > 0)
            {
                q = mesh.mTris[j].cnum_triangles*3+i;

                mesh.mTris[j].triangles[q] = v;

                mesh.mTris[j].texcoors[q][0] = texCoords[i*2];
                mesh.mTris[j].texcoors[q][1] = texCoords[i*2+1];
            }

            // Partial alpha hack
            if (flags & tombraiderFace_PartialAlpha)
            {
                //mesh.colors[v].rgba[3] = 0.45;
            }
        }

        if (flags & tombraiderFace_Alpha ||
                flags & tombraiderFace_PartialAlpha)
        {
            mesh.mTris[j].cnum_alpha_triangles++;
        }
        else
        {
            mesh.mTris[j].cnum_triangles++;
        }
    }

    // Generate textured quads
    count = tr.getRoomRectangleCount(index);

    for (r = 0; r < count; ++r)
    {
        tr.getRoomRectangle(index, r,
                indices, texCoords, &texture, &flags);

        // Adjust texture id using getGame().getTextureStart() to map into
        // correct textures
        texture += getGame().getTextureStart();

        if (texture > (int)TextureLimit)
        {
            texture = TextureLimit - 1;
        }

        unsigned int j = rect_mesh_map[texture] - 1;

        if (mesh.mQuads[j].num_quads <= 0 &&
                mesh.mQuads[j].num_alpha_quads <= 0)
            continue;

        // Setup per vertex
        for (unsigned int i = 0; i < 4; ++i)
        {
            // Get vertex index {(0, a), (1, b), (2, c), (3, d)}
            v = indices[i];

            if ((flags & tombraiderFace_Alpha ||
                        flags & tombraiderFace_PartialAlpha) &&
                    mesh.mQuads[j].num_alpha_quads > 0)
            {
                q = mesh.mQuads[j].cnum_alpha_quads*4+i;

                mesh.mQuads[j].alpha_quads[q] = v;

                mesh.mQuads[j].texcoors2[q][0] = texCoords[i*2];
                mesh.mQuads[j].texcoors2[q][1] = texCoords[i*2+1];
            }
            else if (mesh.mQuads[j].num_quads > 0)
            {
                q = mesh.mQuads[j].cnum_quads*4+i;

                mesh.mQuads[j].quads[q] = v;

                mesh.mQuads[j].texcoors[q][0] = texCoords[i*2];
                mesh.mQuads[j].texcoors[q][1] = texCoords[i*2+1];
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
            mesh.mQuads[j].cnum_alpha_quads++;
        }
        else
        {
            mesh.mQuads[j].cnum_quads++;
        }
    }
#endif
}

#define EMPTY_VECTOR(x)     \
while (!x.empty()) {        \
    delete x[x.size() - 1]; \
    x.pop_back();           \
}

Room::~Room() {
    EMPTY_VECTOR(sprites);
    EMPTY_VECTOR(models);
    EMPTY_VECTOR(portals);
    EMPTY_VECTOR(boxes);
    EMPTY_VECTOR(sectors);
    EMPTY_VECTOR(lights);
}

unsigned int Room::getFlags() {
    return flags;
}

unsigned int Room::getNumXSectors() {
    return numXSectors;
}

unsigned int Room::getNumZSectors() {
    return numZSectors;
}

void Room::getPos(vec3_t p) {
    for (unsigned int i = 0; i < 3; i++)
        p[i] = pos[i];
}

void Room::getBoundingBox(vec3_t box[2]) {
    for (unsigned int i = 0; i < 2; i++)
        for (unsigned int j = 0; j < 3; j++)
            box[i][j] = bbox[i][j];
}

bool Room::inBox(vec_t x, vec_t y, vec_t z) {
    return ((y > bbox[0][1]) && (y < bbox[1][1]) && inBoxPlane(x, z));
}

bool Room::inBoxPlane(vec_t x, vec_t z) {
    return ((x > bbox[0][0]) && (x < bbox[1][0])
            && (z > bbox[0][2]) && (z < bbox[1][2]));
}

unsigned int Room::sizeAdjacentRooms() {
    return adjacentRooms.size();
}

int Room::getAdjacentRoom(unsigned int index) {
    assert(index < adjacentRooms.size());
    return adjacentRooms.at(index);
}

unsigned int Room::sizePortals() {
    return portals.size();
}

Portal &Room::getPortal(unsigned int index) {
    assert(index < portals.size());
    return *portals.at(index);
}

unsigned int Room::sizeSectors() {
    return sectors.size();
}

Sector &Room::getSector(unsigned int index) {
    assert(index < sectors.size());
    return *sectors.at(index);
}

unsigned int Room::sizeBox() {
    return boxes.size();
}

Box &Room::getBox(unsigned int index) {
    assert(index < boxes.size());
    return *boxes.at(index);
}

unsigned int Room::sizeModels() {
    return models.size();
}

StaticModel &Room::getModel(unsigned int index) {
    assert(index < models.size());
    return *models.at(index);
}

void Room::sortModels() {
    std::sort(models.begin(), models.end());
}

unsigned int Room::sizeLights() {
    return lights.size();
}

Light &Room::getLight(unsigned int index) {
    assert(index < lights.size());
    return *lights.at(index);
}

unsigned int Room::sizeSprites() {
    return sprites.size();
}

Sprite &Room::getSprite(unsigned int index) {
    assert(index < sprites.size());
    return *sprites.at(index);
}

Mesh &Room::getMesh() {
    return mesh;
}

