#include "rpg/sprite.h"
#include <SDL_image.h>
#include <iostream>

using namespace rpg;

void Sprite::update()
{
  // Update the current frame based on the frame delay
  uint current_time = SDL_GetTicks();
  uint diff         = current_time - m_frame_time;

  if (diff > m_frame_delay)
  {
    m_current_frame += diff / m_frame_delay;
    if (m_current_frame >= m_frames.size()) {
      if (m_loop) {
        m_current_frame %= m_frames.size();
      } else {
        m_current_frame = m_frames.size() - 1;
      }
    }
    m_frame_time = current_time;
  }
}

void Sprite::draw(SDL_Renderer* renderer, int x, int y, bool flipped)
{
  // Draw the current frame of the sprite at the given position
  SDL_Rect dst_rect = {x - m_frames[m_current_frame].w / 2,
                       y - m_frames[m_current_frame].h,
                       m_frames[m_current_frame].w,
                       m_frames[m_current_frame].h};
  if (flipped)
  {
    SDL_RenderCopyEx(
      renderer, m_texture, &m_frames[m_current_frame], &dst_rect, 0, nullptr, SDL_FLIP_HORIZONTAL);
  }
  else
  {
    SDL_RenderCopy(renderer, m_texture, &m_frames[m_current_frame], &dst_rect);
  }
  // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
  // SDL_RenderFillRect(renderer, &dst_rect);
  SDL_Rect base = {x - 5, y - 5, 10, 10};
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 0);
  SDL_RenderFillRect(renderer, &base);
}

void Sprite::reset()
{
  m_current_frame = 0;
  m_frame_time    = SDL_GetTicks();
}

SpriteManager::~SpriteManager()
{
  for (auto& [name, texture] : m_textures) {
    SDL_DestroyTexture(texture);
  }
}

// Load a sprite from a file
void SpriteManager::addSprite(const std::string& sprite_name, const std::string& filename, float frame_delays, SDL_Renderer* renderer, bool loop)
{
  SDL_Surface* surface = IMG_Load(filename.c_str());
  if (!surface) {
    printf("Failed to load image: %s\n", SDL_GetError());
    return;
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    SDL_FreeSurface(surface);
    printf("Failed to create texture: %s\n", SDL_GetError());
    return;
  }

  int texture_width;
  int texture_height;
  SDL_QueryTexture(texture, nullptr, nullptr, &texture_width, &texture_height);

  std::vector<SDL_Rect> frames;
  for (int x = 0; x < texture_width; x += texture_height)
  {
    frames.push_back({x, 0, texture_height, texture_height});
  }

  m_textures[sprite_name] = texture;
  m_frames[sprite_name] = frames;
  m_frame_delays[sprite_name] = frame_delays;
  m_loops[sprite_name] = loop;
}

// Get a sprite by name
Sprite::Ptr SpriteManager::getSprite(const std::string& sprite_name)
{
  return std::make_shared<Sprite>(
    m_textures.at(sprite_name),
    m_frames.at(sprite_name),
    m_frames.at(sprite_name).size(),
    m_frame_delays.at(sprite_name),
    m_loops.at(sprite_name)
  );
}
