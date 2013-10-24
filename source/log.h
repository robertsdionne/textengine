#ifndef TEXTENGINE_LOG_H_
#define TEXTENGINE_LOG_H_

#include <chrono>
#include <fstream>
#include <string>

namespace textengine {

  class Log {
  public:
    Log(const std::string &filename);

    virtual ~Log();

    void LogMessage(const std::string &message);

  private:
    virtual void Begin();

    virtual void End();

  private:
    std::string filename;
    std::fstream out;
  };

}  // namespace textengine

#endif  // TEXTENGINE_LOG_H_
