#pragma once

#include <glm/glm.hpp>

namespace rpg {
class Camera
{
public:
  glm::vec2 realToScreen(const glm::vec2& real_coord) const { return m_real_to_screen * glm::vec3(real_coord, 1); };
  glm::vec2 screenToReal(const glm::vec2& screen_coord) const { return m_screen_to_real * glm::vec3(screen_coord, 1); };

  void setRealToScreen(const glm::mat3& real_to_screen)
  {
    m_real_to_screen = real_to_screen;
    m_screen_to_real = glm::inverse(real_to_screen);
  }
  glm::mat3 getRealToScreen() const { return m_real_to_screen; }

  void setScreenToReal(const glm::mat3& screen_to_real)
  {
    m_screen_to_real = screen_to_real;
    m_real_to_screen = glm::inverse(screen_to_real);
  }
  glm::mat3 getScreenToReal() const { return m_screen_to_real; }

private:
  glm::mat3 m_real_to_screen;
  glm::mat3 m_screen_to_real;
};
}
