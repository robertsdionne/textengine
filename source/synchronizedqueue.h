#ifndef TEXTENGINE_SYNCHRONIZEDQUEUE_H_
#define TEXTENGINE_SYNCHRONIZEDQUEUE_H_

#include <glm/glm.hpp>
#include <deque>
#include <memory>
#include <mutex>
#include <picojson.h>
#include <string>

#include "base.h"

namespace textengine {
  
  class Message {
    DECLARE_INTERFACE(Message);
    
  public:
    virtual picojson::value ToJson() const = 0;
    
    virtual bool is_movement() const = 0;
  };
  
  class CompositeMessage : public Message {
  public:
    CompositeMessage();
    
    virtual ~CompositeMessage() = default;
    
    virtual picojson::value ToJson() const override;
    
    virtual bool is_movement() const override;
    
  private:
    std::vector<Message> messages;
  };
  
  class EntityMessage : public Message {
  public:
    EntityMessage(glm::vec2 position);
    
    virtual ~EntityMessage() = default;
    
    virtual picojson::value ToJson() const override;
    
    virtual bool is_movement() const override;
    
  private:
    glm::vec2 position;
  };
  
  class ReportMessage : public Message {
  public:
    ReportMessage(const std::string &report);
    
    virtual ~ReportMessage() = default;
    
    virtual picojson::value ToJson() const override;
    
    virtual bool is_movement() const override;
    
  private:
    std::string report;
  };
  
  class StepMessage : public Message {
  public:
    StepMessage();
    
    virtual ~StepMessage() = default;
    
    virtual picojson::value ToJson() const override;
    
    virtual bool is_movement() const override;
  };
  
  class TelemetryMessage : public Message {
  public:
    TelemetryMessage(glm::vec2 position,
                     glm::vec2 direction, const std::vector<glm::vec2> &directions);
    
    virtual ~TelemetryMessage() = default;
    
    virtual picojson::value ToJson() const override;
    
    virtual bool is_movement() const override;
    
  private:
    glm::vec2 position;
    glm::vec2 direction;
  };
  
  class TextMessage : public Message {
  public:
    TextMessage(const std::string &text);
    
    virtual ~TextMessage() = default;
    
    virtual picojson::value ToJson() const override;
    
    virtual bool is_movement() const override;
    
  private:
    std::string text;
  };

  class SynchronizedQueue {
  public:
    SynchronizedQueue() = default;

    virtual ~SynchronizedQueue() = default;

    bool HasMessage();

    Message *PeekMessage();

    std::unique_ptr<Message> PopMessage();
    
    void PushEntity(glm::vec2 position);

    void PushMessage(const std::string &message);

    void PushReport(const std::string &report);

    void PushStep();
    
    void PushMovement(const glm::vec2 &position,
        const glm::vec2 &direction, const std::vector<glm::vec2> &directions);

  private:
    std::mutex mutex;
    std::deque<std::unique_ptr<Message>> queue;
  };

}  // namespace textengine

#endif  // TEXTENGINE_SYNCHRONIZEDQUEUE_H_
