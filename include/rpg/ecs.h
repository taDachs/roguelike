#pragma once
#include <SDL_render.h>
#include <array>
#include <memory>
#include <vector>
#include <bitset>

namespace rpg {

const size_t MAX_COMPONENTS = 32;

using ComponentID = long long unsigned int;
using EntityID    = long long unsigned int;
using ComponentMask = std::bitset<MAX_COMPONENTS>;

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

template <typename T>
ComponentMask inline getComponentMask()
{
  ComponentID id = getComponentId<T>();
  return 1 << id;
}

template<typename T>
void inline addMask(ComponentMask& mask)
{
  mask |= getComponentMask<T>();
}

class Entity
{
public:
  Entity()
    : m_id(getNextComponentId())
  {
  }
  template <typename T>
  T& addComponent()
  {
    m_component_mask |= getComponentMask<T>();
    m_components[getComponentId<T>()] = std::make_unique<T>();
    return getComponent<T>();
  }
  template <typename T>
  T& getComponent() const
  {
    return *static_cast<T*>(m_components[getComponentId<T>()].get());
  }

  template <typename T>
  bool hasComponent() const {
    return (m_component_mask & getComponentMask<T>()).any();
  }

private:
  EntityID m_id;
  std::array<std::unique_ptr<Component>, MAX_COMPONENTS> m_components;
  ComponentMask m_component_mask;
};

class System
{
public:
  virtual bool isApplicable(const Entity& entity) { return true; }
  virtual void update(const Entity& entity) {}
  virtual void draw(const Entity& entity, SDL_Renderer* renderer) {}
};

class ECSManager
{
public:
  EntityID addEntity();
  void addSystem(std::shared_ptr<System> system);
  void update();
  void draw(SDL_Renderer* renderer);
  Entity& getEntity(EntityID id) const { return *m_entities[id]; }

private:
  std::vector<std::unique_ptr<Entity> > m_entities;
  std::vector<std::shared_ptr<System> > m_systems;
};

} // namespace rpg
