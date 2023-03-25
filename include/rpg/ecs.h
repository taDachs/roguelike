#pragma once
#include <array>
#include <memory>
#include <vector>

namespace rpg {

using ComponentID = long long unsigned int;
using EntityID    = long long unsigned int;

class Component
{
};

ComponentID inline getNextComponentId()
{
  static ComponentID next_id = 0;
  return next_id++;
}

template <typename T>
ComponentID inline getComponentId()
{
  static ComponentID component_id = getNextComponentId();
  return component_id;
}

const size_t MAX_COMPONENTS = 32;

class Entity
{
public:
  Entity()
    : m_id(getNextComponentId())
  {
  }
  template <typename T>
  void addComponent(std::shared_ptr<T> comp)
  {
    m_components[getComponentId<T>()] = comp;
  }
  template <typename T>
  std::shared_ptr<T> getComponent() const
  {
    return std::static_pointer_cast<T>(m_components[getComponentId<T>()]);
  }

private:
  EntityID m_id;
  std::array<std::shared_ptr<Component>, MAX_COMPONENTS> m_components;
};

class System
{
public:
  virtual bool isApplicable(const Entity& entity) = 0;
  virtual void update(const Entity& entity)       = 0;
};

class Manager
{
public:
  EntityID addEntity();
  void addSystem(std::unique_ptr<System> system);
  void update();
  Entity& getEntity(EntityID id) const { return *m_entities[id]; }

private:
  std::vector<std::unique_ptr<Entity> > m_entities;
  std::vector<std::unique_ptr<System> > m_systems;
};

} // namespace rpg
