#include "Settings.hpp"
#include <fstream>
#include <sstream>
#include <string>

void GameSettings::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        if (std::getline(iss, key, '=')) {
            std::string value;
            if (std::getline(iss, value)) {
                try {
                    int intValue = std::stoi(value);
                    if (key == "difficulty") gameDifficulty = static_cast<GameDifficulty>(intValue);
                    else if (key == "windowMode") windowMode = static_cast<WindowMode>(intValue);
                    else if (key == "playMusic") playMusic = intValue;
                    else if (key == "VSync") VSync = intValue;
                    else if (key == "showFps") showFps = intValue;
                    else if (key == "showHitbox") showHitbox = intValue;
                    else if (key == "ukrainianLanguage") ukrainianLanguage = intValue;
                } catch (...) {
                    //? Ignore parsing errors for individual lines
                }
            }
        }
    }
}

void GameSettings::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return;

    file << "difficulty=" << static_cast<int>(gameDifficulty) << "\n";
    file << "windowMode=" << static_cast<int>(windowMode) << "\n";
    file << "playMusic=" << playMusic << "\n";
    file << "VSync=" << VSync << "\n";
    file << "showFps=" << showFps << "\n";
    file << "showHitbox=" << showHitbox << "\n";
    file << "ukrainianLanguage=" << ukrainianLanguage << "\n";
}
