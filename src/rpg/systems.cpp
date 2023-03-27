#include "rpg/systems.h"
#include <algorithm>
#include <iostream>

using namespace rpg;

const float RUNNING_THRESH = 0.1;

bool MoveableSystem::isApplicable(const Entity& entity)
{
  bool has_moveable = entity.getComponent<MoveableComponent>() != nullptr;
  bool has_position = entity.getComponent<PositionComponent>() != nullptr;
  return has_moveable && has_position;
}

void MoveableSystem::update(const Entity& entity)
{
  auto position = entity.getComponent<PositionComponent>();
  auto moveable = entity.getComponent<MoveableComponent>();
  auto state    = entity.getComponent<StateComponent>();
  auto stats    = entity.getComponent<StatsComponent>();

  if (state != nullptr)
  {
    if (glm::length(moveable->current_direction) > 0 && stats != nullptr)
    {
      state->state = (stats->speed - moveable->velocity) < RUNNING_THRESH ? "running" : "walking";
    }
    else
    {
      state->state = "idle";
    }
  }

  uint current_time = SDL_GetTicks();
  uint diff         = current_time - m_last_tick;
  if (diff < m_tick_delay)
  {
    return;
  }
  m_last_tick = current_time;

  float seconds = static_cast<float>(diff) / 1000;

  if (moveable->current_direction.x > 0)
  {
    position->orientation = PositionComponent::Orientation::RIGHT;
  }
  if (moveable->current_direction.x < 0)
  {
    position->orientation = PositionComponent::Orientation::LEFT;
  }

  position->pose += moveable->current_direction * moveable->velocity * seconds;
}

