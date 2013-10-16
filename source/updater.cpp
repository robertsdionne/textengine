#include <cmath>
#include <glm/glm.hpp>
#include <limits>
#include <unordered_map>

#include "checks.h"
#include "commandparser.h"
#include "commandqueue.h"
#include "gamestate.h"
#include "keyboard.h"
#include "mesh.h"
#include "updater.h"

namespace textengine {

  Updater::Updater(CommandQueue &queue, CommandParser &parser, Mesh &mesh,
                   const GameState &initial_state)
  : queue(queue), parser(parser), mesh(mesh), current_state(initial_state) {}

  GameState Updater::GetCurrentState() {
    return current_state;
  }

  void Updater::Update() {
    current_state = Update(current_state);
  }

  GameState Updater::Update(const GameState current_state) {
    GameState next_state = current_state;
    if (queue.HasCommand()) {
      next_state = parser.Parse(next_state, queue.PopCommand());
    }

    if (next_state.room_target) {
//      std::cout << "has room target" << std::endl;
      auto current_face = FindFaceThatContainsPoint(next_state.player_position);
      if (current_face && next_state.room_target == current_face->room_info) {
//        std::cout << "reached target" << std::endl;
        next_state.room_target = nullptr;
      } else if (current_face && next_state.room_target) {
//        std::cout << "pursuing target" << std::endl;
        std::unordered_map<Mesh::Face *, float> distances;
        for (auto &face : mesh.get_faces()) {
          if (next_state.room_target == face->room_info) {
            distances.insert({face.get(), 0.0f});
          } else {
            distances.insert({face.get(), std::numeric_limits<float>::infinity()});
          }
        }
        bool done = false;
        while (!done) {
          done = true;
          for (auto &face : mesh.get_faces()) {
            if (std::numeric_limits<float>::infinity() == distances.at(face.get())) {
              done = false;
            } else {
              continue;
            }
            float minimum = std::numeric_limits<float>::infinity();
            glm::vec2 target;
            Mesh::Face *argmin = nullptr;
            Mesh::HalfEdge *edge = face->face_edge;
            do {
              //          std::cout << "finding minimum" << std::endl;
              if (edge->opposite) {
                const auto h01 = edge->opposite->face->face_edge;
                const auto h12 = h01->next;
                const auto h20 = h12->next;
                CHECK_STATE(h01 == h20->next);
                const auto v0 = h01->start->position, v1 = h12->start->position, v2 = h20->start->position;
                const auto centroid = (v0 + v1 + v2) / 3.0f;
                const float distance = distances.at(edge->opposite->face);
                if (distance < minimum) {
                  minimum = distance;
                  argmin = edge->opposite->face;
                  target = centroid;
                }
              }
              edge = edge->next;
            } while (edge != face->face_edge);
            if (argmin) {
              //          std::cout << "found minimum target" << std::endl;
              const auto h01 = face->face_edge;
              const auto h12 = h01->next;
              const auto h20 = h12->next;
              CHECK_STATE(h01 == h20->next);
              const auto v0 = h01->start->position, v1 = h12->start->position, v2 = h20->start->position;
              const auto centroid = (v0 + v1 + v2) / 3.0f;
              distances.at(face.get()) = minimum + glm::length(target - centroid);
//              std::cout << "updating face " << face.get() << " with value " << minimum + glm::length(target - centroid) << std::endl;
            }
          }
        }
        float minimum = std::numeric_limits<float>::infinity();
        glm::vec2 target;
        Mesh::Face *argmin = nullptr;
        Mesh::HalfEdge *edge = current_face->face_edge;
        do {
//          std::cout << "finding minimum" << std::endl;
          if (edge->opposite) {
            const auto h01 = edge->opposite->face->face_edge;
            const auto h12 = h01->next;
            const auto h20 = h12->next;
            CHECK_STATE(h01 == h20->next);
            const auto v0 = h01->start->position, v1 = h12->start->position, v2 = h20->start->position;
            const auto centroid = (v0 + v1 + v2) / 3.0f;
            const float distance = distances.at(edge->opposite->face);
            if (distance < minimum) {
              minimum = distance;
              argmin = edge->opposite->face;
              target = centroid;
            }
          }
          edge = edge->next;
        } while (edge != current_face->face_edge);
        if (argmin) {
//          std::cout << "found minimum target" << std::endl;
          next_state.player_direction_target = glm::normalize(target - next_state.player_position);
          next_state.player_target = target;
        }
      }
    }
    const float angle = glm::atan(next_state.player_direction.y, next_state.player_direction.x);
    float angle_target = glm::atan(next_state.player_direction_target.y,
                                   next_state.player_direction_target.x);
    while (angle_target - angle > M_PI) {
      angle_target -= 2.0 * M_PI;
    }
    while (angle_target - angle < -M_PI) {
      angle_target += 2.0 * M_PI;
    }
    const float final_angle = glm::mix(angle, angle_target, 0.1f);
    next_state.player_direction = glm::vec2(glm::cos(final_angle), glm::sin(final_angle));
    next_state.player_position = glm::mix(next_state.player_position,
                                          next_state.player_target, 0.1f);
    return next_state;
  }

  bool Updater::FaceContainsPoint(Mesh::Face *face, glm::vec2 point) const {
    const auto h01 = face->face_edge;
    const auto h12 = h01->next;
    const auto h20 = h12->next;
    CHECK_STATE(h01 == h20->next);
    const auto v0 = h01->start, v1 = h12->start, v2 = h20->start;
    const auto p0 = glm::vec3(v0->position, 0.0f), p1 = glm::vec3(v1->position, 0.0f), p2 = glm::vec3(v2->position, 0.0f);
    const auto p = glm::vec3(point, 0.0f);
    const float u = (glm::cross(p, p2-p0).z - glm::cross(p0, p2-p0).z) / glm::cross(p1-p0, p2-p0).z;
    const float v = (glm::cross(p0, p1-p0).z - glm::cross(p, p1-p0).z) / glm::cross(p1-p0, p2-p0).z;
//    if (0 <= u && 0 <= v && (u + v) <= 1) {
//      std::cout << u << ", " << v << std::endl;
//    }
    return 0 <= u && 0 <= v && (u + v) <= 1;
  }

  Mesh::Face *Updater::FindFaceThatContainsPoint(glm::vec2 point) const {
    for (auto &face : mesh.get_faces()) {
      if (FaceContainsPoint(face.get(), point)) {
//        std::cout << "face contains point" << std::endl;
        return face.get();
      }
    }
    return nullptr;
  }

}  // namespace textengine
