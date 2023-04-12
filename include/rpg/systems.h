#pragma once

#include "rpg/components.h"
#include "rpg/ecs.h"
#include "rpg/map.h"
#include <SDL.h>
#include <glm/glm.hpp>
#include <utility>

namespace rpg {

class HealthSystem : public System
{
public:
  void update(entt::registry& registry) override;
  void draw(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera) override;
  int getLayer() const override {
    return 20;
  }
};

class StateSystem : public System
{
public:
  void update(entt::registry& registry) override;
};
} // namespace rpg
