#ifndef __textengine__input__
#define __textengine__input__

#include <functional>
#include <glm/glm.hpp>
#include <initializer_list>

namespace textengine {

  class Joystick;
  class Keyboard;
  class Mouse;

  class Input {
  public:
    Input(Joystick &joystick, Keyboard &keyboard, Mouse &mouse);

    virtual ~Input() = default;

    glm::vec2 GetPrimaryAxes() const;

    glm::vec2 GetSecondaryAxes() const;

    float GetXButton() const;

    float GetTriggerVelocity() const;

    void Update();

  private:
    static constexpr auto kSmoothRate = 0.25f;

    static glm::vec2 ArgMax(std::initializer_list<glm::vec2> &&vectors);

    static float ArgMax(std::initializer_list<float> &&values);

    Joystick &joystick;
    Keyboard &keyboard;
    Mouse &mouse;

    glm::vec2 joystick_primary_axes, joystick_secondary_axes;
    glm::vec2 keyboard_primary_smoothed_axes, keyboard_secondary_smoothed_axes;
    glm::vec2 mouse_primary_smoothed_axes;
  };

}  // namespace textengine

#endif /* defined(__textengine__input__) */
