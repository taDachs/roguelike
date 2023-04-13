#include "rpg/body.h"
#include "rpg/components.h"
#include "rpg/movement.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>


using namespace rpg;

void BodySystem::update(entt::registry& registry)
{
  // update bodies
  for (auto&& [entity, body, pose] : registry.view<BodyComponent, PositionComponent>().each())
  {
    if (!registry.all_of<PositionComponent, LimbComponent>(body.body))
    {
      continue;
    }
    auto& body_pose = registry.get<PositionComponent>(body.body);
    auto& body_limb = registry.get<LimbComponent>(body.body);

    body_pose.pose        = pose.pose + body_limb.offset;
    body_pose.orientation = glm::normalize(pose.orientation + body_limb.orientation);

    float total_speed = 0;
    std::vector<entt::entity> slots;
    float total_max_health = 0;
    float total_health     = 0;

    for (const auto& body_limb : body.limbs)
    {
      if (registry.all_of<MoveableComponent>(body_limb))
      {
        auto& moveable = registry.get<MoveableComponent>(body_limb);
        total_speed += moveable.speed;
      }
      if (registry.all_of<HealthComponent>(body_limb))
      {
        auto& health = registry.get<HealthComponent>(body_limb);
        total_max_health += health.max_health;
        total_health += health.health;
      }
      if (registry.all_of<SlotComponent>(body_limb))
      {
        slots.push_back(body_limb);
      }
    }
    if (total_speed > 0)
    {
      auto& moveable = registry.get_or_emplace<MoveableComponent>(entity);
      moveable.speed = total_speed;
    }
    else
    {
      registry.remove<MoveableComponent>(entity);
    }
    if (!slots.empty())
    {
      auto& inventory = registry.get_or_emplace<InventoryComponent>(entity);
      inventory.slots = slots;
    }
    if (total_max_health > 0)
    {
      auto& health      = registry.get_or_emplace<HealthComponent>(entity);
      health.max_health = total_max_health;
      health.health     = total_health;
    }
  }
}

void LimbSystem::update(entt::registry& registry)
{
  // update limbs
  for (auto&& [entity, limb, pose] : registry.view<LimbComponent, PositionComponent>().each())
  {
    if (!registry.valid(limb.attached))
    {
      continue;
    }
    if (!registry.all_of<PositionComponent, LimbComponent>(limb.attached))
    {
      continue;
    }

    auto& attached_pose = registry.get<PositionComponent>(limb.attached);
    pose.orientation    = glm::normalize(attached_pose.orientation + limb.orientation);

    float dot_product = glm::dot({0, -1}, pose.orientation);
    float angle_rad   = acos(dot_product);
    float angle_deg   = glm::degrees(angle_rad);

    glm::vec3 cross_product = glm::cross(glm::vec3(0, -1, 0.0F), glm::vec3(pose.orientation, 0.0F));
    if (cross_product.z < 0)
    {
      // clockwise
      angle_deg = 360.0F - angle_deg;
    }

    angle_rad                 = glm::radians(angle_deg);
    glm::mat2 rotation_matrix = glm::mat2(glm::cos(angle_rad),
                                          glm::sin(angle_rad),
                                          -glm::sin(angle_rad),
                                          glm::cos(angle_rad)); // create a 2D rotation matrix
    pose.pose                 = attached_pose.pose + rotation_matrix * limb.offset;
  }
}
