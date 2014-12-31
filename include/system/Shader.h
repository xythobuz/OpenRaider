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

class ShaderBuffer {
  public:
    ShaderBuffer() : buffer(0), created(false) { }
    ~ShaderBuffer();

    void bufferData(int elem, int size, void* data);

    template<typename T>
    void bufferData(std::vector<T> v)
        { bufferData(v.size(), sizeof(T), &v[0]); }

    void bindBuffer();
    void bindBuffer(int location, int size);
    void unbind(int location);

  private:
    unsigned int buffer;
    bool created;
};

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
    void bufferData(int buff, std::vector<T> v)
        { bufferData(buff, v.size(), sizeof(T), &v[0]); }
    void bufferData(int buff, int elem, int size, void* d);

    void loadUniform(int uni, glm::vec2 vec);
    void loadUniform(int uni, glm::vec4 vec);
    void loadUniform(int uni, glm::mat4 mat);
    void loadUniform(int uni, int texture, TextureManager::TextureStorage store, int slot = 0);

    void bindBuffer(int buff);
    void bindBuffer(int buff, int location, int size);

    void disableAttribs();

    static int initialize();
    static void shutdown();

    static void drawGL(std::vector<glm::vec2>& vertices, std::vector<glm::vec2>& uvs,
                       glm::vec4 color, unsigned int texture);

    static void drawGL(std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs,
                       std::vector<unsigned short>& indices, glm::mat4 MVP, unsigned int texture);

    static void drawGL(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors,
                       std::vector<unsigned short>& indices, glm::mat4 MVP, int mode = GL_TRIANGLES);

  private:
    int programID;
    std::vector<unsigned int> uniforms;
    std::vector<unsigned int> buffers;
    std::vector<bool> attribs;

    static Shader textShader;
    static const char* textShaderVertex;
    static const char* textShaderFragment;

    static Shader textureShader;
    static const char* textureShaderVertex;
    static const char* textureShaderFragment;

    static Shader colorShader;
    static const char* colorShaderVertex;
    static const char* colorShaderFragment;

    static unsigned int vertexArrayID;
};

#endif

