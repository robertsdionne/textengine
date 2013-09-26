#ifndef TEXTENGINE_MOUSE_H_
#define TEXTENGINE_MOUSE_H_

#include <glm/glm.hpp>
#include <map>

namespace textengine {

  class Mouse {
  public:
    Mouse() = default;

    virtual ~Mouse() = default;

    glm::vec2 get_cursor_position() const;

    bool IsButtonDown(int button);

    void OnButtonDown(int button);

    void OnButtonUp(int button);

    void OnCursorMove(glm::vec2 position);

    void Update();

  private:
    glm::vec2 cursor_position, previous_cursor_position;
    std::map<int, bool> buttons, previous_buttons;
  };

}  // namespace textengine

#endif  // TEXTENGINE_MOUSE_H_
