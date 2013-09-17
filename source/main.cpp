#include "glfwapplication.h"
#include "textenginerenderer.h"

int main(int argument_count, char *arguments[]) {
  textengine::TextEngineRenderer renderer;
  textengine::GlfwApplication application(argument_count, arguments,
                                          1280, 800, "textengine", &renderer);
  application.Run();
  return 0;
}
