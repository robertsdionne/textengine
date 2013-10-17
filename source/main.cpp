#include <glm/glm.hpp>

#include "commandlineprompt.h"
#include "commandparser.h"
#include "commandtokenizer.h"
#include "console.h"
#include "gamestate.h"
#include "glfwapplication.h"
#include "keyboard.h"
#include "mesh.h"
#include "mesheditor.h"
#include "meshloader.h"
#include "mouse.h"
#include "synchronizedqueue.h"
#include "textenginerenderer.h"
#include "updater.h"
#include "websocketprompt.h"

constexpr const char *kPrompt = u8"> ";
constexpr int kWindowHeight = 800;
constexpr int kWindowWidth = 1280;
constexpr const char *kWindowTitle = u8"textengine";

int main(int argument_count, char *arguments[]) {
  textengine::Keyboard keyboard;
  textengine::Mouse mouse;
  textengine::SynchronizedQueue command_queue, reply_queue;
  textengine::CommandLinePrompt prompt{command_queue, kPrompt};
  prompt.Run();
  textengine::Console console{reply_queue};
  console.Run();
  textengine::Mesh mesh;
  textengine::MeshLoader loader;
  mesh = loader.ReadMesh("output.json");
  textengine::CommandTokenizer tokenizer;
  textengine::CommandParser parser{tokenizer, mesh, reply_queue};
  textengine::GameState initial_state;
  textengine::Updater updater{command_queue, parser, mesh, initial_state};
  std::default_random_engine engine;
  textengine::MeshEditor editor{kWindowWidth, kWindowHeight, keyboard, mouse, mesh, engine};
  textengine::TextEngineRenderer renderer{updater, mesh, editor};
  textengine::GlfwApplication application{argument_count, arguments, kWindowWidth, kWindowHeight,
                                          kWindowTitle, updater, renderer, keyboard, mouse};
  application.Run();
  return 0;
}
