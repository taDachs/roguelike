#pragma once

#include "rpg/ecs.h"
#include "rpg/asset_manager.h"
#include <glm/glm.hpp>

namespace rpg {

using State = std::string;

struct PositionComponent : public Component {
  glm::vec3 pose;
};

struct StateComponent : public Component {
  State state;
};

struct RenderComponent : public Component {
  std::shared_ptr<Sprite> sprite;
};

struct AnimationComponent : public Component {
  std::map<State, Sprite::Ptr> sprite_map;
  State previous_state;
};

struct PlayerInputComponent : public Component {

};

}

