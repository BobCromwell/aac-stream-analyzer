#ifndef LOGGER
#define LOGGER

#include <string>

namespace aac_sa {
class Logger {
public:
  static Logger* instance();
  void error(const char* fmt, ...);
  void info(const char* fmt, ...);
private:
  Logger();
};
} // namespace aac_sa

#endif // LOGGER
