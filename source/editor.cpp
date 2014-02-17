#include <GLFW/glfw3.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <memory>

#include "editor.h"
#include "keyboard.h"
#include "mouse.h"
#include "scene.h"

namespace textengine {

  Editor::Editor(int width, int height,
                 GameState &initial_state, Keyboard &keyboard, Mouse &mouse, Scene &scene)
  : width(width), height(height), current_state(initial_state), keyboard(keyboard), mouse(mouse),
  scene(scene), selected_item(), selected_area(), selected_object(), start(), stop(), selecting() {}

  GameState &Editor::GetCurrentState() {
    return current_state;
  }
  
  glm::vec2 Editor::GetCursorPosition() const {
    const auto normalized_to_reversed = glm::scale(glm::mat4(), glm::vec3(1.0f, -1.0f, 1.0f));
    const auto reversed_to_offset = glm::translate(glm::mat4(), glm::vec3(glm::vec2(1.0f), 0.0f));
    const auto offset_to_screen = glm::scale(glm::mat4(), glm::vec3(glm::vec2(0.5f), 1.0f));
    const auto screen_to_window = glm::scale(glm::mat4(), glm::vec3(width, height, 1.0f));
    const auto homogeneous = (glm::inverse(screen_to_window * offset_to_screen *
                                           reversed_to_offset * normalized_to_reversed *
                                           model_view_projection * glm::scale(glm::mat4(1), glm::vec3(glm::vec2(0.1f), 1.0f))) *
                              glm::vec4(mouse.get_cursor_position(), 0.0f, 1.0f));
    const auto transformed = homogeneous.xy() / homogeneous.w;
    return transformed;
  }
  
  void Editor::SetModelViewProjection(glm::mat4 model_view_projection) {
    Editor::model_view_projection = model_view_projection;
  }

  void Editor::Setup() {}

  void Editor::Update() {
    if (keyboard.GetKeyVelocity(GLFW_KEY_A) > 0) {
      selected_item = selected_area = scene.AddArea();
    }
    if (keyboard.GetKeyVelocity(GLFW_KEY_O) > 0) {
      selected_item = selected_object = scene.AddObject();
    }
    if (selected_item && keyboard.GetKeyVelocity(GLFW_KEY_SPACE) > 0) {
      if (Shape::kAxisAlignedBoundingBox == selected_item->shape) {
        selected_item->shape = Shape::kCircle;
      } else {
        selected_item->shape = Shape::kAxisAlignedBoundingBox;
      }
    }
    if (!selecting && selected_item && mouse.GetButtonVelocity(GLFW_MOUSE_BUTTON_1) > 0) {
      selecting = true;
      start = GetCursorPosition();
    }
    if (selecting && selected_item && mouse.HasCursorMoved()) {
      stop = GetCursorPosition();
      selected_item->aabb.minimum = glm::min(start, stop);
      selected_item->aabb.maximum = glm::max(start, stop);
    }
    if (selecting && selected_item && mouse.GetButtonVelocity(GLFW_MOUSE_BUTTON_1) < 0) {
      selecting = false;
      stop = GetCursorPosition();
      selected_item->aabb.minimum = glm::min(start, stop);
      selected_item->aabb.maximum = glm::max(start, stop);
    }
  }

}  // namespace textengine
