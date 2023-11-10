#pragma once

#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/box2d.h>
#include <SDL2/SDL.h>

struct PhysicsComponent {
  b2World* b2d = nullptr;
};

struct RigidBodyComponent {
  b2BodyType type = b2_staticBody;
  b2Body* body = nullptr;
  int xo;  // meters
  int yo;
  int w;
  int h;
  SDL_Color color = { 255, 255, 255 };
  float density = 1.0f;
  float friction = 0.3f;
  b2Fixture* fixture = nullptr;
};

struct LifeComponent {
  int hp;
};
