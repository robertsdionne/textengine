#include <chrono>
#include <cmath>
#include <map>

#include "checks.h"
#include "joystick.h"

namespace textengine {

  std::map<Joystick::Axis, std::string> Joystick::axis_names{
    BUILD_MAP_ENTRY(Joystick::Axis::kLeftX),
    BUILD_MAP_ENTRY(Joystick::Axis::kLeftY),
    BUILD_MAP_ENTRY(Joystick::Axis::kRightX),
    BUILD_MAP_ENTRY(Joystick::Axis::kRightY)
  };

  std::map<Joystick::Button, std::string> Joystick::button_names{
    BUILD_MAP_ENTRY(Joystick::Button::kSelect),
    BUILD_MAP_ENTRY(Joystick::Button::kL3),
    BUILD_MAP_ENTRY(Joystick::Button::kR3),
    BUILD_MAP_ENTRY(Joystick::Button::kStart),
    BUILD_MAP_ENTRY(Joystick::Button::kDpadUp),
    BUILD_MAP_ENTRY(Joystick::Button::kDpadRight),
    BUILD_MAP_ENTRY(Joystick::Button::kDpadDown),
    BUILD_MAP_ENTRY(Joystick::Button::kDpadLeft),
    BUILD_MAP_ENTRY(Joystick::Button::kL2),
    BUILD_MAP_ENTRY(Joystick::Button::kR2),
    BUILD_MAP_ENTRY(Joystick::Button::kL1),
    BUILD_MAP_ENTRY(Joystick::Button::kR1),
    BUILD_MAP_ENTRY(Joystick::Button::kTriangle),
    BUILD_MAP_ENTRY(Joystick::Button::kCircle),
    BUILD_MAP_ENTRY(Joystick::Button::kX),
    BUILD_MAP_ENTRY(Joystick::Button::kSquare),
    BUILD_MAP_ENTRY(Joystick::Button::kPs)
  };

  Joystick::Joystick(int joystick_id)
  : joystick_id(joystick_id), axes(), previous_axes(), buttons(), previous_buttons(),
  last_update_time(), dt() {}

  float Joystick::GetAxis(Axis axis) {
    return std::abs(axes[axis]) > kDeadZone ? axes[axis] : 0.0f;
  }

  float Joystick::GetAxisVelocity(Axis axis) {
    return (axes[axis] - previous_axes[axis]) * dt;
  }

  int Joystick::GetButtonVelocity(Button button) {
    return buttons[button] - previous_buttons[button];
  }

  bool Joystick::IsButtonDown(Button button) {
    return buttons[button];
  }

  void Joystick::Update() {
    previous_axes = axes;
    previous_buttons = buttons;
    if (glfwJoystickPresent(joystick_id)) {
      int axis_count = 0;
      const float *axis_data = glfwGetJoystickAxes(joystick_id, &axis_count);
      CHECK_STATE(axis_count);
      CHECK_STATE(axis_data);
      for (auto i = 0; i < axis_count; ++i) {
        axes[static_cast<Axis>(i)] = axis_data[i];
      }
      int button_count = 0;
      const unsigned char *button_data = glfwGetJoystickButtons(joystick_id, &button_count);
      CHECK_STATE(button_count);
      CHECK_STATE(button_data);
      for (auto i = static_cast<int>(Button::kBegin); i < static_cast<int>(Button::kEnd); ++i) {
        buttons[static_cast<Button>(i)] = button_data[i];
      }
    }
    auto now = std::chrono::high_resolution_clock::now();
    dt = std::chrono::duration_cast<std::chrono::duration<float>>(now - last_update_time).count();
    last_update_time = now;
  }

}  // namespace textengine
