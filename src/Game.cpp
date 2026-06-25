#include <iostream>
#include <ctime>
#include "Game.hpp"
using namespace std;
using namespace sf;

Game::Game() : m_loadingText(m_loadingFont),
	m_player(static_cast<float>(VideoMode::getDesktopMode().size.x), static_cast<float>(VideoMode::getDesktopMode().size.y)),
	m_enemy(0.0f, -300.0f),
	m_bgSprite(m_bgTexture),
							 m_menu(static_cast<float>(VideoMode::getDesktopMode().size.x), static_cast<float>(VideoMode::getDesktopMode().size.y)),
							 m_fpsText(m_fpsFont),
							 m_fpsErrorRect({100.f, 100.f}),
							 m_menuButtonSound(m_menuButtonSoundBuffer), m_gameMusic(m_gameMusicSoundBuffer)
{
	m_gameWindow.create(VideoMode::getDesktopMode(), "Fizz Rush!", Style::None, State::Fullscreen);

	m_gameWindow.clear(Color::Black);
	m_gameWindow.display();

	while (m_gameWindow.pollEvent())
	{
		// DECOY CYCLE, TO PREVENT WHITE SCREEN FLASH ON THE START
	}

	//! CONFIG
	if (!m_loadingFont.openFromFile("assets/fonts/Lilita_One.ttf"))
		cerr << "Menu font error!" << endl;
	m_loadingText.setString("LOADING...=)");
	m_loadingText.setCharacterSize(120);
	m_loadingText.setFillColor(Color::White);
	FloatRect m_loadingTextPos = m_loadingText.getLocalBounds();
	m_loadingText.setOrigin({m_loadingTextPos.position.x + m_loadingTextPos.size.x / 2.0f,
													 m_loadingTextPos.position.y + m_loadingTextPos.size.y / 2.0f});
	m_loadingText.setPosition({m_gameWindow.getSize().x / 2.0f, m_gameWindow.getSize().y / 2.0f});
	m_gameWindow.clear(Color::Black);
	m_gameWindow.draw(m_loadingText);
	m_gameWindow.display();
	srand(time(0));
	if (m_gameWindowIcon.loadFromFile("assets/images/app_icon.png"))
		m_gameWindow.setIcon(m_gameWindowIcon);
	else
		cerr << "Icon error!" << endl;

	//! SYSTEM OBJECTS
	m_gameWindow.setVerticalSyncEnabled(true);
	if (!m_bgTexture.loadFromFile("assets/images/background.jpg"))
		cerr << "Background error!" << endl;
	if (!m_gameMusicSoundBuffer.loadFromFile("assets/sound/game_music.ogg"))
		cerr << "Game music error!" << endl;
	m_gameMusic.setLooping(true);
	m_bgSprite.setTexture(m_bgTexture, true);
	float m_bgScaleX = m_gameWindow.getSize().x / static_cast<float>(m_bgTexture.getSize().x);
	float m_bgScaleY = m_gameWindow.getSize().y / static_cast<float>(m_bgTexture.getSize().y);
	m_bgSprite.setScale({m_bgScaleX, m_bgScaleY});
	m_startPosX = m_gameWindow.getSize().x / 2.0f + 103.5f;
	m_startPosY = m_gameWindow.getSize().y / 2.0f - 185.0f;
	m_currentWindowSize = m_gameWindow.getSize();
	m_currentGameState = GameState::MainMenu;
	m_machineLeftWall = 87.0f;
	m_machineRightWall = 130.0f;
	m_fpsText.setCharacterSize(34);
	m_fpsText.setFillColor(Color(255, 220, 0));
	m_fpsText.setOutlineColor(Color(150, 100, 0));
	m_fpsText.setOutlineThickness(4.0f);
	if (m_fpsFont.openFromFile("assets/fonts/Nabla.ttf"))
	{
		m_fpsFontIsLoaded = true;
		m_fpsText.setPosition({m_currentWindowSize.x - m_fpsMargin, 5.0f});
		m_fpsText.setString("FPS: -");
	}
	else
	{
		cerr << "Font error!" << endl;
		m_fpsFontIsLoaded = false;
	}
	m_fpsErrorRect.setFillColor(Color::Red);
	m_fpsErrorRect.setPosition({m_currentWindowSize.x - m_fpsMargin, 50.0f});
	m_player.loadAssets(m_startPosX, m_startPosY);
	m_enemy.loadAssets();
	m_menu.loadAssets();
	m_menu.setupMenuButtons(m_currentGameState, m_currentWindowSize.x, m_currentWindowSize.y, m_gameSettings, m_lastGameState);
	if (!m_menuButtonSoundBuffer.loadFromFile("assets/sound/button.ogg"))
		cerr << "Menu sound error!" << endl;
	m_gameClock.restart();
}

