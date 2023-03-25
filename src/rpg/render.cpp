#include "rpg/render.h"

using namespace rpg;
bool RenderSystem::isApplicable(const Entity& entity)
{
  bool has_render   = entity.getComponent<RenderComponent>() != nullptr;
  bool has_position = entity.getComponent<PositionComponent>() != nullptr;
  return has_render && has_position;
}

void RenderSystem::update(const Entity& entity)
{
  auto render   = entity.getComponent<RenderComponent>();
  auto position = entity.getComponent<PositionComponent>();

  render->sprite->draw(
    m_renderer, static_cast<int>(position->pose.x), static_cast<int>(position->pose.y));
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

  auto state = entity.getComponent<StateComponent>();
  bool needs_sprite_change = state != nullptr && (state->state != animation->previous_state);
  if (needs_sprite_change) {
    render->sprite = animation->sprite_map.at(state->state);
    render->sprite->reset();
    animation->previous_state = state->state;
  } else {
    render->sprite->update();
  }
}
