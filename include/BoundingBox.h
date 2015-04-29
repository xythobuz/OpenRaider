/*!
 * \file include/BoundingBox.h
 * \brief 3D Axis Aligned Bounding Box
 *
 * \author xythobuz
 */

#ifndef _BOUNDING_BOX_H_
#define _BOUNDING_BOX_H_

#include <array>
#include <vector>

class BoundingBox {
  public:
    BoundingBox(glm::vec3 min, glm::vec3 max);

    bool inBox(glm::vec3 p);
    bool inBoxPlane(glm::vec3 p);

    glm::vec3 getCorner(int i) { return corner.at(i); }

    void display(glm::mat4 VP, glm::vec3 colorLine, glm::vec3 colorDot);

    static void display();

  private:
    std::array<glm::vec3, 8> corner;

    static std::vector<glm::vec4> vertices;
    static std::vector<glm::vec3> colorsLine, colorsPoint;
    static std::vector<unsigned short> indicesLine;
};

#endif

