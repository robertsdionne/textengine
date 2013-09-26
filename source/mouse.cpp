#include <glm/glm.hpp>
#include <map>

#include "mouse.h"

namespace textengine {

  glm::vec2 Mouse::get_cursor_position() const {
    return cursor_position;
  }

  bool Mouse::HasMouseMoved() const {
    return glm::vec2(0, 0) != cursor_position - previous_cursor_position;
  }
  
  bool Mouse::IsButtonDown(const int button) {
    return buttons[button];
  }

  bool Mouse::IsButtonJustPressed(const int button) {
    return buttons[button] && !previous_buttons[button];
  }

  bool Mouse::IsButtonJustReleased(const int button) {
    return !buttons[button] && previous_buttons[button];
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
