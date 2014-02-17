#ifndef TEXTENGINE_EDITOR_H_
#define TEXTENGINE_EDITOR_H_

#include <glm/glm.hpp>

#include "controller.h"
#include "gamestate.h"


namespace textengine {

  class Keyboard;
  class Mouse;
  class Object;
  class Scene;

  class Editor : public Controller {
  public:
    Editor(int width, int height,
           GameState &initial_state, Keyboard &keyboard, Mouse &mouse, Scene &scene);

    virtual ~Editor() = default;

    virtual GameState &GetCurrentState();
    
    glm::vec2 GetCursorPosition() const;
    
    virtual void SetModelViewProjection(glm::mat4 model_view_projection);

    virtual void Setup();

    virtual void Update();

  private:
    void MakeDefaultMessageList(Object *object, const std::vector<std::string> &&keys);
    
    int width, height;
    GameState &current_state;
    Keyboard &keyboard;
    Mouse &mouse;
    Scene &scene;
    glm::mat4 model_view_projection;
    Object *last_item, *last_area, *last_object;
    glm::vec2 start, stop;
    bool selecting;
  };

}  // namespace textengine

#endif  // TEXTENGINE_EDITOR_H_
