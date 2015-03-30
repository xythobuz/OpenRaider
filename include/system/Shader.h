/*!
 * \file include/system/Shader.h
 * \brief OpenGL Shader Implementation
 *
 * \author xythobuz
 */

#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>
#include <vector>

#include "TextureManager.h"

#include <glbinding/gl/gl33.h>

class ShaderBuffer {
  public:
    ShaderBuffer() : created(false), buffer(0), boundSize(0) { }
    ~ShaderBuffer();

    void bufferData(int elem, int size, void* data);

    template<typename T>
    void bufferData(std::vector<T> v)
    { bufferData(v.size(), sizeof(T), &v[0]); }

    void bindBuffer();
    void bindBuffer(int location, int size);
    void unbind(int location);

    unsigned int getBuffer() { orAssert(created); return buffer; }
    int getSize() { return boundSize; }

  private:
    bool created;
    unsigned int buffer;
    int boundSize;
};

class ShaderTexture {
  public:
    ShaderTexture(int w = 512, int h = 512);
    ~ShaderTexture();

    void clear();
    void bind();
    int getTexture() { return texture; }

  private:
    int width, height;
    unsigned int framebuffer, depth;
    int texture;
};

class Shader {
  public:
    Shader() : programID(-1) { }
    ~Shader();

    int compile(const char* vertex, const char* fragment);
    void use();

    int addUniform(const char* name);
    unsigned int getUniform(int n);

    void loadUniform(int uni, glm::vec2 vec);
    void loadUniform(int uni, glm::vec4 vec);
    void loadUniform(int uni, glm::mat4 mat);
    void loadUniform(int uni, int texture, TextureStorage store);

    static int initialize();
    static void shutdown();

    static void set2DState(bool on, bool depth = true);

    static void drawGL(std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs,
                       glm::mat4 MVP, unsigned int texture, TextureStorage store,
                       gl::GLenum mode = gl::GL_TRIANGLES, ShaderTexture* target = nullptr,
                       Shader& shader = textureShader);
    static void drawGL(std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs,
                       std::vector<unsigned short>& indices, glm::mat4 MVP,
                       unsigned int texture, TextureStorage store,
                       gl::GLenum mode = gl::GL_TRIANGLES, ShaderTexture* target = nullptr,
                       Shader& shader = textureShader);

    static void drawGLBuffer(std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs,
                             Shader& shader = textureShader);
    static void drawGLOnly(std::vector<unsigned short>& indices, glm::mat4 MVP,
                           unsigned int texture, TextureStorage store,
                           gl::GLenum mode = gl::GL_TRIANGLES, ShaderTexture* target = nullptr,
                           Shader& shader = textureShader);

    static void drawGL(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors,
                       glm::mat4 MVP, gl::GLenum mode = gl::GL_TRIANGLES,
                       ShaderTexture* target = nullptr, Shader& shader = colorShader);
    static void drawGL(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors,
                       std::vector<unsigned short>& indices, glm::mat4 MVP,
                       gl::GLenum mode = gl::GL_TRIANGLES, ShaderTexture* target = nullptr,
                       Shader& shader = colorShader);

    static std::string getVersion(bool linked);

  private:
    int programID;
    std::vector<unsigned int> uniforms;
    ShaderBuffer vertexBuffer, otherBuffer, indexBuffer;

    static void bindProperBuffer(ShaderTexture* target);

    static Shader textureShader;
    static const char* textureShaderVertex;
    static const char* textureShaderFragment;

    static Shader colorShader;
    static const char* colorShaderVertex;
    static const char* colorShaderFragment;

    static unsigned int vertexArrayID;
    static bool lastBufferWasNotFramebuffer;
};

#endif

