#include <GLFW/glfw3.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <limits>
#include <memory>
#include <unordered_set>

#include "editor.h"
#include "keyboard.h"
#include "mouse.h"
#include "scene.h"

namespace textengine {

  Editor::Editor(int width, int height,
                 GameState &initial_state, Keyboard &keyboard, Mouse &mouse, Scene &scene)
  : width(width), height(height), current_state(initial_state), keyboard(keyboard), mouse(mouse),
  scene(scene), selected_item(), start(), stop(), moving(),
  ready(), placing() {}

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
                                           model_view_projection * glm::scale(glm::mat4(1), glm::vec3(glm::vec2(0.1f), 1.0f)) *
                                           glm::translate(glm::mat4(1), glm::vec3(-current_state.camera_position, 0))) *
                              glm::vec4(mouse.get_cursor_position(), 0.0f, 1.0f));
    const auto transformed = homogeneous.xy() / homogeneous.w;
    return transformed;
  }
  
  void Editor::SetModelViewProjection(glm::mat4 model_view_projection) {
    Editor::model_view_projection = model_view_projection;
  }

  void Editor::Setup() {}

  void Editor::Update() {
    ready = !(moving || placing);
    const auto d = keyboard.IsKeyDown(GLFW_KEY_LEFT_SHIFT) ? 1.0 : 0.25;
    const auto dx = glm::vec2(d, 0);
    const auto dy = glm::vec2(0, d);
    if (keyboard.IsKeyDown(GLFW_KEY_W)) {
      current_state.camera_position += dy;
    }
    if (keyboard.IsKeyDown(GLFW_KEY_S)) {
      current_state.camera_position -= dy;
    }
    if (keyboard.IsKeyDown(GLFW_KEY_D)) {
      current_state.camera_position += dx;
    }
    if (keyboard.IsKeyDown(GLFW_KEY_A)) {
      current_state.camera_position -= dx;
    }
    if (ready && keyboard.GetKeyVelocity(GLFW_KEY_Q) > 0) {
      selected_item = scene.AddArea();
    }
    if (ready && keyboard.GetKeyVelocity(GLFW_KEY_E) > 0) {
      selected_item = scene.AddObject();
    }
    if (selected_item && keyboard.GetKeyVelocity(GLFW_KEY_SPACE) > 0) {
      if (Shape::kAxisAlignedBoundingBox == selected_item->shape) {
        selected_item->shape = Shape::kCircle;
      } else {
        selected_item->shape = Shape::kAxisAlignedBoundingBox;
      }
    }
    if (ready && selected_item && mouse.GetButtonVelocity(GLFW_MOUSE_BUTTON_1) > 0) {
      placing = true;
      start = GetCursorPosition();
    }
    if (ready && mouse.GetButtonVelocity(GLFW_MOUSE_BUTTON_2)) {
      const auto cursor = GetCursorPosition();
      std::unordered_set<Object *> candidates;
      for (auto &area : scene.areas) {
        if (area->Contains(cursor)) {
          candidates.insert(area.get());
        }
      }
      for (auto &object : scene.objects) {
        if (object->Contains(cursor)) {
          candidates.insert(object.get());
        }
      }
      auto minimum = std::numeric_limits<float>::infinity();
      Object *argmin = nullptr;
      for (auto candidate : candidates) {
        const auto area = candidate->area();
        if (area < minimum) {
          minimum = area;
          argmin = candidate;
        }
      }
      if (argmin) {
        selected_item = argmin;
      }
    }
    if (ready && selected_item && keyboard.GetKeyVelocity(GLFW_KEY_BACKSPACE) > 0) {
      scene.EraseItem(selected_item);
      selected_item = nullptr;
    }
    if (ready && selected_item && keyboard.GetKeyVelocity(GLFW_KEY_G) > 0) {
      moving = true;
      aabb = selected_item->aabb;
      delta = aabb.minimum - GetCursorPosition();
    }
    if (moving && selected_item) {
      const auto position = GetCursorPosition();
      selected_item->aabb.minimum = position + delta;
      selected_item->aabb.maximum = position + delta + aabb.extent();
    }
    if (moving && keyboard.GetKeyVelocity(GLFW_KEY_ESCAPE) > 0) {
      moving = false;
      selected_item->aabb = aabb;
    }
    if (moving && mouse.GetButtonVelocity(GLFW_MOUSE_BUTTON_1) > 0) {
      moving = false;
    }
    if (selected_item && keyboard.GetKeyVelocity(GLFW_KEY_V) > 0) {
      selected_item->invisible = !selected_item->invisible;
    }
    if (placing && selected_item) {
      stop = GetCursorPosition();
      selected_item->aabb.minimum = glm::min(start, stop);
      selected_item->aabb.maximum = glm::max(start, stop);
    }
    if (placing && selected_item && mouse.GetButtonVelocity(GLFW_MOUSE_BUTTON_1) < 0) {
      placing = false;
      stop = GetCursorPosition();
      selected_item->aabb.minimum = glm::min(start, stop);
      selected_item->aabb.maximum = glm::max(start, stop);
    }
  }

}  // namespace textengine
