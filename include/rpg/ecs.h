#pragma once
#include "rpg/camera.h"
#include <SDL_render.h>
#include <array>
#include <bitset>
#include <memory>
#include <vector>
#include <entt/entt.hpp>

namespace rpg {

const size_t MAX_COMPONENTS = 32;

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

class SystemRunner {
public:
  void update(entt::registry& registry);
  void draw(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera);
  void addSystem(const std::shared_ptr<System>& system) { m_systems.push_back(system); }

private:
  std::vector<std::shared_ptr<System>> m_systems;
};

} // namespace rpg
