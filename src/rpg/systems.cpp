#include "rpg/systems.h"
#include <algorithm>
#include <iostream>

using namespace rpg;

bool MoveableSystem::isApplicable(const Entity& entity)
{
  bool has_moveable = entity.getComponent<PositionComponent>() != nullptr;
  bool has_position = entity.getComponent<MoveableComponent>() != nullptr;
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

bool PathFollowingSystem::isApplicable(const Entity& entity)
{
  bool has_path     = entity.getComponent<PathFollowingComponent>() != nullptr;
  bool has_moveable = entity.getComponent<MoveableComponent>() != nullptr;
  bool has_position = entity.getComponent<PositionComponent>() != nullptr;
  return has_moveable && has_position && has_path;
}

void PathFollowingSystem::update(const Entity& entity)
{
  auto pose = entity.getComponent<PositionComponent>();
  auto path = entity.getComponent<PathFollowingComponent>();
  auto moveable = entity.getComponent<MoveableComponent>();

  if (path->path.empty()) {
    moveable->current_direction.x = 0;
    moveable->current_direction.y = 0;
    return;
  }

  auto min =
    std::min_element(path->path.begin(), path->path.end(), [&](const auto& a, const auto& b) {
      return glm::length(a - pose->pose) < glm::length(b - pose->pose);
    });

  glm::vec2 diff = *min - pose->pose;

  if (glm::length(diff) < m_min_distance) {
    path->path.erase(path->path.begin(), min + 1);
  }

  moveable->current_direction = glm::normalize(diff);

}
