#include <rpg/map.h>

using namespace rpg;

glm::vec2 Map::gridToScreen(const glm::vec2& grid_coord) const {
  glm::vec3 coord_hom(grid_coord, 1);
  return m_grid_to_screen * coord_hom;
}
glm::vec2 Map::screenToGrid(const glm::vec2& screen_coord) const {
  glm::vec3 coord_hom(screen_coord, 1);
  return m_screen_to_grid * coord_hom;
}

glm::vec2 Map::gridToReal(const glm::vec2& grid_coord) const {
 return grid_coord * m_resolution;
}

glm::vec2 Map::realToGrid(const glm::vec2& real_coord) const {
   return real_coord / m_resolution;
}

glm::vec2 Map::realToScreen(const glm::vec2& real_coord) const {
  glm::vec2 grid_coord = realToGrid(real_coord);
  return gridToScreen(grid_coord);
}

glm::vec2 Map::screenToReal(const glm::vec2& screen_coord) const {
  glm::vec2 grid_coord = screenToGrid(screen_coord);
  return gridToReal(grid_coord);
}

bool Map::isOccupied(float x, float y) const {
  return isOccupied({x, y});
}

bool Map::isOccupied(const glm::vec2& pos) const {
  glm::vec2 center(5, 5);
  float radius = 3;

  return glm::length(pos - center) < radius;
}
