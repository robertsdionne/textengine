#include "keyboard.h"

namespace textengine {

  float Keyboard::GetKeyVelocity(int key) {
    return (keys[key] - previous_keys[key]) * dt;
  }

  bool Keyboard::IsKeyDown(const int key) {
    return keys[key];
  }

  void Keyboard::OnKeyDown(const int key) {
    keys[key] = true;
  }

  void Keyboard::OnKeyUp(const int key) {
    keys[key] = false;
  }

  void Keyboard::Update() {
    previous_keys = keys;
    auto now = std::chrono::high_resolution_clock::now();
    dt = std::chrono::duration_cast<std::chrono::duration<float>>(now - last_update_time).count();
    last_update_time = now;
  }

}  // namespace textengine
