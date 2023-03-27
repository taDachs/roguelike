#pragma once
#include <glm/glm.hpp>
#include <memory>

namespace rpg {

class Map
{
public:
  using Ptr = std::shared_ptr<Map>;

  glm::vec2 gridToScreen(const glm::vec2& grid_coord) const;
  glm::vec2 screenToGrid(const glm::vec2& screen_coord) const;
  // real means in m
  glm::vec2 gridToReal(const glm::vec2& grid_coord) const;
  glm::vec2 realToGrid(const glm::vec2& real_coord) const;

  glm::vec2 realToScreen(const glm::vec2& real_coord) const;
  glm::vec2 screenToReal(const glm::vec2& screen_coord) const;

  void setResolution(float resolution) { m_resolution = resolution; }
  float getResolution() const { return m_resolution; }

  void setGridToScreen(const glm::mat3& grid_to_screen) {
    m_grid_to_screen = grid_to_screen;
    m_screen_to_grid = glm::inverse(grid_to_screen);
  }
  glm::mat3 getGridToScreen() const { return m_grid_to_screen; }

  void setScreenToGrid(const glm::mat3& screen_to_grid) {
    m_screen_to_grid = screen_to_grid;
    m_grid_to_screen = glm::inverse(screen_to_grid);
  }
  glm::mat3 getScreenToGrid() const { return m_screen_to_grid; }

private:
  // square size in m
  float m_resolution;
  glm::mat3 m_grid_to_screen;
  glm::mat3 m_screen_to_grid;
};

} // namespace rpg
