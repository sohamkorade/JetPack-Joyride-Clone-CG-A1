#pragma once

// helpers
#include "buffers.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "text.hpp"

struct MeshState {
  glm::vec3 position = glm::vec3(0.0f);
  glm::mat4 rotation = glm::mat4(1.0f);
  int rotation_axis = 0;
  bool visible = true;

  void reset() {
    position = glm::vec3(0.0f);
    rotation = glm::mat4(1.0f);
    rotation_axis = 0;
    visible = true;
  }
};

class Mesh {
 public:
  Shader *shader;
  Texture *texture;
  GLenum draw_mode;
  VAO *vao;
  VBO *vbo;
  EBO *ebo;
  int vertex_count = 0;
  MeshState state;
  bool my_shader_and_texture = false;

  Mesh(std::vector<GLfloat> vertices, std::vector<GLuint> indices,
       GLenum draw_mode = GL_TRIANGLES,
       std::string vertex_path = "shaders/shader.vert",
       std::string fragment_path = "shaders/shader.frag",
       std::string texture_path = "textures/cement_wall.jpeg",
       std::vector<std::tuple<GLint, GLenum>> attributes =
           {
               {3, GL_FLOAT},  // position
               {3, GL_FLOAT},  // color
               {2, GL_FLOAT},  // texture
           })
      : draw_mode(draw_mode) {
    vertex_count = indices.size();
    shader = new Shader(vertex_path, fragment_path);
    texture = new Texture(texture_path);

    vao = new VAO();
    vbo = new VBO(vertices);
    ebo = new EBO(indices);
    // vao->bind();
    // vbo->bind();
    // ebo->bind();
    load_attributes(attributes);
    // vao->unbind();
    // vbo->unbind();
    // ebo->unbind();
  }

  Mesh(std::vector<GLfloat> vertices, std::vector<GLuint> indices,
       GLenum draw_mode, Shader *shader, Texture *texture,
       std::vector<std::tuple<GLint, GLenum>> attributes =
           {
               {3, GL_FLOAT},  // position
               {3, GL_FLOAT},  // color
               {2, GL_FLOAT}   // texture
           })
      : draw_mode(draw_mode), shader(shader), texture(texture) {
    my_shader_and_texture = true;
    vertex_count = indices.size();
    vao = new VAO();
    vbo = new VBO(vertices);
    ebo = new EBO(indices);
    // vao->bind();
    // vbo->bind();
    // ebo->bind();
    load_attributes(attributes);
    // vao->unbind();
    // vbo->unbind();
    // ebo->unbind();
  }

  ~Mesh() {
    if (my_shader_and_texture) {
      delete shader;
      delete texture;
    }
    delete vao;
    delete vbo;
    delete ebo;
  }

  void draw_element() {
    vao->bind();
    glDrawElements(draw_mode, vertex_count, GL_UNSIGNED_INT, 0);
  }

  void set_camera(Camera &camera) {
    // pass projection matrix to shader (note that in this case it could
    // change every frame)
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                            camera.aspect_ratio, 0.1f, 100.0f);
    shader->setMat4("projection", projection);
    shader->setMat4("view", camera.GetViewMatrix());
    // shader->setMat4("view", glm::lookAt(camera.Position, position,
    // camera.Up));

    // move camera to origin
    //  shader->setMat4(
    //      "view", glm::translate(glm::mat4(1),
    //                             glm::vec3(-camera.Position.x,
    //                             -camera.Position.y,
    //                                       -camera.Position.z)));
    //  shader->setMat4("view",
    //                  glm::lookAt(camera.Position, glm::vec3(0),
    //                  camera.WorldUp));
  }

  void rotate(glm::vec3 vec, float angle = 1.0f) {
    state.rotation = glm::rotate(state.rotation, glm::radians(angle), vec);
  }

  void render(Camera &camera) {
    if (state.rotation_axis) {
      glm::vec3 rot;
      if (abs(state.rotation_axis) == 1) rot = BasisVectors::X;
      if (abs(state.rotation_axis) == 2) rot = BasisVectors::Y;
      if (abs(state.rotation_axis) == 3) rot = BasisVectors::Z;
      state.rotation = glm::rotate(
          state.rotation,
          glm::radians((state.rotation_axis > 0 ? 1 : -1) * 1.0f), rot);
    }
    if (!state.visible) return;

    texture->bind();
    shader->use();

    set_camera(camera);

    // calculate the model matrix for each object and pass it to shader before
    // drawing
    // make sure to initialize matrix to identity matrix first
    glm::mat4 model = glm::translate(glm::mat4(1.0f), state.position);
    model = model * state.rotation;
    shader->setMat4("model", model);

    draw_element();
  }

  void load_attributes(std::vector<std::tuple<GLint, GLenum>> attributes) {
    GLsizei stride = 0;
    for (auto &a : attributes)
      stride += std::get<0>(a) * sizeof(std::get<1>(a));

    unsigned long offset = 0;

    int index = 0;
    for (auto &a : attributes) {
      glVertexAttribPointer(index, std::get<0>(a), std::get<1>(a), GL_FALSE,
                            stride, (void *)offset);
      glEnableVertexAttribArray(index);
      offset += std::get<0>(a) * sizeof(std::get<1>(a));
      index++;
    }
  }
};

// class Mesh {
//  public:
//   std::map<std::string, Mesh *> meshes;

//   Mesh(std::string name, std::vector<GLfloat> vertices,
//        std::vector<GLuint> indices, GLenum draw_mode = GL_TRIANGLES,
//        std::string vertex_path = "shaders/shader.vert",
//        std::string fragment_path = "shaders/shader.frag",
//        std::string texture_path = "textures/cement_wall.jpeg",
//        std::vector<std::tuple<GLint, GLenum>> attributes = {
//            {3, GL_FLOAT},  // position
//            {3, GL_FLOAT},  // color
//            {2, GL_FLOAT},  // texture
//        }) {
//     meshes[name] = new Mesh(vertices, indices, draw_mode, vertex_path,
//                              fragment_path, texture_path, attributes);
//   }

//   ~Mesh() {
//     for (auto &s : meshes) delete s.second;
//   }

//   void render(Camera &camera) {
//     for (auto &s : meshes) s.second->render(camera);
//   }

//   void rotate(glm::vec3 vec, float angle = 1.0f) {
//     for (auto &s : meshes) s.second->rotate(vec, angle);
//   }

//   void set_state(State state) {
//     for (auto &s : meshes) s.second->state = state;
//   }
// };