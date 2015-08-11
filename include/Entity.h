/*!
 * \file include/Entity.h
 * \brief Things in the World
 *
 * \author xythobuz
 */

#ifndef _ENTITY_H_
#define _ENTITY_H_

class Entity {
  public:
    Entity(int i, int r, glm::vec3 po, glm::vec3 ro)
        : id(i), room(r), pos(po), rot(ro), cache(-1), cacheType(-1),
          sprite(0), animation(0), frame(0) { }
    void display(glm::mat4 VP);
    void displayUI();

    int getID() { return id; }
    int getRoom() { return room; }
    glm::vec3 getPosition() { return pos; }
    glm::vec3 getRotation() { return rot; }

    int getSprite() { return sprite; }
    void setSprite(int i) { sprite = i; }
    int getAnimation() { return animation; }
    void setAnimation(int i) { animation = i; frame = 0; }
    int getFrame() { return frame; }
    void setFrame(int i) { frame = i; }

    static void setShowEntitySprites(bool s) { showEntitySprites = s; }
    static bool getShowEntitySprites() { return showEntitySprites; }
    static void setShowEntityMeshes(bool s) { showEntityMeshes = s; }
    static bool getShowEntityMeshes() { return showEntityMeshes; }
    static void setShowEntityModels(bool s) { showEntityModels = s; }
    static bool getShowEntityModels() { return showEntityModels; }

  private:
    void find();

    int id;
    int room;
    glm::vec3 pos;
    glm::vec3 rot;
    int cache, cacheType;

    int sprite;
    int animation;
    int frame;

    static bool showEntitySprites;
    static bool showEntityMeshes;
    static bool showEntityModels;
};

#endif

