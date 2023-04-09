#pragma once

#include "rpg/components.h"
#include "rpg/ecs.h"
#include "rpg/map.h"
#include "rpg/sprite.h"
#include <SDL.h>
#include <glm/glm.hpp>
#include <utility>

namespace rpg {

struct RenderComponent : public Component
{
  Sprite::Ptr sprite;
};

class RenderSystem : public System
{
public:
  RenderSystem(std::shared_ptr<Map> map)
    : m_map(std::move(map))
  {
  }
  bool isApplicable(const Entity& entity) override;
  void draw(const Entity& entity, SDL_Renderer* renderer) override;

private:
  std::shared_ptr<Map> m_map;
};

struct AnimationComponent : public Component
{
  std::map<State, Sprite::Ptr> sprite_map;
  State previous_state;
};

class AnimationSystem : public System
{
public:
  bool isApplicable(const Entity& entity) override;
  void update(const Entity& entity) override;
};
} // namespace rpg
