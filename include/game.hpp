#pragma once

// helpers
#include "buffers.hpp"
#include "camera.hpp"
#include "gameobject.hpp"
#include "glfw-helper.hpp"
#include "keyboard.hpp"
#include "mesh.hpp"
#include "renderer.hpp"
#include "shader.hpp"
#include "text.hpp"
#include "utils.hpp"

class Game {
 public:
  std::string title;
  int width, height;
  GLFWwindow *window;
  glm::vec3 bg_color = rgb(0.1f, 0.1f, 0.1f);
  Texture *bg_texture = nullptr;
  std::map<std::string, GameObject *> gameobjects;
  SpriteRenderer *SR;
  TextRenderer *TR;
  GlowRenderer *GR;

  // gravity
  glm::vec2 gravity = glm::vec2(0.0f, 0.0f);

  // camera
  Camera camera;

  // for debugging
  bool FPS_counter = false;

  Game(int width, int height, std::string title)
      : title(title), width(width), height(height) {
    window = make_window(width, height, title);
    camera.Position = glm::vec3(0.0f, 0.0f, 3.0f);
    camera.aspect_ratio = (float)width / (float)height;
    // enable_mouse();
    SR = new SpriteRenderer(width, height);
    TR = new TextRenderer(width, height, "fonts/Jetpackia.ttf");
    GR = new GlowRenderer(width, height);
  }

  ~Game() {
    delete_gameobjects();
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void delete_gameobjects() {
    for (auto &x : gameobjects) delete x.second;
    gameobjects.clear();
  }

  void render_FPS_counter() {
    if (!FPS_counter) return;
    if (camera.dt == 0.0f) return;
    TR->render("FPS " + std::to_string(static_cast<int>(1 / camera.dt)),
               width - 150, 50, 1.0f, Colors::green);
  }

  void update_physics() {
    for (auto &x : gameobjects) x.second->update(camera.dt, gravity);
  }

  bool closing() {
    bool c = glfwWindowShouldClose(window);
    if (c) close();
    return c;
  }

  void doEvents() {
    // glfw: swap buffers and poll IO events
    glfwSwapBuffers(window);
    glfwPollEvents();
    camera.new_frame();
  }

  void close() { glfwSetWindowShouldClose(window, true); }

  void add(std::string name, GameObject *gameobject) {
    gameobjects[name] = gameobject;
  }
  void add(std::initializer_list<GameObject *> &gameobjects) {
    std::string name = std::to_string(gameobjects.size());
    for (auto &x : gameobjects) add(name, x);
  }

  void keyboard_events() {
    if (on_keyup(GLFW_KEY_SLASH)) {
      FPS_counter = !FPS_counter;
    }

    if (on_keypress(GLFW_KEY_W)) camera.move(FORWARD);
    if (on_keypress(GLFW_KEY_S)) camera.move(BACKWARD);
    if (on_keypress(GLFW_KEY_A)) camera.move(LEFT);
    if (on_keypress(GLFW_KEY_D)) camera.move(RIGHT);
    if (on_keypress(GLFW_KEY_Q)) camera.move(UP);
    if (on_keypress(GLFW_KEY_E)) camera.move(DOWN);

    // arcball camera
    if (on_keypress(GLFW_KEY_KP_4)) camera.rotate(LEFT);
    if (on_keypress(GLFW_KEY_KP_6)) camera.rotate(RIGHT);
    if (on_keypress(GLFW_KEY_KP_8)) camera.rotate(UP);
    if (on_keypress(GLFW_KEY_KP_2)) camera.rotate(DOWN);
  }

  void enable_mouse() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
  }

  void disable_mouse() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, NULL);
    glfwSetScrollCallback(window, NULL);
  }

  bool on_keypress(int key) { return glfwGetKey(window, key) == GLFW_PRESS; }
  bool on_keyrelease(int key) {
    return glfwGetKey(window, key) == GLFW_RELEASE;
  }
  bool on_keyup(int key) {
    if (on_keypress(key)) {
      while (!on_keyrelease(key)) glfwWaitEvents();
      return true;
    }
    return false;
  }

  // forward params
  forward_params(text, TR->render);
  forward_params(sprite, SR->render);
  forward_params(center_text, TR->render_centered);
};
