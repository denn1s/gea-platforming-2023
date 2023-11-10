#include "Yoyo.h"

#include "constants.h"

#include "Game/Systems/SpriteSystems.h"
#include "Game/Systems/Systems.h"
#include "Systems.h"

Yoyo::Yoyo()
  : Game("SAMPLE", SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE) { }

void Yoyo::setup() {
  Scene* gameplay = createGamePlayScene();

  setScene(gameplay);
}

Scene* Yoyo::createGamePlayScene() {
  Scene* scene = new Scene("GAMEPLAY SCENE", r);
  addSetupSystem<PhysicsSetupSystem>(scene);
  addSetupSystem<CharacterSetupSystem>(scene);
  addSetupSystem<EnemySetupSystem>(scene);
  addSetupSystem<BgSetupSystem>(scene);
  addSetupSystem<SpriteSetupSystem>(scene, renderer);
  addUpdateSystem<SpriteUpdateSystem>(scene);
  addRenderSystem<BgColorSystem>(scene);
  addSetupSystem<CollisionEventSetupSystem>(scene, collisionEvent);
  addEventSystem<ContactEventSystem>(scene, collisionEvent);
  addUpdateSystem<HpCheckUpdateSystem>(scene);

  addUpdateSystem<PhysicsUpdateSystem>(scene);
  addUpdateSystem<MovementUpdateSystem>(scene);
  addEventSystem<MovementInputSystem>(scene);
  
  addRenderSystem<SpriteRenderSystem>(scene);
  addRenderSystem<FixtureRenderSystem>(scene);
  return scene;
}
