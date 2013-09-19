#include "commandparser.h"
#include "commandqueue.h"
#include "gamestate.h"
#include "glfwapplication.h"
#include "keyboard.h"
#include "prompt.h"
#include "textenginerenderer.h"
#include "updater.h"

constexpr const char *kPrompt = u8"> ";
constexpr int kWindowHeight = 480;
constexpr int kWindowWidth = 640;
constexpr const char *kWindowTitle = u8"textengine";

int main(int argument_count, char *arguments[]) {
  textengine::Keyboard keyboard;
  textengine::CommandQueue queue;
  textengine::Prompt prompt(queue, kPrompt);
  prompt.Run();
  textengine::CommandParser parser;
  textengine::GameState initial_state = textengine::GameState(glm::vec2(), glm::vec2());
  textengine::Updater updater(keyboard, queue, parser, initial_state);
  textengine::TextEngineRenderer renderer(updater);
  textengine::GlfwApplication application(argument_count, arguments, kWindowWidth, kWindowHeight,
                                          kWindowTitle, updater, renderer, keyboard);
  application.Run();
  return 0;
}
