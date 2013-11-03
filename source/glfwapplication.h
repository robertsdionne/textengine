#ifndef TEXTENGINE_GLFWAPPLICATION_H_
#define TEXTENGINE_GLFWAPPLICATION_H_

#include <GLFW/glfw3.h>
#include <string>

#include "application.h"

namespace textengine {

  class Controller;
  class Keyboard;
  class Joystick;
  class Mouse;
  class Renderer;

  class GlfwApplication : public Application {
  public:
    GlfwApplication(int argument_count, char *arguments[], int width, int height,
                    const std::string &title, Controller &controller, Renderer &renderer,
                    Keyboard &keyboard, Mouse &mouse, Joystick &joystick);

    virtual ~GlfwApplication();

    virtual int Run() override;

  protected:
    static constexpr int kMinimizedWidth = 200;

    static constexpr int kMinimizedHeight = 1;

    static void HandleKeyboard(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void HandleMouseButton(GLFWwindow *window, int button, int action, int mods);

    static void HandleMouseCursorMove(GLFWwindow *window, double x, double y);

    static void HandleReshape(GLFWwindow *window, int width, int height);

    static GlfwApplication *instance;

  private:
    GLFWwindow *window;
    int argument_count;
    char **arguments;
    int width, height;
    const std::string title;
    Controller &controller;
    Renderer &renderer;
    Keyboard &keyboard;
    Mouse &mouse;
    Joystick &joystick;
    bool minimized;
  };

}  // namespace textengine

#endif  // TEXTENGINE_GLFWAPPLICATION_H_
