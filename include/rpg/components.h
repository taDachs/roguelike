#pragma once

#include "rpg/ecs.h"
#include "rpg/asset_manager.h"
#include <glm/glm.hpp>

namespace rpg {

using State = std::string;

struct PositionComponent : public Component {
  enum Orientation {
    LEFT,
    RIGHT,
  };
  glm::vec2 pose;
  Orientation orientation = RIGHT;
};

struct StateComponent : public Component {
  State state;
};

struct RenderComponent : public Component {
  std::shared_ptr<Sprite> sprite;
};

struct MoveableComponent : public Component {
  // in m/s
  glm::vec2 current_direction;
  bool is_running = false;
  float walking_velocity;
  float running_velocity;
};

struct AnimationComponent : public Component {
  std::map<State, Sprite::Ptr> sprite_map;
  State previous_state;
};

struct PlayerControlComponent : public Component {};

struct PathFollowingComponent : public Component {
  std::vector<glm::vec2> path;
  glm::vec2 goal;
};

}

