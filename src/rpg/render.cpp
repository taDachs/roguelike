#include "rpg/render.h"
#include <iostream>

using namespace rpg;
bool RenderSystem::isApplicable(const Entity& entity)
{
  bool has_render   = entity.getComponent<RenderComponent>() != nullptr;
  bool has_position = entity.getComponent<PositionComponent>() != nullptr;
  return has_render && has_position;
}

void RenderSystem::draw(const Entity& entity, SDL_Renderer* renderer)
{
  auto render   = entity.getComponent<RenderComponent>();
  auto position = entity.getComponent<PositionComponent>();

  glm::vec3 game_pose(position->pose, 1);
  glm::vec2 screen_pose = m_game_to_screen * game_pose;

  render->sprite->draw(renderer,
                       static_cast<int>(screen_pose.x),
                       static_cast<int>(screen_pose.y),
                       position->orientation == PositionComponent::Orientation::LEFT);
}

bool AnimationSystem::isApplicable(const Entity& entity)
{
  bool has_animation = entity.getComponent<AnimationComponent>() != nullptr;
  bool has_render    = entity.getComponent<RenderComponent>() != nullptr;
  return has_render;
}

void AnimationSystem::update(const Entity& entity)
{
  auto render    = entity.getComponent<RenderComponent>();
  auto animation = entity.getComponent<AnimationComponent>();

  auto state               = entity.getComponent<StateComponent>();
  bool needs_sprite_change = (state != nullptr) && (state->state != animation->previous_state);
  if (needs_sprite_change)
  {
    render->sprite->reset();
    render->sprite            = animation->sprite_map.at(state->state);
    animation->previous_state = state->state;
  }
  else
  {
    render->sprite->update();
  }
}
