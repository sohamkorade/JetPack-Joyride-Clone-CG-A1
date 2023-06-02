#pragma once

#include "gameobject.hpp"
#include "renderer.hpp"
#include "utils.hpp"

class Particle {
 public:
  glm::vec2 position, velocity;
  glm::vec3 color;
  float rotation = 0, angular_velocity = 0;
  float life;

  Particle(glm::vec2 position, glm::vec2 velocity, glm::vec3 color,
           float life_max)
      : position(position), velocity(velocity), color(color), life(life_max) {}

  void update(float dt) {
    position += velocity * dt;
    rotation += angular_velocity * dt;
    rotation = fmod(rotation, 360.0f);
    life -= dt;
  }

  bool is_dead() { return life <= 0.0f; }
};

class ParticleGenerator {
 public:
  std::vector<Particle> particles;
  glm::vec2 size;

  // rendering
  Sprite* sprite = nullptr;
  Texture* texture;

  // randomize
  glm::vec2 velocity_blur = {20, 0};

  ParticleGenerator(Sprite* sprite, glm::vec2 size, int count)
      : sprite(sprite), size(size) {
    create(count);
  }
  ParticleGenerator(Texture* texture, glm::vec2 size, int count)
      : texture(texture), size(size) {
    create(count);
  }

  void create(int count) {
    for (int i = 0; i < count; i++) {
      particles.push_back(Particle({0, 0}, {0, 0}, Colors::white, 0));
    }
  }

  void update(float dt, glm::vec2 gravity = {0, 0}) {
    for (auto& p : particles) p.update(dt);
  }

  void render(SpriteRenderer* SR) {
    for (auto& p : particles) {
      if (p.is_dead()) continue;
      if (p.velocity.y == 0) continue;
      if (sprite) {
        if (sprite->textures.front()->type == GL_TEXTURE_2D_ARRAY)
          SR->render(sprite->textures.front(), p.position, size, p.rotation,
                     sprite->frame, p.color);
        else
          SR->render(sprite->textures[sprite->frame], p.position, size,
                     p.rotation, 0, p.color);
      } else {
        SR->render(texture, p.position, size, p.rotation, 0, p.color);
      }
    }
  }

  void respawn(glm::vec2 position, glm::vec2 velocity, float rotation,
               float life_max, float angular_velocity = 0,
               glm::vec3 color = Colors::white) {
    for (auto& p : particles)
      if (p.is_dead()) {
        p.position = position;
        p.velocity = velocity;
        p.color = color;
        p.life = life_max;
        p.rotation = rotation;
        p.angular_velocity = angular_velocity * randfloat(-10, 10);

        // randomize
        // p.velocity.x += randfloat(-1, 1) * 100;
        // p.velocity.y += randfloat(-100, 100);
        // p.angular_velocity = randfloat(-10, 10);
        // p.rotation += randfloat(-10, 10);

        int direction = rand() % 2 == 0 ? 1 : -1;
        p.velocity.x += 50 * direction;
        p.velocity.x += randfloat(-velocity_blur.x, velocity_blur.x);
        p.velocity.y += randfloat(-velocity_blur.y, velocity_blur.y);
        // p.rotation += randfloat(0, 45) * direction;
        break;
      }
  }
};