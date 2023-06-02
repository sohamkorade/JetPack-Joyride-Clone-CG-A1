#pragma once

#include "buffers.hpp"
#include "renderer.hpp"
#include "utils.hpp"

class Sprite {
 public:
  std::vector<Texture *> textures;
  int frame, frames;
  float time, time_per_frame;

  Sprite(std::vector<Texture *> textures, float time_per_frame = 0)
      : textures(textures),
        frame(0),
        frames(textures.size()),
        time(0.0f),
        time_per_frame(time_per_frame) {}
  Sprite(Texture *texture, float time_per_frame = 0, int frames = 1,
         int frame = 0)
      : textures({texture}),
        frame(frame),
        frames(frames),
        time(0.0f),
        time_per_frame(time_per_frame) {}

  void update(float dt) {
    if (time_per_frame == 0) return;
    time += dt;
    if (time >= time_per_frame) {
      time = 0.0f;
      frame = (frame + 1) % frames;
    }
  }

  Texture *get() { return textures.front(); }
  Texture *get_frame() { return textures[frame]; }
};

class GameObject {
 public:
  // physics
  bool physics;
  glm::vec2 position, size, velocity;
  float rotation, angular_velocity;

  // rendering
  glm::vec3 color;
  Sprite *sprite = nullptr;
  Texture *texture;
  bool glowing;

  // metadata
  std::string tag;
  float timer;

  GameObject(Texture *texture, glm::vec2 pos, glm::vec2 size,
             glm::vec2 velocity = glm::vec2(0.0f, 0.0f), bool physics = false,
             bool glowing = false)
      : position(pos),
        size(size),
        texture(texture),
        color(Colors::white),
        velocity(velocity),
        rotation(0.0f),
        angular_velocity(0.0f),
        glowing(glowing),
        physics(physics) {}

  GameObject(Sprite *sprite, glm::vec2 pos, glm::vec2 size,
             glm::vec2 velocity = glm::vec2(0.0f, 0.0f), bool physics = false,
             bool glowing = false)
      : position(pos),
        size(size),
        sprite(sprite),
        color(Colors::white),
        velocity(velocity),
        rotation(0.0f),
        angular_velocity(0.0f),
        glowing(glowing),
        physics(physics) {
    texture = sprite->textures.front();
  }

  void play(Sprite *sprite) { this->sprite = sprite; }
  void play(Texture *texture) {
    this->sprite = nullptr;
    this->texture = texture;
  }

  void render(SpriteRenderer *SR) {
    if (sprite) {
      if (sprite->textures.front()->type == GL_TEXTURE_2D_ARRAY)
        // render texture array
        SR->render(sprite->textures.front(), position, size, rotation,
                   sprite->frame, color);
      else
        // render sprite
        SR->render(sprite->textures[sprite->frame], position, size, rotation, 0,
                   color);
    } else {
      // render texture
      SR->render(texture, position, size, rotation, 0, color);
    }
  }

  void update(float dt, glm::vec2 gravity = {0, 0}) {
    position += velocity * dt;
    rotation += angular_velocity * dt;
    rotation = fmod(rotation, 360.0f);
    if (!physics) velocity += gravity * dt;
    if (sprite) sprite->update(dt);
  }

  void update_physics(float dt, glm::vec2 gravity = {0, 0}) {
    velocity += gravity * dt;
    position += velocity * dt;
    rotation += angular_velocity * dt;
    rotation = fmod(rotation, 360.0f);
  }

  bool overlaps(GameObject *other) {
    return position.x < other->position.x + other->size.x &&
           position.x + size.x > other->position.x &&
           position.y < other->position.y + other->size.y &&
           position.y + size.y > other->position.y;
  }

  glm::vec2 center() { return position + size / 2.0f; }

  bool left_of(float x) { return position.x + size.x < x; }
  bool right_of(float x) { return position.x >= x; }
  bool above(float y) { return position.y + size.y < y; }
  bool below(float y) { return position.y >= y; }
  bool top_above(float y) { return position.y < y; }
  bool bottom_below(float y) { return position.y + size.y > y; }
};

void delete_gameobjects(std::vector<GameObject *> &gameobjects) {
  for (auto &x : gameobjects) delete x;
  gameobjects.clear();
}

float get_rightmost(std::vector<GameObject *> &gameobjects) {
  if (gameobjects.empty()) return 0.0f;
  float rightmost =
      gameobjects.front()->position.x + gameobjects.front()->size.x;
  for (auto &obj : gameobjects)
    if (obj->position.x > rightmost) rightmost = obj->position.x + obj->size.x;
  return rightmost;
}

bool OBBcollision(GameObject *A, GameObject *B, float angleA) {
  // Calculate the difference vector between centers
  glm::vec2 diff = B->center() - A->center();

  // Rotate the difference vector by the angle of rectangle A
  diff = glm::rotate(diff, angleA);

  // Calculate the absolute difference vector
  glm::vec2 absDiff(abs(diff.x), abs(diff.y));

  // Check if the rectangles are overlapping on the x-axis
  if (absDiff.x * 2 > A->size.x + B->size.x) return false;

  // Check if the rectangles are overlapping on the y-axis
  if (absDiff.y * 2 > A->size.y + B->size.y) return false;

  // If both axes are overlapping, the rectangles are colliding
  return true;
}

bool OBBcollision(GameObject *A, GameObject *B, float angleA, float angleB) {
  // Calculate the difference vector between centers
  glm::vec2 diff = B->center() - A->center();

  // Rotate the difference vector by the angle of rectangle A
  diff = glm::rotate(diff, angleA);

  // Calculate the absolute difference vector
  glm::vec2 absDiff(abs(diff.x), abs(diff.y));

  float halfWidthA = A->size.x / 2.0f;
  float halfHeightA = A->size.y / 2.0f;
  float halfWidthB = B->size.x / 2.0f;
  float halfHeightB = B->size.y / 2.0f;

  // Check if the rectangles are overlapping on the x-axis
  if (absDiff.x > halfWidthA + halfWidthB) return false;

  // Check if the rectangles are overlapping on the y-axis
  if (absDiff.y > halfHeightA + halfHeightB) return false;

  // Rotate the half-widths and half-heights of each rectangle by the angle
  float cA = cos(angleA), sA = sin(angleA);
  float cB = cos(angleB), sB = sin(angleB);
  float rHalfWidthA = halfWidthA * cA + halfHeightA * sA;
  float rHalfHeightA = halfHeightA * cA + halfWidthA * sA;
  float rHalfWidthB = halfWidthB * cB + halfHeightB * sB;
  float rHalfHeightB = halfHeightB * cB + halfWidthB * sB;

  // Check if the rotated rectangles are overlapping on the x-axis
  if (absDiff.x > rHalfWidthA + rHalfWidthB) return false;

  // Check if the rotated rectangles are overlapping on the y-axis
  if (absDiff.y > rHalfHeightA + rHalfHeightB) return false;

  // If both axes are overlapping, the rectangles are colliding
  return true;
}