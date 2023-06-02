#include "screens.hpp"

int main(int argc, char *argv[]) {
  srand(time(NULL));

  G = new Game(WIDTH, HEIGHT, "Assignment 1");
  A = new Assets();

  loading_screen();

  while (!G->closing()) {
    menu_screen();
    game_screen();
  }

  A->clear();

  delete G;
  delete A;
}
