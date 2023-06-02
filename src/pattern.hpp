#pragma once

#include <filesystem>

#include "gameobject.hpp"
#include "utils.hpp"

std::vector<GameObject *> load_pattern(std::string path, Sprite *sprite,
                                       glm::vec2 velocity, int WIDTH,
                                       int HEIGHT) {
  const int COIN_SIZE = 32;

  // load file
  std::vector<GameObject *> pattern;
  std::ifstream in(path);
  std::string line;

  // check if file exists
  if (!in.is_open()) {
    std::cout << "Could not open pattern: " << path << std::endl;
    // return a single coin
    return {new GameObject(sprite, {WIDTH, HEIGHT / 2}, {COIN_SIZE, COIN_SIZE},
                           velocity)};
  }

  // first pass: get dimensions
  int width = 0;
  int height = 0;
  while (std::getline(in, line)) {
    width = std::max(width, (int)line.size());
    height++;
  }

  // multiply by COIN_SIZE to get pixel dimensions
  width *= COIN_SIZE;
  height *= COIN_SIZE;

  // center pattern and keep all the coins just right of the screen
  int x0 = WIDTH;
  int y0 = (HEIGHT - height) / 2 + COIN_SIZE;

  // second pass: load pattern
  in.clear();
  in.seekg(0, std::ios::beg);
  int y = 0;
  while (std::getline(in, line)) {
    for (int x = 0; x < line.size(); x++) {
      if (line[x] == '1') {
        pattern.push_back(
            new GameObject(sprite, {x0 + x * COIN_SIZE, y0 + y * COIN_SIZE},
                           {COIN_SIZE, COIN_SIZE}, velocity));
      }
    }
    y++;
  }

  return pattern;
}

std::string get_random_file(std::string path) {
  std::vector<std::string> files;
  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    files.push_back(entry.path());
  }
  return files[rand() % files.size()];
}