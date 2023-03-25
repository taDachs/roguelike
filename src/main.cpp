#include <SDL.h>
#include <iostream>
#include "rpg/asset_manager.h"
#include "rpg/render.h"
#include "rpg/ecs.h"
#include "rpg/components.h"

const std::vector<rpg::State> STATES = {
  "idle",
  "walking",
  "running",
  "shot_1",
  "dead"
};

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

rpg::EntityID initSoldier(rpg::SpriteManager& sm, rpg::Manager& mg) {
  rpg::EntityID id = mg.addEntity();
  rpg::Entity& entity = mg.getEntity(id);

  auto pose = std::make_shared<rpg::PositionComponent>();
  pose->pose.x = 128;
  pose->pose.y = 128;

  auto render = std::make_shared<rpg::RenderComponent>();
  render->sprite = sm.getSprite("idle");

  auto animation = std::make_shared<rpg::AnimationComponent>();
  std::map<rpg::State, std::shared_ptr<rpg::Sprite>> sprite_map;
  for (const auto& state : STATES) {
    sprite_map[state] = sm.getSprite(state);
  }
  animation->sprite_map = sprite_map;
  // animation->sprite_map =

  auto state = std::make_shared<rpg::StateComponent>();
  state->state = "idle";
  // animation->sprite_map =

  entity.addComponent(pose);
  entity.addComponent(render);
  entity.addComponent(animation);
  entity.addComponent(state);

  return id;
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

  rpg::Manager manager;
  auto render = std::make_unique<rpg::RenderSystem>(renderer);
  auto animation = std::make_unique<rpg::AnimationSystem>();
  manager.addSystem(std::move(render));
  manager.addSystem(std::move(animation));
  rpg::EntityID soldier_id = initSoldier(sprite_manager, manager);

  int fps = 10;
  int frame_delay = 1000 / fps;
  uint frame_start;
  uint frame_time;

  SDL_Rect dst_rect = {0, 0, 384, 384}; // Example sprite size and position

  // Draw the sprite
  // Wait for user to exit the window
  SDL_Event event;
  bool quit = false;

  int state_index = 0;

  while (!quit)
  {
    frame_start = SDL_GetTicks();

    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_n) {
          state_index += 1;
          state_index %= STATES.size();
        }
        if (event.key.keysym.sym == SDLK_p) {
          state_index -= 1;
          state_index  += STATES.size();
          state_index %= STATES.size();
        }
      }
      if (event.type == SDL_QUIT)
      {
        quit = true;
      }
    }


    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    manager.getEntity(soldier_id).getComponent<rpg::StateComponent>()->state = STATES[state_index];
    manager.update();

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
