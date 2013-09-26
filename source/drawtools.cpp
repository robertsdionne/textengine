#include <cmath>
#include <glm/glm.hpp>
#include <memory>

#include "drawable.h"
#include "drawtools.h"

namespace textengine {

  namespace drawtools {

    void DrawCircle(glm::vec2 position, float size, glm::vec4 color) {
      MaybeAllocateCircleDrawable();
//      circle->Use();
//      glUniform2f(circle->GetUniformLocation(u8"shape_position"), position.x, position.y);
//      glUniform2f(circle->GetUniformLocation(u8"shape_size"), size, size);
//      glUniform4f(circle->GetUniformLocation(u8"shape_color"), color.r, color.g, color.b, color.a);
//      circle->Draw();
    }

    void DrawSquare(glm::vec2 position, glm::vec2 size, glm::vec4 color) {
      MaybeAllocateSquareDrawable();
//      square->Use();
//      glUniform2f(circle->GetUniformLocation(u8"shape_position"), position.x, position.y);
//      glUniform2f(circle->GetUniformLocation(u8"shape_size"), size.x, size.y);
//      glUniform4f(circle->GetUniformLocation(u8"shape_color"), color.r, color.g, color.b, color.a);
//      square->Draw();
    }

    void DrawTriangle(glm::vec2 position, float size, glm::vec4 color) {
      MaybeAllocateTriangleDrawable();
//      triangle->Use();
//      glUniform2f(circle->GetUniformLocation(u8"shape_position"), position.x, position.y);
//      glUniform2f(circle->GetUniformLocation(u8"shape_size"), size, size);
//      glUniform4f(circle->GetUniformLocation(u8"shape_color"), color.r, color.g, color.b, color.a);
//      triangle->Draw();
    }

    void MaybeAllocateCircleDrawable() {
      if (!circle) {
        float circle_data[kCircleResolution * 6];
        for (int i = 0; i < kCircleResolution; ++i) {
          const float theta_0 = 2.0 * M_PI * i / kCircleResolution;
          const float theta_1 = 2.0 * M_PI * (i + 1) / kCircleResolution;
          circle_data[i] = cosf(theta_0);
          circle_data[i + 1] = sinf(theta_0);
          circle_data[i + 2] = cosf(theta_1);
          circle_data[i + 3] = sinf(theta_1);
          circle_data[i + 4] = 0.0f;
          circle_data[i + 5] = 0.0f;
        }
        circle = std::unique_ptr<Drawable>(new Drawable());
//        circle->Create(vertex_shader_source, fragment_shader_source,
//                       circle_data, sizeof(float) * kCircleResolution * 6, kCircleResolution);
      }
    }

    void MaybeAllocateSquareDrawable() {
      if (!square) {
        float square_data[] = {
          0.5, 0.5,
          -0.5, 0.5,
          -0.5, -0.5,
          0.5, 0.5,
          -0.5, -0.5,
          0.5, -0.5
        };
        square = std::unique_ptr<Drawable>(new Drawable());
//        square->Create(vertex_shader_source, fragment_shader_source,
//                       square_data, sizeof(float) * 6, 2);
      }
    }

    void MaybeAllocateTriangleDrawable() {
      if (!triangle) {
        float triangle_data[] = {
          0.0, 0.5,
          -0.5, -0.5,
          0.5, -0.5
        };
        triangle = std::unique_ptr<Drawable>(new Drawable());
//        triangle->Create(vertex_shader_source, fragment_shader_source,
//                         triangle_data, sizeof(float) * 3, 1);
      }
    }

  }  // namespace drawtools

}  // namespace textengine
