#pragma once

#include "rpg/components.h"
#include "rpg/ecs.h"
#include <SDL.h>
#include <iostream>

namespace rpg {

class PlayerControlSystem : public System
{
public:
  bool isApplicable(const Entity& entity) override;
  void update(const Entity& entity) override;

  void addEvent(SDL_Event e) { m_buffer.push_back(e); };

private:
  std::vector<SDL_Event> m_buffer;

  bool m_shift_pressed = false;
  bool m_left_pressed  = false;
  bool m_right_pressed = false;
  bool m_up_pressed    = false;
  bool m_down_pressed  = false;
};

} // namespace rpg
