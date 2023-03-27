#pragma once

#include "rpg/components.h"
#include "rpg/ecs.h"
#include <SDL.h>
#include <glm/glm.hpp>

namespace rpg {

class RenderSystem : public System
{
public:
  bool isApplicable(const Entity& entity) override;
  void draw(const Entity& entity, SDL_Renderer* renderer) override;

  glm::mat3 getScreenToGame() const { return m_screen_to_game; }
  glm::mat3 getGameToScreen() const { return m_game_to_screen; }

  void setScreenToGame(const glm::mat3& mat)
  {
    m_screen_to_game = mat;
    m_game_to_screen = glm::inverse(mat);
  }
  void setGameToScreen(const glm::mat3& mat)
  {
    m_game_to_screen = mat;
    m_screen_to_game = glm::inverse(mat);
  }

private:
  glm::mat3 m_game_to_screen;
  glm::mat3 m_screen_to_game;
};

class AnimationSystem : public System
{
public:
  bool isApplicable(const Entity& entity) override;
  void update(const Entity& entity) override;
};
} // namespace rpg
