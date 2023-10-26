#pragma once

#include "Yoyo/Components.h"
#include "constants.h"

#include "ECS/Components.h"
#include "ECS/Entity.h"
#include "ECS/System.h"
#include <SDL_keycode.h>
#include <SDL_render.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_math.h>
#include <box2d/b2_world.h>
#include <cmath>

class CharacterSetupSystem : public SetupSystem {
public:
  void run() {
    scene->player = new Entity(scene->r.create(), scene);
    
    auto transform = scene->player->addComponent<TransformComponent>(0, 0, 16 * SCALE, 16 * SCALE);
    scene->player->addComponent<SpriteComponent>(
      "yoyo.png",
      16, 16,
      0, 3,
      2, 500
    );

    auto world = scene->world->get<PhysicsComponent>().b2d;

    float x = 20 * PIXELS_PER_METER; 
    float y = 45 * PIXELS_PER_METER; 
    float hx = (16.0f * PIXELS_PER_METER) / 2.0f;
    float hy = (16.0f * PIXELS_PER_METER) / 2.0f;

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x + hx, y + hy);

    b2Body* body = world->CreateBody(&bodyDef);

    b2PolygonShape box;
    box.SetAsBox(hx, hy);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &box;
    fixtureDef.density = 0.0000001f;
    fixtureDef.friction = 0.0f;
    
    body->CreateFixture(&fixtureDef);

    scene->player->addComponent<RigidBodyComponent>(
      bodyDef.type,
      body,
      transform.x,
      transform.y,
      transform.w,
      transform.h,
      SDL_Color{0, 255, 0}
    );
  }
};

class BgSetupSystem : public SetupSystem {
public:
  void run() {
    if (scene->world == nullptr) {
      scene->world = new Entity(scene->r.create(), scene);
    }
    const auto transform = scene->world->addComponent<TransformComponent>(16 * SCALE, 64 * SCALE, 96 * SCALE, 16 * SCALE);
    scene->world->addComponent<SpriteComponent>(
      "background.png",
      128, 112,
      0, 0
    );

    auto world = scene->world->get<PhysicsComponent>().b2d;

    float x = 16 * PIXELS_PER_METER; 
    float y = 64 * PIXELS_PER_METER; 
    float hx = (96 * PIXELS_PER_METER) / 2.0f;  // this is half width
    float hy = (16 * PIXELS_PER_METER) / 2.0f;

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(x + hx, y + hy);
    
    b2Body* body = world->CreateBody(&bodyDef);

    b2PolygonShape groundBox;
    groundBox.SetAsBox(hx, hy);

    body->CreateFixture(&groundBox, 0.0f);
    
    scene->world->addComponent<RigidBodyComponent>(
      bodyDef.type,
      body,
      transform.x,
      transform.y,
      transform.w,
      transform.h,
      SDL_Color{0, 0, 255}
    );
    
  }
};

class BgColorSystem : public RenderSystem {
public:
  void run(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 155, 155, 155, 255);
    SDL_Rect rect = {0, 0, SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE};
    SDL_RenderFillRect(renderer, &rect);
  }
};

class PhysicsSetupSystem : public SetupSystem {
public:
  void run() {
    if (scene->world == nullptr) {
      scene->world = new Entity(scene->r.create(), scene);
    }
    b2Vec2 gravity(0.0f, 50.0f * PIXELS_PER_METER);  // 100 p/m    9.8 m/s * 10 p/m = 98 p/s;
    scene->world->addComponent<PhysicsComponent>(new b2World(gravity));
  }
};

class PhysicsUpdateSystem : public UpdateSystem {
public:
  void run(double dT) {
    const int velocityIterations = 6;
    const int positionIterations = 2;

    auto world = scene->world->get<PhysicsComponent>().b2d;
    world->Step(dT, velocityIterations, positionIterations);
  }
};

class MovementUpdateSystem : public UpdateSystem {
public:
  void run(double dT) {
    const auto view = scene->r.view<RigidBodyComponent, TransformComponent>();

    for (const auto e : view) {
      const auto rb = view.get<RigidBodyComponent>(e);
      auto& transform = view.get<TransformComponent>(e);
      
      b2Vec2 position = rb.body->GetPosition(); // x, y meters

      // Convert the Box2D position (center of the body) to screen coordinates
      float centerX = position.x * SCALE / PIXELS_PER_METER; // 20 pixels * 8 pixels / 10 meters  
      float centerY = position.y * SCALE / PIXELS_PER_METER;

      // Adjust for the entity's dimensions to get the top-left corner
      transform.x = centerX - static_cast<float>(transform.w)/2.0f;
      transform.y = centerY - static_cast<float>(transform.h)/2.0f;
    }
  }
};

