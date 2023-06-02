#pragma once

// Main game logic

#include "loader.hpp"
#include "objects.hpp"
#include "particles.hpp"
#include "pattern.hpp"
#include "sound.hpp"

// constants

// velocity is in pixels per second

const int WIDTH = 1200;
const int HEIGHT = 600;

const float JETPACK_VY = -250;
float BARRY_VX = 300;  // 200;
const float GROUND_Y = 550;
const float ROOF_Y = 60;
const float GRAVITY_Y = 500;
const float MISSILE_VX = -BARRY_VX * 2;

const float BRAKE = 0.99;
const float WAVE_GAP_X = 500;
const glm::vec2 COIN_COUNTER_POS = {0, 70};

const float LASER_DURATION = 3;
const float LEVEL_CLEAR_DISTANCE = 400;

const float METERS_PER_PIXEL = 1.0 / 30;
const float BASE_LEVEL_SPEED = 200;
const float LEVEL_SPEED_INCREASE = 100;

const std::vector<std::string> Backdrops = {
    "Lab1", "Lab2", "Sector1", "Volcano1", "Volcano2", "Warehouse"};
const float BG_WIDTH = 2665;

extern Game *G;
extern Assets *A;

enum Wave { COIN, ZAPPER, MISSILE, LASER, EMPTY };

// entities
std::vector<GameObject *> coins, zappers, missiles, lasers, workers;

// game state
float distance = 0;
int score = 0;
bool dead = false;
bool jetpack = false;
bool bullet_flash = false;
int level = 1;

Wave wave = COIN;
float wave_width = 0;
float wave_timer = 0;

// sfx state
int run = 1;

ParticleGenerator *bullets, *shells;

// functions
void game_screen();

float rand_Y(float height) { return randfloat(ROOF_Y, GROUND_Y - height); }

void draw_BB(GameObject *obj) {
  G->SR->render(A->Textures["Progress"], obj->position, obj->size, 0, 0,
                Colors::green, 0.5f);
}

void update_all_vx() {
  auto &backdrop1 = G->gameobjects["Backdrop1"];
  auto &backdrop2 = G->gameobjects["Backdrop2"];
  backdrop1->velocity.x = -BARRY_VX;
  backdrop2->velocity.x = -BARRY_VX;

  for (auto &coin : coins) coin->velocity.x = -BARRY_VX;
  for (auto &zapper : zappers) zapper->velocity.x = -BARRY_VX;
  for (auto &laser : lasers) laser->velocity.x = -BARRY_VX;
}

void reload_coins(std::string pattern = "") {
  if (pattern == "") pattern = get_random_file("patterns");
  std::cout << "Pattern: " << pattern.substr(pattern.find_last_of("/") + 1)
            << std::endl;
  coins = load_pattern(pattern, A->Sprites["Coin"], {-BARRY_VX, 0}, WIDTH,
                       GROUND_Y);

  wave_width = get_rightmost(coins);
}

void reload_zappers() {
  int y = rand_Y(200);

  GameObject *zapper = new GameObject(A->Sprites["Zapper"], {WIDTH * 1.5, y},
                                      {100, 200}, {-BARRY_VX, 0}, true, true);
  std::cout << "Enemy: ";

  const int zapper_type = randint(1, 6);
  if (zapper_type == 1) {
    // vertical zapper
    std::cout << "Vertical zapper" << std::endl;
  } else if (zapper_type == 2) {
    // horizontal zapper
    zapper->rotation = 90;
    std::cout << "Horizontal zapper" << std::endl;
  } else if (zapper_type == 3) {
    // diagonal zapper
    zapper->rotation = 45;
    std::cout << "Diagonal zapper" << std::endl;
  } else if (zapper_type == 4) {
    // diagonal zapper
    zapper->rotation = -45;
    std::cout << "Diagonal zapper" << std::endl;
  } else if (zapper_type == 5) {
    // rotating zapper
    zapper->angular_velocity = 100;
    std::cout << "Rotating zapper" << std::endl;
  } else if (zapper_type == 6) {
    // rotating zapper
    zapper->angular_velocity = -100;
    std::cout << "Rotating zapper" << std::endl;
  }
  zappers.push_back(zapper);

  wave_width = get_rightmost(zappers);
}

