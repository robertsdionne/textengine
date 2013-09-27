#include <glm/glm.hpp>

#include "commandparser.h"
#include "commandqueue.h"
#include "commandtokenizer.h"
#include "gamestate.h"
#include "glfwapplication.h"
#include "keyboard.h"
#include "mesh.h"
#include "mesheditor.h"
#include "mouse.h"
#include "prompt.h"
#include "textenginerenderer.h"
#include "updater.h"

constexpr const char *kPrompt = u8"> ";
constexpr int kWindowHeight = 480;
constexpr int kWindowWidth = 640;
constexpr const char *kWindowTitle = u8"textengine";

int main(int argument_count, char *arguments[]) {
  textengine::Keyboard keyboard;
  textengine::Mouse mouse;
  textengine::CommandQueue queue;
  textengine::Prompt prompt{queue, kPrompt};
  prompt.Run();
  textengine::CommandTokenizer tokenizer;
  textengine::CommandParser parser{tokenizer};
  textengine::GameState initial_state;
  textengine::Updater updater{queue, parser, initial_state};
  textengine::Mesh mesh;
  mesh.AddDefaultFace(glm::vec2(0.5, 0.5));
  textengine::MeshEditor editor{keyboard, mouse, mesh};
  textengine::TextEngineRenderer renderer{updater, mesh, editor};
  textengine::GlfwApplication application{argument_count, arguments, kWindowWidth, kWindowHeight,
                                          kWindowTitle, updater, renderer, keyboard, mouse};
  application.Run();
  return 0;
}
