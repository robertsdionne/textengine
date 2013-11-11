#include <string>

#include "commandparser.h"
#include "commandtokenizer.h"
#include "gamestate.h"
#include "glfwapplication.h"
#include "input.h"
#include "joystick.h"
#include "keyboard.h"
#include "log.h"
#include "mesh.h"
#include "meshloader.h"
#include "mouse.h"
#include "subjectivemeshrenderer.h"
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
  const std::string filename = argument_count > 1 ? arguments[1] : "../resource/cave.json";
  textengine::Joystick joystick(GLFW_JOYSTICK_1);
  textengine::Keyboard keyboard;
  textengine::Mouse mouse;
  textengine::Input input{joystick, keyboard, mouse};
  textengine::SynchronizedQueue command_queue, reply_queue;
  textengine::WebSocketPrompt prompt{command_queue, reply_queue, kPrompt};
//  prompt.Run();
  textengine::Mesh mesh;
  textengine::MeshLoader loader;
  mesh = loader.ReadMesh(filename);
  textengine::CommandTokenizer tokenizer;
  textengine::CommandParser parser{tokenizer, mesh, reply_queue};
  textengine::GameState initial_state;
  textengine::Log playtest_log{kPlaytestLog};
  textengine::Updater updater{
    command_queue, reply_queue,
    playtest_log, parser, input, mesh, initial_state
  };
  textengine::SubjectiveMeshRenderer mesh_renderer{mesh};
  textengine::TextEngineRenderer renderer{updater, mesh_renderer};
  textengine::GlfwApplication application{
    argument_count, arguments, kWindowWidth, kWindowHeight,
    kWindowTitle, updater, renderer, input, joystick,
    keyboard, mouse
  };
  return application.Run();
}
