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
	GameOver
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

struct GameSettings
{
	GameDifficulty gameDifficulty = GameDifficulty::Easy;
	WindowMode windowMode = WindowMode::Fullscreen;
	bool playMusic = false;
	bool debugMode = true;
	bool showFps = true;
};