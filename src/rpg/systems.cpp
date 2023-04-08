#include "rpg/systems.h"
#include <algorithm>
#include <iostream>

using namespace rpg;

const float RUNNING_THRESH = 0.1;

bool MoveableSystem::isApplicable(const Entity& entity)
{
  return entity.hasComponent<MoveableComponent>()
      && entity.hasComponent<PositionComponent>();
}

void MoveableSystem::update(const Entity& entity)
{
  auto& position = entity.getComponent<PositionComponent>();
  auto& moveable = entity.getComponent<MoveableComponent>();

  if (entity.hasComponent<StateComponent>())
  {
    auto state    = entity.getComponent<StateComponent>();
    if (glm::length(moveable.current_direction) > 0 && entity.hasComponent<StatsComponent>())
    {
      auto stats    = entity.getComponent<StatsComponent>();
      state.state = (stats.speed - moveable.velocity) < RUNNING_THRESH ? "running" : "walking";
    }
    else
    {
      state.state = "idle";
    }
  }

  uint current_time = SDL_GetTicks();
  uint diff         = current_time - m_last_tick;
  if (diff < m_tick_delay)
  {
    return;
  }
  m_last_tick = current_time;

  float seconds = static_cast<float>(diff) / 1000;

  if (moveable.current_direction.x > 0)
  {
    position.orientation = PositionComponent::Orientation::RIGHT;
  }
  if (moveable.current_direction.x < 0)
  {
    position.orientation = PositionComponent::Orientation::LEFT;
  }

  position.pose += moveable.current_direction * moveable.velocity * seconds;
}

void GridDrawingComponent::draw(const Entity& entity, SDL_Renderer* renderer)
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  //
  // int rows = m_map->getResolution();
  //
  // // Draw the grid
  // for (int row = 0; row < rows; row++) {
  //     for (int col = 0; col < cols; col++) {
  //         // Calculate the position and size of the current cell
  //         int x = col * grid_size;
  //         int y = row * grid_size;
  //         SDL_Rect rect = { x, y, grid_size, grid_size };
  //         // Draw the cell
  //         SDL_RenderDrawRect(renderer, &rect);
  //     }
  // }
}

