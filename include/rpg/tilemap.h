#pragma once
#include "rpg/ecs.h"
#include <memory>
#include <SDL.h>
#include <map>

namespace rpg {

class Tile {
public:
  using Ptr = std::shared_ptr<Tile>;
  Tile(SDL_Texture* texture): m_texture(texture) {};
  ~Tile() = default;

  void draw(SDL_Renderer* renderer, const SDL_Rect& dst);
private:
  SDL_Texture* m_texture;
};

struct TileComponent : Component {
  Tile::Ptr tile;
  float size;
};

class TileSystem : public System
{
public:
  void draw(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera) override;
  int getLayer() const override {
    return 0;
  }
};

class TileManager
{
public:
  // Constructor and destructor
  TileManager()  = default;
  ~TileManager();

  // Load a tile from a file
  void addTile(const std::string& tile_name, const std::string& filename, SDL_Renderer* renderer);
  // Get a tile by name
  Tile::Ptr getTile(const std::string& tile_name) const;

private:
  // Map of tile names to SDL_Texture pointers
  std::map<std::string, SDL_Texture*> m_textures;
};

}
