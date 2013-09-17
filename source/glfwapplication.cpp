#include <GLFW/glfw3.h>
#include <iostream>

#include "checks.h"
#include "glfwapplication.h"
#include "renderer.h"

namespace textengine {

  GlfwApplication *GlfwApplication::glfw_application = nullptr;

  GlfwApplication::GlfwApplication(int argument_count, char *arguments[], int width, int height,
                                   const std::string &title, Renderer *renderer)
  : argument_count(argument_count), arguments(arguments), width(width), height(height),
    title(title), renderer(renderer) {
    glfw_application = this;
  }

  GlfwApplication::~GlfwApplication() {
    glfw_application = nullptr;
  }

  void GlfwApplication::Display() {
    if (glfw_application && glfw_application->renderer) {
      glfw_application->renderer->Render();
    }
  }

  void GlfwApplication::Keyboard(GLFWwindow *window, int key, int scancode, int action, int mods) {
  }

  void GlfwApplication::Reshape(GLFWwindow *window, int width, int height) {
    if (glfw_application && glfw_application->renderer) {
      glfw_application->renderer->Change(width, height);
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
    glfwSetKeyCallback(window, Keyboard);
    glfwSetWindowSizeCallback(window, Reshape);
    glfwMakeContextCurrent(window);
    if (renderer) {
      renderer->Create();
    }
    Reshape(window, width, height);
    while (!glfwWindowShouldClose(window)) {
      Display();
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }

}  // namespace empathy
