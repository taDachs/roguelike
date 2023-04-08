#include "rpg/movement.h"
#include "rpg/components.h"
#include <SDL.h>
#include <algorithm>
#include <iostream>
#include <map>
#include <set>

using namespace rpg;

bool PathFollowingSystem::isApplicable(const Entity& entity)
{
  return entity.hasComponent<PathComponent>() && entity.hasComponent<PositionComponent>() &&
         entity.hasComponent<StatsComponent>();
}

void PathFollowingSystem::update(const Entity& entity)
{
  auto& pose  = entity.getComponent<PositionComponent>();
  auto& path  = entity.getComponent<PathComponent>();
  auto& stats = entity.getComponent<StatsComponent>();

  if (path.reached)
  {
    if (entity.hasComponent<StateComponent>())
    {
      auto& state = entity.getComponent<StateComponent>();
      state.state = State::IDLE;
    }
    return;
  }

  if (path.path.empty())
  {
    return;
  }

  uint current_time = SDL_GetTicks();
  if (glm::length(path.path.at(0) - pose.pose) < m_min_distance)
  {
    pose.pose = path.path.at(0);
    path.path.erase(path.path.begin());
    path.last_position = pose.pose;
    path.last_tick     = current_time;
    if (path.path.empty())
    {
      path.reached = true;
    }
    return;
  }

  glm::vec2 diff = path.path.at(0) - path.last_position;
  float norm     = glm::length(diff);

  if (diff.x < 0)
  {
    pose.orientation = PositionComponent::Orientation::LEFT;
  }
  else if (diff.x > 0)
  {
    pose.orientation = PositionComponent::Orientation::RIGHT;
  }

  uint time_diff          = current_time - path.last_tick;
  float traveled_distance = std::fminf(norm, (time_diff / 1000.0) * stats.speed);

  pose.pose = path.last_position + (traveled_distance / norm) * diff;

  // update state if available
  if (entity.hasComponent<StateComponent>())
  {
    auto& state = entity.getComponent<StateComponent>();
    state.state = State::WALKING;
  }
}

void PathFollowingSystem::draw(const Entity& entity, SDL_Renderer* renderer)
{
  auto& path = entity.getComponent<PathComponent>();
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
  for (const auto& node : path.path)
  {
    glm::vec2 screen_pos = m_map->realToScreen(node);

    SDL_Rect base = {
      static_cast<int>(screen_pos.x - 5.F), static_cast<int>(screen_pos.y - 5.F), 10, 10};
    SDL_RenderFillRect(renderer, &base);
  }
}

bool PathPlanningSystem::isApplicable(const Entity& entity)
{
  return entity.hasComponent<PositionComponent>() && entity.hasComponent<PathComponent>();
}

void PathPlanningSystem::update(const Entity& entity)
{
  auto& pose = entity.getComponent<PositionComponent>();
  auto& path = entity.getComponent<PathComponent>();

  if (path.reached || !path.path.empty())
  {
    return;
  }

  glm::vec2 start_grid_coords = m_map->realToGrid(pose.pose);
  glm::vec2 goal_grid_coords  = m_map->realToGrid(path.goal);

  std::pair<int, int> start(start_grid_coords.x, start_grid_coords.y);
  std::pair<int, int> goal(goal_grid_coords.x, goal_grid_coords.y);

  std::set<std::pair<int, int> > open_set;
  open_set.insert(start);
  std::map<std::pair<int, int>, std::pair<int, int> > came_from;

  std::map<std::pair<int, int>, float> g_score;
  std::map<std::pair<int, int>, float> f_score;
  g_score[start] = 0;
  f_score[start] =
    glm::length(glm::vec2(start.first, start.second) - glm::vec2(goal.first, goal.second));
  std::pair<int, int> current;
  while (!open_set.empty())
  {
    current =
      *std::min_element(open_set.begin(), open_set.end(), [&](const auto& a, const auto& b) {
        return f_score[a] < f_score[b];
      });

    if (current.first == goal.first && current.second == goal.second)
    {
      break;
    }

    open_set.erase(current);
    for (int i = -1; i <= 1; ++i)
    {
      for (int j = -1; j <= 1; ++j)
      {
        if (i == 0 && j == 0)
        {
          continue;
        }

        std::pair<int, int> neighbour(current.first + i, current.second + j);
        if (m_map->isOccupied(neighbour.first, neighbour.second))
        {
          continue;
        }

        float tentative_g_score =
          g_score[current] + glm::length(glm::vec2(current.first, current.second) -
                                         glm::vec2(neighbour.first, neighbour.second));
        if (g_score.count(neighbour) == 0 || tentative_g_score < g_score[neighbour])
        {
          came_from[neighbour] = current;
          g_score[neighbour]   = tentative_g_score;
          f_score[neighbour] =
            tentative_g_score + glm::length(glm::vec2(neighbour.first, neighbour.second) -
                                            glm::vec2(goal.first, goal.second));
          open_set.insert(neighbour);
        }
      }
    }
  }

  // std::cout << "current goal: " << goal.first << ", " << goal.second << std::endl;
  // std::cout << "last found poser " << current.first << ", " << current.second << std::endl;
  if (current != goal)
  {
    std::cout << "Couldnt find path" << std::endl;
    path.reached = true;
    return;
  }

  std::vector<glm::vec2> total_path;
  glm::vec2 grid_coords(current.first, current.second);
  total_path.emplace_back(m_map->gridToReal(grid_coords + 0.5F));
  while (came_from.count(current) > 0)
  {
    current = came_from.at(current);
    glm::vec2 grid_coords(current.first, current.second);
    total_path.emplace_back(m_map->gridToReal(grid_coords + 0.5F));
  }

  total_path   = std::vector<glm::vec2>(total_path.rbegin(), total_path.rend());
  path.path    = total_path;
  path.reached = false;
}
