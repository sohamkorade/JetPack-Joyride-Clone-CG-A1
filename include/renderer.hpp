#pragma once

#include "buffers.hpp"
#include "shader.hpp"
#include "text.hpp"

class SpriteRenderer {
 public:
  Shader *sprite_shader, *spritesheet_shader;
  GLuint quadVAO;
  float width, height;

  SpriteRenderer(int width, int height) : width(width), height(height) {
    spritesheet_shader =
        new Shader("shaders/spritesheet.vs", "shaders/spritesheet.fs");
    sprite_shader = new Shader("shaders/sprite.vs", "shaders/sprite.fs");
    // configure VAO/VBO
    GLuint VBO;
    float vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,  //
        1.0f, 0.0f, 1.0f, 0.0f,  //
        0.0f, 0.0f, 0.0f, 0.0f,  //

        0.0f, 1.0f, 0.0f, 1.0f,  //
        1.0f, 1.0f, 1.0f, 1.0f,  //
        1.0f, 0.0f, 1.0f, 0.0f   //
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
  ~SpriteRenderer() { glDeleteVertexArrays(1, &quadVAO); }
  void render(Texture *texture, glm::vec2 position = {0, 0},
              glm::vec2 size = {1, 1}, float rotate = 0.0f, int index = 0,
              glm::vec3 color = Colors::white, float alpha = 1.0f) {
    Shader *shader;
    if (texture->type == GL_TEXTURE_2D)
      shader = sprite_shader;
    else
      shader = spritesheet_shader;

    // prepare transformations
    shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, {position, 0});
    model = glm::translate(model, {size.x / 2, size.y / 2, 0});
    model = glm::rotate(model, glm::radians(rotate), {0, 0, 1});
    model = glm::translate(model, {-size.x / 2, -size.y / 2, 0});
    model = glm::scale(model, {size, 1});
    shader->setMat4("model", model);

    glm::mat4 projection = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
    shader->setMat4("projection", projection);

    // set uniforms
    shader->setVec3("spriteColor", color);
    shader->setFloat("alpha", alpha);

    if (shader == spritesheet_shader) shader->setInt("spriteIndex", index);

    // render textured quad
    glActiveTexture(GL_TEXTURE0);
    texture->bind();

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
  }
};

class TextRenderer {
 public:
  Shader *shader;
  Font *default_font;
  float width, height;

  TextRenderer(int width, int height,
               std::string font_path = "fonts/Antonio-Bold.ttf")
      : width(width), height(height) {
    shader = new Shader("shaders/text.vs", "shaders/text.fs");
    default_font = new Font(font_path);
  }

  ~TextRenderer() {
    delete shader;
    delete default_font;
  }

  // get bounding box of text
  glm::vec2 get_BB(std::string text, float scale = 1.0f, Font *font = nullptr) {
    if (!font) font = default_font;

    float width = 0, height = 0;
    for (auto &c : text) {
      Character ch = font->glyphs[c];
      width += (ch.Advance >> 6) * scale;
      height = std::max(height, (float)(ch.Size.y * scale));
    }
    return {width, height};
  }

