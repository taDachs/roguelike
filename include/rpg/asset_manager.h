#pragma once
#include <SDL.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace rpg {

class Sprite
{
public:
  using Ptr = std::shared_ptr<Sprite>;

  Sprite(const std::string& filename,
         SDL_Renderer* renderer,
         int frame_width,
         int frame_height,
         int num_frames,
         int frame_delay,
         const SDL_Rect& sprite_rect);

  Sprite(const std::string& filename,
         SDL_Renderer* renderer,
         int frame_delay,
         const SDL_Rect& sprite_rect);

  Sprite(const std::string& filename, SDL_Renderer* renderer, int frame_delay);

  Sprite(const std::string& filename,
         SDL_Renderer* renderer,
         int frame_width,
         int frame_height,
         int num_frames,
         int frame_delay)
    : Sprite(filename,
             renderer,
             frame_width,
             frame_height,
             num_frames,
             frame_delay,
             {0, 0, frame_width, frame_height}){};

  ~Sprite();

  void update();
  void draw(SDL_Renderer* renderer, int x, int y, bool flipped = false);
  void draw(SDL_Renderer* renderer, SDL_Rect dst, bool flipped = false);

  void reset();

private:
  SDL_Texture* m_texture;
  std::vector<SDL_Rect> m_frames;
  int m_current_frame;
  int m_frame_delay;
  uint m_frame_time;
};

class SpriteManager
{
public:
  // Constructor and destructor
  SpriteManager()  = default;
  ~SpriteManager() = default;

  // Load a sprite from a file
  void addSprite(const std::string& sprite_name, const Sprite::Ptr& sprite);
  // Get a sprite by name
  Sprite::Ptr getSprite(const std::string& sprite_name);

  std::vector<std::string> getKeys() const
  {
    std::vector<std::string> out;
    out.reserve(m_sprites.size());
    for (const auto& p : m_sprites) {
      out.push_back(p.first);
    }
    return out;
  }


private:
  // Map of sprite names to SDL_Texture pointers
  std::map<std::string, Sprite::Ptr> m_sprites;
};

} // namespace rpg
