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

  Sprite(SDL_Texture* texture,
         std::vector<SDL_Rect>& frames,
         int num_frames,
         int frame_delay,
         bool loop = true)
    : m_texture(texture)
    , m_frames(frames)
    , m_frame_delay(frame_delay)
    , m_loop(loop)
  {
  }

  ~Sprite() = default;

  void update();
  void draw(SDL_Renderer* renderer, int x, int y, bool flipped = false);
  void draw(SDL_Renderer* renderer, SDL_Rect dst, bool flipped = false);

  void reset();

private:
  SDL_Texture* m_texture;
  std::vector<SDL_Rect>& m_frames;
  int m_current_frame = 0;
  int m_frame_delay;
  uint m_frame_time;
  bool m_loop;
};

class SpriteManager
{
public:
  // Constructor and destructor
  SpriteManager()  = default;
  ~SpriteManager();

  // Load a sprite from a file
  void addSprite(const std::string& sprite_name, const std::string& filename, float frame_delays, SDL_Renderer* renderer, bool loop = true);
  // Get a sprite by name
  Sprite::Ptr getSprite(const std::string& sprite_name);
  SDL_Texture* getTexture(const std::string& sprite_name) const { return m_textures.at(sprite_name); }
  SDL_Rect getRect(const std::string& sprite_name, int frame) const { return m_frames.at(sprite_name).at(frame); }

private:
  // Map of sprite names to SDL_Texture pointers
  std::map<std::string, SDL_Texture*> m_textures;
  std::map<std::string, std::vector<SDL_Rect> > m_frames;
  std::map<std::string, int> m_frame_delays;
  std::map<std::string, bool> m_loops;
};

} // namespace rpg
