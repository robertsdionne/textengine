#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <limits>

#include "input.h"
#include "joystick.h"
#include "keyboard.h"
#include "mouse.h"

namespace textengine {

  constexpr float Input::kSmoothRate;

  Input::Input(Joystick &joystick, Keyboard &keyboard, Mouse &mouse)
  : joystick(joystick), keyboard(keyboard), mouse(mouse), looking() {}

  glm::vec2 Input::GetPrimaryAxes() const {
    const auto result = ArgMax({
      joystick_primary_axes,
      keyboard_primary_smoothed_axes
    });
    return glm::length(result) > Joystick::kDeadZone ? result : glm::vec2();
  }

  glm::vec2 Input::GetSecondaryAxes() const {
    const auto result = ArgMax({
      joystick_secondary_axes,
      keyboard_secondary_smoothed_axes
    });
    return glm::length(result) > Joystick::kDeadZone ? result : glm::vec2();
  }

  float Input::GetXButton() const {
    const auto result = ArgMax({
      joystick.IsButtonDown(Joystick::Button::kX),
    });
    return glm::abs(result) > Joystick::kDeadZone ? result : 0.0f;
  }

  float Input::GetLookVelocity() const {
    return looking - previous_looking;
  }

  float Input::GetTriggerPressure() const {
    const auto result = ArgMax({
      joystick.IsButtonDown(Joystick::Button::kR1),
      keyboard.IsKeyDown(GLFW_KEY_LEFT_SHIFT)
    });
    return result;
  }

  float Input::GetTriggerVelocity() const {
    const auto result = ArgMax({
      joystick.GetButtonVelocity(Joystick::Button::kR1),
      keyboard.GetKeyVelocity(GLFW_KEY_LEFT_SHIFT)
    });
    return result;
  }

  void Input::Update() {
    previous_looking = looking;
    joystick_primary_axes = glm::vec2(joystick.GetAxis(Joystick::Axis::kLeftX),
                                      joystick.GetAxis(Joystick::Axis::kLeftY));
    joystick_secondary_axes = glm::vec2(joystick.GetAxis(Joystick::Axis::kRightX),
                                        joystick.GetAxis(Joystick::Axis::kRightY));
    auto keyboard_primary_axes = (glm::vec2(keyboard.IsKeyDown(GLFW_KEY_D),
                                                  keyboard.IsKeyDown(GLFW_KEY_W)) -
                                        glm::vec2(keyboard.IsKeyDown(GLFW_KEY_A),
                                                  keyboard.IsKeyDown(GLFW_KEY_S)));
    keyboard_primary_axes = (glm::length(keyboard_primary_axes) > 0.0f ?
                             glm::normalize(keyboard_primary_axes) : glm::vec2());
    keyboard_primary_smoothed_axes = glm::mix(keyboard_primary_smoothed_axes,
                                              keyboard_primary_axes, kSmoothRate);
    auto keyboard_secondary_axes = (glm::vec2(keyboard.IsKeyDown(GLFW_KEY_RIGHT),
                                                    keyboard.IsKeyDown(GLFW_KEY_UP)) -
                                          glm::vec2(keyboard.IsKeyDown(GLFW_KEY_LEFT),
                                                    keyboard.IsKeyDown(GLFW_KEY_DOWN)));
    keyboard_secondary_axes = (glm::length(keyboard_secondary_axes) > 0.0f ?
                             glm::normalize(keyboard_secondary_axes) : glm::vec2());
    keyboard_secondary_smoothed_axes = glm::mix(keyboard_secondary_smoothed_axes,
                                                keyboard_secondary_axes, kSmoothRate);
    const auto mouse_primary_axes = (glm::length(mouse.GetCursorVelocity()) > 0.0f ?
                                     glm::normalize(mouse.GetCursorVelocity() * glm::vec2(1, -1)) : glm::vec2());
    mouse_primary_smoothed_axes = glm::mix(mouse_primary_smoothed_axes,
                                           mouse_primary_axes, kSmoothRate);
    looking = glm::length(joystick_secondary_axes) > Joystick::kDeadZone;
    looking |= glm::length(keyboard_secondary_smoothed_axes) > Joystick::kDeadZone;
  }

  glm::vec2 Input::ArgMax(std::initializer_list<glm::vec2> &&vectors) {
    auto argmax = glm::vec2();
    float maximum = -std::numeric_limits<float>::infinity();
    for (auto vector : vectors) {
      const auto length = glm::length(vector);
      if (maximum < length) {
        maximum = length;
        argmax = vector;
      }
    }
    return argmax;
  }

  float Input::ArgMax(std::initializer_list<float> &&values) {
    auto argmax = 0.0f;
    float maximum = -std::numeric_limits<float>::infinity();
    for (auto value : values) {
      const auto length = std::abs(value);
      if (maximum < length) {
        maximum = length;
        argmax = value;
      }
    }
    return argmax;
  }

}  // namespace textengine
