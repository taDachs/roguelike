#include "rpg/systems.h"
#include <iostream>

using namespace rpg;

bool MoveableSystem::isApplicable(const Entity& entity)
{
  bool has_moveable = entity.getComponent<PositionComponent>() != nullptr;
  bool has_position = entity.getComponent<PositionComponent>() != nullptr;
  return has_moveable && has_position;
}

void MoveableSystem::update(const Entity& entity)
{
  auto position = entity.getComponent<PositionComponent>();
  auto moveable = entity.getComponent<MoveableComponent>();

  auto state = entity.getComponent<StateComponent>();
  if (state != nullptr)
  {
    if (glm::length(moveable->current_direction) > 0)
    {
      state->state = moveable->is_running ? "running" : "walking";
    }
    else
    {
      state->state = "idle";
    }
  }

  uint current_time = SDL_GetTicks();
  uint diff         = current_time - m_last_time;
  if (diff < m_tick_delay)
  {
    return;
  }
  m_last_time = current_time;

  float seconds = static_cast<float>(diff) / 1000;

  if (moveable->current_direction.x > 0)
  {
    position->orientation = PositionComponent::Orientation::RIGHT;
  }
  if (moveable->current_direction.x < 0)
  {
    position->orientation = PositionComponent::Orientation::LEFT;
  }

  float velocity = moveable->is_running ? moveable->running_velocity : moveable->walking_velocity;

  position->pose += moveable->current_direction * velocity * seconds;
}
