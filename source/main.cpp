#include <string>

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
#include "scene.h"
#include "sceneloader.h"
#include "subjectivemeshrenderer.h"
#include "synchronizedqueue.h"
#include "textenginerenderer.h"
#include "updater.h"

constexpr const char *kPlaytestLog = u8"playtest.log";
constexpr const char *kPrompt = u8"> ";
constexpr int kWindowHeight = 800;
constexpr int kWindowWidth = 1280;
constexpr const char *kWindowTitle = u8"textengine";

int main(int argument_count, char *arguments[]) {
  const std::string filename = argument_count > 1 ? arguments[1] : "../resource/subway.json";
  textengine::Joystick joystick(GLFW_JOYSTICK_1);
  textengine::Keyboard keyboard;
  textengine::Mouse mouse;
  textengine::Input input{joystick, keyboard, mouse};
  textengine::SynchronizedQueue command_queue, reply_queue;
  textengine::Scene scene;
  textengine::SceneLoader scene_loader;
  scene = scene_loader.ReadScene("../resource/messages.json");
  textengine::CommandTokenizer tokenizer;
  textengine::GameState initial_state{std::vector<std::unique_ptr<std::vector<glm::vec2>>>()};
  textengine::Log playtest_log{kPlaytestLog};
  textengine::Updater updater{
    command_queue, reply_queue,
    playtest_log, input, initial_state
  };
  textengine::TextEngineRenderer renderer{updater, scene};
  textengine::GlfwApplication application{
    argument_count, arguments, kWindowWidth, kWindowHeight,
    kWindowTitle, updater, renderer, input, joystick,
    keyboard, mouse
  };
  return application.Run();
}
