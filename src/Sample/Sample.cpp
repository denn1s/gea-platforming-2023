#include "Sample.h"

Sample::Sample()
  : Game("SAMPLE", 1200, 800) { }

void Sample::setup() {
  Scene* gameplay = createGamePlayScene();

  setScene(gameplay);
}

Scene* Sample::createGamePlayScene() {
  Scene* scene = new Scene("GAMEPLAY SCENE", r);

  return scene;
}
