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
  enum Orientation
  {
    LEFT,
    RIGHT,
  };
  glm::vec2 pose;
  Orientation orientation = RIGHT;

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
};

struct StatsComponent : public Component
{
  // in m/s
  float speed;
};

struct InventoryComponent : public Component
{
  std::vector<entt::entity> equipped;
};


struct PlayerControlComponent : public Component
{
};

struct HandsComponent : public Component
{
  entt::entity left = entt::null;
  entt::entity right = entt::null;
};

} // namespace rpg