class FixtureRenderSystem : public RenderSystem {
public:
  void run(SDL_Renderer* renderer) {
    auto world = scene->world->get<PhysicsComponent>().b2d;

    for (b2Body* body = world->GetBodyList(); body != nullptr ; body = body->GetNext()) {
      for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
        b2Shape* shape = fixture->GetShape();

        if (fixture->GetType() == b2Shape::e_polygon)
        {
          b2PolygonShape* polygonShape = (b2PolygonShape*)fixture->GetShape();

          // Ensure the rectangle has 4 vertices
          int vertexCount = polygonShape->m_count;
          if(vertexCount == 4)
          {
            // Considering vertices[1] and vertices[3] are opposite corners of the rectangle.
            b2Vec2 vertex1 = polygonShape->m_vertices[1];
            b2Vec2 vertex3 = polygonShape->m_vertices[3];

            // Calculating the width and height in meters
            float width = abs(vertex1.x - vertex3.x);
            float height = abs(vertex1.y - vertex3.y);

            // Getting the position of the body (center of mass) in meters
            b2Body* body = fixture->GetBody();
            b2Vec2 position = body->GetPosition();

            // Convert the values into pixel coordinates
            int PPM = PIXELS_PER_METER; // Pixels Per Meter scale
            float x_px = position.x / PPM;   // m  / (m / p)
            float y_px = position.y / PPM;
            float width_px = width / PPM;
            float height_px = height / PPM;

            // Create an SDL_Rect and initialize it
            SDL_Rect renderRect;
            renderRect.x = x_px - width_px/2.0f;  // Adjust for center of mass
            renderRect.y = y_px - height_px/2.0f; // Adjust for center of mass
            renderRect.w = width_px;
            renderRect.h = height_px;

            renderRect.x *= SCALE;
            renderRect.y *= SCALE;
            renderRect.w *= SCALE;
            renderRect.h *= SCALE;

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &renderRect);
          }
        }
      }
    }
  }
};

class MovementInputSystem : public EventSystem {
public:
  void run(SDL_Event event) {
    if (event.type == SDL_KEYDOWN) {
      // move
      if (event.key.keysym.sym == SDLK_LEFT) {
        print("Left!");
        moveCharacter(-1);
      } else if (event.key.keysym.sym == SDLK_RIGHT) {
        print("Right!");
        moveCharacter(1);
      } else if (event.key.keysym.sym == SDLK_SPACE) {
        print("jump!");
        jumpCharacter();
      }
    } else if (event.type == SDL_KEYUP) {
      // stop movement
      if (event.key.keysym.sym == SDLK_LEFT) {
        stopCharacter(-1);
      } else if (event.key.keysym.sym == SDLK_RIGHT) {
        stopCharacter(1);
      }
    }
  }

private:
  float hForceMagnitude = 1000.0f * SCALE * PIXELS_PER_METER;
  float vForceMagnitude = 50000.0f * SCALE * PIXELS_PER_METER;

  void moveCharacter(int direction) {
    const auto playerBody = scene->player->get<RigidBodyComponent>().body;
    playerBody->ApplyForceToCenter(b2Vec2(hForceMagnitude * direction, 0), true);

    auto& playerSprite = scene->player->get<SpriteComponent>();
    if (direction == -1) {
      playerSprite.yIndex = 0;
    } else {
      playerSprite.yIndex = 1;
    }
  }

  void stopCharacter(int direction) {
    const auto playerBody = scene->player->get<RigidBodyComponent>().body;
    playerBody->SetLinearVelocity(b2Vec2(0, 0));

    auto& playerSprite = scene->player->get<SpriteComponent>();
    if (direction == -1) {
      playerSprite.yIndex = 2;
    } else {
      playerSprite.yIndex = 3;
    }
  }

  void jumpCharacter() {
    const auto playerBody = scene->player->get<RigidBodyComponent>().body;
    playerBody->ApplyLinearImpulseToCenter(b2Vec2(0, -vForceMagnitude), true);
  }
};



