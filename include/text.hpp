#pragma once

#include "shader.hpp"
#include "utils.hpp"

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
  unsigned int TextureID;  // ID handle of the glyph texture
  glm::ivec2 Size;         // Size of glyph
  glm::ivec2 Bearing;      // Offset from baseline to left/top of glyph
  unsigned int Advance;    // Horizontal offset to advance to next glyph
};

class Font {
 public:
  std::map<GLchar, Character> glyphs;
  GLuint VAO, VBO;

  Font(std::string font_name) {
    // FreeType
    FT_Library ft;

    // All functions return a value different than 0 whenever an error
    // occurred
    if (FT_Init_FreeType(&ft))
      die("ERROR::FREETYPE: Could not init FreeType Library");

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face))
      die("ERROR::FREETYPE: Failed to load font");
    else {
      // set size to load glyphs as
      FT_Set_Pixel_Sizes(face, 0, 48);

      // disable byte-alignment restriction
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      // load first 128 characters of ASCII set
      for (unsigned char c = 0; c < 128; c++) {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
          die("ERROR::FREETYTPE: Failed to load Glyph");
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                     face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        glyphs[c] = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)};
      }
      glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  ~Font() {
    glDeleteTextures(1, &glyphs['A'].TextureID);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
  }
};

void font_blend_enable() {
  // glEnable(GL_CULL_FACE);
  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void font_blend_disable() {
  // glDisable(GL_CULL_FACE);
  // glDisable(GL_BLEND);
}
