#include "rpg/render.h"
#include <iostream>

using namespace rpg;
void SpriteSystem::draw(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera)
{
  for (auto &&[entity, sprite, position] : registry.view<SpriteComponent, PositionComponent>().each())
  {
    glm::vec2 screen_pose = camera.realToScreen(position.pose);
    sprite.sprite->draw(renderer,
                        static_cast<int>(screen_pose.x),
                        static_cast<int>(screen_pose.y),
                        position.orientation == PositionComponent::Orientation::LEFT);

  }
}

void SpriteSystem::update(entt::registry& registry)
{
  for (auto &&[entity, sprite] : registry.view<SpriteComponent>().each())
  {
    sprite.sprite->update();
  }
}

void AnimationStateSystem::update(entt::registry& registry)
{
  for (auto &&[entity, sprite, animation, state] : registry.view<SpriteComponent, AnimationStateComponent, StateComponent>().each())
  {
    if (state.state != animation.previous_state)
    {
      sprite.sprite            = animation.sprite_map.at(state.state);
      sprite.sprite->reset();
      animation.previous_state = state.state;
      return;
    }
  }
}
