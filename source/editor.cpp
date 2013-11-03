#include <GLFW/glfw3.h>

#include "checks.h"
#include "editorrenderer.h"
#include "glfwapplication.h"
#include "keyboard.h"
#include "joystick.h"
#include "mesh.h"
#include "mesheditor.h"
#include "meshloader.h"
#include "meshrenderer.h"
#include "meshserializer.h"
#include "mouse.h"

constexpr const char *kPlaytestLog = u8"editor.log";
constexpr int kWindowHeight = 800;
constexpr int kWindowWidth = 1280;
constexpr const char *kWindowTitle = u8"textengine editor";

int main(int argument_count, char *arguments[]) {
  CHECK_STATE(argument_count > 1);
  textengine::Keyboard keyboard;
  textengine::Mouse mouse;
  textengine::Joystick joystick{GLFW_JOYSTICK_1};
  textengine::Mesh mesh;
  textengine::MeshLoader loader;
  mesh = loader.ReadOrCreateMesh(arguments[1]);
  std::default_random_engine engine;
  textengine::MeshEditor editor{kWindowWidth, kWindowHeight,
    keyboard, mouse, mesh, engine, arguments[1]};
  textengine::MeshRenderer mesh_renderer{mesh};
  textengine::EditorRenderer renderer{mesh_renderer, editor};
  textengine::GlfwApplication application{argument_count, arguments, kWindowWidth, kWindowHeight,
    kWindowTitle, editor, renderer, keyboard, mouse, joystick};
  application.Run();
  textengine::MeshSerializer serializer;
  serializer.WriteMesh(arguments[1], mesh);
  return 0;
}
