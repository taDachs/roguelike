#include "rpg/render.h"
#include <iostream>

using namespace rpg;
void SpriteSystem::draw(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera)
{
  registry.sort<SpriteComponent>([](const auto& a, const auto& b) { return a.layer < b.layer; });
  for (auto&& [entity, sprite, position] :
       registry.view<SpriteComponent, PositionComponent>().each())
  {
    glm::vec2 screen_pose = camera.realToScreen(position.pose);

    SDL_Rect dst_rect = {static_cast<int>(screen_pose.x - sprite.frame.w / 2),
                         static_cast<int>(screen_pose.y - sprite.frame.h / 2),
                         sprite.frame.w,
                         sprite.frame.h};

    float dot_product = glm::dot({0, -1}, position.orientation);
    float angle_rad   = acos(dot_product);
    float angle_deg   = glm::degrees(angle_rad);

    glm::vec3 cross_product =
      glm::cross(glm::vec3(0, -1, 0.0f), glm::vec3(position.orientation, 0.0f));
    if (cross_product.z < 0)
    {
      // clockwise
      angle_deg = 360.0f - angle_deg;
    }


    SDL_RenderCopyEx(renderer,
                     sprite.texture,
                     &sprite.frame,
                     &dst_rect,
                     angle_deg,
                     nullptr,
                     sprite.flipped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
  }
}

void SpriteSystem::update(entt::registry& registry) {}

void AnimationStateSystem::update(entt::registry& registry)
{
  for (auto&& [entity, sprite, animation, state] :
       registry.view<SpriteComponent, AnimationStateComponent, StateComponent>().each())
  {
    if (state.state != animation.previous_state)
    {
      // sprite.sprite            = animation.sprite_map.at(state.state);
      // sprite.sprite->reset();
      animation.previous_state = state.state;
      return;
    }
  }
}