void reload_missiles(int n = 1) {
  std::cout << "Enemy: Missile" << std::endl;
  auto &barry = G->gameobjects["Barry"];
  int y = barry->position.y;

  for (int i = 1; i <= n; i++) {
    missiles.push_back(new GameObject(A->Sprites["Rocket"],
                                      {WIDTH * (1 + i), y}, {100, 50},
                                      {MISSILE_VX, 0}, true, true));
  }
  wave_width = get_rightmost(missiles);
}

void reload_lasers(int n = 2) {
  if (n == 2) {
    std::cout << "Enemy: Laser 2" << std::endl;
    int y1 = randfloat(ROOF_Y, GROUND_Y - 300 - 50);
    int y2 = y1 + 300;
    lasers.push_back(new GameObject(A->Textures["LaserInactive"], {WIDTH, y1},
                                    {WIDTH, 50}, {-BARRY_VX, 0}, true, true));
    lasers.push_back(new GameObject(A->Textures["LaserInactive"], {WIDTH, y2},
                                    {WIDTH, 50}, {-BARRY_VX, 0}, true, true));
    reload_coins("patterns/coinPatternOnBoardingBetweenLasers.txt");

    // center the coins between the lasers
    for (auto &coin : coins) {
      coin->position.y += (y1 + y2) / 2 - HEIGHT / 2;
    }
  } else {
    std::cout << "Enemy: Laser 1" << std::endl;
    int y1 = rand_Y(50);
    lasers.push_back(new GameObject(A->Textures["LaserInactive"], {WIDTH, y1},
                                    {WIDTH, 50}, {-BARRY_VX, 0}, true, true));
  }
  for (auto &laser : lasers) {
    laser->timer = 0;
    laser->tag = "LaserBegin";
  }
  wave_width = get_rightmost(lasers);
  // add wait time
  wave_width += LASER_DURATION * 200;
}

void reload_workers(int n = 3) {
  // workers.push_back(new GameObject(A->Sprites["Worker"], {WIDTH, GROUND_Y},
  //                                  {100, 100}, {-BARRY_VX, 0}, true,
  //                                  true));
  // wave_width = workers.back();
}

void game_over() {
  std::cout << "Game over!" << std::endl;
  wave_timer = 3;
  dead = true;
  auto &barry = G->gameobjects["Barry"];
  barry->play(A->Textures["BarryDead"]);
  barry->velocity.x = BARRY_VX * 0.7;
  sfx("fall_bounce");
}

void check_wave(int k = -1) {
  // std::cout << "Checking wave" << std::endl;
  // reload coins, zappers, missiles, lasers

  // update wave state
  wave_width -= BARRY_VX * G->camera.dt;
  wave_timer -= G->camera.dt;

  if (!dead) {
    // choose next wave
    if (wave_width < -WAVE_GAP_X) {
      wave_width = 0;
      std::cout << "Wave change!" << std::endl;

      // clean up old wave
      delete_gameobjects(workers);

      if (k == -1) k = randint(0, std::min(level, 3));
      wave = (Wave)k;
      if (k == COIN) {
        delete_gameobjects(coins);
        reload_coins();
      } else if (k == ZAPPER) {
        delete_gameobjects(zappers);
        reload_zappers();
      } else if (k == MISSILE) {
        delete_gameobjects(missiles);
        reload_missiles();
      } else if (k == LASER) {
        delete_gameobjects(lasers);
        reload_lasers();
      }
    }
  }

  // // workers
  // for (auto &worker : workers) {
  //   if (worker->left_of(0)) {
  //     worker->position.x = WIDTH;
  //     worker->velocity.x = -BARRY_VX;
  //   }
  // }
}

