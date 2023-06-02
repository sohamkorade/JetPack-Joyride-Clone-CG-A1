#pragma once

#include "utils.hpp"

// mouse
bool mouse_first = true;
float mouse_yaw = -90.0f;  // yaw is initialized to -90.0 degrees since a yaw of
                           // 0.0 results in a direction vector pointing to the
                           // right so we initially rotate a bit to the left.
float mouse_pitch = 0.0f;
float mouse_lastX = 800.0f / 2.0;
float mouse_lastY = 600.0 / 2.0;
float mouse_fov = 45.0f;
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (mouse_first) {
    mouse_lastX = xpos;
    mouse_lastY = ypos;
    mouse_first = false;
  }

  float xoffset = xpos - mouse_lastX;
  float yoffset =
      mouse_lastY - ypos;  // reversed since y-coordinates go from bottom to top
  mouse_lastX = xpos;
  mouse_lastY = ypos;

  float sensitivity = 0.01f;  // change this value to your liking
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  mouse_yaw += xoffset;
  mouse_pitch += yoffset;

  // make sure that when pitch is out of bounds, screen doesn't get flipped
  if (mouse_pitch > 89.0f) mouse_pitch = 89.0f;
  if (mouse_pitch < -89.0f) mouse_pitch = -89.0f;

  glm::vec3 front;
  front.x = cos(glm::radians(mouse_yaw)) * cos(glm::radians(mouse_pitch));
  front.y = sin(glm::radians(mouse_pitch));
  front.z = sin(glm::radians(mouse_yaw)) * cos(glm::radians(mouse_pitch));
  cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  mouse_fov -= (float)yoffset;
  if (mouse_fov < 1.0f) mouse_fov = 1.0f;
  if (mouse_fov > 45.0f) mouse_fov = 45.0f;
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);

    // toggle wireframe mode (useful for debugging)
    if (key == GLFW_KEY_COMMA) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (key == GLFW_KEY_PERIOD) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
}

// glfw: initialize and configure
void glfw_ready() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

// glad: load all OpenGL function pointers
void glad_ready() {
  // gladLoadGL();
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    die("Failed to initialize GLAD");

  // glEnable(GL_DEPTH_TEST);
}

GLFWwindow *make_window(int width, int height, std::string title) {
  glfw_ready();

  // glfw window creation
  GLFWwindow *window =
      glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  if (!window) {
    glfwTerminate();
    die("Failed to create GLFW window");
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, key_callback);

  glad_ready();

  return window;
}

void clear_screen(glm::vec3 color = Colors::black, float alpha = 1.0f) {
  glClearColor(color.r, color.b, color.g, alpha);
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT);
}