#pragma once

#include "rpg/ecs.h"
namespace rpg {

struct BodyComponent : public Component
{
  entt::entity body;
  std::vector<entt::entity> limbs;
};

struct LimbComponent : public Component
{
  entt::entity attached = entt::null;
  glm::vec2 offset;               // relativ to parent
  glm::vec2 orientation = {0, 0}; // relativ to parent
  bool vital;
};

class BodySystem : public System
{
public:
  void update(entt::registry& registry) override;
};

class LimbSystem : public System
{
public:
  void update(entt::registry& registry) override;
};
} // namespace rpg
