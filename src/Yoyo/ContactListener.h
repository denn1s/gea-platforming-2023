#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <box2d/box2d.h>

class ContactListener : public b2ContactListener {
  private:
    Uint32 collisionEvent;

  public:
    ContactListener(Uint32 ce) : collisionEvent(ce) {
        std::cout << "CONTAACT CLASS" << std::endl;
    }

    void BeginContact(b2Contact* contact) {
        std::cout << "BeginContact" << std::endl;

        if (collisionEvent != ((Uint32)-1)) {
            SDL_Event event;
            SDL_memset(&event, 0, sizeof(event));
            event.type = collisionEvent;
            event.user.code = 1;
            event.user.data1 = (void*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
            event.user.data2 = (void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
            SDL_PushEvent(&event);
        }
    }

    void EndContact(b2Contact* contact) {
        std::cout << "EndContact" << std::endl;

        /*
        void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
        if (bodyUserData) {
        static_cast<Entity*>(bodyUserData)->endContact();
        }
        bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
        if (bodyUserData) {
        static_cast<Ball*>(bodyUserData)->endContact();
        }
        */
    }
};
