#pragma once

#include "rpg/ecs.h"
#include "rpg/map.h"
#include "rpg/task_queue.h"
#include <SDL.h>
#include <glm/glm.hpp>
#include <utility>

namespace rpg {

struct MoveableComponent : public Component
{
  float speed;
};

struct PathComponent : public Component
{
  std::vector<glm::vec2> path;
  glm::vec2 last_position;
  uint last_tick;
};

class MoveTask : public Task
{
public:
  MoveTask(const glm::vec2& goal, float tolerance)
    : m_goal(goal)
    , m_tolerance(tolerance)
  {
  }
  bool isDone(entt::registry& registry, const entt::entity& entity) const override;
  void start(entt::registry& registry, const entt::entity& entity) override;
  void finish(entt::registry& registry, const entt::entity& entity) override;

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
