#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include "editorrenderer.h"
#include "glfwapplication.h"
#include "keyboard.h"
#include "joystick.h"
#include "mesh.h"
#include "mesheditor.h"
#include "meshloader.h"
#include "meshrenderer.h"
#include "mouse.h"
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
  textengine::MeshRenderer mesh_renderer{mesh};
  textengine::EditorRenderer renderer{mesh_renderer, editor};
  textengine::GlfwApplication application{argument_count, arguments, kWindowWidth, kWindowHeight,
    kWindowTitle, editor, renderer, keyboard, mouse, joystick};
  application.Run();
  return 0;
}
