#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <simplexnoise1234.h>

#include "mover.h"

namespace textengine {

  glm::vec3 Mover::GetPosition() {
    return position;
  }

  void Mover::Update() {
    velocity.x = SimplexNoise1234::noise(position.x + glfwGetTime());
    velocity.z = SimplexNoise1234::noise(position.z + 3.0 * glfwGetTime());
    position += velocity * 1.0f / 60.0f;
  }

}  // namespace textengine
