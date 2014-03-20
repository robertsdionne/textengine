#ifndef __textengine__synchronizedqueue__
#define __textengine__synchronizedqueue__

#include <glm/glm.hpp>
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <picojson.h>
#include <string>

#include "interface.h"

namespace textengine {
  
  class Message {
    DECLARE_INTERFACE(Message);
    
  public:
    virtual picojson::value ToJson() const = 0;
    
    virtual bool is_movement() const = 0;
  };
  
  class MixedMessage : public Message {
  public:
    MixedMessage() = default;
    
    MixedMessage(MixedMessage &message) = default;
    
    virtual ~MixedMessage() = default;
  };
  
  class CompositeMessage : public Message {
  public:
    CompositeMessage(std::vector<std::unique_ptr<MixedMessage>> &&messages);
    
    virtual ~CompositeMessage() = default;
    
    virtual picojson::value ToJson() const override;
    
    virtual bool is_movement() const override;
    
    std::vector<std::unique_ptr<MixedMessage>> messages;
  };
  
  class EntityMessage : public MixedMessage {
  public:
    EntityMessage(long id);
    
    virtual ~EntityMessage() = default;
    
    virtual picojson::value ToJson() const override;
    
    virtual bool is_movement() const override;
    
    long id;
  };
  
  class ReportMessage : public MixedMessage {
  public:
    ReportMessage(const std::string &report);
    
    virtual ~ReportMessage() = default;
    
    virtual picojson::value ToJson() const override;
    
    virtual bool is_movement() const override;
    
    std::string report;
  };
  
  class TelemetryMessage : public Message {
  public:
    TelemetryMessage(glm::vec2 position,
                     glm::vec2 direction, const std::map<long, glm::vec2> &directions);
    
    virtual ~TelemetryMessage() = default;
    
    virtual picojson::value ToJson() const override;
    
    virtual bool is_movement() const override;
    
    glm::vec2 position;
    glm::vec2 direction;
    std::map<long, glm::vec2> directions;
  };
  
  class TextMessage : public MixedMessage {
  public:
    TextMessage(const std::string &text);
    
    virtual ~TextMessage() = default;
    
    virtual picojson::value ToJson() const override;
    
    virtual bool is_movement() const override;
    
    std::string text;
  };

  class SynchronizedQueue {
  public:
    SynchronizedQueue() = default;

    virtual ~SynchronizedQueue() = default;

    bool HasMessage();

    Message *PeekMessage();

    std::unique_ptr<Message> PopMessage();
    
    void PushEntity(long id);

    void PushMessages(std::vector<MixedMessage *> &&messages);
    
    void PushMovement(const glm::vec2 &position,
                      const glm::vec2 &direction, const std::map<long, glm::vec2> &directions);

    void PushReport(const std::string &report);
    
    void PushText(const std::string &text);

  private:
    std::mutex mutex;
    std::deque<std::unique_ptr<Message>> queue;
  };

}  // namespace textengine

#endif /* defined(__textengine__synchronizedqueue__) */
