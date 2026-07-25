#include "Game.hpp"
#include "Logger.hpp"
#include <csignal>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
LONG WINAPI unhandledExceptionFilter(EXCEPTION_POINTERS* ep) {
    Logger::error("CRASH DETECTED: Unhandled Windows Exception! Code: 0x" + 
                  std::to_string(ep->ExceptionRecord->ExceptionCode));
    Logger::shutdown();
    return EXCEPTION_EXECUTE_HANDLER;
}
#else
void signalHandler(int signum) {
    Logger::error("Caught signal: " + std::to_string(signum));
    Logger::shutdown();
    std::exit(signum);
}
#endif

int main()
{
    Logger::init();
    
#ifdef _WIN32
    SetUnhandledExceptionFilter(unhandledExceptionFilter);
#else
    std::signal(SIGSEGV, signalHandler);
    std::signal(SIGABRT, signalHandler);
    std::signal(SIGILL, signalHandler);
    std::signal(SIGFPE, signalHandler);
#endif
    
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