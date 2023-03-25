#pragma once
#include <utility>
#include <glm/glm.hpp>

#include "rpg/asset_manager.h"

namespace rpg {

enum State {
  IDLE,
  FIRING,
  WALKING,
  RUNNING,
  DEAD,
  NONE,
};

class Actor;

class Visualizer {
public:
  Visualizer() = default;
  Visualizer(std::map<State, rpg::Sprite::Ptr>  sprites): m_sprites(std::move(sprites)) {}
  void draw(SDL_Renderer* renderer);
  void update() {
    for (const auto& p : m_sprites) {
      p.second->update();
    }
  }

private:
  std::map<State, rpg::Sprite::Ptr> m_sprites;
};

class Actor {
public:
  void update();

  State getState() const { return m_state; }
  void setState(State state) { m_state = state; }

  Visualizer getVisual() const { return m_visual; }
  void setVisual(const Visualizer& visual) { m_visual = visual; }

private:
  State m_state;
  Visualizer m_visual;
};
}

