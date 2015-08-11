/*!
 * \file include/BoundingSphere.h
 * \brief 3D Bounding Sphere
 *
 * \author xythobuz
 */

#ifndef _BOUNDING_SPHERE_H_
#define _BOUNDING_SPHERE_H_

#include <vector>

class BoundingSphere {
  public:
    BoundingSphere(glm::vec3 p = glm::vec3(0.0f, 0.0f, 0.0f), float r = 100.0f, int res = 42) : pos(p), radius(r), resolution(res) { }

    void setPosition(glm::vec3 p) { pos = p; }
    glm::vec3 getPosition() { return pos; }

    void setRadius(float r) { radius = r; }
    float getRadius() { return radius; }

    void display(glm::mat4 VP, glm::vec3 color);

    static void display();

  private:
    glm::vec3 pos;
    float radius;
    int resolution;

    static std::vector<glm::vec4> vertices;
    static std::vector<glm::vec3> colors;
};

#endif

