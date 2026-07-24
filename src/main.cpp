#include "Game.hpp"
#include "Logger.hpp"
#include <csignal>
#include <cstdlib>

void signalHandler(int signum) {
    Logger::error("Caught signal: " + std::to_string(signum));
    Logger::shutdown();
    std::exit(signum);
}

int main()
{
    Logger::init();
    
    std::signal(SIGSEGV, signalHandler);
    std::signal(SIGABRT, signalHandler);
    std::signal(SIGILL, signalHandler);
    std::signal(SIGFPE, signalHandler);
    
    try {
        Game* game = new Game();
        game->run();
        delete game;
    } catch (const std::exception& e) {
        Logger::error(std::string("Fatal Exception: ") + e.what());
    } catch (...) {
        Logger::error("Fatal Unknown Exception");
    }
    
    Logger::shutdown();
    return 0;
}