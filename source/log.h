#ifndef __textengine__log__
#define __textengine__log__

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

#endif /* defined(__textengine__log__) */
