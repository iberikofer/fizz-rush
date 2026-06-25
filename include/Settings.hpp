#pragma once

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
 	bool playMusic = true;
	bool debugMode = false;
	bool showFps = false;
};