#pragma once

#include "rpg/ecs.h"
#include "rpg/map.h"
#include <SDL.h>
#include <glm/glm.hpp>
#include <utility>

namespace rpg {

struct PathComponent : public Component
{
  std::vector<glm::vec2> path;
  glm::vec2 last_position;
  glm::vec2 goal;
  bool reached = true;
  uint last_tick;
};

class PathFollowingSystem : public System
{
public:
  PathFollowingSystem(Map::Ptr map)
    : m_map(std::move(map))
  {
  }
  bool isApplicable(const Entity& entity) override;
  void update(const Entity& entity) override;
  void draw(const Entity& entity, SDL_Renderer* renderer) override;

private:
  float m_min_distance = 0.1;
  Map::Ptr m_map;
};

class PathPlanningSystem : public System
{
public:
  PathPlanningSystem(Map::Ptr map)
    : m_map(std::move(map))
  {
  }
  bool isApplicable(const Entity& entity) override;
  void update(const Entity& entity) override;

private:
  Map::Ptr m_map;
};

} // namespace rpg
