#include "rpg/ecs.h"
#include <algorithm>

using namespace rpg;

void SystemRunner::update(entt::registry& registry)
{
  for (auto& system : m_systems)
  {
    system->update(registry);
  }
}

void SystemRunner::draw(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera)
{
  std::sort(m_systems.begin(), m_systems.end(), [](const auto& a, const auto& b) { return a->getLayer() < b->getLayer(); });
  for (auto& system : m_systems)
  {
    system->draw(registry, renderer, camera);
  }
}
