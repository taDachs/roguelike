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

void HealthSystem::draw(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera)
{
  for (auto &&[entity, health, pose] : registry.view<HealthComponent, PositionComponent>().each())
  {
    glm::vec2 screen_pose = camera.realToScreen(pose.pose);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect full_bar = {
      static_cast<int>(screen_pose.x - 50), static_cast<int>(screen_pose.y), 100, 10};
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
    if (registry.all_of<HealthComponent>(entity))
    {
      auto& health = registry.get<HealthComponent>(entity);
      if (health.health <= 0) {
        state.state = State::DEAD;
        continue;
      }
    }
    state.state = State::IDLE;
  }
}
