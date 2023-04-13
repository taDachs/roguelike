#pragma once

#include "rpg/attack_system.h"
#include "rpg/body.h"
#include "rpg/components.h"
#include "rpg/ecs.h"
#include "rpg/mouse_input.h"
#include "rpg/movement.h"
#include "rpg/ranged_ai.h"
#include "rpg/render.h"
#include "rpg/sprite.h"
#include "rpg/tilemap.h"
#include <SDL.h>

namespace rpg {

void loadTiles(TileManager& tm, SDL_Renderer* renderer)
{
  tm.addTile("occupied", "assets/tiles/black.png", renderer);
  tm.addTile("free", "assets/tiles/white.png", renderer);
}


void loadSprites(SpriteManager& sm, SDL_Renderer* renderer)
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

  sm.addSprite("human/body", "assets/topdown/human_body/body.png", frame_delay, renderer);
  sm.addSprite("human/head", "assets/topdown/human_body/head.png", frame_delay, renderer);
  sm.addSprite("human/leg", "assets/topdown/human_body/leg.png", frame_delay, renderer);
  sm.addSprite("human/arm", "assets/topdown/human_body/arm.png", frame_delay, renderer);

  sm.addSprite("weapons/gun", "assets/topdown/weapons/gun.png", frame_delay, renderer);
}

entt::entity initGun(entt::registry& registry, rpg::SpriteManager& sm)
{
  entt::entity entity = registry.create();

  auto& weapon  = registry.emplace<WeaponComponent>(entity);
  weapon.damage = 5;
  weapon.speed  = 100;
  weapon.range  = 5;

  auto& sprite   = registry.emplace<SpriteComponent>(entity);
  sprite.texture = sm.getTexture("weapons/gun");
  sprite.frame   = sm.getRect("weapons/gun", 0);
  sprite.layer   = 10;

  auto& pose = registry.emplace<PositionComponent>(entity);

  return entity;
}

entt::entity spawnSoldier(entt::registry& registry, rpg::SpriteManager& sm, int x, int y)
{
  entt::entity entity = registry.create();

  auto& pose  = registry.emplace<rpg::PositionComponent>(entity);
  pose.pose.x = x;
  pose.pose.y = y;
  auto& path  = registry.emplace<rpg::PathComponent>(entity);

  auto& render   = registry.emplace<rpg::SpriteComponent>(entity);
  render.texture = sm.getTexture("idle");
  render.frame   = sm.getRect("idle", 0);

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

  auto& clickable  = registry.emplace<rpg::ClickableComponent>(entity);
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

  // entt::entity gun = initGun(registry, sm);
  // auto& weapon = registry.get<rpg::WeaponComponent>(gun);
  // weapon.damage = 10;
  // weapon.range = 5;
  //
  // auto& inventory = registry.emplace<rpg::InventoryComponent>(soldier);
  // inventory.equipped.push_back(gun);
  return soldier;
}

entt::entity spawnEnemy(entt::registry& registry, rpg::SpriteManager& sm, int x, int y)
{
  entt::entity soldier       = spawnSoldier(registry, sm, x, y);
  auto& ranged_ai            = registry.emplace<rpg::RangedAIComponent>(soldier);
  ranged_ai.perimeter_radius = 7;

  entt::entity gun = initGun(registry, sm);
  auto& weapon     = registry.get<rpg::WeaponComponent>(gun);
  weapon.damage    = 2;
  weapon.range     = 3;

  auto& inventory = registry.emplace<rpg::InventoryComponent>(soldier);
  // inventory.equipped.push_back(gun);
  return soldier;
}

entt::entity buildHead(entt::registry& registry,
                       rpg::SpriteManager& sm,
                       entt::entity torso,
                       entt::entity entity = entt::null)
{
  if (!registry.valid(entity))
  {
    entity = registry.create();
  }

  auto& sprite   = registry.emplace<SpriteComponent>(entity);
  sprite.texture = sm.getTexture("human/head");
  sprite.frame   = sm.getRect("human/head", 0);
  sprite.layer   = 10;
  auto& limb     = registry.emplace<LimbComponent>(entity);
  limb.vital     = true;
  limb.attached  = torso;
  auto& pose     = registry.emplace<PositionComponent>(entity);

  auto& health      = registry.emplace<HealthComponent>(entity);
  health.max_health = 20;
  health.health     = 20;

  return entity;
}

