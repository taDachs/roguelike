#include "rpg/attack_system.h"
#include "rpg/camera.h"
#include "rpg/components.h"
#include "rpg/ecs.h"
#include "rpg/game.h"
#include "rpg/map.h"
#include "rpg/mouse_input.h"
#include "rpg/movement.h"
#include "rpg/ranged_ai.h"
#include "rpg/render.h"
#include "rpg/sprite.h"
#include "rpg/systems.h"
#include "rpg/task_queue.h"
#include "rpg/tilemap.h"
#include <SDL.h>
#include <SDL_image.h>
#include <entt/entt.hpp>
#include <iostream>

const int SCREEN_WIDTH  = 1200;
const int SCREEN_HEIGHT = 600;

const int GRID_RESOLUTION = 0.3;

void loadTiles(rpg::TileManager& tm, SDL_Renderer* renderer)
{
  tm.addTile("occupied", "assets/tiles/black.png", renderer);
  tm.addTile("free", "assets/tiles/white.png", renderer);
}


void loadSprites(rpg::SpriteManager& sm, SDL_Renderer* renderer)
{
  int frame_delay = 100;
  sm.addSprite("attack", "assets/soldier_1/Attack.png", frame_delay, renderer);
  sm.addSprite("dead", "assets/soldier_1/Dead.png", frame_delay, renderer, false);
  sm.addSprite("hurt", "assets/soldier_1/Hurt.png", frame_delay, renderer);
  sm.addSprite("idle", "assets/soldier_1/Idle.png", frame_delay, renderer);
  sm.addSprite("recharge", "assets/soldier_1/Recharge.png", frame_delay, renderer);
  sm.addSprite("running", "assets/soldier_1/Run.png", frame_delay, renderer);
  sm.addSprite("shot_1", "assets/soldier_1/Shot_1.png", 25, renderer);
  sm.addSprite("shot_2", "assets/soldier_1/Shot_2.png", frame_delay, renderer);
  sm.addSprite("walking", "assets/soldier_1/Walk.png", frame_delay, renderer);
}

entt::entity initGun(entt::registry& registry, rpg::SpriteManager& sm)
{
  entt::entity entity = registry.create();

  auto& weapon  = registry.emplace<rpg::WeaponComponent>(entity);
  weapon.damage = 5;
  weapon.speed  = 100;
  weapon.range  = 5;

  return entity;
}

entt::entity spawnSoldier(entt::registry& registry, rpg::SpriteManager& sm, int x, int y)
{
  entt::entity entity = registry.create();

  auto& pose   = registry.emplace<rpg::PositionComponent>(entity);
  pose.pose.x  = x;
  pose.pose.y  = y;
  auto& path   = registry.emplace<rpg::PathComponent>(entity);

  auto& render  = registry.emplace<rpg::SpriteComponent>(entity);
  render.sprite = sm.getSprite("idle");

  auto& animation = registry.emplace<rpg::AnimationStateComponent>(entity);
  std::map<rpg::State, std::shared_ptr<rpg::Sprite> > sprite_map;
  sprite_map[rpg::State::WALKING]  = sm.getSprite("walking");
  sprite_map[rpg::State::DEAD]     = sm.getSprite("dead");
  sprite_map[rpg::State::IDLE]     = sm.getSprite("idle");
  sprite_map[rpg::State::SHOOTING] = sm.getSprite("shot_1");
  sprite_map[rpg::State::RUNNING]  = sm.getSprite("running");
  animation.sprite_map             = sprite_map;

  auto& state = registry.emplace<rpg::StateComponent>(entity);
  state.state = rpg::State::IDLE;

  auto& stats = registry.emplace<rpg::StatsComponent>(entity);
  stats.speed = 2;

  auto& moveable = registry.emplace<rpg::MoveableComponent>(entity);
  moveable.speed = 2;

  auto& task_queue = registry.emplace<rpg::TaskQueueComponent>(entity);

  auto& clickable = registry.emplace<rpg::ClickableComponent>(entity);
  clickable.bbox.x = -16;
  clickable.bbox.y = -70;
  clickable.bbox.w = 32;
  clickable.bbox.h = 70;

  auto& health  = registry.emplace<rpg::HealthComponent>(entity);
  health.health = 100;
  // auto& player_control = entity.addComponent<rpg::PlayerControlComponent>();

  return entity;
}

entt::entity spawnFriendlySoldier(entt::registry& registry, rpg::SpriteManager& sm, int x, int y)
{
  entt::entity soldier = spawnSoldier(registry, sm, x, y);
  registry.emplace<rpg::PlayerFactionComponent>(soldier);

  entt::entity gun = initGun(registry, sm);
  auto& weapon = registry.get<rpg::WeaponComponent>(gun);
  weapon.damage = 10;
  weapon.range = 5;

  auto& inventory = registry.emplace<rpg::InventoryComponent>(soldier);
  inventory.equipped.push_back(gun);
  return soldier;
}

entt::entity spawnEnemy(entt::registry& registry, rpg::SpriteManager& sm, int x, int y)
{
  entt::entity soldier = spawnSoldier(registry, sm, x, y);
  auto& ranged_ai = registry.emplace<rpg::RangedAIComponent>(soldier);
  ranged_ai.perimeter_radius = 7;

  entt::entity gun = initGun(registry, sm);
  auto& weapon = registry.get<rpg::WeaponComponent>(gun);
  weapon.damage = 2;
  weapon.range = 3;

  auto& inventory = registry.emplace<rpg::InventoryComponent>(soldier);
  inventory.equipped.push_back(gun);
  return soldier;
}


