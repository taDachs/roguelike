#include "rpg/systems.h"
#include "rpg/attack_system.h"
#include "rpg/movement.h"
#include <algorithm>
#include <iostream>

using namespace rpg;

const float RUNNING_THRESH = 0.1;

void HealthSystem::update(entt::registry& registry)
{
}

void SlotSystem::update(entt::registry& registry)
{
  for (auto &&[entity, slot, pose] : registry.view<SlotComponent, PositionComponent>().each())
  {
    if (!registry.valid(slot.equipped)) {
      continue;
    }

    if (!registry.all_of<PositionComponent>(slot.equipped))
    {
      continue;
    }

    auto& equipped_pose = registry.get<PositionComponent>(slot.equipped);
    equipped_pose.pose = pose.pose + slot.offset;
    equipped_pose.orientation    = glm::normalize(pose.orientation + slot.orientation);

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
    equipped_pose.pose                 = pose.pose + rotation_matrix * slot.offset;
  }
}


void HealthSystem::draw(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera)
{
  for (auto &&[entity, health, pose] : registry.view<HealthComponent, PositionComponent>().each())
  {
    glm::vec2 screen_pose = camera.realToScreen(pose.pose);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect full_bar = {
      static_cast<int>(screen_pose.x - 50), static_cast<int>(screen_pose.y), static_cast<int>(health.max_health), 10};
    SDL_RenderFillRect(renderer, &full_bar);

    if (health.health > 0) {
      SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
      SDL_Rect depleted_bar = {full_bar.x, full_bar.y, static_cast<int>(health.health), 10};
      SDL_RenderFillRect(renderer, &depleted_bar);
    }
  }
}

void StateSystem::update(entt::registry& registry)
{
  for (auto &&[entity, state] : registry.view<StateComponent>().each())
  {
    if (registry.all_of<HealthComponent>(entity))
    {
      auto& health = registry.get<HealthComponent>(entity);
      if (health.health <= 0) {
        state.state = State::DEAD;
        continue;
      }
    }
    if (registry.all_of<AttackComponent>(entity))
    {
      state.state = State::SHOOTING;
      continue;
    }
    if (registry.all_of<PathComponent>(entity))
    {
      state.state = State::WALKING;
      continue;
    }
    state.state = State::IDLE;
  }
}
