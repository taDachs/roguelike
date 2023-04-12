#pragma once
#include "rpg/ecs.h"
#include "rpg/tilemap.h"
#include <SDL.h>
#include <bitset>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <vector>

namespace rpg {

const size_t MAX_MAP_SIZE = 4096;

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

  // real means in m
  glm::vec2 gridToReal(const glm::vec2& grid_coord) const;
  glm::vec2 realToGrid(const glm::vec2& real_coord) const;

  int getWidth() const { return m_width; }
  int getHeight() const { return m_height; }

  void setResolution(float resolution) { m_resolution = resolution; }
  float getResolution() const { return m_resolution; }

  // access vector like y * cols + x
  bool isOccupied(float x, float y) const;
  bool isOccupied(const glm::vec2& pos) const;

  void loadOccupancyFromFile(const std::string& filename, float resolution);

  void addTileEntities(entt::registry& registry, TileManager& tm) const;

  std::vector<glm::vec2> findPath(glm::vec2 start, glm::vec2 goal) const;

private:
  // m / cell
  float m_resolution;

  // in cells
  int m_width;
  // in cells
  int m_height;

  std::bitset<MAX_MAP_SIZE> m_occupancy;
};

} // namespace rpg