int main(int argc, char* args[])
{
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO))
  {
    printf("Failed to initialize SDL: %s\n", SDL_GetError());
    return 1;
  }

  if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
  {
    printf("Failed to initialize SDL_Image: %s\n", SDL_GetError());
    return 1;
  }

  // Create a window
  SDL_Window* window = SDL_CreateWindow("My Window",
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SCREEN_WIDTH,
                                        SCREEN_HEIGHT,
                                        SDL_WINDOW_SHOWN);

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

  if (!renderer)
  {
    printf("Failed to create renderer: %s\n", SDL_GetError());
  }

  rpg::SpriteManager sprite_manager;
  loadSprites(sprite_manager, renderer);
  rpg::TileManager tile_manager;
  loadTiles(tile_manager, renderer);

  entt::registry registry;

  rpg::Game::map.loadOccupancyFromFile("./assets/map.txt", 0.3);

  rpg::Game::map.addTileEntities(registry, tile_manager);

  glm::mat3 game_to_screen(
    glm::vec3(SCREEN_WIDTH / (rpg::Game::map.getWidth() * rpg::Game::map.getResolution()), 0, 0),
    glm::vec3(0, SCREEN_HEIGHT / (rpg::Game::map.getHeight() * rpg::Game::map.getResolution()), 0),
    glm::vec3(0, 0, 1));
  rpg::Camera camera;
  camera.setRealToScreen(game_to_screen);

  auto sprite_render = std::make_shared<rpg::SpriteSystem>();
  auto tilemap       = std::make_shared<rpg::TileSystem>();
  auto state       = std::make_shared<rpg::StateSystem>();
  auto clickable       = std::make_shared<rpg::ClickableSystem>(camera);
  auto ranged_ai       = std::make_shared<rpg::RangedAISystem>();

  auto animation = std::make_shared<rpg::AnimationStateSystem>();

  auto path_following = std::make_shared<rpg::PathFollowingSystem>();

  auto task_queue    = std::make_shared<rpg::TaskQueueSystem>();
  auto health_system = std::make_shared<rpg::HealthSystem>();
  auto attack_system = std::make_shared<rpg::AttackSystem>();

  rpg::SystemRunner system_runner;

  system_runner.addSystem(sprite_render);
  system_runner.addSystem(tilemap);
  system_runner.addSystem(state);
  system_runner.addSystem(clickable);
  system_runner.addSystem(ranged_ai);
  system_runner.addSystem(animation);
  system_runner.addSystem(path_following);
  system_runner.addSystem(task_queue);
  system_runner.addSystem(health_system);
  system_runner.addSystem(attack_system);

  entt::entity friendly_soldier_id = spawnFriendlySoldier(registry, sprite_manager, 1, 4);
  entt::entity enemy_soldier_id_1    = spawnEnemy(registry, sprite_manager, 10, 4);
  entt::entity enemy_soldier_id_2    = spawnEnemy(registry, sprite_manager, 10, 3);

  int fps         = 10;
  int frame_delay = 1000 / fps;
  uint frame_start;
  uint frame_time;

  SDL_Rect dst_rect = {0, 0, 384, 384}; // Example sprite size and position

  // Draw the sprite
  // Wait for user to exit the window
  SDL_Event event;
  bool quit = false;

  while (!quit)
  {
    frame_start = SDL_GetTicks();

    while (SDL_PollEvent(&event))
    {
      glm::vec2 mouse_pos;
      mouse_pos.x = event.button.x;
      mouse_pos.y = event.button.y;
      clickable->setMousePose(mouse_pos);
      if (event.type == SDL_MOUSEBUTTONDOWN)
      {
        // Handle mouse click
        if (event.button.button == SDL_BUTTON_LEFT)
        {
          // Handle left mouse button click

          glm::vec2 game_pos = camera.screenToReal(mouse_pos);
          auto move_task =
            std::make_unique<rpg::MoveTask>(game_pos, rpg::Game::map.getResolution());
          auto& queue = registry.get<rpg::TaskQueueComponent>(friendly_soldier_id);
          queue.queue.push(std::move(move_task));
        }
        if (event.button.button == SDL_BUTTON_RIGHT)
        {
          // Handle left mouse button click
          entt::entity clicked;
          if (!rpg::findClicked(registry, mouse_pos, camera, clicked))
          {
            continue;
          }

          auto& queue = registry.get<rpg::TaskQueueComponent>(friendly_soldier_id);
          auto attack_task   = std::make_unique<rpg::AttackTask>(clicked);
          queue.queue        = {};
          queue.current_task = nullptr;
          queue.queue.push(std::move(attack_task));
        }
      }
      if (event.type == SDL_QUIT)
      {
        quit = true;
      }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // drawGrid(renderer, 20, 20, 30);

    // map->draw(renderer);
    system_runner.update(registry);
    system_runner.draw(registry, renderer, camera);

    SDL_RenderPresent(renderer);
  }
  // Clean up and exit
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
