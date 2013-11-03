#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include "commandparser.h"
#include "commandtokenizer.h"
#include "gamestate.h"
#include "glfwapplication.h"
#include "keyboard.h"
#include "joystick.h"
#include "log.h"
#include "mesh.h"
#include "mesheditor.h"
#include "meshloader.h"
#include "mouse.h"
#include "synchronizedqueue.h"
#include "textenginerenderer.h"
#include "updater.h"

constexpr const char *kPlaytestLog = u8"editor.log";
constexpr int kWindowHeight = 800;
constexpr int kWindowWidth = 1280;
constexpr const char *kWindowTitle = u8"textengine";

int main(int argument_count, char *arguments[]) {
  textengine::Keyboard keyboard;
  textengine::Mouse mouse;
  textengine::Joystick joystick{GLFW_JOYSTICK_1};
  textengine::Mesh mesh;
  textengine::MeshLoader loader;
  mesh = loader.ReadMesh("../resource/output.json");
  std::default_random_engine engine;
  textengine::MeshEditor editor{kWindowWidth, kWindowHeight, keyboard, mouse, mesh, engine};
  textengine::SynchronizedQueue command_queue, reply_queue;
  textengine::CommandTokenizer tokenizer;
  textengine::CommandParser parser{tokenizer, mesh, reply_queue};
  textengine::GameState initial_state{mesh.Boundaries()};
  textengine::Log playtest_log{kPlaytestLog};
  textengine::Updater updater{command_queue, reply_queue,
    playtest_log, parser, joystick, mesh, initial_state};
  textengine::TextEngineRenderer renderer{updater, mesh, editor};
  textengine::GlfwApplication application{argument_count, arguments, kWindowWidth, kWindowHeight,
    kWindowTitle, updater, renderer, keyboard, mouse, joystick};
  application.Run();
  return 0;
}
