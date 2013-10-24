#include <fstream>
#include <glm/glm.hpp>

#include "commandparser.h"
#include "commandtokenizer.h"
#include "gamestate.h"
#include "glfwapplication.h"
#include "keyboard.h"
#include "log.h"
#include "mesh.h"
#include "mesheditor.h"
#include "meshloader.h"
#include "mouse.h"
#include "synchronizedqueue.h"
#include "textenginerenderer.h"
#include "updater.h"
#include "websocketprompt.h"

constexpr const char *kPlaytestLog = u8"playtest.log";
constexpr const char *kPrompt = u8"> ";
constexpr int kWindowHeight = 800;
constexpr int kWindowWidth = 1280;
constexpr const char *kWindowTitle = u8"textengine";

int main(int argument_count, char *arguments[]) {
  textengine::Keyboard keyboard;
  textengine::Mouse mouse;
  textengine::SynchronizedQueue command_queue, reply_queue;
  textengine::WebSocketPrompt prompt{command_queue, reply_queue, kPrompt};
  prompt.Run();
  textengine::Mesh mesh;
  textengine::MeshLoader loader;
  mesh = loader.ReadMesh("../resource/output.json");
  textengine::CommandTokenizer tokenizer;
  textengine::CommandParser parser{tokenizer, mesh, reply_queue};
  textengine::GameState initial_state;
  textengine::Log playtest_log{kPlaytestLog};
  textengine::Updater updater{command_queue, reply_queue,
      playtest_log, parser, mesh, initial_state};
  std::default_random_engine engine;
  textengine::MeshEditor editor{kWindowWidth, kWindowHeight, keyboard, mouse, mesh, engine};
  textengine::TextEngineRenderer renderer{updater, mesh, editor};
  textengine::GlfwApplication application{argument_count, arguments, kWindowWidth, kWindowHeight,
                                          kWindowTitle, updater, renderer, keyboard, mouse};
  application.Run();
  return 0;
}
