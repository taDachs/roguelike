#include "rpg/actor.h"
#include <SDL.h>
#include <iostream>
#include <rpg/asset_manager.h>

void loadSprites(rpg::SpriteManager& sm, SDL_Renderer* renderer) {
  SDL_Rect attack_rect = {32, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr attack_sprite = std::make_shared<rpg::Sprite>("assets/soldier_1/Attack.png", renderer, 30, attack_rect);
  sm.addSprite("attack", attack_sprite);

  SDL_Rect dead_rect = {32, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr dead_sprite = std::make_shared<rpg::Sprite>("assets/soldier_1/Dead.png", renderer, 30, dead_rect);
  sm.addSprite("dead", dead_sprite);

  SDL_Rect hurt_rect = {32, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr hurt_sprite = std::make_shared<rpg::Sprite>("assets/soldier_1/Hurt.png", renderer, 30, hurt_rect);
  sm.addSprite("hurt", hurt_sprite);

  SDL_Rect idle_rect = {32, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr idle_sprite = std::make_shared<rpg::Sprite>("assets/soldier_1/Idle.png", renderer, 30, idle_rect);
  sm.addSprite("idle", idle_sprite);

  SDL_Rect recharge_rect = {32, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr recharge_sprite = std::make_shared<rpg::Sprite>("assets/soldier_1/Recharge.png", renderer, 30, recharge_rect);
  sm.addSprite("recharge", recharge_sprite);

  SDL_Rect run_rect = {40, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr run_sprite = std::make_shared<rpg::Sprite>("assets/soldier_1/Run.png", renderer, 30, run_rect);
  sm.addSprite("running", run_sprite);

  SDL_Rect shot_1_rect = {32, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr shot_1_sprite = std::make_shared<rpg::Sprite>("assets/soldier_1/Shot_1.png", renderer, 30, shot_1_rect);
  sm.addSprite("shot_1", shot_1_sprite);

  SDL_Rect shot_2_rect = {40, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr shot_2_sprite = std::make_shared<rpg::Sprite>("assets/soldier_1/Shot_2.png", renderer, 30, shot_2_rect);
  sm.addSprite("shot_2", shot_2_sprite);

  SDL_Rect walk_rect = {40, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr walk_sprite = std::make_shared<rpg::Sprite>("assets/soldier_1/Walk.png", renderer, 30, walk_rect);
  sm.addSprite("walking", walk_sprite);
}

rpg::Actor setupActor(rpg::SpriteManager& sm) {
  std::map<rpg::State, rpg::Sprite::Ptr> sprites;
  sprites[rpg::State::IDLE] = sm.getSprite("idle");
  sprites[rpg::State::FIRING] = sm.getSprite("shot_2");
  sprites[rpg::State::WALKING] = sm.getSprite("walking");
  sprites[rpg::State::RUNNING] = sm.getSprite("running");
  sprites[rpg::State::DEAD] = sm.getSprite("dead");
  sprites[rpg::State::NONE] = sm.getSprite("idle");

  rpg::Visualizer vis(sprites);

  rpg::Actor actor;
  actor.setVisual(vis);

  return actor;
}

int main(int argc, char* args[])
{
  // Initialize SDL
  SDL_Init(SDL_INIT_VIDEO);

  // Create a window
  SDL_Window* window = SDL_CreateWindow(
    "My Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

  if (!renderer) {
    printf("Failed to create renderer: %s\n", SDL_GetError());
  }

  rpg::SpriteManager sprite_manager;
  loadSprites(sprite_manager, renderer);


  int fps = 10;
  int frame_delay = 1000 / fps;
  uint frame_start;
  uint frame_time;

  std::vector<rpg::State> states = {
    rpg::IDLE,
    rpg::FIRING,
    rpg::WALKING,
    rpg::RUNNING,
    rpg::DEAD
  };

  SDL_Rect dst_rect = {0, 0, 384, 384}; // Example sprite size and position

  // Draw the sprite
  // Wait for user to exit the window
  SDL_Event event;
  bool quit = false;

  int state_index = 0;
  rpg::Actor actor = setupActor(sprite_manager);
  actor.setState(states[state_index]);

  while (!quit)
  {
    frame_start = SDL_GetTicks();

    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_n) {
          state_index += 1;
          state_index %= states.size();
        }
      }
      if (event.type == SDL_KEYUP) {
        if (event.key.keysym.sym == SDLK_n) {
          state_index -= 1 + states.size(); // for avoiding underflows
          state_index %= states.size();
        }
      }
      if (event.type == SDL_QUIT)
      {
        quit = true;
      }
    }


    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    actor.getVisual().draw(renderer);

    SDL_RenderPresent(renderer);

    frame_time = SDL_GetTicks() - frame_start;
    if (frame_time < frame_delay) {
        SDL_Delay(frame_delay - frame_time);
    }
  }

  // Clean up and exit
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
