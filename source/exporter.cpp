#include <fstream>
#include <picojson.h>

#include "checks.h"

int main(int argument_count, char *arguments[]) {
  CHECK_STATE(argument_count > 2);
  std::ifstream in(arguments[1]);
  CHECK_STATE(in.is_open());
  std::ofstream out(arguments[2]);
  CHECK_STATE(out.is_open());
  picojson::value value;
  in >> value;
  CHECK_STATE(value.is<picojson::object>());
  picojson::object object = value.get<picojson::object>();
  CHECK_STATE(object["vertices"].is<picojson::array>());
  for (auto element : object["vertices"].get<picojson::array>()) {
    CHECK_STATE(element.is<picojson::object>());
    picojson::object vertex = element.get<picojson::object>();
    CHECK_STATE(vertex["position"].is<picojson::object>());
    picojson::object position = vertex["position"].get<picojson::object>();
    CHECK_STATE(position["x"].is<double>());
    const double x = position["x"].get<double>();
    CHECK_STATE(position["y"].is<double>());
    const double y = position["y"].get<double>();
    out << "v " << x << " " << y << " " << 0 << std::endl;
  }
  CHECK_STATE(object["half_edges"].is<picojson::array>());
  picojson::array half_edges = object["half_edges"].get<picojson::array>();
  CHECK_STATE(object["faces"].is<picojson::array>());
  for (auto element : object["faces"].get<picojson::array>()) {
    CHECK_STATE(element.is<picojson::object>());
    picojson::object face = element.get<picojson::object>();
    CHECK_STATE(face["face_edge"].is<double>());
    auto face_edge = static_cast<long>(face["face_edge"].get<double>());
    CHECK_STATE(half_edges.at(face_edge).is<picojson::object>());
    picojson::object half_edge = half_edges.at(face_edge).get<picojson::object>();
    long next;
    out << "f";
    do {
      CHECK_STATE(half_edge["start"].is<double>());
      auto start = static_cast<long>(half_edge["start"].get<double>());
      next = static_cast<long>(half_edge["next"].get<double>());
      CHECK_STATE(half_edges.at(next).is<picojson::object>());
      half_edge = half_edges.at(next).get<picojson::object>();
      out << " " << (start + 1);
    } while (face_edge != next);
    out << std::endl;
  }
  return 0;
}
