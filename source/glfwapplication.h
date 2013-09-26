#ifndef TEXTENGINE_GLFWAPPLICATION_H_
#define TEXTENGINE_GLFWAPPLICATION_H_

#include <GLFW/glfw3.h>
#include <string>

#include "application.h"

namespace textengine {

  class Keyboard;
  class Mouse;
  class Renderer;
  class Updater;

  class GlfwApplication : public Application {
  public:
    GlfwApplication(int argument_count, char *arguments[], int width, int height,
                    const std::string &title, Updater &updater, Renderer &renderer,
                    Keyboard &keyboard, Mouse &mouse);

    virtual ~GlfwApplication();

    virtual void Run() override;

  protected:
    static void HandleKeyboard(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void HandleMouseButton(GLFWwindow *window, int button, int action, int mods);

    static void HandleMouseCursorMove(GLFWwindow *window, double x, double y);

    static void HandleReshape(GLFWwindow *window, int width, int height);

    static GlfwApplication *glfw_application;

  private:
    GLFWwindow *window;
    int argument_count;
    char **arguments;
    int width, height;
    const std::string title;
    Updater &updater;
    Renderer &renderer;
    Keyboard &keyboard;
    Mouse &mouse;
  };

}  // namespace textengine

#endif  // TEXTENGINE_GLFWAPPLICATION_H_
