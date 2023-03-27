#pragma once

#include <rpg/ecs.h>
#include <glm/glm.hpp>
#include <utility>
#include "rpg/map.h"

namespace rpg {

struct PathComponent : public Component {
  std::vector<glm::vec2> path;
  glm::vec2 last_position;
  uint last_tick;
};

class PathFollowingSystem : public System
{
public:
  bool isApplicable(const Entity& entity) override;
  void update(const Entity& entity) override;
private:
  float m_min_distance = 0.1;
};

class PathPlanningSystem : public System {
public:
  PathPlanningSystem(Map::Ptr map) : m_map(std::move(map)) {}
  bool isApplicable(const Entity& entity) override;
  void update(const Entity& entity) override;
private:
  Map::Ptr m_map;
};

}
