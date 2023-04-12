#include "rpg/attack_system.h"
#include "rpg/components.h"
#include "rpg/game.h"
#include "rpg/movement.h"
#include <SDL.h>
#include <iostream>

using namespace rpg;

bool AttackTask::isDone(entt::registry& registry, const entt::entity& entity) const
{
  return !registry.all_of<AttackComponent>(entity);
}

void AttackTask::start(entt::registry& registry, const entt::entity& entity) {
  auto& attack = registry.emplace_or_replace<AttackComponent>(entity);
  attack.target = m_target;
}

void AttackTask::update(entt::registry& registry, const entt::entity& entity)
{
  auto &&[pose, inventory, attack] = registry.get<PositionComponent, InventoryComponent, AttackComponent>(entity);
  float target_distance = pose.distance(registry.get<PositionComponent>(m_target));

  bool in_range = false;

  for (const auto& equipped_id : inventory.equipped) {
    auto* weapon = registry.try_get<WeaponComponent>(equipped_id);
    if (!weapon) {
      continue;
    }

    auto& target_position = registry.get<PositionComponent>(m_target);
    if (target_distance < weapon->range) {
      in_range = true;
    }
  }

  if (!in_range) {
    registry.erase<AttackComponent>(entity);
  }
}

void AttackTask::finish(entt::registry& registry, const entt::entity& entity)
{
  registry.remove<AttackComponent>(entity);
}

void AttackSystem::update(entt::registry& registry)
{
  for (auto &&[entity, inventory, attack, pose] : registry.view<InventoryComponent, AttackComponent, PositionComponent>().each())
  {
    if (!registry.valid(attack.target))
    {
      registry.erase<AttackComponent>(entity);
      continue;
    }

    auto& target_health = registry.get<HealthComponent>(attack.target);
    auto& target_pose = registry.get<PositionComponent>(attack.target);

    float target_distance = pose.distance(target_pose);

    for (const auto& equipped_id : inventory.equipped) {
      auto* weapon = registry.try_get<WeaponComponent>(equipped_id);
      if (!weapon) {
        continue;
      }
      if (target_distance > weapon->range) {
        continue;
      }

      uint time_diff = SDL_GetTicks() - weapon->last_attack;
      if (time_diff > weapon->speed) {
        weapon->last_attack = SDL_GetTicks();
        target_health.health -= weapon->damage;;
      }
    }

    if (target_health.health <= 0)
    {
      registry.erase<AttackComponent>(entity);
    }
  }
}
