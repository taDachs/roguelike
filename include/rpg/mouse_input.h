#pragma once
#include "rpg/camera.h"
#include "rpg/ecs.h"
#include <SDL.h>
#include <entt/entt.hpp>

namespace rpg {
struct ClickableComponent {
  SDL_Rect bbox;
  bool hovered = false;
};

bool findClicked(entt::registry& registry, const glm::vec2 screen_pos, const Camera& camera, entt::entity& clicked);

class ClickableSystem : public System {
public:
  ClickableSystem(Camera& camera) : m_camera(camera) {}
  void update(entt::registry& registry) override;
  void draw(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera) override;
  int getLayer() const override { return 7; }

  void setMousePose(const glm::vec2& mouse_pose) { m_mouse_pose = mouse_pose; }

private:
  glm::vec2 m_mouse_pose;
  Camera& m_camera;
};
}
