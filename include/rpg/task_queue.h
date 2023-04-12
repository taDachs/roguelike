#pragma once

#include "rpg/ecs.h"
#include <queue>

namespace rpg {
class Task {
public:
  virtual bool isDone(entt::registry& registry, const entt::entity& entity) const = 0;
  virtual void start(entt::registry& registry, const entt::entity& entity) = 0;
  virtual void update(entt::registry& registry, const entt::entity& entity) {};
  virtual void finish(entt::registry& registry, const entt::entity& entity) {};
};

struct TaskQueueComponent : public Component
{
  std::queue<std::unique_ptr<Task>> queue;
  std::unique_ptr<Task> current_task;
};

class TaskQueueSystem : public System
{
public:
  void update(entt::registry& registry) override;
};
}

