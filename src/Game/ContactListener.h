#pragma once
#include <SDL_events.h>
#include <box2d/b2_contact.h>
#include <print.h>
#include <SDL2/SDL.h>
#include <box2d/box2d.h>
#include "ECS/Entity.h"

class ContactListener : public b2ContactListener {
private:
  Uint32& collisionEvent;

public:
  ContactListener(Uint32& collisionEvent)
    : collisionEvent(collisionEvent) { }

  void BeginContact(b2Contact* contact) {
    if (collisionEvent != -1) {
      SDL_Event event;
      event.type = collisionEvent;
      event.user.code = 1;
      event.user.data1 = (void*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
      event.user.data2 = (void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
      SDL_PushEvent(&event);      
    }
  }

  void EndContact(b2Contact* contact) {
  }
};

