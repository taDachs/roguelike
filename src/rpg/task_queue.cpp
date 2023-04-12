#include "rpg/task_queue.h"
#include <iostream>

using namespace rpg;

void TaskQueueSystem::update(entt::registry& registry)
{
  for (auto &&[entity, queue] : registry.view<TaskQueueComponent>().each())
  {
    if (queue.current_task == nullptr) {
      if (queue.queue.empty()) {
        continue;
      }

      queue.current_task = std::move(queue.queue.front());
      queue.queue.pop();
      queue.current_task->start(registry, entity);
    }

    if (queue.current_task->isDone(registry, entity)) {
      queue.current_task->finish(registry, entity);
      queue.current_task = nullptr;
    } else {
      queue.current_task->update(registry, entity);
    }
  }
}
