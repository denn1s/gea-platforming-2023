#include "SpriteSystems.h"

#include <SDL_stdinc.h>
#include <SDL_timer.h>
#include <print.h>
#include <constants.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "Game/Graphics/TextureManager.h"

void SpriteRenderSystem::run(SDL_Renderer* renderer) {
    auto view = scene->r.view<TransformComponent, SpriteComponent>();
    for(auto entity : view) {
        const auto spriteComponent = view.get<SpriteComponent>(entity);
        const auto transformComponent = view.get<TransformComponent>(entity);
        Texture* texture = TextureManager::GetTexture(spriteComponent.name, spriteComponent.shader.name);

        const int width = spriteComponent.w > 0 ? spriteComponent.w : texture->width;
        const int height = spriteComponent.h > 0 ? spriteComponent.h : texture->height;

        SDL_Rect clip = {
            spriteComponent.xIndex * width,
            spriteComponent.yIndex * height,
            width,
            height
        };

        texture->render(
            transformComponent.x,
            transformComponent.y,
            transformComponent.w,
            transformComponent.h,
            &clip
        );
    }
}

SpriteSetupSystem::~SpriteSetupSystem() {
    auto view = scene->r.view<SpriteComponent>();

    for(auto entity : view) {
        const auto spriteComponent = view.get<SpriteComponent>(entity);
        TextureManager::UnloadTexture(spriteComponent.name, spriteComponent.shader.name);
    }
}

SpriteSetupSystem::SpriteSetupSystem(SDL_Renderer* renderer)
    : renderer(renderer) { }

void SpriteSetupSystem::run() {
    auto view = scene->r.view<SpriteComponent>();

    for(auto entity : view) {
        const auto spriteComponent = view.get<SpriteComponent>(entity);
        TextureManager::LoadTexture(spriteComponent.name, renderer, spriteComponent.shader);
    }
}

void SpriteUpdateSystem::run(double dT) {
    auto view = scene->r.view<SpriteComponent>();

    Uint32 now = SDL_GetTicks();

    for(auto entity : view) {
        auto& spriteComponent = view.get<SpriteComponent>(entity);

        if (spriteComponent.animationFrames > 0) {
            if (!spriteComponent.lastUpdate) {
                spriteComponent.lastUpdate = SDL_GetTicks();
                continue;
            }

            float timeSinceLastUpdate = now - spriteComponent.lastUpdate;

            if (spriteComponent.delay != 0) {
                if (timeSinceLastUpdate < spriteComponent.delay) {
                    continue;
                } else {
                    spriteComponent.delay = 0;
                    spriteComponent.lastUpdate = 0;
                    timeSinceLastUpdate = 0;
                }
            }

            int framesToUpdate = static_cast<int>(
                timeSinceLastUpdate / 
                spriteComponent.animationDuration * spriteComponent.animationFrames
            );

            if (framesToUpdate > 0) {
                if (!spriteComponent.once || spriteComponent.xIndex < (spriteComponent.animationFrames - 1)) {
                    spriteComponent.xIndex += framesToUpdate;
                    spriteComponent.xIndex %= spriteComponent.animationFrames;
                    spriteComponent.lastUpdate = now;
                }
            }
        }
    }
}

