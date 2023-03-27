
#include <rpg/movement.h>
#include <rpg/components.h>

using namespace rpg;

bool PathFollowingSystem::isApplicable(const Entity& entity)
{
  bool has_path     = entity.getComponent<PathComponent>() != nullptr;
  bool has_position = entity.getComponent<PositionComponent>() != nullptr;
  bool has_stats    = entity.getComponent<StatsComponent>() != nullptr;
  return has_position && has_path && has_stats;
}

void PathFollowingSystem::update(const Entity& entity)
{
  auto pose  = entity.getComponent<PositionComponent>();
  auto path  = entity.getComponent<PathComponent>();
  auto stats = entity.getComponent<StatsComponent>();

  if (path->path.empty())
  {
    return;
  }

  uint current_time = SDL_GetTicks();
  if (glm::length(path->path.at(0) - pose->pose) < m_min_distance)
  {
    pose->pose = path->path.at(0);
    path->path.erase(path->path.begin());
    path->last_position = pose->pose;
    path->last_tick   = current_time;
    return;
  }

  glm::vec2 diff = path->path.at(0) - path->last_position;
  float norm     = glm::length(diff);

  uint time_diff = current_time - path->last_tick;
  float traveled_distance = std::fminf(norm, (time_diff / 1000.0) * stats->speed);

  pose->pose = path->last_position + (traveled_distance / norm) * diff;
}
