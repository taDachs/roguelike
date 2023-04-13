#pragma once

#include "rpg/components.h"
#include "rpg/ecs.h"
#include "rpg/map.h"
#include "rpg/sprite.h"
#include <SDL.h>
#include <glm/glm.hpp>
#include <utility>

namespace rpg {

struct SpriteComponent : public Component
{
  SDL_Texture* texture;
  SDL_Rect frame;
  bool flipped;
  int layer = 0;
};

class SpriteSystem : public System
{
public:
  void update(entt::registry& registry) override;
  void draw(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera) override;
  int getLayer() const override { return 10; }
};

struct AnimationStateComponent : public Component
{
  std::map<State, Sprite::Ptr> sprite_map;
  State previous_state;
};

class AnimationStateSystem : public System
{
public:
  void update(entt::registry& registry) override;
};
} // namespace rpg
