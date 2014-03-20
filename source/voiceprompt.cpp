#include <cstdlib>

#include "synchronizedqueue.h"
#include "voiceprompt.h"

namespace textengine {
  
  VoicePrompt::VoicePrompt(SynchronizedQueue &voice_queue) : voice_queue(voice_queue) {}
  
  void VoicePrompt::Loop() {
    while (true) {
      if (voice_queue.HasMessage()) {
        const auto message = voice_queue.PopMessage();
        const auto text = dynamic_cast<TextMessage *>(message.get());
        std::system(("say --rate=250 \"" + text->text + "\"").c_str());
      }
    }
  }

  void VoicePrompt::Run() {
    thread = std::thread(&VoicePrompt::Loop, this);
    thread.detach();
  }
  
}
