#include "glfwapplication.h"
#include "keyboard.h"
#include "prompt.h"
#include "textenginerenderer.h"

constexpr const char *kPrompt = "> ";
constexpr int kWindowHeight = 800;
constexpr int kWindowWidth = 1280;
constexpr const char *kWindowTitle = "textengine";

int main(int argument_count, char *arguments[]) {
  textengine::Prompt prompt(kPrompt);
  prompt.Run();
  textengine::TextEngineRenderer renderer;
  textengine::Keyboard keyboard;
  textengine::GlfwApplication application(argument_count, arguments, kWindowWidth, kWindowHeight,
                                          kWindowTitle, renderer, keyboard);
  application.Run();
  return 0;
}
