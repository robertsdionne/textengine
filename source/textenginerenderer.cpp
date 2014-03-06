#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imguiRenderGL3.h>
#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <memory>

#include "checks.h"
#include "controller.h"
#include "gamestate.h"
#include "mouse.h"
#include "textenginerenderer.h"

namespace textengine {

  TextEngineRenderer::TextEngineRenderer(Mouse &mouse, Controller &updater, Scene &scene, bool edit)
  : mouse(mouse), updater(updater), scene(scene), edit(edit), model_view(glm::mat4()),
    projection(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f)), matrix_stack{glm::mat4(1)} {}

  void TextEngineRenderer::Change(int width, int height) {
    this->width = width;
    this->height = height;
    glViewport(0, 0, width, height);
    inverse_aspect_ratio = static_cast<float>(height) / static_cast<float>(width);
    projection = glm::ortho(-1.0f, 1.0f, -inverse_aspect_ratio, inverse_aspect_ratio, -1.0f, 1.0f);
    updater.SetModelViewProjection(projection);
  }

  void TextEngineRenderer::Create() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    vertex_shader.Create(GL_VERTEX_SHADER, {kVertexShaderSource});
    fragment_shader.Create(GL_FRAGMENT_SHADER, {kFragmentShaderSource});

    face_program.Create({&vertex_shader, &fragment_shader});
    face_program.CompileAndLink();

    vertex_format.Create({
      {u8"vertex_position", GL_FLOAT, 2}
    });

    unit_circle.data.insert(unit_circle.data.cend(), {
      0.0f, 0.0f
    });
    for (auto i = 0.0f; i < 101.0f; ++i) {
      const auto theta = 2.0f * i / 100.0f * M_PI;
      const auto point = glm::vec2(glm::cos(theta), glm::sin(theta));
      unit_circle.data.insert(unit_circle.data.cend(), {
        point.x, point.y
      });
    }
    unit_circle.element_count = 102;
    unit_circle.element_type = GL_TRIANGLE_FAN;

    unit_square.data.insert(unit_square.data.cend(), {
      0.5f, -0.5f,
      0.5f, 0.5f,
      -0.5f, -0.5f,
      -0.5f, 0.5f
    });
    unit_square.element_count = 4;
    unit_square.element_type = GL_TRIANGLE_STRIP;

    circle_buffer.Create(GL_ARRAY_BUFFER);
    circle_buffer.Data(unit_circle.data_size(), unit_circle.data.data(), GL_STATIC_DRAW);
    circle_array.Create();
    vertex_format.Apply(circle_array, face_program);
    CHECK_STATE(!glGetError());

    rectangle_buffer.Create(GL_ARRAY_BUFFER);
    rectangle_buffer.Data(unit_square.data_size(), unit_square.data.data(), GL_STATIC_DRAW);
    rectangle_array.Create();
    vertex_format.Apply(rectangle_array, face_program);
    CHECK_STATE(!glGetError());

    stroke_width = 0.0025f;
    stroke = glm::vec4(0, 0, 0, 1);
    fill = glm::vec4(0.5, 0.5, 0.5, 1);

    lines.element_type = GL_LINES;

    CHECK_STATE(imguiRenderGLInit("../resource/fonts/ubuntu-font-family-0.80/Ubuntu-R.ttf"));
  }

  void TextEngineRenderer::Render() {
    GameState &current_state = updater.GetCurrentState();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    PushMatrix();
    matrix_stack.back() *= glm::scale(glm::mat4(1), glm::vec3(glm::vec2(current_state.zoom * 0.1f), 1.0f));
    matrix_stack.back() *= glm::translate(glm::mat4(1), glm::vec3(-current_state.camera_position, 0));

    const glm::vec2 position = glm::vec2(current_state.player_body->GetPosition().x,
                                         current_state.player_body->GetPosition().y);

    fill = glm::vec4(0.0f, 0.5f, 0.3f, 0.5f);
    for (const auto &area : scene.areas) {
      if (area->invisible) {
        continue;
      }
      if (Shape::kAxisAlignedBoundingBox == area->shape) {
        DrawAxisAlignedBoundingBox(area->aabb);
      } else {
        DrawCircle(area->aabb.center(), area->aabb.radius());
      }
    }
    fill = glm::vec4(1.0f, 0.0f, 0.0f, 0.5f);
    for (const auto &object : scene.objects) {
      if (object->invisible) {
        continue;
      }
      if (Shape::kAxisAlignedBoundingBox == object->shape) {
        DrawAxisAlignedBoundingBox(object->aabb);
      } else {
        DrawCircle(object->aabb.center(), object->aabb.radius());
      }
    }


    const glm::mat4 normalized_to_reversed = glm::scale(glm::mat4(), glm::vec3(1.0f, -1.0f, 1.0f));
    const glm::mat4 reversed_to_offset = glm::translate(glm::mat4(), glm::vec3(glm::vec2(1.0f), 0.0f));
    const glm::mat4 offset_to_screen = glm::scale(glm::mat4(), glm::vec3(glm::vec2(0.5f), 1.0f));
    const glm::mat4 screen_to_window = glm::scale(glm::mat4(), glm::vec3(width, height, 1.0f));

    const glm::mat4 transform = (screen_to_window * offset_to_screen *
                                 reversed_to_offset * normalized_to_reversed *
                                 model_view * projection * matrix_stack.back());
    const glm::vec4 homogeneous = transform * glm::vec4(position, 0.0f, 1.0f);
    const glm::vec2 transformed = homogeneous.xy() / homogeneous.w;

    fill = glm::vec4(0.5f, 0.5f, 0.6f, 1.0f);
    PushMatrix();
    matrix_stack.back() *= glm::translate(glm::mat4(1), glm::vec3(position, 0));
    matrix_stack.back() *= glm::rotate(glm::mat4(1),
                                       current_state.player_body->GetAngle(),
                                       glm::vec3(0, 0, 1));
    DrawRectangle(glm::vec2(0), glm::vec2(0.25f, 0.5f));
    PopMatrix();
    PopMatrix();

    const auto mouse_position = 2.0f * mouse.get_cursor_position();
    unsigned char mouse_buttons = 0;
    if (mouse.IsButtonDown(GLFW_MOUSE_BUTTON_1)) {
      mouse_buttons |= IMGUI_MBUT_LEFT;
    }
    if (mouse.IsButtonDown(GLFW_MOUSE_BUTTON_2)) {
      mouse_buttons |= IMGUI_MBUT_RIGHT;
    }
    
    if (edit) {
      imguiBeginFrame(mouse_position.x, height - mouse_position.y, mouse_buttons, 0);
      
      for (auto &area : scene.areas) {
        const glm::vec4 homogeneous = transform * glm::vec4(area->aabb.minimum.x, area->aabb.maximum.y, 0.0f, 1.0f);
        const glm::vec2 transformed = homogeneous.xy() / homogeneous.w;
        imguiDrawText(transformed.x, height - transformed.y, IMGUI_ALIGN_LEFT, area->name.c_str(), imguiRGBA(0, 0, 0));
      }
      
      for (auto &object : scene.objects) {
        const glm::vec4 homogeneous = transform * glm::vec4(object->aabb.minimum.x, object->aabb.maximum.y, 0.0f, 1.0f);
        const glm::vec2 transformed = homogeneous.xy() / homogeneous.w;
        imguiDrawText(transformed.x, height - transformed.y, IMGUI_ALIGN_LEFT, object->name.c_str(), imguiRGBA(0, 0, 0));
      }
      
      imguiEndFrame();
    }

    imguiRenderGLDraw(width, height);
  }

  void TextEngineRenderer::DrawAxisAlignedBoundingBox(AxisAlignedBoundingBox aabb) {
    DrawRectangle(aabb.center(), aabb.extent());
  }

  void TextEngineRenderer::DrawCircle(glm::vec2 center, float radius) {
    PushMatrix();
    matrix_stack.back() *= glm::scale(glm::translate(glm::mat4(1), glm::vec3(center, 0.0f)),
                                      glm::vec3(glm::vec2(radius), 1));

    face_program.Use();
    face_program.Uniforms({
      {u8"projection", &projection},
      {u8"model_view", &matrix_stack.back()}
    });
    face_program.Uniforms({
      {u8"color", fill}
    });
    circle_array.Bind();
    glDrawArrays(unit_circle.element_type, 0, unit_circle.element_count);
    CHECK_STATE(!glGetError());

    PopMatrix();
  }

  void TextEngineRenderer::DrawRectangle(glm::vec2 center, glm::vec2 dimensions) {
    PushMatrix();
    matrix_stack.back() *= glm::scale(glm::translate(glm::mat4(1), glm::vec3(center, 0.0f)),
                                      glm::vec3(dimensions, 1));

    face_program.Use();
    face_program.Uniforms({
      {u8"projection", &projection},
      {u8"model_view", &matrix_stack.back()}
    });
    face_program.Uniforms({
      {u8"color", fill}
    });
    rectangle_array.Bind();
    glDrawArrays(unit_square.element_type, 0, unit_square.element_count);
    CHECK_STATE(!glGetError());

    PopMatrix();
  }

  void TextEngineRenderer::PopMatrix() {
    if (matrix_stack.size() > 1) {
      matrix_stack.pop_back();
    }
  }

  void TextEngineRenderer::PushMatrix() {
    matrix_stack.push_back(matrix_stack.back());
  }

}  // namespace textengine
