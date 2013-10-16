#include <glm/glm.hpp>

#include "commandparser.h"
#include "commandqueue.h"
#include "commandtokenizer.h"
#include "gamestate.h"
#include "glfwapplication.h"
#include "keyboard.h"
#include "mesh.h"
#include "mesheditor.h"
#include "meshloader.h"
#include "mouse.h"
#include "prompt.h"
#include "textenginerenderer.h"
#include "updater.h"

constexpr const char *kPrompt = u8"> ";
constexpr int kWindowHeight = 800;
constexpr int kWindowWidth = 1280;
constexpr const char *kWindowTitle = u8"textengine";

int main(int argument_count, char *arguments[]) {
  textengine::Keyboard keyboard;
  textengine::Mouse mouse;
  textengine::CommandQueue queue;
  textengine::Prompt prompt{queue, kPrompt};
  prompt.Run();
  textengine::GameState initial_state;
  textengine::Mesh mesh;
  textengine::MeshLoader loader;
  mesh = loader.ReadMesh("output.json");
  textengine::CommandTokenizer tokenizer;
  textengine::CommandParser parser{tokenizer, mesh};
  textengine::Updater updater{queue, parser, mesh, initial_state};
  std::default_random_engine engine;
  textengine::MeshEditor editor{kWindowWidth, kWindowHeight, keyboard, mouse, mesh, engine};
  textengine::TextEngineRenderer renderer{updater, mesh, editor};
  textengine::GlfwApplication application{argument_count, arguments, kWindowWidth, kWindowHeight,
                                          kWindowTitle, updater, renderer, keyboard, mouse};
  application.Run();
  return 0;
}
