#pragma once
#include <string>

namespace Logger {
    void init();
    void shutdown();
    void log(const std::string& message);
    void error(const std::string& message);
}
