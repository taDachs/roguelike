#include "rpg/tilemap.h"
#include "rpg/components.h"
#include <SDL_image.h>

using namespace rpg;

void Tile::draw(SDL_Renderer* renderer, const SDL_Rect& dst)
{
  SDL_RenderCopy(renderer, m_texture, nullptr, &dst);
}

TileManager::~TileManager()
{
  for (auto& [name, t] : m_textures)
  {
    SDL_DestroyTexture(t);
  }
}

void TileManager::addTile(const std::string& tile_name,
                          const std::string& filename,
                          SDL_Renderer* renderer)
{
  SDL_Surface* surface = IMG_Load(filename.c_str());
  if (!surface)
  {
    printf("Failed to load image: %s\n", SDL_GetError());
    return;
  }

  // Create the texture from the image surface
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture)
  {
    printf("Failed to create texture: %s\n", SDL_GetError());
    SDL_FreeSurface(surface);
    return;
  }
  SDL_FreeSurface(surface);

  m_textures[tile_name] = texture;
}

Tile::Ptr TileManager::getTile(const std::string& tile_name) const
{
  return std::make_shared<Tile>(m_textures.at(tile_name));
}

void TileSystem::draw(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera)
{
  for (auto&& [entity, pose, tile] : registry.view<PositionComponent, TileComponent>().each())
  {
    glm::vec2 screen_pos_start = camera.realToScreen(pose.pose);
    screen_pos_start           = glm::round(screen_pos_start);
    glm::vec2 screen_pos_end   = camera.realToScreen(pose.pose + tile.size);
    screen_pos_end             = glm::round(screen_pos_end);
    glm::vec2 size             = screen_pos_end - screen_pos_start;
    tile.tile->draw(renderer,
                    {static_cast<int>(screen_pos_start.x),
                     static_cast<int>(screen_pos_start.y),
                     static_cast<int>(size.x),
                     static_cast<int>(size.y)});
  }
}
