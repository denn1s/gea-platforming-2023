#pragma once

#include "Game/Game.h"

class Yoyo : public Game {
public:
  Yoyo();

  void setup() override;

private:
  Scene* createGamePlayScene();
  Uint32 collisionEvent;
};

