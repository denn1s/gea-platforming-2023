#pragma once

#include "Game/Game.h"

class Yoyo : public Game {
public:
  Yoyo();

  void setup() override;

private:
  Uint32 collisionEvent;
  Scene* createGamePlayScene();
};

