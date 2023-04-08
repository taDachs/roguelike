#include "rpg/asset_manager.h"
#include "rpg/components.h"
#include "rpg/ecs.h"
#include "rpg/map.h"
#include "rpg/player_control.h"
#include "rpg/render.h"
#include "rpg/systems.h"
#include <SDL.h>
#include <iostream>
#include "rpg/movement.h"

const std::vector<rpg::State> STATES = {"idle", "walking", "running", "shot_1", "dead"};

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

rpg::EntityID initSoldier(rpg::SpriteManager& sm, rpg::ECSManager& mg)
{
  rpg::EntityID id    = mg.addEntity();
  rpg::Entity& entity = mg.getEntity(id);


  auto& pose = entity.addComponent<rpg::PositionComponent>();
  pose.pose.x = 0;
  pose.pose.y = 0;
  auto& path = entity.addComponent<rpg::PathComponent>();
  path.reached = true;

  auto& render    = entity.addComponent<rpg::RenderComponent>();
  render.sprite = sm.getSprite("idle");

  auto& animation = entity.addComponent<rpg::AnimationComponent>();
  std::map<rpg::State, std::shared_ptr<rpg::Sprite> > sprite_map;
  for (const auto& state : STATES)
  {
    sprite_map[state] = sm.getSprite(state);
  }
  animation.sprite_map = sprite_map;

  auto& state   = entity.addComponent<rpg::StateComponent>();
  state.state = "idle";

  auto& stats                    = entity.addComponent<rpg::StatsComponent>();
  stats.speed = 5;

  // auto& player_control = entity.addComponent<rpg::PlayerControlComponent>();

  return id;
}

void drawGrid(SDL_Renderer* renderer, int rows, int cols, int grid_size) {

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

  rpg::ECSManager manager;

  auto map = std::make_shared<rpg::Map>();

  auto render = std::make_shared<rpg::RenderSystem>();
  glm::mat3 game_to_screen(glm::vec3(20, 0, 0), glm::vec3(0, 20, 0), glm::vec3(0, 0, 1));
  render->setGridToScreen(game_to_screen);

  auto animation      = std::make_shared<rpg::AnimationSystem>();
  auto player_control = std::make_shared<rpg::PlayerControlSystem>();

  auto moveable = std::make_shared<rpg::MoveableSystem>();

  auto path_following = std::make_shared<rpg::PathFollowingSystem>();
  auto path_planning = std::make_shared<rpg::PathPlanningSystem>(map);

  manager.addSystem(render);
  manager.addSystem(animation);
  manager.addSystem(player_control);
  manager.addSystem(moveable);
  manager.addSystem(path_following);
  manager.addSystem(path_planning);


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

          rpg::Entity& soldier = manager.getEntity(soldier_id);
          auto& path = soldier.getComponent<rpg::PathComponent>();
          auto& pose = soldier.getComponent<rpg::PositionComponent>();

          glm::vec2 game_pos = render->getScreenToGame() * click_pos;
          std::cout << game_pos.x << ", " << game_pos.y << std::endl;
          if (path.path.empty()) {
            path.last_tick = SDL_GetTicks();
            path.last_position = pose.pose;
          }
          path.goal = game_pos;
          path.path.clear();
          path.reached = false;
          // g_path->path.push_back(game_pos);
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

    // drawGrid(renderer, 20, 20, 30);

    manager.update();
    manager.draw(renderer);

    SDL_RenderPresent(renderer);
  }

  // Clean up and exit
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
