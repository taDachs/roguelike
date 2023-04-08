#include <fstream>
#include <iostream>
#include <rpg/map.h>

using namespace rpg;

glm::vec2 Map::gridToScreen(const glm::vec2& grid_coord) const
{
  glm::vec3 coord_hom(grid_coord, 1);
  return m_grid_to_screen * coord_hom;
}
glm::vec2 Map::screenToGrid(const glm::vec2& screen_coord) const
{
  glm::vec3 coord_hom(screen_coord, 1);
  return m_screen_to_grid * coord_hom;
}

glm::vec2 Map::gridToReal(const glm::vec2& grid_coord) const
{
  return grid_coord * m_resolution;
}

glm::vec2 Map::realToGrid(const glm::vec2& real_coord) const
{
  return real_coord / m_resolution;
}

glm::vec2 Map::realToScreen(const glm::vec2& real_coord) const
{
  glm::vec2 grid_coord = realToGrid(real_coord);
  return gridToScreen(grid_coord);
}

glm::vec2 Map::screenToReal(const glm::vec2& screen_coord) const
{
  glm::vec2 grid_coord = screenToGrid(screen_coord);
  return gridToReal(grid_coord);
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
      size_t index       = x + height * m_width;
      m_occupancy[index] = c == '1';
      x++;
    }
    // std::cout << std::endl;
    height++;
  }

  m_height = height;
  // std::cout << m_occupancy << std::endl;
}

void Map::draw(SDL_Renderer* renderer) const
{
  for (int x = 0; x < m_width; ++x)
  {
    for (int y = 0; y < m_height; ++y)
    {
      glm::vec2 grid_pose(x, y);
      glm::vec2 screen_pose = gridToScreen(grid_pose);
      glm::vec2 rect_size   = gridToScreen(grid_pose + 1.F) - screen_pose;
      SDL_Rect rect;
      rect.x = screen_pose.x;
      rect.y = screen_pose.y;
      rect.w = rect_size.x;
      rect.h = rect_size.y;

      // draw cells
      if (m_occupancy[x + y * m_width])
      {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
      }
      else
      {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);
      }
    }
  }
}
