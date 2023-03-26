#include "rpg/asset_manager.h"
#include <SDL_image.h>
#include <iostream>

using namespace rpg;

Sprite::Sprite(const std::string& filename, SDL_Renderer* renderer, int frame_width, int frame_height, int num_frames, int frame_delay, const SDL_Rect& sprite_rect)
  : m_current_frame(0)
  , m_frame_delay(frame_delay)
  , m_frame_time(SDL_GetTicks())
{
  // Load the image surface
  SDL_Surface* surface = IMG_Load(filename.c_str());
  if (!surface)
  {
    printf("Failed to load image: %s\n", SDL_GetError());
    return;
  }

  // Create the texture from the image surface
  m_texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!m_texture)
  {
    printf("Failed to create texture: %s\n", SDL_GetError());
    SDL_FreeSurface(surface);
    return;
  }

  // Split the image into frames and create textures for each frame
  for (int i = 0; i < num_frames; i++)
  {
    SDL_Rect frame_rect = {i * frame_width + sprite_rect.x, sprite_rect.h, sprite_rect.w, sprite_rect.h};
    m_frames.push_back(frame_rect);
  }

  SDL_FreeSurface(surface);
}

Sprite::Sprite(const std::string& filename, SDL_Renderer* renderer, int frame_delay, const SDL_Rect& sprite_rect)
  : m_current_frame(0)
  , m_frame_delay(frame_delay)
  , m_frame_time(SDL_GetTicks())
{
  // Load the image surface
  SDL_Surface* surface = IMG_Load(filename.c_str());
  if (!surface)
  {
    printf("Failed to load image: %s\n", SDL_GetError());
    return;
  }

  // Create the texture from the image surface
  m_texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!m_texture)
  {
    printf("Failed to create texture: %s\n", SDL_GetError());
    SDL_FreeSurface(surface);
    return;
  }

  // Get the size of the texture
  int width;
  int height;
  SDL_QueryTexture(m_texture, nullptr, nullptr, &width, &height);
  int num_frames = width / height;

  // Split the image into frames and create textures for each frame
  for (int i = 0; i < num_frames; i++)
  {
    SDL_Rect frame_rect = {i * height + sprite_rect.x, sprite_rect.y, sprite_rect.w, sprite_rect.h};
    m_frames.push_back(frame_rect);
  }

  SDL_FreeSurface(surface);
}

Sprite::Sprite(const std::string& filename, SDL_Renderer* renderer, int frame_delay)
  : m_current_frame(0)
  , m_frame_delay(frame_delay)
  , m_frame_time(0)
{
  // Load the image surface
  SDL_Surface* surface = IMG_Load(filename.c_str());
  if (!surface)
  {
    printf("Failed to load image: %s\n", SDL_GetError());
    return;
  }

  // Create the texture from the image surface
  m_texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!m_texture)
  {
    printf("Failed to create texture: %s\n", SDL_GetError());
    SDL_FreeSurface(surface);
    return;
  }

  // Get the size of the texture
  int width;
  int height;
  SDL_QueryTexture(m_texture, nullptr, nullptr, &width, &height);
  int num_frames = width / height;

  // Split the image into frames and create textures for each frame
  for (int i = 0; i < num_frames; i++)
  {
    SDL_Rect frame_rect = {i * height, 0, height, height};
    m_frames.push_back(frame_rect);
  }

  SDL_FreeSurface(surface);
}
Sprite::~Sprite()
{
  SDL_DestroyTexture(m_texture);
}

void Sprite::update()
{
  // Update the current frame based on the frame delay
  uint current_time = SDL_GetTicks();
  uint diff = current_time - m_frame_time;

  if (diff > m_frame_delay) {
    m_current_frame += diff / m_frame_delay;
    m_current_frame %= m_frames.size();
    m_frame_time = current_time;
  }
}

void Sprite::draw(SDL_Renderer* renderer, int x, int y, bool flipped)
{
  // Draw the current frame of the sprite at the given position
  SDL_Rect dst_rect = {x, y, m_frames[m_current_frame].w, m_frames[m_current_frame].h};
  if (flipped) {
    SDL_RenderCopyEx(renderer, m_texture, &m_frames[m_current_frame], &dst_rect, 0, nullptr, SDL_FLIP_HORIZONTAL);
  } else {
    SDL_RenderCopy(renderer, m_texture, &m_frames[m_current_frame], &dst_rect);
  }
}

void Sprite::reset()
{
  m_current_frame = 0;
  m_frame_time = SDL_GetTicks();
}


// Load a sprite from a file
void SpriteManager::addSprite(const std::string& sprite_name, const Sprite::Ptr& sprite) {
  m_sprites[sprite_name] = sprite;
}

// Get a sprite by name
Sprite::Ptr SpriteManager::getSprite(const std::string& sprite_name)
{
  return m_sprites.at(sprite_name);
}
