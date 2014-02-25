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
      kRightY = 3,
      kLeftTrigger = 4,
      kRightTrigger = 5
    };
    static std::map<Axis, std::string> axis_names;
    
    enum class Button {
      kSquare = 0,
      kX = 1,
      kCircle = 2,
      kTriangle = 3,
      kL1 = 4,
      kR1 = 5,
      kL2 = 6,
      kR2 = 7,
      kSelect = 8,
      kStart = 9,
      kL3 = 10,
      kR3 = 11,
      kPs = 12,
      kClick = 13,
      kDpadUp = 14,
      kDpadRight = 15,
      kDpadDown = 16,
      kDpadLeft = 17,
      kEnd,
      kBegin = kSquare
    };
    static std::map<Button, std::string> button_names;

    static constexpr float kDeadZone = 0.1f;

    Joystick(int joystick_id);

    virtual ~Joystick() = default;

    float GetAxis(Axis axis);

    float GetAxisVelocity(Axis axis);

    int GetButtonVelocity(Button button);

    bool IsButtonDown(Button button);

    void Update();

  private:
    int joystick_id;
    std::map<Axis, float> axes, previous_axes;
    std::map<Button, bool> buttons, previous_buttons;
    std::chrono::high_resolution_clock::time_point last_update_time;
    float dt;
  };

}  // namespace textengine

#endif /* defined(__textengine__joystick__) */
