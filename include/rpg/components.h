#pragma once

#include "rpg/ecs.h"
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
};

struct StateComponent : public Component
{
  State state;
};

struct StatsComponent : public Component
{
  // in m/s
  float speed;
};

struct MoveableComponent : public Component
{
  // in m/s
  glm::vec2 current_direction;
  float velocity;
};


struct PlayerControlComponent : public Component
{
};

} // namespace rpg