entt::entity
buildTorso(entt::registry& registry, rpg::SpriteManager& sm, entt::entity entity = entt::null)
{
  if (!registry.valid(entity))
  {
    entity = registry.create();
  }

  auto& sprite   = registry.emplace<SpriteComponent>(entity);
  sprite.layer   = 9;
  sprite.texture = sm.getTexture("human/body");
  sprite.frame   = sm.getRect("human/body", 0);
  auto& limb     = registry.emplace<LimbComponent>(entity);
  limb.vital     = true;
  auto& pose     = registry.emplace<PositionComponent>(entity);

  auto& health      = registry.emplace<HealthComponent>(entity);
  health.max_health = 50;
  health.health     = 50;

  return entity;
}

entt::entity buildArm(entt::registry& registry,
                      rpg::SpriteManager& sm,
                      entt::entity torso,
                      bool left,
                      entt::entity entity = entt::null)
{
  if (!registry.valid(entity))
  {
    entity = registry.create();
  }

  auto& sprite   = registry.emplace<SpriteComponent>(entity);
  sprite.texture = sm.getTexture("human/arm");
  sprite.frame   = sm.getRect("human/arm", 0);
  sprite.layer   = 9;

  auto& limb    = registry.emplace<LimbComponent>(entity);
  limb.vital    = false;
  limb.attached = torso;
  if (left)
  {
    limb.offset.x = -0.3;
  }
  else
  {
    limb.offset.x = 0.3;
  }
  auto& pose = registry.emplace<PositionComponent>(entity);

  auto& slot    = registry.emplace<SlotComponent>(entity);
  slot.offset.y = -0.2;

  auto& health      = registry.emplace<HealthComponent>(entity);
  health.max_health = 50;
  health.health     = 50;

  return entity;
}

entt::entity buildLeg(entt::registry& registry,
                      rpg::SpriteManager& sm,
                      entt::entity torso,
                      bool left,
                      entt::entity entity = entt::null)
{
  if (!registry.valid(entity))
  {
    entity = registry.create();
  }

  auto& sprite   = registry.emplace<SpriteComponent>(entity);
  sprite.texture = sm.getTexture("human/leg");
  sprite.frame   = sm.getRect("human/leg", 0);
  sprite.layer   = 8;

  auto& limb    = registry.emplace<LimbComponent>(entity);
  limb.vital    = false;
  limb.attached = torso;
  if (left)
  {
    limb.offset.x = -0.1;
  }
  else
  {
    limb.offset.x = 0.1;
  }
  limb.offset.y = -0.1;
  auto& pose    = registry.emplace<PositionComponent>(entity);

  auto& moveable = registry.emplace<MoveableComponent>(entity);
  moveable.speed = 0.5;

  auto& health      = registry.emplace<HealthComponent>(entity);
  health.max_health = 20;
  health.health     = 20;

  return entity;
}


entt::entity
buildHuman(entt::registry& registry, rpg::SpriteManager& sm, entt::entity entity = entt::null)
{
  if (!registry.valid(entity))
  {
    entity = registry.create();
  }

  entt::entity torso = buildTorso(registry, sm);
  entt::entity head  = buildHead(registry, sm, torso);
  entt::entity l_arm = buildArm(registry, sm, torso, true);
  entt::entity r_arm = buildArm(registry, sm, torso, false);
  entt::entity l_leg = buildLeg(registry, sm, torso, true);
  entt::entity r_leg = buildLeg(registry, sm, torso, false);

  auto& body = registry.emplace<BodyComponent>(entity);
  body.body  = torso;
  body.limbs.push_back(head);
  body.limbs.push_back(l_arm);
  body.limbs.push_back(r_arm);
  body.limbs.push_back(l_leg);
  body.limbs.push_back(r_leg);

  auto& pose = registry.emplace<PositionComponent>(entity);

  auto& slot       = registry.get<SlotComponent>(r_arm);
  entt::entity gun = initGun(registry, sm);
  slot.equipped    = gun;

  return entity;
}

entt::entity
buildPlayerChar(entt::registry& registry, rpg::SpriteManager& sm, entt::entity entity = entt::null)
{
  if (!registry.valid(entity))
  {
    entity = registry.create();
  }


  auto& clickable  = registry.emplace<rpg::ClickableComponent>(entity);
  clickable.bbox.x = -32;
  clickable.bbox.y = -16;
  clickable.bbox.w = 64;
  clickable.bbox.h = 32;

  auto& task_queue = registry.emplace<rpg::TaskQueueComponent>(entity);

  return entity;
}
} // namespace rpg
