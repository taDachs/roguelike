#include <rpg/actor.h>

using namespace rpg;

void Visualizer::draw(SDL_Renderer* renderer, State state, const glm::vec3& pos) {
  m_sprites[state]->draw(renderer, pos.x, pos.y);
}

void Actor::update() {
}
