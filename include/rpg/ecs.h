#pragma once
#include "rpg/camera.h"
#include <SDL_render.h>
#include <array>
#include <bitset>
#include <entt/entt.hpp>
#include <memory>
#include <vector>

namespace rpg {

class Component
{
};

class System
{
public:
  virtual void update(entt::registry& registry) {}
  virtual void draw(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera) {}
  virtual int getLayer() const { return -1; }
};

class SystemRunner
{
public:
  void update(entt::registry& registry);
  void draw(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera);
  void addSystem(const std::shared_ptr<System>& system)
  {
    std::sort(m_systems.begin(), m_systems.end(), [](const auto& a, const auto& b) {
      return a->getLayer() < b->getLayer();
    });
    m_systems.push_back(system);
  }

private:
  std::vector<std::shared_ptr<System> > m_systems;
};

} // namespace rpg
