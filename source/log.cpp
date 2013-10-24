#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#include "log.h"

namespace textengine {

  Log::Log(const std::string &filename) : filename(filename), out(filename, std::ios_base::app) {
    Begin();
  }

  Log::~Log() {
    End();
    out.close();
  }

  void Log::LogMessage(const std::string &message) {
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    out << std::put_time(std::localtime(&now), "%F %T %Z") << ": " << message << std::endl;
  }

  void Log::Begin() {
    std::ostringstream to_string;
    LogMessage("Beginning session.");
  }

  void Log::End() {
    LogMessage("Ending session.");
  }

}  // namespace textengine
