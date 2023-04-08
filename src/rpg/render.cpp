#include "rpg/render.h"
#include <iostream>

using namespace rpg;
bool RenderSystem::isApplicable(const Entity& entity)
{
  return entity.hasComponent<RenderComponent>()
      && entity.hasComponent<PositionComponent>();
}

void RenderSystem::draw(const Entity& entity, SDL_Renderer* renderer)
{
  auto& render   = entity.getComponent<RenderComponent>();
  auto& position = entity.getComponent<PositionComponent>();

  glm::vec3 game_pose(position.pose, 1);
  glm::vec2 screen_pose = m_game_to_screen * game_pose;

  render.sprite->draw(renderer,
                       static_cast<int>(screen_pose.x),
                       static_cast<int>(screen_pose.y),
                       position.orientation == PositionComponent::Orientation::LEFT);
}

bool AnimationSystem::isApplicable(const Entity& entity)
{
  return entity.hasComponent<RenderComponent>()
      && entity.hasComponent<AnimationComponent>();
}

void AnimationSystem::update(const Entity& entity)
{
  auto& render    = entity.getComponent<RenderComponent>();
  auto& animation = entity.getComponent<AnimationComponent>();

  if (entity.hasComponent<StateComponent>())
  {
    auto& state               = entity.getComponent<StateComponent>();
    if (state.state != animation.previous_state) {
      render.sprite->reset();
      render.sprite            = animation.sprite_map.at(state.state);
      animation.previous_state = state.state;
    }
  }
  else
  {
    render.sprite->update();
  }
}
