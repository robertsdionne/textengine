#ifndef TEXTENGINE_DRAWTOOLS_H_
#define TEXTENGINE_DRAWTOOLS_H_

#include <glm/glm.hpp>
#include <memory>

#include "drawable.h"

namespace textengine {

  namespace drawtools {

    static constexpr const char *vertex_shader_source = u8R"glsl(#version 150 core
    uniform vec2 shape_position;
    uniform vec2 shape_size;

    in vec2 vertex_position;

    void main() {
      gl_Position = shape_position + vertex_position * shape_size;
    }
    )glsl";

    static constexpr const char *fragment_shader_source = u8R"glsl(#version 150 core
    uniform vec4 shape_color;

    out vec4 color;

    void main() {
      color = shape_color;
    }
    )glsl";

    static constexpr int kCircleResolution = 100;

    static std::unique_ptr<Drawable> circle, square, triangle;

    void DrawCircle(glm::vec2 position, float size, glm::vec4 color);

    void DrawSquare(glm::vec2 position, glm::vec2 size, glm::vec4 color);

    void DrawTriangle(glm::vec2 position, float size, glm::vec4 color);

    static void MaybeAllocateCircleDrawable();

    static void MaybeAllocateSquareDrawable();

    static void MaybeAllocateTriangleDrawable();

  }  // namespace drawtools

}  // namespace textengine

#endif  // TEXTENGINE_DRAWTOOLS_H_
