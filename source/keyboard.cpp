#include "keyboard.h"

namespace textengine {

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
  }

}  // namespace textengine