  // render a line of text
  void render(std::string text, float x, float y, float scale = 1.0f,
              glm::vec3 color = Colors::white, Font *font = nullptr,
              float alpha = 1) {
    if (!font) font = default_font;

    // flip y axis
    y = height - y;

    font_blend_enable();
    // activate corresponding render state
    shader->use();
    glm::mat4 projection = glm::ortho(0.0f, width, 0.0f, height);
    shader->setMat4("projection", projection);
    shader->setVec3("textColor", color);
    shader->setFloat("alpha", alpha);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(font->VAO);

    // iterate through all characters
    for (auto &c : text) {
      Character ch = font->glyphs[c];

      float xpos = x + ch.Bearing.x * scale;
      float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

      float w = ch.Size.x * scale;
      float h = ch.Size.y * scale;
      // update VBO for each character
      float vertices[6][4] = {
          {xpos, ypos + h, 0.0f, 0.0f},    {xpos, ypos, 0.0f, 1.0f},
          {xpos + w, ypos, 1.0f, 1.0f},

          {xpos, ypos + h, 0.0f, 0.0f},    {xpos + w, ypos, 1.0f, 1.0f},
          {xpos + w, ypos + h, 1.0f, 0.0f}};
      // render glyph texture over quad
      glBindTexture(GL_TEXTURE_2D, ch.TextureID);
      // update content of VBO memory
      glBindBuffer(GL_ARRAY_BUFFER, font->VBO);
      glBufferSubData(
          GL_ARRAY_BUFFER, 0, sizeof(vertices),
          vertices);  // be sure to use glBufferSubData and not glBufferData

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      // render quad
      glDrawArrays(GL_TRIANGLES, 0, 6);
      // now advance cursors for next glyph (note that advance is number of 1/64
      // pixels)
      x += (ch.Advance >> 6) *
           scale;  // bitshift by 6 to get value in pixels (2^6 = 64 (divide
                   // amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    font_blend_disable();
  }

  // render multiple lines of text
  void render(std::vector<std::string> lines, float x, float y,
              float scale = 1.0f, glm::vec3 color = Colors::white,
              Font *font = nullptr) {
    for (auto &line : lines) {
      render(line, x, y, scale, color, font);
      y -= 50.0f * scale + 10.0f;
    }
  }

  // render number
  void render(int number, float x, float y, float scale = 1.0f,
              glm::vec3 color = Colors::white, Font *font = nullptr) {
    render(std::to_string(number), x, y, scale, color, font);
  }

  // render text in center of screen (x axis)
  void render_centered(std::string text, float x_offset, float y_offset,
                       float scale = 1.0f, glm::vec3 color = Colors::white,
                       Font *font = nullptr, float alpha = 1) {
    glm::vec2 BB = get_BB(text, scale, font);
    render(text, x_offset + width / 2 - BB.x / 2,
           y_offset + height / 2 - BB.y / 2, scale, color, font, alpha);
  }
};

class GlowRenderer {
 public:
  Shader *shader;
  GLuint quadVAO;
  float width, height;

  GlowRenderer(int width, int height) : width(width), height(height) {
    shader = new Shader("shaders/glow.vs", "shaders/glow.fs");
    // configure VAO/VBO
    GLuint VBO;
    float vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,  //
        1.0f, 0.0f, 1.0f, 0.0f,  //
        0.0f, 0.0f, 0.0f, 0.0f,  //

        0.0f, 1.0f, 0.0f, 1.0f,  //
        1.0f, 1.0f, 1.0f, 1.0f,  //
        1.0f, 0.0f, 1.0f, 0.0f   //
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
  ~GlowRenderer() { glDeleteVertexArrays(1, &quadVAO); }
  void glow(glm::vec2 point1 = {0, 0}, glm::vec2 point2 = {0, 0},
            int glow_mode = 1, float rotate = 0.0f,
            glm::vec3 color = Colors::white, float alpha = 1.0f,
            float glow_radius = 0.2f) {
    // prepare transformations
    shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, {0, 0, 0});
    model = glm::translate(model, {width / 2, height / 2, 0});
    model = glm::rotate(model, glm::radians(rotate), {0, 0, 1});
    model = glm::translate(model, {-width / 2, -height / 2, 0});
    model = glm::scale(model, {width, height, 1});
    shader->setMat4("model", model);

    glm::mat4 projection = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
    shader->setMat4("projection", projection);

    // set uniforms
    shader->setVec3("spriteColor", color);
    shader->setFloat("alpha", alpha);

    shader->setVec2("point1", adjust(point1));
    shader->setVec2("point2", adjust(point2));
    shader->setInt("glowMode", glow_mode);
    shader->setFloat("radius", glow_radius);
    shader->setFloat("aspectRatio", width / height);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
  }

  // adjust to 0-1 range
  glm::vec2 adjust(glm::vec2 point) {
    // TODO: understand why this is needed
    return glm::vec2{point.x / height, point.y / height};
  }
};
