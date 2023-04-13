#include "rpg/movement.h"
#include "rpg/components.h"
#include "rpg/game.h"
#include <SDL.h>
#include <algorithm>
#include <iostream>
#include <map>
#include <set>

using namespace rpg;

bool MoveTask::isDone(entt::registry& registry, const entt::entity& entity) const
{
  if (registry.all_of<StateComponent>(entity) && registry.get<StateComponent>(entity).state == State::DEAD) {
    return true;
  }
  return !registry.all_of<PathComponent>(entity);
}

void MoveTask::start(entt::registry& registry, const entt::entity& entity)
{
  auto& pose = registry.get<PositionComponent>(entity);
  std::vector<glm::vec2> path_to_goal = Game::map.findPath(pose.pose, m_goal);
  if (path_to_goal.empty()) {
    return;
  }
  auto& path = registry.emplace_or_replace<PathComponent>(entity);
  path.last_tick = SDL_GetTicks();
  path.path = path_to_goal;
  path.last_position = pose.pose;
}

void MoveTask::finish(entt::registry& registry, const entt::entity& entity)
{
  registry.remove<PathComponent>(entity);
}

void PathFollowingSystem::update(entt::registry& registry)
{
  for (auto &&[entity, pose, path, moveable] : registry.view<PositionComponent, PathComponent, MoveableComponent>().each())
  {
    if (path.path.empty())
    {
      registry.erase<PathComponent>(entity);
      continue;
    }

    uint current_time = SDL_GetTicks();
    if (glm::length(path.path.at(0) - pose.pose) < m_min_distance)
    {
      pose.pose = path.path.at(0);
      path.path.erase(path.path.begin());
      path.last_position = pose.pose;
      path.last_tick     = current_time;
      if (path.path.empty())
      {
        registry.erase<PathComponent>(entity);
      }
      continue;
    }

    glm::vec2 diff = path.path.at(0) - path.last_position;
    float norm     = glm::length(diff);

    pose.orientation = glm::normalize(diff);

    uint time_diff          = current_time - path.last_tick;
    float traveled_distance = std::fminf(norm, (time_diff / 1000.0) * moveable.speed);

    pose.pose = path.last_position + (traveled_distance / (norm + 0.00001F)) * diff;
  }
}

void PathFollowingSystem::draw(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera)
{
  for (auto &&[entity, path] : registry.view<PathComponent>().each())
  {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
    for (const auto& node : path.path)
    {
      glm::vec2 screen_pos = camera.realToScreen(node);

      SDL_Rect base = {
        static_cast<int>(screen_pos.x - 5.F), static_cast<int>(screen_pos.y - 5.F), 10, 10};
      SDL_RenderFillRect(renderer, &base);
    }
  }
}
