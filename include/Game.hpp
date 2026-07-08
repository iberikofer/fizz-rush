#pragma once
#include "Menu.hpp"
#include "Settings.hpp"
#include "Player.hpp"
#include "Enemy.hpp"

enum class GameEpisode
{
	VendingMachine,
	Survival,
	BossFight
};

class Game
{
public:
	Game();
	void run();

private:
	//* SETTINGS
	std::string m_gameWindowName = "Fizz Rush!";
	sf::RenderWindow m_gameWindow;
	sf::Vector2u m_currentWindowSize;
	sf::Image m_gameWindowIcon;
	GameSettings m_gameSettings;
	GameState m_currentGameState;
	GameState m_lastGameState;
	GameEpisode m_currentEpisode;
	void startNewGame();

	//* SYSTEM OBJECTS
	sf::Font m_loadingFont;
	sf::Text m_loadingText;
	sf::Texture m_bgTexture;
	sf::Sprite m_bgSprite;
	float m_startPosX;
	float m_startPosY;
	sf::Clock m_gameClock;
	sf::Font m_fpsFont;
	sf::Text m_fpsText;
	sf::RectangleShape m_fpsErrorRect;
	float m_fpsUpdateTimer = 0.0f;
	float m_fpsMargin = 135.0f;
	bool m_fpsFontIsLoaded = false;
	bool m_isFullscreen = false;
	int m_menuClickType;
	float m_machineLeftWall;
	float m_machineRightWall;
	sf::SoundBuffer m_menuButtonSoundBuffer;
	sf::Sound m_menuButtonSound;
	sf::SoundBuffer m_gameMusicBuffer;
	sf::Sound m_gameMusic;
	sf::SoundBuffer m_deathSoundBuffer;
	sf::Sound m_deathSound;
	float m_spawnTimer;

	//* === GAME OBJETS ===
	Menu m_menu;
	Player m_player;
	sf::Texture m_enemyBarTexture;
	sf::Sprite m_enemyBar;
	sf::Texture m_enemyBar2Texture;
	sf::Sprite m_enemyBar2;
	std::vector<sf::Texture> m_enemyCanTextures;
	std::vector<Enemy> m_enemies;
};