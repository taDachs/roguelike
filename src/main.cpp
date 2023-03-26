#include "rpg/asset_manager.h"
#include "rpg/components.h"
#include "rpg/ecs.h"
#include "rpg/player_control.h"
#include "rpg/render.h"
#include "rpg/systems.h"
#include <SDL.h>
#include <iostream>

const std::vector<rpg::State> STATES = {"idle", "walking", "running", "shot_1", "dead"};
auto g_path = std::make_shared<rpg::PathFollowingComponent>();

void loadSprites(rpg::SpriteManager& sm, SDL_Renderer* renderer)
{
  int frame_delay                = 100;
  SDL_Rect attack_rect           = {32, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr attack_sprite = std::make_shared<rpg::Sprite>(
    "assets/soldier_1/Attack.png", renderer, frame_delay, attack_rect);
  sm.addSprite("attack", attack_sprite);

  SDL_Rect dead_rect = {32, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr dead_sprite =
    std::make_shared<rpg::Sprite>("assets/soldier_1/Dead.png", renderer, frame_delay, dead_rect);
  sm.addSprite("dead", dead_sprite);

  SDL_Rect hurt_rect = {32, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr hurt_sprite =
    std::make_shared<rpg::Sprite>("assets/soldier_1/Hurt.png", renderer, frame_delay, hurt_rect);
  sm.addSprite("hurt", hurt_sprite);

  SDL_Rect idle_rect = {32, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr idle_sprite =
    std::make_shared<rpg::Sprite>("assets/soldier_1/Idle.png", renderer, frame_delay, idle_rect);
  sm.addSprite("idle", idle_sprite);

  SDL_Rect recharge_rect           = {32, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr recharge_sprite = std::make_shared<rpg::Sprite>(
    "assets/soldier_1/Recharge.png", renderer, frame_delay, recharge_rect);
  sm.addSprite("recharge", recharge_sprite);

  SDL_Rect run_rect = {40, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr run_sprite =
    std::make_shared<rpg::Sprite>("assets/soldier_1/Run.png", renderer, frame_delay, run_rect);
  sm.addSprite("running", run_sprite);

  SDL_Rect shot_1_rect           = {32, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr shot_1_sprite = std::make_shared<rpg::Sprite>(
    "assets/soldier_1/Shot_1.png", renderer, frame_delay, shot_1_rect);
  sm.addSprite("shot_1", shot_1_sprite);

  SDL_Rect shot_2_rect           = {40, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr shot_2_sprite = std::make_shared<rpg::Sprite>(
    "assets/soldier_1/Shot_2.png", renderer, frame_delay, shot_2_rect);
  sm.addSprite("shot_2", shot_2_sprite);

  SDL_Rect walk_rect = {40, 64, 64, 64}; // Example sprite size and position
  rpg::Sprite::Ptr walk_sprite =
    std::make_shared<rpg::Sprite>("assets/soldier_1/Walk.png", renderer, frame_delay, walk_rect);
  sm.addSprite("walking", walk_sprite);
}

rpg::EntityID initSoldier(rpg::SpriteManager& sm, rpg::Manager& mg)
{
  rpg::EntityID id    = mg.addEntity();
  rpg::Entity& entity = mg.getEntity(id);

  auto pose    = std::make_shared<rpg::PositionComponent>();
  pose->pose.x = 10;
  pose->pose.y = 10;

  auto render    = std::make_shared<rpg::RenderComponent>();
  render->sprite = sm.getSprite("idle");

  auto animation = std::make_shared<rpg::AnimationComponent>();
  std::map<rpg::State, std::shared_ptr<rpg::Sprite> > sprite_map;
  for (const auto& state : STATES)
  {
    sprite_map[state] = sm.getSprite(state);
  }
  animation->sprite_map = sprite_map;

  auto state   = std::make_shared<rpg::StateComponent>();
  state->state = "idle";

  auto moveable                 = std::make_shared<rpg::MoveableComponent>();
  moveable->current_direction.x = 0;
  moveable->current_direction.y = 0;
  moveable->walking_velocity    = 3;
  moveable->running_velocity    = 9;

  auto player_control = std::make_shared<rpg::PlayerControlComponent>();

  entity.addComponent(pose);
  entity.addComponent(render);
  entity.addComponent(animation);
  entity.addComponent(state);
  // entity.addComponent(player_control);
  entity.addComponent(moveable);
  entity.addComponent(g_path);

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

  if (!renderer)
  {
    printf("Failed to create renderer: %s\n", SDL_GetError());
  }

  rpg::SpriteManager sprite_manager;
  loadSprites(sprite_manager, renderer);

  rpg::Manager manager;

  auto render = std::make_shared<rpg::RenderSystem>(renderer);
  glm::mat3 game_to_screen(glm::vec3(20, 0, 0), glm::vec3(0, 20, 0), glm::vec3(0, 0, 1));
  render->setGameToScreen(game_to_screen);

  auto animation      = std::make_shared<rpg::AnimationSystem>();
  auto player_control = std::make_shared<rpg::PlayerControlSystem>();

  auto moveable = std::make_shared<rpg::MoveableSystem>();

  auto path = std::make_shared<rpg::PathFollowingSystem>();

  manager.addSystem(render);
  manager.addSystem(animation);
  manager.addSystem(player_control);
  manager.addSystem(moveable);
  manager.addSystem(path);

  rpg::EntityID soldier_id = initSoldier(sprite_manager, manager);

  int fps         = 10;
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
      if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
      {
        player_control->addEvent(event);
      }
      if (event.type == SDL_MOUSEBUTTONDOWN)
      {
        // Handle mouse click
        if (event.button.button == SDL_BUTTON_LEFT)
        {
          // Handle left mouse button click
          glm::vec3 click_pos;
          click_pos.x = event.button.x;
          click_pos.y = event.button.y;
          click_pos.z = 1;

          glm::vec2 screen_pos = render->getScreenToGame() * click_pos;
          std::cout << screen_pos.x << ", " << screen_pos.y << std::endl;
          g_path->path.push_back(screen_pos);
          // Do something with x and y coordinates
        }
        // Handle other mouse button clicks as needed
      }
      if (event.type == SDL_QUIT)
      {
        quit = true;
      }
    }


    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    manager.update();

    SDL_RenderPresent(renderer);

    // frame_time = SDL_GetTicks() - frame_start;
    // if (frame_time < frame_delay)
    // {
    //   SDL_Delay(frame_delay - frame_time);
    // }
  }

  // Clean up and exit
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
