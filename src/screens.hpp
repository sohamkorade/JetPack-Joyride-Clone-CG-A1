#pragma once

#include "engine.hpp"
#include "jj.hpp"

void loading_screen() {
  A->loadTexture("Splashy", "assets/Splash.jpg");
  load_all(A);
}

void menu_screen() {
  sfx("menu_amb_lp");
  while (!G->closing()) {
    clear_screen();
    if (G->on_keyup(GLFW_KEY_SPACE)) break;
    if (G->on_keyup(GLFW_KEY_Q)) {
      G->close();
      break;
    }
    G->SR->render(G->bg_texture, {0, 0}, {G->width, G->height}, 0, 0,
                  Colors::grey);

    G->text("Press SPACE to start", 700, 150, 1.0f, Colors::green, nullptr,
            0.5f + pulse(0.5f, 10));
    G->text("Press Q to quit", 700, 200, 1.0f, Colors::red);

    G->doEvents();
  }
  stopAllSounds();
}

void pause_screen() {
  while (!G->closing()) {
    clear_screen();
    render();
    G->SR->render(A->Textures["Progress"], {0, 0}, {WIDTH, HEIGHT}, 0, 0,
                  Colors::grey, 0.5f);
    if (G->on_keyup(GLFW_KEY_SPACE)) break;
    if (G->on_keyup(GLFW_KEY_Q)) {
      G->close();
      break;
    }
    G->center_text("Game Paused", 0, -100, 1.0f, Colors::yellow);
    G->center_text("Press SPACE to continue", 0, 50, 1.0f, Colors::green,
                   nullptr, 0.5f + pulse(0.5f, 10));
    G->center_text("Press Q to quit", 0, 100, 1.0f, Colors::red);
    G->doEvents();
  }
}

void game_over_screen() {
  while (!G->closing()) {
    clear_screen();
    render();
    G->SR->render(A->Textures["Progress"], {0, 0}, {WIDTH, HEIGHT}, 0, 0,
                  Colors::grey, 0.5f);
    if (G->on_keyup(GLFW_KEY_SPACE)) break;
    if (G->on_keyup(GLFW_KEY_R)) {
      game_screen();
      break;
    }
    G->center_text("Game Over", 0, -100, 1.0f, Colors::red);
    G->center_text("You collected " + std::to_string(score) + " coins!", 0, 50,
                   1.0f, Colors::yellow);
    G->center_text("Press R to Play Again", 0, 150, 1.0f, Colors::green,
                   nullptr, 0.5f + pulse(0.5f, 10));
    G->doEvents();
  }
}

void winner_screen() {
  while (!G->closing()) {
    clear_screen();
    render();
    G->SR->render(A->Textures["Progress"], {0, 0}, {WIDTH, HEIGHT}, 0, 0,
                  Colors::grey, 0.5f);
    if (G->on_keyup(GLFW_KEY_SPACE)) break;
    if (G->on_keyup(GLFW_KEY_R)) {
      game_screen();
      break;
    }
    G->center_text("You win", 0, -100, 1.0f, Colors::white);
    G->center_text("You collected " + std::to_string(score) + " coins!", 0, 50,
                   1.0f, Colors::yellow);
    G->center_text("Press R to Play Again", 0, 150, 1.0f, Colors::green,
                   nullptr, 0.5f + pulse(0.5f, 10));
    G->doEvents();
  }
}

void game_screen() {
  init_game();
  reload_coins("patterns/author.txt");
  reload_workers();

  while (!G->closing()) {
    clear_screen();
    if (G->on_keyup(GLFW_KEY_Q)) break;
    if (G->on_keyup(GLFW_KEY_P)) pause_screen();
    game_controls();
    update();
    G->update_physics();
    sound_effects();
    render();
    G->doEvents();

    if (level > 3 && wave_timer <= 0) {
      winner_screen();
      break;
    }
    // game over
    if (dead && wave_timer <= 0) {
      game_over_screen();
      break;
    }
  }

  cleanup();
}
