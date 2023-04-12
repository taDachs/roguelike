#include "rpg/ranged_ai.h"
#include "rpg/components.h"
#include "rpg/movement.h"
#include "rpg/task_queue.h"
#include "rpg/attack_system.h"

using namespace rpg;


void RangedAISystem::update(entt::registry& registry)
{
  for (auto &&[entity, pose, queue, ai, inventory] : registry.view<PositionComponent, TaskQueueComponent, RangedAIComponent, InventoryComponent>().each())
  {
    for (auto &&[target_entity, target_pose] : registry.view<PositionComponent, PlayerFactionComponent>().each())
    {
      float target_distance = target_pose.distance(pose);
      if (target_distance > ai.perimeter_radius) {
        continue;
      }

      // check if any weapon is in range
      bool in_range = false;
      for (const auto& equipped_id : inventory.equipped) {
        auto* weapon = registry.try_get<WeaponComponent>(equipped_id);
        if (!weapon) {
          continue;
        }

        if (target_distance < weapon->range) {
          in_range = true;
        }
      }

      if (!in_range) {
        registry.remove<AttackComponent>(entity);
        auto move_task =
          std::make_unique<MoveTask>(target_pose.pose, 0.5);
        auto& queue = registry.get<TaskQueueComponent>(entity);
        queue.queue.push(std::move(move_task));
      } else {
        registry.remove<PathComponent>(entity);
        auto attack_task   = std::make_unique<AttackTask>(target_entity);
        queue.queue        = {};
        queue.current_task = nullptr;
        queue.queue.push(std::move(attack_task));
      }
      break;
    }
  }
}
