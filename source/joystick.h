#ifndef __textengine__joystick__
#define __textengine__joystick__

#include <GLFW/glfw3.h>
#include <chrono>
#include <map>

namespace textengine {

#define BUILD_MAP_ENTRY(value) {value, #value}

  class Joystick {
  public:
    enum class Axis {
      kLeftX = 0,
      kLeftY = 1,
      kRightX = 2,
      kRightY = 3
    };
    static std::map<Axis, std::string> axis_names;

    enum class Button {
      kSelect = 0,
      kL3 = 1,
      kR3 = 2,
      kStart = 3,
      kDpadUp = 4,
      kDpadRight = 5,
      kDpadDown = 6,
      kDpadLeft = 7,
      kL2 = 8,
      kR2 = 9,
      kL1 = 10,
      kR1 = 11,
      kTriangle = 12,
      kCircle = 13,
      kX = 14,
      kSquare = 15,
      kPs = 16,
      kEnd,
      kBegin = kSelect
    };
    static std::map<Button, std::string> button_names;

    enum class PressureButton {
      kDpadUp = 4 + 21,
      kDpadRight = 5 + 21,
      kDpadDown = 6 + 21,
      kDpadLeft = 7 + 21,
      kL2 = 8 + 21,
      kR2 = 9 + 21,
      kL1 = 10 + 21,
      kR1 = 11 + 21,
      kTriangle = 12 + 21,
      kCircle = 13 + 21,
      kX = 14 + 21,
      kSquare = 15 + 21,
      kEnd,
      kBegin = kDpadUp
    };
    static std::map<PressureButton, std::string> pressure_button_names;

    static constexpr float kDeadZone = 0.1f;

    Joystick(int joystick_id);

    virtual ~Joystick() = default;

    float GetAxis(Axis axis);

    float GetAxisVelocity(Axis axis);

    float GetButtonPressure(PressureButton pressure_button);

    float GetButtonPressureVelocity(PressureButton pressure_button);

    int GetButtonVelocity(Button button);

    bool IsButtonDown(Button button);

    void Update();

  private:
    int joystick_id;
    std::map<Axis, float> axes, previous_axes;
    std::map<Button, bool> buttons, previous_buttons;
    std::map<PressureButton, float> pressure_buttons, previous_pressure_buttons;
    std::chrono::high_resolution_clock::time_point last_update_time;
    float dt;
  };

}  // namespace textengine

#endif /* defined(__textengine__joystick__) */
