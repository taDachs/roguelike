#pragma once

#include "rpg/ecs.h"
#include "rpg/components.h"
#include <SDL.h>

namespace rpg {

class RenderSystem : public System {
public:
  RenderSystem(SDL_Renderer* renderer) : m_renderer(renderer), System() {}
  bool isApplicable(const Entity& entity) override;
  void update(const Entity& entity) override;

private:
  SDL_Renderer* m_renderer;
};

class AnimationSystem : public System {
public:
  bool isApplicable(const Entity& entity) override;
  void update(const Entity& entity) override;
};
}

