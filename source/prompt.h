#ifndef TEXTENGINE_PROMPT_H_
#define TEXTENGINE_PROMPT_H_

#include <string>
#include <thread>

namespace textengine {

  class CommandQueue;
  
  class Prompt {
  public:
    Prompt(CommandQueue &queue, const std::string &prompt);

    virtual ~Prompt() = default;

    void Join();

    void Run();

  private:
    void Loop();

  private:
    CommandQueue &queue;
    const std::string &prompt;
    std::thread thread;
  };

}  // namespace textengine

#endif  // TEXTENGINE_PROMPT_H_
