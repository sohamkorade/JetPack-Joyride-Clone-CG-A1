#pragma once

#include <irrKlang.h>

#include <map>
#include <string>

irrklang::ISoundEngine *sfx_engine = irrklang::createIrrKlangDevice();

#define SOUNDS_DIR "assets/sounds/"

void playSound(const char *x, bool loop = false, bool press = false) {
  if (sfx_engine->isCurrentlyPlaying(x)) return;
  auto temp = sfx_engine->play2D(x, loop, false, true);
  if (press) {
    auto L = temp->getPlayLength();
    temp->setPlayPosition(L - 250);
  }
}

#define rnd(f, x) \
  (SOUNDS_DIR f "_" + std::to_string(rand() % x + 1) + ".ogg").c_str()
#define fix(f) SOUNDS_DIR f ".ogg"
#define check(f, z) \
  if (alias == f) { \
    z;              \
    return;         \
  }
#define effect(f) check(f, playSound(fix(f), false))
#define effects(f, x) check(f, playSound(rnd(f, x), false))
#define loop(f) check(f, playSound(fix(f), true))
#define loops(f, x) check(f, playSound(rnd(f, x), true))
#define press(f) check(f, playSound(fix(f), false, true))

void stopAllSounds() { sfx_engine->stopAllSounds(); }

void setSoundVolume(float volume) { sfx_engine->setSoundVolume(volume); }

void sfx(std::string alias) {
  effects("coin_pickup", 3);
  effects("fall_bounce", 3);
  effect("missile_launch");
  effect("missile_warning");
  effect("Player_bones");
  effect("rocket_explode_1");

  effect("run_metal_left_1");
  effect("run_metal_left_2");
  effect("run_metal_left_3");
  effect("run_metal_left_4");

  effect("laser_fire_lp");
  effect("laser_start");
  effect("laser_stop");
  effect("laser_warning");

  press("jetpack_fireLP");

  loops("Music_Level", 3);
  loops("menu_amb_lp", 4);

  std::cout << "Sound effect not found: " << alias << std::endl;
}

#undef rnd
#undef fix
#undef check
#undef effect
#undef effects
#undef loop
#undef loops
#undef press
