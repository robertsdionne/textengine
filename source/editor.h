#ifndef __textengine__editor__
#define __textengine__editor__

#include <glm/glm.hpp>

#include "controller.h"
#include "gamestate.h"
#include "scene.h"

namespace textengine {

  class Keyboard;
  class Mouse;

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
    int width, height;
    GameState &current_state;
    Keyboard &keyboard;
    Mouse &mouse;
    Scene &scene;
    glm::mat4 model_view_projection;
    glm::vec2 start, stop, delta;
    AxisAlignedBoundingBox aabb;
    bool moving, naming, ready, placing;
  };

}  // namespace textengine

#endif /* defined(__textengine__editor__) */
