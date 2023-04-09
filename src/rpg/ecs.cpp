#include "rpg/ecs.h"

using namespace rpg;

EntityID ECSManager::addEntity()
{
  auto e    = std::make_unique<Entity>();
  size_t id = m_entities.size();
  m_entities.push_back(std::move(e));
  return id;
}

void ECSManager::addSystem(std::shared_ptr<System> system)
{
  m_systems.push_back(std::move(system));
}

void ECSManager::update()
{
  for (const auto& e : m_entities)
  {
    for (const auto& s : m_systems)
    {
      if (s->isApplicable(*e))
      {
        s->update(*e);
      }
    }
  }
}

void ECSManager::draw(SDL_Renderer* renderer)
{
  for (const auto& e : m_entities)
  {
    for (const auto& s : m_systems)
    {
      if (s->isApplicable(*e))
      {
        s->draw(*e, renderer);
      }
    }
  }
}

void ECSManager::cleanUp()
{
  m_entities.clear();
  m_systems.clear();
}
