#include <GLFW/glfw3.h>
#include <iostream>

#include "checks.h"
#include "glfwapplication.h"
#include "keyboard.h"
#include "renderer.h"

namespace textengine {

  GlfwApplication *GlfwApplication::glfw_application = nullptr;

  GlfwApplication::GlfwApplication(int argument_count, char *arguments[], int width, int height,
                                   const std::string &title, Renderer &renderer, Keyboard &keyboard)
  : argument_count(argument_count), arguments(arguments), width(width), height(height),
    title(title), renderer(renderer), keyboard(keyboard) {
    glfw_application = this;
  }

  GlfwApplication::~GlfwApplication() {
    glfw_application = nullptr;
  }

  void GlfwApplication::HandleKeyboard(GLFWwindow *window, int key,
                                       int scancode, int action, int mods) {
    if (glfw_application) {
      switch (action) {
        case GLFW_PRESS:
        case GLFW_REPEAT: {
          glfw_application->keyboard.OnKeyDown(key);
          if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(glfw_application->window, true);
          }
          break;
        }
        case GLFW_RELEASE: {
          glfw_application->keyboard.OnKeyUp(key);
          break;
        }
      }
    }
  }

  void GlfwApplication::HandleReshape(GLFWwindow *window, int width, int height) {
    if (glfw_application) {
      glfw_application->renderer.Change(width, height);
    }
  }

  void GlfwApplication::Run() {
    CHECK_STATE(glfwInit() != -1);
    monitor = glfwGetPrimaryMonitor();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    CHECK_STATE(window != nullptr);
    glfwSetKeyCallback(window, HandleKeyboard);
    glfwSetWindowSizeCallback(window, HandleReshape);
    glfwMakeContextCurrent(window);
    renderer.Create();
    HandleReshape(window, width, height);
    while (!glfwWindowShouldClose(window)) {
      renderer.Render();
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
    glfwTerminate();
  }

}  // namespace empathy