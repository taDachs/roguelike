#include "rpg/mouse_input.h"
#include "rpg/components.h"

using namespace rpg;

bool rpg::findClicked(entt::registry& registry,
                      const glm::vec2 screen_pos,
                      const Camera& camera,
                      entt::entity& clicked)
{
  for (auto&& [entity, pose, clickable] :
       registry.view<PositionComponent, ClickableComponent>().each())
  {
    glm::vec2 entity_screen_pose = camera.realToScreen(pose.pose);
    glm::vec2 relative_pose =
      screen_pos - entity_screen_pose - glm::vec2(clickable.bbox.x, clickable.bbox.y);
    if (relative_pose.x >= 0 && relative_pose.x < clickable.bbox.w && relative_pose.y >= 0 &&
        relative_pose.y < clickable.bbox.h)
    {
      clicked = entity;
      return true;
    }
  }
  return false;
}

void ClickableSystem::update(entt::registry& registry)
{
  for (auto&& [entity, pose, clickable] :
       registry.view<PositionComponent, ClickableComponent>().each())
  {
    glm::vec2 entity_screen_pose = m_camera.realToScreen(pose.pose);
    glm::vec2 relative_pose =
      m_mouse_pose - entity_screen_pose - glm::vec2(clickable.bbox.x, clickable.bbox.y);
    clickable.hovered = relative_pose.x >= 0 && relative_pose.x < clickable.bbox.w &&
                        relative_pose.y >= 0 && relative_pose.y < clickable.bbox.h;
  }
}

void ClickableSystem::draw(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera)
{
  for (auto&& [entity, pose, clickable] :
       registry.view<PositionComponent, ClickableComponent>().each())
  {
    if (!clickable.hovered)
    {
      continue;
    }
    glm::vec2 entity_screen_pose = camera.realToScreen(pose.pose);
    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
    SDL_Rect rect = {static_cast<int>(clickable.bbox.x + entity_screen_pose.x),
                     static_cast<int>(clickable.bbox.y + entity_screen_pose.y),
                     clickable.bbox.w,
                     clickable.bbox.h};
    SDL_RenderDrawRect(renderer, &rect);
  }
}
