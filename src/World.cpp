/*!
 * \file src/World.cpp
 * \brief World Model
 *
 * \author Mongoose
 */

#include "global.h"
#include "World.h"

#include "imgui/imgui.h"

std::vector<std::unique_ptr<Room>> World::rooms;
std::vector<std::unique_ptr<Sprite>> World::sprites;
std::vector<std::unique_ptr<SpriteSequence>> World::spriteSequences;
std::vector<std::unique_ptr<Entity>> World::entities;
std::vector<std::unique_ptr<SkeletalModel>> World::models;
std::vector<std::unique_ptr<StaticMesh>> World::staticMeshes;
std::vector<std::unique_ptr<Mesh>> World::meshes;

void World::destroy() {
    rooms.clear();
    sprites.clear();
    spriteSequences.clear();
    entities.clear();
    models.clear();
    staticMeshes.clear();
    meshes.clear();
}

void World::addRoom(Room* room) {
    rooms.emplace_back(std::unique_ptr<Room>(room));
}

unsigned long World::sizeRoom() {
    return rooms.size();
}

Room& World::getRoom(unsigned long index) {
    orAssertLessThan(index, rooms.size());
    return *rooms.at(index);
}

void World::addSprite(Sprite* sprite) {
    sprites.emplace_back(std::unique_ptr<Sprite>(sprite));
}

unsigned long World::sizeSprite() {
    return sprites.size();
}

Sprite& World::getSprite(unsigned long index) {
    orAssertLessThan(index, sprites.size());
    return *sprites.at(index);
}

void World::addSpriteSequence(SpriteSequence* sprite) {
    spriteSequences.emplace_back(std::unique_ptr<SpriteSequence>(sprite));
}

unsigned long World::sizeSpriteSequence() {
    return spriteSequences.size();
}

SpriteSequence& World::getSpriteSequence(unsigned long index) {
    orAssertLessThan(index, spriteSequences.size());
    return *spriteSequences.at(index);
}

void World::addEntity(Entity* entity) {
    entities.emplace_back(std::unique_ptr<Entity>(entity));
}

unsigned long World::sizeEntity() {
    return entities.size();
}

Entity& World::getEntity(unsigned long index) {
    orAssertLessThan(index, entities.size());
    return *entities.at(index);
}

void World::addSkeletalModel(SkeletalModel* model) {
    models.emplace_back(std::unique_ptr<SkeletalModel>(model));
}

unsigned long World::sizeSkeletalModel() {
    return models.size();
}

SkeletalModel& World::getSkeletalModel(unsigned long index) {
    orAssertLessThan(index, models.size());
    return *models.at(index);
}

void World::addStaticMesh(StaticMesh* model) {
    staticMeshes.emplace_back(std::unique_ptr<StaticMesh>(model));
}

unsigned long World::sizeStaticMesh() {
    return staticMeshes.size();
}

StaticMesh& World::getStaticMesh(unsigned long index) {
    orAssertLessThan(index, staticMeshes.size());
    return *staticMeshes.at(index);
}

void World::addMesh(Mesh* mesh) {
    meshes.emplace_back(mesh);
}

unsigned long World::sizeMesh() {
    return meshes.size();
}

Mesh& World::getMesh(unsigned long index) {
    orAssertLessThan(index, meshes.size());
    return *meshes.at(index);
}

void World::displayUI() {
    // Rooms
    static bool offsets = false;
    if (ImGui::CollapsingHeader("Rooms")) {
        ImGui::Columns(6, "rooms");
        ImGui::Text("No");
        ImGui::NextColumn();
        ImGui::Text("Ind.");
        ImGui::NextColumn();
        ImGui::Text("Alt.");
        ImGui::NextColumn();
        ImGui::Text("Flags");
        ImGui::NextColumn();
        ImGui::Text("Models");
        ImGui::NextColumn();
        ImGui::Text("Portals");
        ImGui::NextColumn();
        ImGui::Separator();
        if (!offsets) {
            ImGui::SetColumnOffset(1, 40.0f);
            ImGui::SetColumnOffset(2, 80.0f);
            ImGui::SetColumnOffset(3, 120.0f);
            ImGui::SetColumnOffset(4, 180.0f);
            ImGui::SetColumnOffset(5, 300.0f);
            offsets = true;
        }
        for (int i = 0; i < rooms.size(); i++) {
            ImGui::Text("%03d", i);
            ImGui::NextColumn();
            rooms.at(i)->displayUI();
        }
        ImGui::Columns(1);
    }

    // Static Meshes
    static bool offsets2 = false;
    if (ImGui::CollapsingHeader("StaticMeshes")) {
        ImGui::Columns(3, "staticmeshes");
        ImGui::Text("No");
        ImGui::NextColumn();
        ImGui::Text("ID");
        ImGui::NextColumn();
        ImGui::Text("Mesh");
        ImGui::NextColumn();
        ImGui::Separator();
        if (!offsets2) {
            ImGui::SetColumnOffset(1, 40.0f);
            ImGui::SetColumnOffset(2, 80.0f);
            offsets2 = true;
        }
        for (int i = 0; i < staticMeshes.size(); i++) {
            ImGui::Text("%03d", i);
            ImGui::NextColumn();
            staticMeshes.at(i)->displayUI();
        }
        ImGui::Columns(1);
    }
}

