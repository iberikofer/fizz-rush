#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Settings.hpp"

class Menu
{
public:
	Menu(float winWidth, float winHeight);
	void loadAssets();
	void updateMusicVolume(bool isPlaying);
	void setupMenuButtons(GameState m_gameState, float winWidth, float winHeight, GameSettings &settings, GameState m_lastGameState);
	int mouseClickPos(int mouseX, int mouseY, GameState m_gameState);
	void draw(sf::RenderWindow &window, GameState m_gameState);

private:
	sf::RectangleShape m_menuBG;
	sf::Font m_menuFont;
	sf::Text m_gameNameLogo;
	sf::Text m_playButtonText;
	sf::RectangleShape m_playButton;
	sf::Text m_settingsButtonText;
	sf::RectangleShape m_settingsButton;
	sf::Text m_exitButtonText;
	sf::RectangleShape m_exitButton;

	sf::Text m_settingsTitle;
	sf::Text m_difficultyText;
	sf::RectangleShape m_difficultyButton;
	sf::Text m_FPSCounterText;
	sf::RectangleShape m_FPSCounterButton;
	sf::Text m_debugText;
	sf::RectangleShape m_debugButton;
	sf::Text m_backButtonText;
	sf::RectangleShape m_backButton;
	sf::Text m_musicText;
	sf::RectangleShape m_musicButton;

	sf::SoundBuffer m_menuMusicBuffer;
	sf::Sound m_menuMusic;

	bool m_isDifficultyLocked;
};