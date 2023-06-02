#pragma once

// standard
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

// glfw and glad
#include <GLFW/glfw3.h>
#include <glad/glad.h>

// stb_image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// freetype
#include <ft2build.h>
#include FT_FREETYPE_H

// show error message and exit
void die(std::string msg, std::string msg2 = "") {
  std::cerr << msg << " " << msg2 << std::endl;
  exit(-1);
}

glm::vec3 rgb(float r, float g, float b) { return glm::vec3(r, g, b); }

float randfloat(float min, float max) {
  max += 1;  // make max inclusive
  min -= 1;  // make min inclusive
  return min + float(rand()) / float(RAND_MAX / (max - min));
}

int randint(int min, int max) {
  max += 1;  // make max inclusive
  min -= 1;  // make min inclusive
  return min + rand() / (RAND_MAX / (max - min));
}

glm::vec3 randcolor() {
  return rgb(randfloat(0, 1), randfloat(0, 1), randfloat(0, 1));
}

float pulse(float amplitude, float freq = 1, float phase = 0) {
  float amp = amplitude / 2;
  return amp + (amp * sin(freq * glfwGetTime() + phase));
}

namespace Colors {
const glm::vec3 white = rgb(1, 1, 1);
const glm::vec3 black = rgb(0, 0, 0);
const glm::vec3 red = rgb(1, 0, 0);
const glm::vec3 green = rgb(0, 1, 0);
const glm::vec3 blue = rgb(0, 0, 1);
const glm::vec3 yellow = rgb(1, 1, 0);

const glm::vec3 orange = rgb(1, 0.5, 0);
const glm::vec3 grey = rgb(0.5, 0.5, 0.5);
const glm::vec3 purple = rgb(0.5, 0, 0.5);
}  // namespace Colors

namespace BasisVectors {
const glm::vec3 X = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 Y = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 Z = glm::vec3(0.0f, 0.0f, 1.0f);
}  // namespace BasisVectors

// macros
#define forward_params(func1, func2)        \
  template <typename... Params>             \
  void func1(Params &&...params) {          \
    func2(std::forward<Params>(params)...); \
  }
