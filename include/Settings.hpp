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

struct GameSettings
{
	GameDifficulty gameDifficulty = GameDifficulty::Normal;
	WindowMode windowMode = WindowMode::Fullscreen;
	bool playMusic = true;
	bool VSync = true;
	bool showFps = false;
	bool showHitbox = false;
};