#include <string>

#include "editor.h"
#include "gamestate.h"
#include "glfwapplication.h"
#include "input.h"
#include "joystick.h"
#include "keyboard.h"
#include "log.h"
#include "mouse.h"
#include "scene.h"
#include "sceneloader.h"
#include "sceneserializer.h"
#include "synchronizedqueue.h"
#include "textenginerenderer.h"
#include "updater.h"
#include "voiceprompt.h"
#include "websocketprompt.h"

constexpr const char *kPlaytestLog = u8"playtest.log";
constexpr const char *kPrompt = u8"> ";
constexpr int kWindowHeight = 800;
constexpr int kWindowWidth = 1280/2;
constexpr const char *kWindowTitle = u8"Palimpsest";

int main(int argument_count, char *arguments[]) {
  const std::string filename = argument_count > 1 ? arguments[1] : "../resource/scenes/terrarium.json";
  const auto edit = argument_count > 2 && "edit" == std::string(arguments[2]);
  const auto voice = false;
  textengine::Joystick joystick(GLFW_JOYSTICK_1);
  textengine::Keyboard keyboard;
  textengine::Mouse mouse;
  textengine::Input input(joystick, keyboard, mouse);
  textengine::Scene scene;
  textengine::SceneLoader scene_loader;
  scene = scene_loader.ReadScene(filename);
  textengine::GameState initial_state{scene};
  textengine::Log playtest_log(kPlaytestLog);
  textengine::SynchronizedQueue reply_queue, voice_queue;
  textengine::Updater updater(
    kWindowWidth, kWindowHeight, reply_queue, voice_queue,
    playtest_log, input, mouse, keyboard, initial_state, scene);
  textengine::WebSocketPrompt prompt(reply_queue, kPrompt, playtest_log);
  textengine::VoicePrompt voice_prompt(voice_queue);
  textengine::Editor editor(edit ? 2 * kWindowWidth : kWindowWidth, kWindowHeight, initial_state,
                            keyboard, mouse, scene);
  if (!edit) {
    prompt.Run();
    if (voice) {
      voice_prompt.Run();
    }
  }
  textengine::Controller *controller = &updater;
  if (edit) {
    controller = &editor;
  }
  textengine::TextEngineRenderer renderer(mouse, *controller, scene, true);
  textengine::GlfwApplication application(
    argument_count, arguments, edit ? 2 * kWindowWidth : kWindowWidth, kWindowHeight,
    kWindowTitle, *controller, renderer, input, joystick,
    keyboard, mouse, !edit);
  const auto result = application.Run();
  if (edit) {
    textengine::SceneSerializer serializer;
    serializer.WriteScene(filename, scene);
  }
  return result;
}
