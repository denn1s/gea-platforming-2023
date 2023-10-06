#pragma once

#include "Game/Game.h"

class Sample : public Game {
public:
  Sample();

  void setup() override;

private:
  Scene* createGamePlayScene();
};

