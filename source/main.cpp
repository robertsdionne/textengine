#include "glfwapplication.h"
#include "textenginerenderer.h"

constexpr int kWindowHeight = 800;
constexpr int kWindowWidth = 1280;
constexpr const char *kWindowTitle = "textengine";

int main(int argument_count, char *arguments[]) {
  textengine::TextEngineRenderer renderer;
  textengine::GlfwApplication application(argument_count, arguments,
                                          kWindowWidth, kWindowHeight, kWindowTitle, &renderer);
  application.Run();
  return 0;
}