void check_level() {
  if (distance > level * LEVEL_CLEAR_DISTANCE) {
    level++;
    BARRY_VX = BASE_LEVEL_SPEED + level * LEVEL_SPEED_INCREASE;
    wave_timer = 3;

    update_all_vx();
  }
}

void render_progress(float percentage, glm::vec2 pos, glm::vec2 size,
                     glm::vec3 fg = Colors::green, glm::vec3 bg = Colors::black,
                     float rot = 0, float alpha = 1.0f) {
  G->SR->render(A->Textures["Progress"], pos, {size.x, size.y}, rot, 0, bg,
                alpha);
  G->SR->render(A->Textures["Progress"], pos, {size.x * percentage, size.y},
                rot, 0, fg, alpha);
}

void update_backdrop() {
  // Backdrops

  auto &backdrop1 = G->gameobjects["Backdrop1"];
  auto &backdrop2 = G->gameobjects["Backdrop2"];

  if (backdrop1->left_of(0)) {
    backdrop1->position.x = BG_WIDTH;
    // backdrop1->play(A->Textures["BackdropMain"]);
    backdrop1->play(A->Textures[Backdrops[rand() % Backdrops.size()]]);
  }
  if (backdrop2->left_of(0)) {
    backdrop2->position.x = BG_WIDTH;
    backdrop2->play(A->Textures[Backdrops[rand() % Backdrops.size()]]);
  }
}

void update() {
  update_backdrop();

  // Barry
  auto &barry = G->gameobjects["Barry"];

  if (jetpack) {
    // Jetpack bullets
    auto pos = barry->position;
    pos.x -= 5;
    pos.y += barry->size.y;
    if (rand() % 5 == 0) {
      bullets->respawn(pos, {0, -JETPACK_VY * 3}, -90, 1);
      bullet_flash = true;
      // Jetpack shells
      shells->respawn(pos, {-BARRY_VX * 1.5, -JETPACK_VY}, -45, 1, 5);
    } else {
      bullet_flash = false;
    }
  } else {
    bullet_flash = false;
  }
  bullets->update(G->camera.dt);
  shells->update(G->camera.dt);
  // check collision with bullets
  for (auto &bullet : bullets->particles) {
    if (bullet.position.y + 50 > GROUND_Y) {
      bullet.position.y = GROUND_Y - 50;
      bullet.velocity.y = 0;
    }
  }

  // Barry bullet flash
  auto &flash = A->Sprites["BulletFlash"];
  flash->update(G->camera.dt);

  // limit Barry's y position
  barry->update(G->camera.dt);

  if (barry->bottom_below(GROUND_Y)) {
    barry->position.y = GROUND_Y - barry->size.y;
    barry->velocity.y = 0;
  } else if (barry->top_above(ROOF_Y)) {
    barry->position.y = ROOF_Y;
    barry->velocity.y = 0;
  }

  // Coins

  // check collision with coins
  A->Sprites["Coin"]->update(G->camera.dt);
  A->Sprites["CoinCollected"]->update(G->camera.dt);
  for (auto &coin : coins) {
    if (coin->tag == "CoinCollected") {
      coin->velocity = COIN_COUNTER_POS - coin->position;
      coin->velocity *= 10;
    } else {
      if (coin->overlaps(barry)) {
        coin->tag = "CoinCollected";
        score += 1;
        sfx("coin_pickup");
      }
    }
    coin->update_physics(G->camera.dt);
  }

  // Zappers
  // check collision with zappers

  for (auto &zapper : zappers) {
    if (!dead)
      if (OBBcollision(zapper, barry, glm::radians(zapper->rotation))) {
        sfx("Player_bones");
        game_over();
      }
    // if (!dead)
    //   if (zapper->overlaps(barry)) {
    //     sfx("Player_bones");
    //     game_over();
    //   }

    zapper->update(G->camera.dt);
  }

  // TODO: check collision with rotating zappers

  // Missiles
  // check collision with missiles

  GameObject *front_missile = nullptr;

  for (auto &missile : missiles) {
    if (!dead)
      if (missile->overlaps(barry)) {
        sfx("rocket_explode_1");
        game_over();
      }

    missile->update(G->camera.dt);
    if (missile->right_of(WIDTH)) {
      if (!front_missile) {
        front_missile = missile;
      }
      if (front_missile->left_of(WIDTH * 1.2)) sfx("missile_warning");
      if (front_missile->left_of(WIDTH * 1.1)) sfx("missile_launch");
    }
    if (missile->left_of(WIDTH)) {
      // don't aim while on screen
      missile->velocity.y = 0;
    }
  }

  if (front_missile)
    if (front_missile->right_of(WIDTH)) {
      // aim front_missile at Barry
      front_missile->velocity.y =
          (barry->position.y - front_missile->position.y) * 2;
    }

  // Lasers
  // check collision with lasers
  for (auto &laser : lasers) {
    if (!dead)
      if (laser->tag == "LaserActive")
        if (laser->overlaps(barry)) {
          sfx("laser_fire_lp");
          game_over();
        }

    // don't move laser if it's active
    if (laser->timer > 0)
      laser->sprite->update(G->camera.dt);
    else {
      // laser leaves screen
      laser->update(G->camera.dt);
      if (laser->tag == "LaserActive") {
        laser->tag = "LaserInactive";
        sfx("laser_stop");
      }
    }

    laser->timer -= G->camera.dt;

    // laser enters screen
    if (laser->left_of(WIDTH))
      if (laser->tag == "LaserBegin") {
        sfx("laser_start");
        laser->tag = "LaserActive";
        laser->timer = LASER_DURATION;
        std::cout << "Laser Active!" << std::endl;
      }
  }

  // Workers
  for (auto &worker : workers) {
    worker->update(G->camera.dt);
  }

  check_wave();
  check_level();

  // if dead, slow down
  if (dead) {
    auto &backdrop1 = G->gameobjects["Backdrop1"];
    auto &backdrop2 = G->gameobjects["Backdrop2"];

    backdrop1->velocity.x *= BRAKE;
    backdrop2->velocity.x *= BRAKE;
    for (auto &coin : coins) coin->velocity.x *= BRAKE;
    for (auto &zapper : zappers) zapper->velocity.x *= BRAKE;
    for (auto &laser : lasers) laser->velocity.x *= BRAKE;

    barry->velocity.x *= BRAKE;
  } else {
    distance += G->camera.dt * BARRY_VX * METERS_PER_PIXEL;
  }
}

