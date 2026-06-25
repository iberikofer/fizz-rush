#pragma once
#include "Menu.hpp"
#include "Settings.hpp"
#include "Player.hpp"
#include "Enemy.hpp"

class Game
{
public:
	Game();
	void run();

private:
	//* SETTINGS
	std::string m_gameWindowName = "Fizz Rush!";
	sf::RenderWindow m_gameWindow;
	sf::Image m_gameWindowIcon;
	GameSettings m_gameSettings;
	GameState m_currentGameState;
	GameState m_lastGameState;
	int currentEpisode = 1;
	void startNewGame();

	//* SYSTEM OBJECTS
	sf::Font m_loadingFont;
	sf::Text m_loadingText;
	sf::Texture m_bgTexture;
	sf::Sprite m_bgSprite;
	float m_startPosX;
	float m_startPosY;
	sf::Clock m_gameClock;
	bool m_isFullscreen = false;
	sf::Font m_fpsFont;
	sf::Text m_fpsText;
	sf::RectangleShape m_fpsErrorRect;
	float m_fpsUpdateTimer = 0.0f;
	float m_fpsMargin = 135.0f;
	bool m_fpsFontIsLoaded = false;
	sf::Vector2u m_currentWindowSize;
	int m_menuClickType;
	float m_machineLeftWall;
	float m_machineRightWall;
	sf::SoundBuffer m_menuButtonSoundBuffer;
	sf::Sound m_menuButtonSound;
	sf::SoundBuffer m_gameMusicSoundBuffer;
	sf::Sound m_gameMusic;

	//* === GAME OBJETS ===
	Menu m_menu;
	Player m_player;
	Enemy m_enemy;
};