#pragma once
#include <SFML/Graphics.hpp>

struct CollisionCircle {
  sf::Vector2f center;
  float radius;
};

enum class GameState {
  MainMenu,
  Playing,
  Paused,
  Settings,
  Controls,
  GameOver,
  EpisodeTransition
};

enum class WindowMode { Windowed, WindowedFull, Fullscreen };

enum class GameDifficulty { Easy, Normal, Hard };

#include <string>

struct GameSettings {
  GameDifficulty gameDifficulty = GameDifficulty::Normal;
  WindowMode windowMode = WindowMode::Fullscreen;
  bool playMusic = true;
  bool playSfx = true;
  bool VSync = true;
  bool showFps = false;
  bool showHitbox = false;
  bool ukrainianLanguage = false;

  void loadFromFile(const std::string &filename);
  void saveToFile(const std::string &filename) const;
};

namespace AudioConfig {
//* Menu
constexpr float MENU_MUSIC = 40.0f;
constexpr float BUTTON_SELECT = 50.0f;
constexpr float BUTTON_SWITCH = 50.0f;

//* Gameplay
constexpr float GAME_MUSIC = 30.0f;
constexpr float BOSS_MUSIC = 45.0f;
constexpr float WIN_MUSIC = 50.0f;

//* Player
constexpr float PLAYER_WALL = 20.0f;
constexpr float PLAYER_ROLL = 45.0f;
constexpr float PLAYER_HIT = 50.0f;

//* UI / SFX
constexpr float COOKIE = 20.0f;
constexpr float HEAL = 50.0f;
constexpr float HEART_SPAWN = 25.0f;
constexpr float DEATH = 50.0f;
constexpr float TRANSITION = 50.0f;
constexpr float WIN_SOUND = 100.0f;
constexpr float GAME_LOADING = 60.0f;
constexpr float GAME_START = 100.0f;
constexpr float DEATH_FIZZ = 70.0f;

//* Boss
constexpr float BOSS_ATTACK = 50.0f;
constexpr float BOSS_WARNING = 50.0f;
constexpr float BOSS_DEATH = 50.0f;
constexpr float RICOCHET = 50.0f;
} // namespace AudioConfig