void debug_renders() { G->render_FPS_counter(); }

void render_HUD() {
  debug_renders();

  // level
  std::string level_text = "Level " + std::to_string(level);
  auto x = WIDTH - G->TR->get_BB(level_text).x;
  render_progress(
      fmod(distance, LEVEL_CLEAR_DISTANCE) / (level * LEVEL_CLEAR_DISTANCE),
      {WIDTH - 200, 0}, {200, 60});
  G->text(level_text, x, 50, 1.0f, Colors::white);

  // distance and score
  G->text(std::to_string((int)distance) + "m", 0, 50, 1.0f, Colors::yellow);
  G->text(score, 40, 100, 1.0f, Colors::yellow);

  // coin counter
  G->SR->render(A->Textures["Coin"], COIN_COUNTER_POS, {32, 32});

  // enemy list
  if (level >= 1)
    G->SR->render(A->Textures["Zapper1"], {WIDTH - 250, 0}, {32, 64}, 45);
  if (level >= 2)
    G->SR->render(A->Textures["Rocket1"], {WIDTH - 320, 0}, {64, 32});
  if (level >= 3)
    G->SR->render(A->Textures["LaserIcon"], {WIDTH - 370, 0}, {64, 32});
}

void sound_effects() {
  // Laser
  for (auto &laser : lasers) {
    if (laser->tag == "LaserActive") {
      sfx("laser_warning");
    }
  }

  // Barry
  auto &barry = G->gameobjects["Barry"];
  if (!dead && !barry->above(GROUND_Y)) {
    run++;
    sfx(("run_metal_left_" + std::to_string(run % 4 + 1)));
  }
}

