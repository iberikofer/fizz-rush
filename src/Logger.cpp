#include "Logger.hpp"
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace Logger {
const std::string LOG_FILE = "log.txt";
const size_t MAX_SIZE = 1024 * 1024; //* 1 MB

std::ofstream fileStream;
std::streambuf *coutBuf = nullptr;
std::streambuf *cerrBuf = nullptr;

void init() {
  if (std::filesystem::exists(LOG_FILE) &&
      std::filesystem::file_size(LOG_FILE) >= MAX_SIZE) {
    std::filesystem::remove(LOG_FILE);
  }

  fileStream.open(LOG_FILE, std::ios::app);
  if (fileStream.is_open()) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    fileStream << "\n========================================\n";
    fileStream << "Session started at: "
               << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S")
               << "\n";
    fileStream << "========================================\n";

    //? Redirect cout and cerr
    coutBuf = std::cout.rdbuf(fileStream.rdbuf());
    cerrBuf = std::cerr.rdbuf(fileStream.rdbuf());
  }
}

void shutdown() {
  if (coutBuf)
    std::cout.rdbuf(coutBuf);
  if (cerrBuf)
    std::cerr.rdbuf(cerrBuf);
  if (fileStream.is_open())
    fileStream.close();
}

void log(const std::string &message) {
  if (fileStream.is_open()) {
    fileStream << "[INFO] " << message << "\n";
    fileStream.flush();
  }
}

void error(const std::string &message) {
  if (fileStream.is_open()) {
    fileStream << "[ERROR] " << message << "\n";
    fileStream.flush();
  }
}
} // namespace Logger
