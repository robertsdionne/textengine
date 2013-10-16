#ifndef TEXTENGINE_COMMANDLINEPROMPT_H_
#define TEXTENGINE_COMMANDLINEPROMPT_H_

#include <string>
#include <thread>

#include "prompt.h"

namespace textengine {

  class CommandQueue;

  class CommandLinePrompt : public Prompt {
  public:
    CommandLinePrompt(CommandQueue &queue, const std::string &prompt);

    virtual ~CommandLinePrompt() = default;

    void Run();

  private:
    void Loop();

  private:
    CommandQueue &queue;
    const std::string &prompt;
    std::thread thread;
  };

}  // namespace textengine

#endif  // TEXTENGINE_COMMANDLINEPROMPT_H_
