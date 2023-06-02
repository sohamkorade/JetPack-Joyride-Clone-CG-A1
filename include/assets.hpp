#pragma once

#include <filesystem>

#include "buffers.hpp"
#include "camera.hpp"
#include "game.hpp"
#include "gameobject.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "text.hpp"
#include "utils.hpp"

extern Game *G;

bool progress_bg(std::string text, float progress, float total) {
  clear_screen();
  G->SR->render(G->bg_texture, {0, 0}, {G->width, G->height}, 0, 0,
                Colors::grey, 0.5f);
  // G->TR->render_centered("Jetpack Joyride Clone", 0, -100, 1.0f,
  //                        Colors::yellow);
  G->TR->render(text, 700, 150, 1.0f, Colors::green);
  G->TR->render(std::to_string(int(progress * 100.0 / total)) + "%", 700, 200,
                1.0f, Colors::yellow);
  G->doEvents();
  return !G->closing();
}

class Assets {
 public:
  std::map<std::string, Texture *> Textures;
  std::map<std::string, Shader *> Shaders;
  std::map<std::string, Mesh *> Meshes;
  std::map<std::string, Font *> Fonts;

  std::map<std::string, Sprite *> Sprites;

  Texture *loadTexture(std::string name, std::string path) {
    return Textures[name] = new Texture(path);
  }

  Shader *loadShader(std::string name, std::string vertexPath,
                     std::string fragmentPath) {
    return Shaders[name] = new Shader(vertexPath, fragmentPath);
  }

  Mesh *loadMesh(std::string name, std::vector<float> vertices,
                 std::vector<unsigned int> indices) {
    return Meshes[name] = new Mesh(vertices, indices);
  }

  Font *loadFont(std::string name, std::string path) {
    return Fonts[name] = new Font(path);
  }

  Sprite *loadSprite(std::string name, std::string texture_name,
                     float time_per_frame = 0) {
    return Sprites[name] = new Sprite(Textures[texture_name], time_per_frame);
  }

  Sprite *loadSprite(std::string name, std::vector<std::string> texture_names,
                     float time_per_frame = 0.1f) {
    return Sprites[name] =
               new Sprite(texture_list(texture_names), time_per_frame);
  }

  Sprite *loadSprites(std::string name, std::string texture_name_prefix,
                      int num_frames, float time_per_frame = 0.1f) {
    std::vector<std::string> texture_names;
    for (int i = 1; i <= num_frames; i++) {
      texture_names.push_back(texture_name_prefix + std::to_string(i));
    }
    return Sprites[name] =
               new Sprite(texture_list(texture_names), time_per_frame);
  }

  Sprite *loadSpriteArray(std::string name, std::string path, int x, int y,
                          float time_per_frame = 0) {
    Texture *texture = new Texture(path, x, y);
    Sprites[name] = new Sprite(texture, time_per_frame, x * y, 0);
    return Sprites[name];
  }

  void clear() {
    for (auto &pair : Textures) delete pair.second;
    for (auto &pair : Shaders) delete pair.second;
    for (auto &pair : Meshes) delete pair.second;
    for (auto &pair : Fonts) delete pair.second;

    for (auto &pair : Sprites) delete pair.second;
  }

  // helpers

  std::vector<Texture *> texture_list(std::vector<std::string> names) {
    std::vector<Texture *> t;
    for (auto &name : names) {
      t.push_back(Textures[name]);
    }
    return t;
  }

  std::map<std::string, Texture *> texture_map(std::vector<std::string> names) {
    std::map<std::string, Texture *> t;
    for (auto &name : names) {
      t[name] = Textures[name];
    }
    return t;
  }

  std::map<std::string, Sprite *> sprite_map(
      std::vector<std::string> texture_names) {
    std::map<std::string, Sprite *> t;
    for (auto &name : texture_names) {
      t[name] = new Sprite(Textures[name], 0);
    }
    return t;
  }

  // load all *.png files in textures/ folder
  void load_textures(std::string path = "textures/",
                     std::string match = ".png") {
    int progress = 0;
    int total = 0;

    // first pass: count files
    for (auto &p : std::filesystem::directory_iterator(path)) {
      std::string filename = p.path().filename().string();
      if (filename.find(match) != std::string::npos) {
        total++;
      }
    }

    // second pass: load files
    for (auto &p : std::filesystem::directory_iterator(path)) {
      std::string filename = p.path().filename().string();
      if (filename.find(match) != std::string::npos) {
        Textures[filename.substr(0, filename.find(match))] =
            new Texture(p.path().string());
        progress++;
      }
      // max 50% progress
      if (!progress_bg("Loading textures...", progress / 2, total)) break;
    }
  }
};