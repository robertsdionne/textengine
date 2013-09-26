#include <glm/glm.hpp>
#include <map>

#include "mouse.h"

namespace textengine {

  glm::vec2 Mouse::get_cursor_position() const {
    return cursor_position;
  }
  
  bool Mouse::IsButtonDown(const int button) {
    return buttons[button];
  }

  void Mouse::OnButtonDown(const int button) {
    buttons[button] = true;
  }

  void Mouse::OnButtonUp(const int button) {
    buttons[button] = false;
  }

  void Mouse::OnCursorMove(const glm::vec2 position) {
    cursor_position = position;
  }

  void Mouse::Update() {
    previous_buttons = buttons;
    previous_cursor_position = cursor_position;
  }

}  // namespace textengine