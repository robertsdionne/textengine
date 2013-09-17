#ifndef TEXTENGINE_GLFWAPPLICATION_H_
#define TEXTENGINE_GLFWAPPLICATION_H_

#include <GLFW/glfw3.h>
#include <string>

#include "application.h"

namespace textengine {

  class Renderer;

  class GlfwApplication : public Application {
  public:
    GlfwApplication(int argument_count, char *arguments[], int width, int height,
                    const std::string &title, Renderer *renderer);

    virtual ~GlfwApplication();

    virtual void Run() override;

  protected:
    static void Display();

    static void Keyboard(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void Reshape(GLFWwindow *window, int width, int height);

  protected:
    static GlfwApplication *glfw_application;

    GLFWmonitor *monitor;
    GLFWwindow *window;
    int argument_count;
    char **arguments;
    int width, height;
    const std::string title;
    Renderer *renderer;
  };

}  // namespace textengine

#endif  // TEXTENGINE_GLFWAPPLICATION_H_