void render_shadow(GameObject *obj, float alpha = 0.7) {
  float ratio = (GROUND_Y - obj->position.y) / GROUND_Y;
  glm::vec2 shadow_size = glm::mix(obj->size, obj->size / 4.0f, ratio);
  if (obj->sprite)
    G->SR->render(obj->sprite->get(),
                  {obj->position.x + shadow_size.x / 2.0, GROUND_Y - 25},
                  shadow_size, obj->rotation, obj->sprite->frame, Colors::black,
                  (1 - ratio) * alpha);
  else
    G->SR->render(
        obj->texture, {obj->position.x + shadow_size.x / 2.0, GROUND_Y - 25},
        shadow_size, obj->rotation, 0, Colors::black, (1 - ratio) * alpha);
}

void render() {
  // Backdrops
  G->gameobjects["Backdrop1"]->render(G->SR);
  G->gameobjects["Backdrop2"]->render(G->SR);
  // auto &backdrop1 = G->gameobjects["Backdrop1"];
  // auto &backdrop2 = G->gameobjects["Backdrop2"];
  // G->SR->render(backdrop1->texture, backdrop1->position, {WIDTH, HEIGHT}, 0,
  // 0,
  //               Colors::white, 0.7);
  // G->SR->render(backdrop2->texture, backdrop2->position, {WIDTH, HEIGHT}, 0,
  // 0,
  //               Colors::white, 0.7);

  // Coins
  for (auto &coin : coins) {
    if (coin->tag == "CoinCollected") {
      // coin->play(A->Sprites["CoinCollected"]);
      coin->play(A->Textures["CoinCollected"]);
    } else {
      coin->play(A->Sprites["Coin"]);
      render_shadow(coin);
      // coin->play(A->Textures["Coin"]);
    }
    coin->render(G->SR);
  }

  render_HUD();

  // Laser
  for (auto &laser : lasers) {
    if (laser->tag == "LaserActive") {
      // laser glow
      glm::vec2 x = {laser->position.x, laser->position.y + laser->size.y / 2};
      glm::vec2 y = {laser->position.x + laser->size.x,
                     laser->position.y + laser->size.y / 2};
      G->GR->glow(x, y, 3, 0, Colors::red, 1.0f, pulse(0.2, 10));
      laser->play(A->Sprites["LaserFire"]);
      render_shadow(laser, pulse(0.7, 10, M_PI));
    } else {
      laser->play(A->Sprites["LaserCharge"]);
      render_shadow(laser);
    }
    laser->render(G->SR);
  }

  // Zapper
  for (auto &zapper : zappers) {
    // zapper glow
    G->GR->glow(zapper->position + zapper->size / 2.0f, {0, 0}, 1, 0,
                Colors::yellow, 1.0f);
    zapper->render(G->SR);
    render_shadow(zapper);
  }

  // Missile
  for (auto &missile : missiles) {
    missile->render(G->SR);

    if (missile->right_of(WIDTH)) {
      G->SR->render(A->Textures["RocketWarning"],
                    {WIDTH - 100, missile->position.y}, {64, 64});
    }
    render_shadow(missile);
  }

  // Barry
  auto &barry = G->gameobjects["Barry"];
  if (dead)
    barry->play(A->Textures["BarryDead"]);
  else if (barry->above(GROUND_Y))
    barry->play(A->Textures["BarryFly"]);
  else {
    barry->play(A->Sprites["BarryRun"]);
  }

  // shadows for Barry and workers
  float ratio = (GROUND_Y - barry->position.y) / GROUND_Y;
  auto shadow_size = glm::mix(glm::vec2(64, 64), glm::vec2(16, 16), ratio);
  G->SR->render(A->Textures["Shadow"], {barry->position.x + 10, GROUND_Y - 25},
                shadow_size, 0, 0, Colors::black, (1 - ratio) * 0.7);

  // Jetpack glow
  if (jetpack)
    G->GR->glow(barry->position + barry->size / 2.0f, {0, 0}, 1, 0,
                Colors::yellow);

  barry->render(G->SR);

  if (jetpack) {  // Barry bullet flash
    // render only when shooting bullets
    auto &flash = A->Sprites["BulletFlash"];
    auto pos = barry->position;
    pos.y += barry->size.y;
    G->SR->render(flash->get(), pos, {64, 64}, 0, flash->frame);
  }
  // Jetpack bullets
  bullets->render(G->SR);

  // Jetpack shells
  shells->render(G->SR);

  auto &collision = A->Sprites["BulletCollision"];
  collision->update(G->camera.dt);
  for (auto &p : bullets->particles) {
    if (p.is_dead()) continue;
    if (p.velocity.y == 0) {
      //   G->SR->render(collision->get(), p.position, {32, 32},
      //   collision->frame,
      //                 0);
      G->SR->render(collision->textures[collision->frame],
                    {p.position.x, p.position.y + 25}, {32, 32}, 0, 0);
    }
  }

  // Workers
  for (auto &worker : workers) worker->render(G->SR);
}

