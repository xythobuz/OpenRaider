/*!
 * \file include/system/Shader.h
 * \brief OpenGL Shader implementation
 *
 * \author xythobuz
 */

#ifndef _SHADER_H_
#define _SHADER_H_

#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "TextureManager.h"

class Shader {
  public:
    Shader() : programID(-1) { }
    ~Shader();

    int compile(const char* vertex, const char* fragment);
    void use();

    int addUniform(const char* name);
    unsigned int getUniform(int n);

    void addBuffer(int n = 1);
    unsigned int getBuffer(int n);

    template<typename T>
    void bufferData(int buff, std::vector<T> v) {
        glBindBuffer(GL_ARRAY_BUFFER, getBuffer(buff));
        glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(T), &v[0], GL_STATIC_DRAW);
    }

    void loadUniform(int uni, glm::vec2 vec);
    void loadUniform(int uni, glm::vec4 vec);
    void loadUniform(int uni, glm::mat4 mat);
    void loadUniform(int uni, int texture, TextureManager::TextureStorage store, int slot = 0);

    void bindBuffer(int buff);
    void bindBuffer(int buff, int location, int size);

    void disableAttribs();

  private:
    int programID;
    std::vector<unsigned int> uniforms;
    std::vector<unsigned int> buffers;
    std::vector<bool> attribs;
};

#endif

