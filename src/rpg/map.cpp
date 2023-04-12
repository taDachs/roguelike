#include "rpg/components.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <rpg/map.h>
#include <set>

using namespace rpg;

glm::vec2 Map::gridToReal(const glm::vec2& grid_coord) const
{
  return grid_coord * m_resolution;
}

glm::vec2 Map::realToGrid(const glm::vec2& real_coord) const
{
  return real_coord / m_resolution;
}

bool Map::isOccupied(float x, float y) const
{
  return isOccupied({x, y});
}

bool Map::isOccupied(const glm::vec2& pos) const
{
  if (pos.x < 0 || pos.y < 0)
  {
    return true;
  }
  if (pos.x >= m_width || pos.y >= m_height)
  {
    return true;
  }

  size_t index = pos.x + pos.y * m_width;
  return m_occupancy[index];
}

void Map::loadOccupancyFromFile(const std::string& filename, float resolution)
{
  m_resolution = resolution;
  std::ifstream f;
  f.open(filename);
  int height = 0;

  std::string line;
  while (std::getline(f, line))
  {
    m_width = line.length();
    int x   = 0;
    for (const auto& c : line)
    {
      // std::cout << c;
      size_t index = x + height * m_width;
      m_occupancy.set(index, c == '1');
      x++;
    }
    // std::cout << std::endl;
    height++;
  }
  f.close();

  m_height = height;
  // std::cout << m_occupancy << std::endl;
}

void Map::addTileEntities(entt::registry& registry, TileManager& tm) const
{
  for (int x = 0; x < m_width; ++x)
  {
    for (int y = 0; y < m_height; ++y)
    {
      entt::entity entity = registry.create();
      glm::vec2 grid_pose(x, y);
      glm::vec2 real_pose = gridToReal(grid_pose);
      SDL_Rect rect;

      auto& pose = registry.emplace<PositionComponent>(entity);
      pose.pose  = real_pose;
      auto& tile = registry.emplace<TileComponent>(entity);
      tile.size = m_resolution;

      // draw cells
      if (m_occupancy[x + y * m_width])
      {
        tile.tile = tm.getTile("occupied");
      }
      else
      {
        tile.tile = tm.getTile("free");
      }
    }
  }
}

std::vector<glm::vec2> Map::findPath(glm::vec2 real_start, glm::vec2 real_goal) const
{
  glm::vec2 start_grid_coords = realToGrid(real_start);
  glm::vec2 goal_grid_coords  = realToGrid(real_goal);

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
        if (isOccupied(neighbour.first, neighbour.second))
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

  if (current != goal)
  {
    std::cout << "Couldnt find path" << std::endl;
    return {};
  }
  std::cout << "Found path" << std::endl;

  std::vector<glm::vec2> total_path;
  glm::vec2 grid_coords(current.first, current.second);
  total_path.emplace_back(gridToReal(grid_coords + 0.5F));
  while (came_from.count(current) > 0)
  {
    current = came_from.at(current);
    glm::vec2 grid_coords(current.first, current.second);
    total_path.emplace_back(gridToReal(grid_coords + 0.5F));
  }

  total_path = std::vector<glm::vec2>(total_path.rbegin(), total_path.rend());
  return total_path;
 }