void game_controls() {
  // Barry
  auto &barry = G->gameobjects["Barry"];
  jetpack = false;
  if (!dead) {
    if (G->on_keypress(GLFW_KEY_SPACE)) {
      // if not touching roof
      barry->velocity.y += (JETPACK_VY - barry->velocity.y) * 0.1;
      jetpack = true;
      // barry->velocity.y = JETPACK_VY;
      sfx("jetpack_fireLP");
    }
  }
  // Other
  if (G->on_keyup(GLFW_KEY_C)) reload_coins();
  if (G->on_keyup(GLFW_KEY_Z)) reload_zappers();
  if (G->on_keyup(GLFW_KEY_M)) reload_missiles(level > 1 ? 2 : 1);
  if (G->on_keyup(GLFW_KEY_L)) reload_lasers(level > 1 ? 2 : 1);
  if (G->on_keyup(GLFW_KEY_R)) game_screen();

  if (G->on_keypress(GLFW_KEY_RIGHT)) {
    BARRY_VX += 10;
    update_all_vx();
  }
  if (G->on_keypress(GLFW_KEY_LEFT)) {
    BARRY_VX -= 10;
    update_all_vx();
  }

  if (G->on_keyup(GLFW_KEY_EQUAL)) {
    distance = (level)*LEVEL_CLEAR_DISTANCE;
    check_level();
  }

  if (G->on_keypress(GLFW_KEY_H)) sfx("coin_pickup");
}

void cleanup() {
  delete_gameobjects(coins);
  delete_gameobjects(zappers);
  delete_gameobjects(missiles);
  delete_gameobjects(lasers);
  delete_gameobjects(workers);
  stopAllSounds();
  if (bullets) bullets->particles.clear();
  if (shells) shells->particles.clear();
}

void init_game() {
  cleanup();

  sfx("Music_Level");

  BARRY_VX = 300;

  // game state
  distance = 0;
  score = 0;
  dead = false;
  jetpack = false;
  bullet_flash = false;
  level = 1;

  wave = COIN;
  wave_width = 0;
  wave_timer = 0;

  // sfx state
  run = 1;

  bullets = new ParticleGenerator(A->Textures["Bullet"], {40, 10}, 50);
  shells = new ParticleGenerator(A->Textures["Shell"], {20, 5}, 50);

  shells->velocity_blur = {50, 50};
  bullets->velocity_blur = {50, 50};

  G->gravity = {0, GRAVITY_Y};
  G->add("Barry", new GameObject(A->Textures["BarryFly"], {300, 200}, {60, 100},
                                 {0, 10}, false));
  G->add("Backdrop1", new GameObject(A->Textures["Lab Start"], {0, 0},
                                     {BG_WIDTH, HEIGHT}, {-BARRY_VX, 0}, true));
  G->add("Backdrop2", new GameObject(A->Textures["Lab1"], {BG_WIDTH, 0},
                                     {BG_WIDTH, HEIGHT}, {-BARRY_VX, 0}, true));
}