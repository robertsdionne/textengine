#include "commandqueue.h"
#include "glfwapplication.h"
#include "keyboard.h"
#include "prompt.h"
#include "textenginerenderer.h"

constexpr const char *kPrompt = "> ";
constexpr int kWindowHeight = 480;
constexpr int kWindowWidth = 640;
constexpr const char *kWindowTitle = "textengine";

int main(int argument_count, char *arguments[]) {
  textengine::CommandQueue queue;
  textengine::Prompt prompt(queue, kPrompt);
  prompt.Run();
  textengine::TextEngineRenderer renderer;
  textengine::Keyboard keyboard;
  textengine::GlfwApplication application(argument_count, arguments, kWindowWidth, kWindowHeight,
                                          kWindowTitle, renderer, keyboard);
  application.Run();
  return 0;
}
