#include <algorithm>
#include <rpg/components.h>
#include <rpg/movement.h>
#include <set>
#include <iostream>

using namespace rpg;

bool PathFollowingSystem::isApplicable(const Entity& entity)
{
  bool has_path     = entity.getComponent<PathComponent>() != nullptr;
  bool has_position = entity.getComponent<PositionComponent>() != nullptr;
  bool has_stats    = entity.getComponent<StatsComponent>() != nullptr;
  return has_position && has_path && has_stats;
}

void PathFollowingSystem::update(const Entity& entity)
{
  auto pose  = entity.getComponent<PositionComponent>();
  auto path  = entity.getComponent<PathComponent>();
  auto stats = entity.getComponent<StatsComponent>();

  if (path->path.empty())
  {
    path->reached = true;
    return;
  }

  uint current_time = SDL_GetTicks();
  if (glm::length(path->path.at(0) - pose->pose) < m_min_distance)
  {
    pose->pose = path->path.at(0);
    path->path.erase(path->path.begin());
    path->last_position = pose->pose;
    path->last_tick     = current_time;
    return;
  }

  glm::vec2 diff = path->path.at(0) - path->last_position;
  float norm     = glm::length(diff);

  uint time_diff          = current_time - path->last_tick;
  float traveled_distance = std::fminf(norm, (time_diff / 1000.0) * stats->speed);

  pose->pose = path->last_position + (traveled_distance / norm) * diff;
}

bool PathPlanningSystem::isApplicable(const Entity& entity)
{
  bool pose = entity.hasComponent<PositionComponent>();
  bool path = entity.hasComponent<PathComponent>();
  return pose && path;
}

void PathPlanningSystem::update(const Entity& entity)
{
  auto pose = entity.getComponent<PositionComponent>();
  auto path = entity.getComponent<PathComponent>();

  if (!path->path.empty()) {
    return;
  }

  if (path->goal == pose->pose) {
    return;
  }

  std::pair<int, int> start(pose->pose.x, pose->pose.y);
  std::pair<int, int> goal(path->goal.x, path->goal.y);

  std::set<std::pair<int, int>> open_set;
  open_set.insert(start);
  std::map<std::pair<int, int>, std::pair<int, int>> came_from;

  std::map<std::pair<int, int>, float> g_score;
  std::map<std::pair<int, int>, float> f_score;
  g_score[start] = 0;
  f_score[start] = glm::length(glm::vec2(start.first, start.second) - glm::vec2(goal.first, goal.second));
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

        float tentative_g_score = g_score[current] + glm::length(glm::vec2(current.first, current.second) - glm::vec2(neighbour.first, neighbour.second));
        if (g_score.count(neighbour) == 0 || tentative_g_score < g_score[neighbour])
        {
          came_from[neighbour] = current;
          g_score[neighbour]   = tentative_g_score;
          f_score[neighbour]   = tentative_g_score + glm::length(glm::vec2(neighbour.first, neighbour.second) - glm::vec2(goal.first, goal.second));
          open_set.insert(neighbour);
        }
      }
    }
  }

  std::cout << "current goal: " << goal.first << ", " << goal.second << std::endl;
  std::cout << "last found pose" << current.first << ", " << current.second << std::endl;
  if (current != goal) {
    std::cout << "Couldnt find path" << std::endl;
    return;
  }

  std::vector<glm::vec2> total_path;
  total_path.emplace_back(current.first, current.second);
  while (came_from.count(current) > 0) {
    current = came_from.at(current);
    total_path.emplace_back(current.first, current.second);
  }

  total_path = std::vector<glm::vec2>(total_path.rbegin(), total_path.rend());
  path->path = total_path;
  path->reached = false;
}
