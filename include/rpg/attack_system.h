#pragma once

#include "rpg/ecs.h"
#include "task_queue.h"

namespace rpg {

class AttackTask : public Task
{
public:
  AttackTask(const entt::entity& target)
    : m_target(target)
  {
  }
  bool isDone(entt::registry& registry, const entt::entity& entity) const override;
  void start(entt::registry& registry, const entt::entity& entity) override;
  void update(entt::registry& registry, const entt::entity& entity) override;
  void finish(entt::registry& registry, const entt::entity& entity) override;

private:
  entt::entity m_target;
};


struct WeaponComponent : public Component
{
  float damage;
  float speed; // delay between shots
  float range; // in real coords

  uint last_attack = 0;
};

struct AttackComponent : public Component
{
  entt::entity target = entt::null;
};

class AttackSystem : public System
{
public:
  void update(entt::registry& registry) override;

private:
};
} // namespace rpg
