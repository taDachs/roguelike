#pragma once

#include "rpg/ecs.h"
#include <entt/entt.hpp>

namespace rpg {
struct RangedAIComponent : public Component
{
  float perimeter_radius;
};

class RangedAISystem : public System
{
public:
  void update(entt::registry& registry) override;
};

} // namespace rpg
