#include <mutex>
#include <picojson.h>
#include <string>

#include "synchronizedqueue.h"

namespace textengine {
  
  CompositeMessage::CompositeMessage() : messages() {}
  
  picojson::value CompositeMessage::ToJson() const {
    picojson::array messages;
    for (auto &message : this->messages) {
      messages.push_back(message.ToJson());
    }
    picojson::object object;
    object["type"] = picojson::value("composite");
    object["messages"] = picojson::value(messages);
    return picojson::value(object);
  }

  bool CompositeMessage::is_movement() const {
    return false;
  }
  
  EntityMessage::EntityMessage(long id) :id(id) {}
  
  picojson::value EntityMessage::ToJson() const {
    picojson::object object;
    object["type"] = picojson::value("entity");
    object["id"] = picojson::value(static_cast<double>(id));
    return picojson::value(object);
  }
  
  bool EntityMessage::is_movement() const {
    return false;
  }
  
  ReportMessage::ReportMessage(const std::string &report) : report(report) {}
  
  picojson::value ReportMessage::ToJson() const {
    picojson::object object;
    object["type"] = picojson::value("report");
    object["report"] = picojson::value(report);
    return picojson::value(object);
  }
  
  bool ReportMessage::is_movement() const {
    return false;
  }
  
  StepMessage::StepMessage() {}
  
  picojson::value StepMessage::ToJson() const {
    picojson::object object;
    object["type"] = picojson::value("step");
    return picojson::value(object);
  }
  
  bool StepMessage::is_movement() const {
    return false;
  }
  
  TelemetryMessage::TelemetryMessage(glm::vec2 position,
                                     glm::vec2 direction, const std::vector<glm::vec2> &directions)
  : position(position), direction(direction) {}
  
  picojson::value TelemetryMessage::ToJson() const {
    picojson::object position, direction, object;
    position["x"] = picojson::value(this->position.x);
    position["y"] = picojson::value(this->position.y);
    direction["x"] = picojson::value(this->direction.x);
    direction["y"] = picojson::value(this->direction.y);
    object["type"] = picojson::value("telemetry");
    object["position"] = picojson::value(position);
    object["direction"] = picojson::value(direction);
    return picojson::value(object);
  }
  
  bool TelemetryMessage::is_movement() const {
    return true;
  }
  
  TextMessage::TextMessage(const std::string &text) : text(text) {}
  
  picojson::value TextMessage::ToJson() const {
    picojson::object object;
    object["type"] = picojson::value("text");
    object["text"] = picojson::value(text);
    return picojson::value(object);
  }
  
  bool TextMessage::is_movement() const {
    return false;
  }

  bool SynchronizedQueue::HasMessage() {
    std::lock_guard<std::mutex> lock(mutex);
    return !queue.empty();
  }

  Message *SynchronizedQueue::PeekMessage() {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.front().get();
  }

  std::unique_ptr<Message> SynchronizedQueue::PopMessage() {
    std::lock_guard<std::mutex> lock(mutex);
    auto result = std::move(queue.front());
    queue.pop_front();
    return result;
  }
  
  void SynchronizedQueue::PushEntity(long id) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.emplace_back(new EntityMessage(id));
  }
  
  void SynchronizedQueue::PushMessage(const std::string &message) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!message.empty()) {
      queue.emplace_back(new TextMessage{message});
    }
  }

  void SynchronizedQueue::PushReport(const std::string &report) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.emplace_back(new ReportMessage{report});
  }

  void SynchronizedQueue::PushStep() {
    queue.emplace_back(new StepMessage);
  }
  
  void SynchronizedQueue::PushMovement(const glm::vec2 &position,
      const glm::vec2 &direction, const std::vector<glm::vec2> &directions) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!queue.empty() && queue.back()->is_movement()) {
      queue.pop_back();
    }
    queue.emplace_back(new TelemetryMessage{position, direction, directions});
  }

}  // namespace textengine
