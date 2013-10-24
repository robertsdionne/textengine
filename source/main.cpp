#include <fstream>
#include <glm/glm.hpp>
#include <random>
#include <string>

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

std::random_device generator;
std::uniform_real_distribution<float> my_random;

glm::vec2 FaceCentroid(const textengine::Mesh::Face *face) {
  glm::vec2 total = glm::vec2();
  float count = 0;
  textengine::Mesh::HalfEdge *edge = face->face_edge;
  do {
    total += edge->start->position;
    count += 1;
    edge = edge->next;
  } while (edge != face->face_edge);
  return total / count;
}

void PutItemInRoom(const std::string &item, const std::string &room, textengine::Mesh &mesh, textengine::GameState &state) {
  for (auto &face : mesh.get_faces()) {
    if (face->room_info && room == face->room_info->name) {
      state.items.push_back({
        FaceCentroid(face.get()), item, glm::vec4(my_random(generator), my_random(generator), my_random(generator), 1)
      });
      return;
    }
  }
}

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
  PutItemInRoom("book", "RoomA", mesh, initial_state);
  PutItemInRoom("teacup", "RoomB", mesh, initial_state);
  PutItemInRoom("bread", "RoomC", mesh, initial_state);
  PutItemInRoom("lamp", "RoomD", mesh, initial_state);
  PutItemInRoom("tire", "RoomE", mesh, initial_state);
  PutItemInRoom("crowbar", "RoomF", mesh, initial_state);
  PutItemInRoom("wallet", "RoomG", mesh, initial_state);
  PutItemInRoom("teapot", "RoomH", mesh, initial_state);
  PutItemInRoom("coin", "RoomI", mesh, initial_state);
  PutItemInRoom("pencil", "RoomJ", mesh, initial_state);
  PutItemInRoom("turtle", "RoomK", mesh, initial_state);
  PutItemInRoom("note", "RoomL", mesh, initial_state);
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
