#include <GLFW/glfw3.h>

#include "checks.h"
#include "drawtools.h"
#include "gamestate.h"
#include "textenginerenderer.h"
#include "updater.h"

namespace textengine {

  TextEngineRenderer::TextEngineRenderer(Updater &updater) : updater(updater) {}

  void TextEngineRenderer::Change(int width, int height) {
    glViewport(0, 0, 2 * width, 2 * height);
  }

  void TextEngineRenderer::Create() {
    glClearColor(0.0, 0.0, 0.0, 1.0);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    constexpr const char *vertex_shader_source = u8R"glsl(#version 150 core
    uniform vec2 shape_position;
    uniform vec2 shape_size;

    in vec2 vertex_position;

    void main() {
      gl_Position = vec4(shape_position + vertex_position * shape_size, 0, 1);
    }
    )glsl";
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    GLint compile_status;
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
    char info_log[4 * 1024];
    if (!compile_status) {
      glGetShaderInfoLog(vertex_shader, sizeof(info_log), nullptr, info_log);
      FAIL(info_log);
    }
    CHECK_STATE(!glGetError());

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    constexpr const char *fragment_shader_source = u8R"glsl(#version 150 core
    uniform vec4 shape_color;

    out vec4 color;

    void main() {
      color = shape_color;
    }
    )glsl";
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
    if (!compile_status) {
      glGetShaderInfoLog(fragment_shader, sizeof(info_log), nullptr, info_log);
      FAIL(info_log);
    }
    CHECK_STATE(!glGetError());

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if (!link_status) {
      glGetProgramInfoLog(program, sizeof(info_log), nullptr, info_log);
      FAIL(info_log);
    }
    CHECK_STATE(!glGetError());

    constexpr int kCircleResolution = 100;
    float circle_data[kCircleResolution * 6];
    for (int i = 0; i < kCircleResolution; ++i) {
      const float theta_0 = 2.0 * M_PI * i / kCircleResolution;
      const float theta_1 = 2.0 * M_PI * (i + 1) / kCircleResolution;
      circle_data[6 * i] = cosf(theta_0);
      circle_data[6 * i + 1] = sinf(theta_0);
      circle_data[6 * i + 2] = cosf(theta_1);
      circle_data[6 * i + 3] = sinf(theta_1);
      circle_data[6 * i + 4] = 0.0f;
      circle_data[6 * i + 5] = 0.0f;
    }

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle_data), circle_data, GL_STATIC_DRAW);
    CHECK_STATE(!glGetError());

    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(glGetAttribLocation(program, u8"vertex_position"),
                          2, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(glGetAttribLocation(program, u8"vertex_position"));
    CHECK_STATE(!glGetError());

    float world_data[] = {
      0.400000, 0.140000,
      0.440000, 0.260000,
      0.440000, 0.140000,
      
      0.600000, 0.400000,
      0.480000, 0.400000,
      0.480000, 0.440000,
      
      0.640000, 0.480000,
      0.640000, 0.600000,
      0.680000, 0.600000,

      0.680000, 0.240000,
      0.640000, 0.360000,
      0.680000, 0.360000,

      0.300000, 0.640000,
      0.180000, 0.640000,
      0.180000, 0.680000,

      0.600000, 0.640000,
      0.480000, 0.640000,
      0.480000, 0.680000,

      0.400000, 0.140000,
      0.400000, 0.260000,
      0.440000, 0.260000,

      0.600000, 0.440000,
      0.600000, 0.400000,
      0.480000, 0.440000,

      0.680000, 0.480000,
      0.640000, 0.480000,
      0.680000, 0.600000,

      0.640000, 0.240000,
      0.640000, 0.360000,
      0.680000, 0.240000,

      0.300000, 0.680000,
      0.300000, 0.640000,
      0.180000, 0.680000,

      0.600000, 0.680000,
      0.600000, 0.640000,
      0.480000, 0.680000,

      0.680000, 0.200000,
      1.000000, 0.640000,
      1.000000, 0.200000,

      0.680000, 0.600000,
      0.680000, 0.640000,
      1.000000, 0.640000,

      0.680000, 0.480000,
      0.680000, 0.600000,
      1.000000, 0.640000,

      0.680000, 0.360000,
      0.680000, 0.480000,
      1.000000, 0.640000,

      0.680000, 0.240000,
      0.680000, 0.360000,
      1.000000, 0.640000,


      0.680000, 0.240000,
      0.680000, 0.200000,
      1.000000, 0.640000,

      -0.000000, 0.300000,
      0.400000, -0.000000,
      -0.000000, 0.000000,

      0.100000, 0.400000,
      0.400000, 0.400000,
      0.400000, 0.260000,

      -0.000000, 0.300000,
      0.100000, 0.400000,
      0.400000, 0.260000,

      0.400000, -0.000000,
      -0.000000, 0.300000,
      0.400000, 0.140000,

      -0.000000, 0.300000,
      0.400000, 0.260000,
      0.400000, 0.140000,

      0.640000, 0.360000,
      0.600000, 0.400000,
      0.640000, 0.400000,

      0.640000, 0.440000,
      0.600000, 0.440000,
      0.640000, 0.480000,

      0.640000, 0.160000,
      0.840000, 0.160000,
      0.840000, -0.000000,

      0.640000, 0.160000,
      0.840000, -0.000000,
      0.440000, 0.000000,

      0.640000, 0.160000,
      0.440000, 0.000000,
      0.440000, 0.140000,

      0.640000, 0.160000,
      0.440000, 0.140000,
      0.440000, 0.260000,

      0.640000, 0.160000,
      0.440000, 0.400000,
      0.440000, 0.260000,

      0.640000, 0.360000,
      0.640000, 0.240000,
      0.600000, 0.400000,

      0.600000, 0.400000,
      0.640000, 0.240000,
      0.480000, 0.400000,

      0.640000, 0.240000,
      0.640000, 0.160000,
      0.480000, 0.400000,

      0.640000, 0.160000,
      0.440000, 0.400000,
      0.480000, 0.400000,

      0.640000, 0.600000,
      0.600000, 0.640000,
      0.640000, 0.640000,

      0.480000, 0.440000,
      0.140000, 0.440000,
      0.140000, 0.640000,

      0.640000, 0.480000,
      0.600000, 0.440000,
      0.640000, 0.600000,

      0.600000, 0.440000,
      0.480000, 0.440000,
      0.640000, 0.600000,

      0.640000, 0.600000,
      0.480000, 0.440000,
      0.600000, 0.640000,

      0.600000, 0.640000,
      0.480000, 0.440000,
      0.480000, 0.640000,

      0.480000, 0.640000,
      0.480000, 0.440000,
      0.300000, 0.640000,

      0.480000, 0.440000,
      0.140000, 0.640000,
      0.180000, 0.640000,

      0.300000, 0.640000,
      0.480000, 0.440000,
      0.180000, 0.640000,

      0.380000, 0.680000,
      0.100000, 1.000000,
      0.380000, 1.000000,

      0.660000, 0.680000,
      0.420000, 1.000000,
      0.660000, 1.000000,

      0.180000, 0.680000,
      0.100000, 0.680000,
      0.100000, 1.000000,

      0.300000, 0.680000,
      0.180000, 0.680000,
      0.100000, 1.000000,

      0.380000, 0.680000,
      0.300000, 0.680000,
      0.100000, 1.000000,

      0.480000, 0.680000,
      0.420000, 0.680000,
      0.420000, 1.000000,

      0.600000, 0.680000,
      0.480000, 0.680000,
      0.420000, 1.000000,

      0.660000, 0.680000,
      0.600000, 0.680000,
      0.420000, 1.000000
    };

    glGenBuffers(1, &world_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, world_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(world_data), world_data, GL_STATIC_DRAW);
    CHECK_STATE(!glGetError());

    glGenVertexArrays(1, &world_vertex_array);
    glBindVertexArray(world_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, world_vertex_buffer);
    glVertexAttribPointer(glGetAttribLocation(program, u8"vertex_position"),
                          2, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(glGetAttribLocation(program, u8"vertex_position"));
    CHECK_STATE(!glGetError());
  }

  void TextEngineRenderer::Render() {    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GameState current_state = updater.GetCurrentState();
    glUseProgram(program);
    glUniform2f(glGetUniformLocation(program, u8"shape_position"), -1, -1);
    glUniform2f(glGetUniformLocation(program, u8"shape_size"), 2, 2);
    glUniform4f(glGetUniformLocation(program, u8"shape_color"), 0.640000, 0.640000, 0.640000, 1);
    glBindVertexArray(world_vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, 12*3);
    CHECK_STATE(!glGetError());
    glUniform4f(glGetUniformLocation(program, u8"shape_color"), 0.640000, 0.320000, 0.320000, 1);
    glDrawArrays(GL_TRIANGLES, 12*3, 6*3);
    CHECK_STATE(!glGetError());
    glUniform4f(glGetUniformLocation(program, u8"shape_color"), 0.320640, 0.320000, 0.640000, 1);
    glDrawArrays(GL_TRIANGLES, (12+6)*3, 5*3);
    CHECK_STATE(!glGetError());
    glUniform4f(glGetUniformLocation(program, u8"shape_color"), 0.640000, 0.640000, 0.320000, 1);
    glDrawArrays(GL_TRIANGLES, (12+6+5)*3, 20*3);
    CHECK_STATE(!glGetError());
    glUniform4f(glGetUniformLocation(program, u8"shape_color"), 0.320000, 0.640000, 0.320000, 1);
    glDrawArrays(GL_TRIANGLES, (12+6+5+20)*3, 8*3);
    CHECK_STATE(!glGetError());
    glUseProgram(program);
    glUniform2f(glGetUniformLocation(program, u8"shape_position"),
                current_state.player_position.x, current_state.player_position.y);
    glUniform2f(glGetUniformLocation(program, u8"shape_size"), 0.05, 0.05);
    glUniform4f(glGetUniformLocation(program, u8"shape_color"), 1, 0, 0, 1);
    glBindVertexArray(vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, 100*3);
    CHECK_STATE(!glGetError());
    glUseProgram(program);
    glUniform2f(glGetUniformLocation(program, u8"shape_position"),
                current_state.player_position.x + current_state.player_direction.x * 0.1,
                current_state.player_position.y + current_state.player_direction.y * 0.1);
    glUniform2f(glGetUniformLocation(program, u8"shape_size"), 0.01, 0.01);
    glUniform4f(glGetUniformLocation(program, u8"shape_color"), 1, 0, 0, 1);
    glBindVertexArray(vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, 100*3);
    CHECK_STATE(!glGetError());

  }

}  // namespace textengine
