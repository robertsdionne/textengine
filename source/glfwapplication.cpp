#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

#include "checks.h"
#include "controller.h"
#include "glfwapplication.h"
#include "input.h"
#include "joystick.h"
#include "keyboard.h"
#include "mouse.h"
#include "renderer.h"

namespace textengine {

  GlfwApplication *GlfwApplication::instance = nullptr;

  GlfwApplication::GlfwApplication(int argument_count, char *arguments[], int width, int height,
                                   const std::string &title, Controller &controller,
                                   Renderer &renderer, Input &input, Joystick &joystick,
                                   Keyboard &keyboard, Mouse &mouse)
  : window(nullptr), argument_count(argument_count), arguments(arguments), width(width),
  height(height), title(title), controller(controller), renderer(renderer), input(input),
  joystick(joystick), keyboard(keyboard), mouse(mouse), minimized(false) {
    instance = this;
  }

  GlfwApplication::~GlfwApplication() {
    instance = nullptr;
  }

  void GlfwApplication::HandleKeyboard(GLFWwindow *window, int key,
                                       int scancode, int action, int mods) {
    if (instance) {
      switch (action) {
        case GLFW_PRESS:
        case GLFW_REPEAT: {
          instance->keyboard.OnKeyDown(key);
          break;
        }
        case GLFW_RELEASE: {
          instance->keyboard.OnKeyUp(key);
          break;
        }
      }
    }
  }

  void GlfwApplication::HandleMouseButton(GLFWwindow *window, int button, int action, int mods) {
    if (instance) {
      switch (action) {
        case GLFW_PRESS: {
          instance->mouse.OnButtonDown(button);
          break;
        }
        case GLFW_RELEASE: {
          instance->mouse.OnButtonUp(button);
          break;
        }
      }
    }
  }

  void GlfwApplication::HandleReshape(GLFWwindow *window, int width, int height) {
    if (instance) {
      instance->renderer.Change(width, height);
    }
  }

  int GlfwApplication::Run() {
    CHECK_STATE(glfwInit() != -1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    CHECK_STATE(window != nullptr);
    glfwSetKeyCallback(window, HandleKeyboard);
    glfwSetMouseButtonCallback(window, HandleMouseButton);
    glfwSetFramebufferSizeCallback(window, HandleReshape);
    glfwMakeContextCurrent(window);
    std::cout << glGetString(GL_VERSION) << std::endl;
    std::cout << glfwGetJoystickName(GLFW_JOYSTICK_1) << std::endl;
    renderer.Create();
    int framebuffer_width, framebuffer_height;
    glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
    HandleReshape(window, framebuffer_width, framebuffer_height);
    controller.Setup();
    while (!glfwWindowShouldClose(window)) {
      if (keyboard.GetKeyVelocity(GLFW_KEY_TAB) > 0) {
        if (minimized) {
          glfwSetWindowSize(window, width, height);
          minimized = false;
        } else {
          glfwSetWindowSize(window, kMinimizedWidth, kMinimizedHeight);
          minimized = true;
        }
      }
      if (!minimized) {
        renderer.Render();
      }
      controller.Update();
      keyboard.Update();
      mouse.Update();
      double x, y;
      glfwGetCursorPos(window, &x, &y);
      mouse.OnCursorMove(glm::vec2(x, y));
      joystick.Update();
      input.Update();
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
    glfwTerminate();
    return 0;
  }

}  // namespace textengine
