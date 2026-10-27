#include "logger.h"
#include "errors.h"
#include <ctime>
#include <iomanip>

Logger::Logger(const std::string& name)
{
    f.open(name, std::ios::app);
    if (!f.is_open())
        throw IOError("Cannot open log file: " + name, true);
}

void Logger::write(bool critical, const std::string& msg)
{
    std::time_t t = std::time(nullptr);
    f << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S") << " | "
      << (critical ? "CRITICAL" : "NON-CRITICAL") << " | "
      << msg << std::endl;
}
