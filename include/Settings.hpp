#pragma once
#include <SFML/Graphics.hpp>

struct CollisionCircle
{
	sf::Vector2f center;
	float radius;
};

enum class GameState
{
	MainMenu,
	Playing,
	Paused,
	Settings,
	Controls,
	GameOver,
	EpisodeTransition
};

enum class WindowMode
{
	Windowed,
	WindowedFull,
	Fullscreen
};

enum class GameDifficulty
{
	Easy,
	Normal,
	Hard
};

#include <string>

struct GameSettings
{
	GameDifficulty gameDifficulty = GameDifficulty::Normal;
	WindowMode windowMode = WindowMode::Fullscreen;
	bool playMusic = true;
	bool VSync = true;
	bool showFps = false;
	bool showHitbox = false;
	bool ukrainianLanguage = false;

	void loadFromFile(const std::string& filename);
	void saveToFile(const std::string& filename) const;
};