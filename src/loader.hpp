#pragma once

#include "assets.hpp"

extern Game *G;

void load_all(Assets *A) {
  G->bg_texture = A->loadTexture("Splashy", "assets/Splash.jpg");
  A->load_textures();

  float progress = 0;
  float total = 100;

  if (!progress_bg("Loading sprites...", progress = 50, total)) return;

  // barry runnning
  A->loadSprites("BarryRun", "BarryRun", 3, 0.1f);

  // zapper
  A->loadSprites("Zapper", "Zapper", 4, 0.1f);

  // missile
  A->loadSprites("Rocket", "Rocket", 3, 0.1f);

  if (!progress_bg("Loading spritesheets...", progress = 75, total)) return;

  // coin
  A->loadSpriteArray("Coin", "assets/spritesheets/Coin.png", 8, 1, 0.1f);
  A->loadSpriteArray("CoinCollected", "assets/spritesheets/CoinCollected.png",
                     4, 1, 0.1f);

  // laser
  A->loadSprites("LaserCharge", "LaserCharge", 3, 0.1f);
  A->loadSprites("LaserFire", "LaserFire", 2, 0.1f);

  // bullets
  A->loadSprites("BulletCollision", "BulletCollision", 3, 0.1f);
  // A->loadSpriteArray("BulletCollision",
  // "assets/spritesheets/BulletSplash.png",
  //                    4, 1, 0.1f);
  // A->loadTexture("Bullet", "assets/sprites/Bullet.png");

  // shells
  A->loadTexture("Shell", "assets/sprites/Shell.png");

  A->loadSpriteArray("BulletFlash", "assets/spritesheets/BulletFlash.png", 4, 1,
                     0.1f);

  // Barry's shadow
  A->loadTexture("Shadow", "assets/sprites/Shadow.png");

  if (!progress_bg("Loading complete...", progress = 100, total)) return;
}