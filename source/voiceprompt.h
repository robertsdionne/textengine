#ifndef __textengine__voiceprompt__
#define __textengine__voiceprompt__

#include <thread>

#include "prompt.h"

namespace textengine {
  
  class SynchronizedQueue;
  
  class VoicePrompt : public Prompt {
  public:
    VoicePrompt(SynchronizedQueue &voice_queue);
    
    virtual ~VoicePrompt() = default;
    
    virtual void Run() override;
    
  private:
    void Loop();
    
  private:
    SynchronizedQueue &voice_queue;
    std::thread thread;
  };
  
}  // namespace textengine

#endif /* defined(__textengine__voiceprompt__) */
