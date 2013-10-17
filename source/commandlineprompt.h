#ifndef TEXTENGINE_COMMANDLINEPROMPT_H_
#define TEXTENGINE_COMMANDLINEPROMPT_H_

#include <string>
#include <thread>

#include "prompt.h"

namespace textengine {

  class SynchronizedQueue;

  class CommandLinePrompt : public Prompt {
  public:
    CommandLinePrompt(SynchronizedQueue &queue, const std::string &prompt);

    virtual ~CommandLinePrompt() = default;

    void Run();

  private:
    void Loop();

  private:
    SynchronizedQueue &queue;
    const std::string &prompt;
    std::thread thread;
  };

}  // namespace textengine

#endif  // TEXTENGINE_COMMANDLINEPROMPT_H_
