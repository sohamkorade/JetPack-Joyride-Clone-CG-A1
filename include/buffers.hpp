#pragma once

#include "utils.hpp"

class VBO {
 public:
  GLuint ID;
  VBO(std::vector<GLfloat> &vertices) {
    glGenBuffers(1, &ID);
    bind();
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);
  }
  ~VBO() { glDeleteBuffers(1, &ID); }
  void bind() { glBindBuffer(GL_ARRAY_BUFFER, ID); }
  void unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
};

class VAO {
 public:
  GLuint ID;
  VAO() {
    glGenVertexArrays(1, &ID);
    bind();
  }
  ~VAO() { glDeleteVertexArrays(1, &ID); }
  void add_attributes(VBO &vbo, GLuint index, GLint size, GLenum type,
                      GLboolean normalized, GLsizei stride,
                      const void *pointer) {
    vbo.bind();
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    glEnableVertexAttribArray(index);
    vbo.unbind();
  }
  void bind() { glBindVertexArray(ID); }
  void unbind() { glBindVertexArray(0); }
};

class EBO {
 public:
  GLuint ID;
  EBO(std::vector<GLuint> &indices) {
    glGenBuffers(1, &ID);
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);
  }
  ~EBO() { glDeleteBuffers(1, &ID); }
  void bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }
  void unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
};

class Texture {
 public:
  GLuint ID;
  GLenum type;

  // Texture
  Texture(std::string path, int x = 1, int y = 1) {
    type = x * y == 1 ? GL_TEXTURE_2D : GL_TEXTURE_2D_ARRAY;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenTextures(1, &ID);
    bind();
    int width, height, channels;
    GLenum format;
    GLubyte *texels;
    if (load(path, width, height, channels, format, texels)) {
      if (type == GL_TEXTURE_2D_ARRAY) {
        load_texture_array(x, y, width, height, channels, format, texels);
      } else {
        glTexImage2D(type, 0, format, width, height, 0, format,
                     GL_UNSIGNED_BYTE, texels);
        glGenerateMipmap(type);
      }
    }
    stbi_image_free(texels);
    unbind();
  }
  ~Texture() { glDeleteTextures(1, &ID); }
  void bind() {
    // blend
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

    glBindTexture(type, ID);
  }
  void unbind() {
    glBindTexture(type, 0);
    // blend
    // glDisable(GL_BLEND);
  }

  void load_texture_array(int x, int y, int width, int height, int channels,
                          GLenum format, GLubyte *texels) {
    const int tile_width = width / x;
    const int tile_height = height / y;
    glTexImage3D(type, 0, format, tile_width, tile_height, x * y, 0, format,
                 GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(type);

    // fill texture array
    std::vector<GLubyte> tile(tile_width * tile_height * channels);
    const int tile_bytes = tile_width * channels;
    const int row_bytes = x * tile_bytes;

    for (int iy = 0; iy < y; iy++) {
      for (int ix = 0; ix < x; ix++) {
        GLubyte *ptr = texels + iy * row_bytes + ix * tile_bytes;
        for (int row = 0; row < tile_height; row++)
          std::copy_n(ptr + row * row_bytes, tile_bytes,
                      tile.begin() + row * tile_bytes);

        int i = iy * x + ix;
        glTexSubImage3D(type, 0, 0, 0, i, tile_width, tile_height, 1, format,
                        GL_UNSIGNED_BYTE, tile.data());
      }
    }
  }

  // Texture Loading
  bool load(std::string path, int &width, int &height, int &channels,
            GLenum &format, GLubyte *&texels) {
    // TODO: maybe change texture array parameters
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    // if (flip_y) stbi_set_flip_vertically_on_load(true);
    // std::cout << "Loading texture: " << path << std::endl;
    texels = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!texels) {
      std::cout << "Failed to load texture: " << path << std::endl;
      return false;
    }

    if (channels == 1)
      format = GL_RED;
    else if (channels == 3)
      format = GL_RGB;
    else if (channels == 4)
      format = GL_RGBA;
    return true;
  }
};