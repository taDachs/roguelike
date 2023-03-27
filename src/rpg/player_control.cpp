#include "rpg/player_control.h"
#include <iostream>

using namespace rpg;

bool PlayerControlSystem::isApplicable(const Entity& entity)
{
  bool has_control  = entity.getComponent<PlayerControlComponent>() != nullptr;
  bool has_moveable = entity.getComponent<MoveableComponent>() != nullptr;
  return has_control && has_moveable;
}

void PlayerControlSystem::update(const Entity& entity)
{
  auto control  = entity.getComponent<PlayerControlComponent>();
  auto moveable = entity.getComponent<MoveableComponent>();

  for (const auto& event : m_buffer)
  {
    if (event.type == SDL_KEYDOWN)
    {
      m_shift_pressed = (SDL_GetModState() & KMOD_SHIFT) != 0;
      if (event.key.keysym.sym == SDLK_w)
      {
        m_up_pressed = true;
      }
      if (event.key.keysym.sym == SDLK_s)
      {
        m_down_pressed = true;
      }
      if (event.key.keysym.sym == SDLK_a)
      {
        m_left_pressed = true;
      }
      if (event.key.keysym.sym == SDLK_d)
      {
        m_right_pressed = true;
      }
    }
    if (event.type == SDL_KEYUP)
    {
      if (event.key.keysym.sym == SDLK_w)
      {
        m_up_pressed = false;
      }
      if (event.key.keysym.sym == SDLK_s)
      {
        m_down_pressed = false;
      }
      if (event.key.keysym.sym == SDLK_a)
      {
        m_left_pressed = false;
      }
      if (event.key.keysym.sym == SDLK_d)
      {
        m_right_pressed = false;
      }
    }
  }
  m_buffer.clear();


  if (m_up_pressed)
  {
    moveable->current_direction.y = -1;
  }
  if (m_down_pressed)
  {
    moveable->current_direction.y = 1;
  }
  if (!m_up_pressed && !m_down_pressed) {
    moveable->current_direction.y = 0;
  }
  if (m_left_pressed)
  {
    moveable->current_direction.x = -1;
  }
  if (m_right_pressed)
  {
    moveable->current_direction.x = 1;
  }
  if (!m_left_pressed && !m_right_pressed) {
    moveable->current_direction.x = 0;
  }

  if (m_up_pressed || m_down_pressed || m_left_pressed || m_right_pressed) {
  } else {
    m_shift_pressed = false;
  }
}
