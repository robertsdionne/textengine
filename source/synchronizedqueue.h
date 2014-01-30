#ifndef TEXTENGINE_SYNCHRONIZEDQUEUE_H_
#define TEXTENGINE_SYNCHRONIZEDQUEUE_H_

#include <glm/glm.hpp>
#include <deque>
#include <mutex>
#include <string>

namespace textengine {

  class SynchronizedQueue {
  public:
    struct Message {
      std::string message;
      glm::vec2 position;
      glm::vec2 direction;
      bool is_report;
      bool is_step;
      bool is_movement;
    };

    SynchronizedQueue() = default;

    virtual ~SynchronizedQueue() = default;

    bool HasMessage();

    Message PeekMessage();

    Message PopMessage();

    void PushMessage(const std::string &message);

    void PushReport(const std::string &report);

    void PushStep();
    
    void PushMovement(const glm::vec2 &position, const glm::vec2 &direction);

  private:
    std::mutex mutex;
    std::deque<Message> queue;
  };

}  // namespace textengine

#endif  // TEXTENGINE_SYNCHRONIZEDQUEUE_H_
