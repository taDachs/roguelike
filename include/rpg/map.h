#pragma once
#include <SDL.h>
#include <bitset>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <vector>

namespace rpg {

const size_t MAX_MAP_SIZE = 1024L;

class Map
{
public:
  Map() = default;
  Map(int width, int height)
    : m_width(width)
    , m_height(height)
  {
  }

  ~Map() = default;

  using Ptr = std::shared_ptr<Map>;

  glm::vec2 gridToScreen(const glm::vec2& grid_coord) const;
  glm::vec2 screenToGrid(const glm::vec2& screen_coord) const;
  // real means in m
  glm::vec2 gridToReal(const glm::vec2& grid_coord) const;
  glm::vec2 realToGrid(const glm::vec2& real_coord) const;

  glm::vec2 realToScreen(const glm::vec2& real_coord) const;
  glm::vec2 screenToReal(const glm::vec2& screen_coord) const;

  int getWidth() const { return m_width; }
  int getHeight() const { return m_height; }

  void setResolution(float resolution) { m_resolution = resolution; }
  float getResolution() const { return m_resolution; }

  void setGridToScreen(const glm::mat3& grid_to_screen)
  {
    m_grid_to_screen = grid_to_screen;
    m_screen_to_grid = glm::inverse(grid_to_screen);
  }
  glm::mat3 getGridToScreen() const { return m_grid_to_screen; }

  void setScreenToGrid(const glm::mat3& screen_to_grid)
  {
    m_screen_to_grid = screen_to_grid;
    m_grid_to_screen = glm::inverse(screen_to_grid);
  }
  glm::mat3 getScreenToGrid() const { return m_screen_to_grid; }

  // access vector like y * cols + x
  bool isOccupied(float x, float y) const;
  bool isOccupied(const glm::vec2& pos) const;

  void loadOccupancyFromFile(const std::string& filename, float resolution);

  void draw(SDL_Renderer* renderer) const;

  void print() const
  {
    for (int y = 0; y < m_height; ++y)
    {
      for (int x = 0; x < m_width; ++x)
      {
        glm::vec2 grid_pose(x, y);
        size_t index = x + y * m_width;

        if (m_occupancy[index])
        {
          std::cout << "x";
        }
        else
        {
          std::cout << "o";
        }
      }
      std::cout << "\n";
    }
    std::cout << std::endl;
  }

private:
  // m / cell
  float m_resolution;
  glm::mat3 m_grid_to_screen;
  glm::mat3 m_screen_to_grid;

  // in cells
  int m_width;
  // in cells
  int m_height;

  std::bitset<MAX_MAP_SIZE> m_occupancy;
};

} // namespace rpg