void Game::run()
{
	while (m_gameWindow.isOpen())
	{
		float currentW = static_cast<float>(m_currentWindowSize.x);
		float currentH = static_cast<float>(m_currentWindowSize.y);
		Time dt = m_gameClock.restart();
		m_fpsUpdateTimer += dt.asSeconds();
		float FPS = 1.0f / dt.asSeconds();
		if (m_fpsUpdateTimer >= 0.3f)
		{
			if (m_fpsFontIsLoaded)
			{
				m_fpsText.setString("FPS: " + to_string(int(FPS)));
			}
			m_fpsUpdateTimer = 0.0f;
		}

		while (const optional event = m_gameWindow.pollEvent())
		{
			if (event->is<Event::Closed>())
			{
				m_gameWindow.close();
			}
			else if (const auto *mouseClick = event->getIf<Event::MouseButtonPressed>())
			{
				if (mouseClick->button == Mouse::Button::Left)
				{
					int m_menuClickType = m_menu.mouseClickPos(mouseClick->position.x, mouseClick->position.y, m_currentGameState);

					if (m_menuClickType == 1)
					{
						m_menuButtonSound.play();
						if (m_currentGameState == GameState::MainMenu)
						{
							startNewGame();
							m_gameMusic.play();
						}
						m_currentGameState = GameState::Playing;
						if (m_gameSettings.playMusic)
							m_gameMusic.setVolume(15.0f);
						else
						{
							m_gameMusic.setVolume(0);
						}

						m_menu.setupMenuButtons(m_currentGameState, m_currentWindowSize.x, m_currentWindowSize.y, m_gameSettings, m_lastGameState);
					}
					else if (m_menuClickType == 2)
					{
						m_menuButtonSound.play();
						m_lastGameState = m_currentGameState;
						m_currentGameState = GameState::Settings;
						m_menu.setupMenuButtons(m_currentGameState, currentW, currentH, m_gameSettings, m_lastGameState);
					}
					else if (m_menuClickType == 3)
					{
						m_menuButtonSound.play();
						if (m_gameSettings.gameDifficulty == GameDifficulty::Easy)
							m_gameSettings.gameDifficulty = GameDifficulty::Normal;
						else if (m_gameSettings.gameDifficulty == GameDifficulty::Normal)
							m_gameSettings.gameDifficulty = GameDifficulty::Hard;
						else
							m_gameSettings.gameDifficulty = GameDifficulty::Easy;
						m_menu.setupMenuButtons(m_currentGameState, currentW, currentH, m_gameSettings, m_lastGameState);
					}
					else if (m_menuClickType == 4)
					{
						m_gameSettings.playMusic = !m_gameSettings.playMusic;
						if (m_gameSettings.playMusic)
						{
							if (m_currentGameState == GameState::MainMenu || m_currentGameState == GameState::Settings && m_lastGameState == GameState::MainMenu)
							{
								m_menu.updateMusicVolume(true);
								m_gameMusic.setVolume(0);
							}
							else
							{
								m_menu.updateMusicVolume(false);
							}
						}
						else
						{
							m_gameMusic.setVolume(0);
							m_menu.updateMusicVolume(false);
						}

						m_menuButtonSound.play();
						m_menu.setupMenuButtons(m_currentGameState, currentW, currentH, m_gameSettings, m_lastGameState);
					}
					else if (m_menuClickType == 5)
					{
						m_menuButtonSound.play();
						m_gameSettings.showFps = !m_gameSettings.showFps;
						m_menu.setupMenuButtons(m_currentGameState, currentW, currentH, m_gameSettings, m_lastGameState);
					}
					else if (m_menuClickType == 6)
					{
						m_menuButtonSound.play();
						m_gameSettings.debugMode = !m_gameSettings.debugMode;
						m_menu.setupMenuButtons(m_currentGameState, currentW, currentH, m_gameSettings, m_lastGameState);
					}
					else if (m_menuClickType == 7)
					{
						m_menuButtonSound.play();
						m_currentGameState = m_lastGameState;
						m_menu.setupMenuButtons(m_currentGameState, currentW, currentH, m_gameSettings, m_lastGameState);
					}
					else if (m_menuClickType == 8)
					{
						m_menuButtonSound.play();
						if (m_currentGameState == GameState::MainMenu)
							m_gameWindow.close();
						else
						{
							m_currentGameState = GameState::MainMenu;
							m_gameMusic.stop();
							m_menu.setupMenuButtons(m_currentGameState, currentW, currentH, m_gameSettings, m_lastGameState);
						}
					}
				}
			}
			else if (const auto *keyPressed = event->getIf<Event::KeyPressed>())
			{
				if (keyPressed->code == Keyboard::Key::Escape)
				{
					if (m_currentGameState == GameState::Playing)
					{
						m_currentGameState = GameState::Paused;
						m_menu.setupMenuButtons(m_currentGameState, currentW, currentH, m_gameSettings, m_lastGameState);
						m_gameMusic.setVolume(0);
						m_player.updateSound();
					}
					else if (m_currentGameState == GameState::Paused)
					{
						m_menu.setupMenuButtons(m_currentGameState, currentW, currentH, m_gameSettings, m_lastGameState);
						m_currentGameState = GameState::Playing;
						if (m_gameSettings.playMusic)
							m_gameMusic.setVolume(15.0f);
						else
							m_gameMusic.setVolume(0);
					}
					else if (m_currentGameState == GameState::Settings)
					{
						m_currentGameState = m_lastGameState;
						m_menu.setupMenuButtons(m_currentGameState, currentW, currentH, m_gameSettings, m_lastGameState);
					}
				}

				if (m_currentGameState == GameState::MainMenu && keyPressed->code == Keyboard::Key::Enter)
				{
					startNewGame();
					if (m_gameSettings.playMusic)
						m_gameMusic.setVolume(15.0f);
					else
						m_gameMusic.setVolume(0);
					m_currentGameState = GameState::Playing;
					m_menu.setupMenuButtons(m_currentGameState, m_currentWindowSize.x, m_currentWindowSize.y, m_gameSettings, m_lastGameState);
				}
				if (m_currentGameState == GameState::Paused && keyPressed->code == Keyboard::Key::Enter)
				{
					m_currentGameState = GameState::Playing;
					if (m_gameSettings.playMusic)
						m_gameMusic.setVolume(15.0f);
					else
						m_gameMusic.setVolume(0);
				}

				if (keyPressed->code == Keyboard::Key::Tab)
					m_gameWindow.close();
				if (keyPressed->code == Keyboard::Key::F11)
				{
					m_isFullscreen = !m_isFullscreen;

					if (m_isFullscreen)
					{
						m_gameWindow.create(VideoMode::getDesktopMode(), m_gameWindowName, Style::Default);
						m_gameWindow.setPosition({0, 0});
						m_currentWindowSize = m_gameWindow.getSize();
					}
					else
					{
						m_gameWindow.create(VideoMode::getDesktopMode(), m_gameWindowName, State::Fullscreen);
					}
					m_currentWindowSize = m_gameWindow.getSize();
					m_gameWindow.setVerticalSyncEnabled(true);
					if (m_gameWindowIcon.getSize().x > 0)
						m_gameWindow.setIcon(m_gameWindowIcon);
					m_bgSprite.setScale({static_cast<float>(m_currentWindowSize.x) / static_cast<float>(m_bgTexture.getSize().x), static_cast<float>(m_currentWindowSize.y) / static_cast<float>(m_bgTexture.getSize().y)});
					m_fpsText.setPosition({m_currentWindowSize.x - m_fpsMargin, 0.0f});
					m_fpsErrorRect.setPosition({m_currentWindowSize.x - m_fpsMargin, 0.0f});
				}
			}
		}

		switch (m_currentGameState)
		{
		case GameState::Playing:
		{
			m_player.update(dt, currentW, currentH, m_machineLeftWall, m_machineRightWall);
			if (m_player.hasPlayerMoved())
			{
				m_enemy.update(dt, currentW, currentH, m_machineLeftWall, m_machineRightWall);
			}

			if (m_player.getHitbox().findIntersection(m_enemy.getHitbox()))
			{
				m_player.loseHealth(dt);
			}
			break;
		}

		case GameState::MainMenu:
		{
			break;
		}

		case GameState::Paused:
		{
			break;
		}
		
		case GameState::Settings:
		case GameState::GameOver:
		{
			break;
		}
		}

		m_gameWindow.clear(Color::Black);
		m_gameWindow.draw(m_bgSprite);

		if (m_currentGameState == GameState::Playing || m_currentGameState == GameState::Paused || m_currentGameState == GameState::Settings)
		{
			m_player.draw(m_gameWindow, m_gameSettings);
			m_enemy.draw(m_gameWindow, m_gameSettings);
		}

		if (m_currentGameState == GameState::Paused || m_currentGameState == GameState::MainMenu || m_currentGameState == GameState::Settings)
		{
			m_menu.draw(m_gameWindow, m_currentGameState);
		}
		if (!m_fpsFontIsLoaded)
			m_gameWindow.draw(m_fpsErrorRect);
		else if (m_gameSettings.showFps)
			m_gameWindow.draw(m_fpsText);
		m_gameWindow.display();
	}
};

void Game::startNewGame()
{
	int difficultyHP = 3;
	float difficultyPlayerSpeed;
	float difficultyEnemySpeed;
	float difficultyInvincibility;

	currentEpisode = 1;

	switch (m_gameSettings.gameDifficulty)
	{
	case GameDifficulty::Easy:
	{
		difficultyHP = 4;
		difficultyPlayerSpeed = 600.0f;
		difficultyEnemySpeed = 1000.0f;
		difficultyInvincibility = 3.0f;
		break;
	}
	case GameDifficulty::Normal:
	{
		difficultyHP = 3;
		difficultyPlayerSpeed = 500.0f;
		difficultyEnemySpeed = 1100.0f;
		difficultyInvincibility = 2.0f;
		break;
	}
	case GameDifficulty::Hard:
	{
		difficultyHP = 1;
		difficultyPlayerSpeed = 400.0f;
		difficultyEnemySpeed = 1300.0f;
		difficultyInvincibility = 0.0f;
		break;
	}
	}

	m_player.resetGame(m_startPosX, m_startPosY, difficultyHP, difficultyPlayerSpeed, difficultyInvincibility);

	m_enemy.reset(m_startPosX - 40.0f, -300.0f, difficultyEnemySpeed);

	m_gameClock.restart();
}