#ifndef RSD_GLFWAPPLICATION_H_
#define RSD_GLFWAPPLICATION_H_

#include <GLFW/glfw3.h>
#include <string>

#include "application.h"

namespace rsd {

  class Renderer;

  class GlfwApplication : public Application {
  public:
    GlfwApplication(int argument_count, char *arguments[], int width, int height,
                    const std::string &title, Renderer &renderer);

    virtual ~GlfwApplication();

    virtual int Run() override;

  protected:
    static void HandleKeyboard(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void HandleMouseButton(GLFWwindow *window, int button, int action, int mods);

    static void HandleReshape(GLFWwindow *window, int width, int height);

    static GlfwApplication *instance;

  private:
    GLFWwindow *window;
    int argument_count;
    char **arguments;
    int width, height;
    const std::string title;
    Renderer &renderer;
  };

}  // namespace rsd

#endif  // RSD_GLFWAPPLICATION_H_
