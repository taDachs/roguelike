#pragma once

#include "rpg/ecs.h"
#include "rpg/map.h"
#include "rpg/task_queue.h"
#include <SDL.h>
#include <glm/glm.hpp>
#include <utility>

namespace rpg {

struct PathComponent : public Component
{
  std::vector<glm::vec2> path;
  glm::vec2 last_position;
  glm::vec2 goal;
  uint last_tick;

  void setNewGoal(glm::vec2 new_goal)
  {
    last_tick     = SDL_GetTicks();
    goal          = new_goal;
    path.clear();
  }

  void abort()
  {
    last_tick     = SDL_GetTicks();
    goal          = last_position;
    path.clear();
  }
};

class MoveTask : public Task
{
public:
  MoveTask(const glm::vec2& goal, float tolerance) : m_goal(goal), m_tolerance(tolerance) {}
  bool isDone(entt::registry& registry, const entt::entity& entity) const override;
  void start(entt::registry& registry, const entt::entity& entity) override;
private:
  glm::vec2 m_goal;
  float m_tolerance;
};

class PathFollowingSystem : public System
{
public:
  void update(entt::registry& registry) override;
  void draw(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera) override;
  int getLayer() const override { return 5; }

private:
  float m_min_distance = 0.1;
};
} // namespace rpg
