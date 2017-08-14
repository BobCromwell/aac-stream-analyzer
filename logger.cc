#include "logger.h"

namespace aac_sa {

  Logger::Logger()
  {
  }

  Logger* Logger::instance(){
    static Logger logger;
    return &logger;
  }

  void Logger::error(const char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
  }

  void Logger::info(const char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
  }
} // namespace aac-sa
