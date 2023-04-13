#pragma once

#include "rpg/ecs.h"
#include <functional>
#include <glm/glm.hpp>
#include <queue>

namespace rpg {

enum State
{
  WALKING,
  IDLE,
  RUNNING,
  SHOOTING,
  DEAD
};

struct PositionComponent : public Component
{
  glm::vec2 pose;
  glm::vec2 orientation = {0, -1};

  float distance(const PositionComponent& other) const
  {
    return glm::length(other.pose - pose);
  }
};

struct StateComponent : public Component
{
  State state;
};

struct HealthComponent : public Component {
  float health;
  float max_health;
};

struct StatsComponent : public Component
{
  // in m/s
  float speed;
};

struct InventoryComponent : public Component
{
  std::vector<entt::entity> slots;
  size_t size;
};

struct SlotComponent : public Component
{
  entt::entity equipped;
  glm::vec2 offset;
  glm::vec2 orientation;
};


struct PlayerControlComponent : public Component
{
};

struct PlayerFactionComponent : public Component
{
};


} // namespace rpg
