#include <iostream>
#include <ctime>
#include <cmath>
#include <algorithm>
#include "Game.hpp"
using namespace std;
using namespace sf;

Game::Game() : m_loadingText(m_loadingFont),
							 m_bgSprite(m_bgTexture),
							 m_fpsText(m_fpsFont),
							 m_fpsErrorRect({100.f, 100.f}),
							 m_menuButtonSound(m_menuButtonSoundBuffer),
							 m_gameMusic(m_gameMusicBuffer),
							 m_deathSound(m_deathSoundBuffer),
							 m_menu(static_cast<float>(VideoMode::getDesktopMode().size.x), static_cast<float>(VideoMode::getDesktopMode().size.y)),
							 m_player(static_cast<float>(VideoMode::getDesktopMode().size.x), static_cast<float>(VideoMode::getDesktopMode().size.y)),
							 m_enemyBar(m_enemyBarTexture),
							 m_enemyBar2(m_enemyBar2Texture)
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

	//! SYSTEM OBJECTS
	if (m_gameWindowIcon.loadFromFile("assets/images/app_icon.png"))
		m_gameWindow.setIcon(m_gameWindowIcon);
	else
		cerr << "Icon error!" << endl;

	//* ENEMIES
	if (m_enemyBarTexture.loadFromFile("assets/images/enemy_bar1.png"))
		m_enemyBar.setTexture(m_enemyBarTexture);
	else
		cerr << "Enemy Bar texture error!" << endl;
	if (m_enemyBar2Texture.loadFromFile("assets/images/enemy_bar2.png"))
		m_enemyBar2.setTexture(m_enemyBar2Texture);
	else
		cerr << "Enemy Bar 2 texture error!" << endl;

	Texture tempTexture;

	if (tempTexture.loadFromFile("assets/images/enemy_can1.png"))
		m_enemyCanTextures.push_back(tempTexture);
	else
		cerr << "Enemy 1 texture error!" << endl;
	if (tempTexture.loadFromFile("assets/images/enemy_can2.png"))
		m_enemyCanTextures.push_back(tempTexture);
	else
		cerr << "Enemy 2 texture error!" << endl;
	if (tempTexture.loadFromFile("assets/images/enemy_can3.png"))
		m_enemyCanTextures.push_back(tempTexture);
	else
		cerr << "Enemy 3 texture error!" << endl;
	if (tempTexture.loadFromFile("assets/images/enemy_can4.png"))
		m_enemyCanTextures.push_back(tempTexture);
	else
		cerr << "Enemy 4 texture error!" << endl;
	if (tempTexture.loadFromFile("assets/images/enemy_can5.png"))
		m_enemyCanTextures.push_back(tempTexture);
	else
		cerr << "Enemy 5 texture error!" << endl;
	m_gameWindow.setVerticalSyncEnabled(true);
	if (!m_bgTexture.loadFromFile("assets/images/background.jpg"))
		cerr << "Background error!" << endl;
	if (!m_gameMusicBuffer.loadFromFile("assets/sound/game_music.ogg"))
		cerr << "Game music error!" << endl;
	m_gameMusic.setLooping(true);
	m_menuButtonSound.setVolume(50.0f);
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
	m_menu.loadAssets();
	m_menu.setupMenuButtons(m_currentGameState, m_currentWindowSize.x, m_currentWindowSize.y, m_gameSettings, m_lastGameState);
	if (!m_menuButtonSoundBuffer.loadFromFile("assets/sound/button.ogg"))
		cerr << "Menu sound error!" << endl;
	if (!m_deathSoundBuffer.loadFromFile("assets/sound/death.ogg"))
		cerr << "Death sound error!" << endl;
	m_deathSound.setVolume(50.0f);
	m_gameClock.restart();
}

void Game::run()
{
	//* === GAME LOOP ===
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
						if (m_currentGameState == GameState::MainMenu || m_currentGameState == GameState::GameOver)
						{
							startNewGame();
							if (m_player.hasPlayerMoved())
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
							if (m_currentGameState == GameState::MainMenu || (m_currentGameState == GameState::Settings && m_lastGameState == GameState::MainMenu))
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
					else if (m_currentGameState == GameState::GameOver)
					{
						m_currentGameState = GameState::MainMenu;
						m_menu.setupMenuButtons(m_currentGameState, currentW, currentH, m_gameSettings, m_lastGameState);
					}
				}

				if ((m_currentGameState == GameState::MainMenu && keyPressed->code == Keyboard::Key::Enter) || (m_currentGameState == GameState::GameOver && keyPressed->code == Keyboard::Key::Enter))
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

		//* === UPDATE LOGIC ===
		switch (m_currentGameState)
		{
		case GameState::Playing:
		{
			m_player.update(dt, currentW, currentH, m_machineLeftWall, m_machineRightWall);
			if (m_player.hasPlayerMoved() &&
					m_gameMusic.getStatus() != Sound::Status::Playing &&
					m_gameSettings.playMusic)
			{
				m_gameMusic.setVolume(15.0f);
				m_gameMusic.play();
			}
			if (m_player.hasPlayerMoved())
			{
				m_spawnTimer -= dt.asSeconds();
				if (m_spawnTimer < 0)
				{
					Enemy tempEnemy(0, 0);

					Type currentType;
					const Texture *currentTexture = nullptr;
					if (rand() % 100 <= 20)
					{
						currentType = Type::Bar;
						if (rand() % 100 <= 49)
						{
							currentTexture = &m_enemyBarTexture;
						}
						else
						{
							currentTexture = &m_enemyBar2Texture;
							currentType = Type::Bar2;
						}
					}
					else
					{
						currentType = Type::Can;
						currentTexture = &m_enemyCanTextures[rand() % (m_enemyCanTextures.size())];
					}

					float enemyScale = 0.2f;
					float halfWidth = (currentTexture->getSize().x * enemyScale) / 2.0f;

					float minX = m_machineLeftWall + halfWidth;
					float maxX = currentW - m_machineRightWall - halfWidth;
					float spawnX = minX + (rand() % static_cast<int>(maxX - minX));
					float spawnY;
					float finalSpeed;
					if (m_gameSettings.gameDifficulty == GameDifficulty::Easy)
					{
						if (currentType == Type::Bar || currentType == Type::Bar2)
							finalSpeed = -500.0f;
						else
							finalSpeed = 900.0f;
						m_spawnTimer = 0.4f;
					}
					else if (m_gameSettings.gameDifficulty == GameDifficulty::Normal)
					{
						if (currentType == Type::Bar || currentType == Type::Bar2)
							finalSpeed = -700.0f;
						else
							finalSpeed = 1000.0f;
						m_spawnTimer = 0.3f;
					}
					else if (m_gameSettings.gameDifficulty == GameDifficulty::Hard)
					{
						if (currentType == Type::Bar || currentType == Type::Bar2)
							finalSpeed = -900.0f;
						else 
							finalSpeed = 1100.0f;
						m_spawnTimer = 0.25f;
					}

					if (currentType == Type::Bar || currentType == Type::Bar2)
						spawnY = currentH + 100.0f;
					else
						spawnY = -300.0f;

					tempEnemy.spawn(spawnX, spawnY, finalSpeed, *currentTexture, currentType);
					m_enemies.push_back(tempEnemy);
				}

				for (auto &enemy : m_enemies)
				{
					enemy.update(dt, currentW, currentH, m_machineLeftWall, m_machineRightWall);
				}

				for (auto it = m_enemies.begin(); it != m_enemies.end();)
				{
					if (it->getPosition().y > currentH + 200.0f || it->getPosition().y < -400.0f)
					{
						it = m_enemies.erase(it);
					}
					else
					{
						++it;
					}
				}
			}

			const auto &playerCircles = m_player.getHitboxes();

			bool isHit = false;

			for (auto &enemy : m_enemies)
			{
				if (enemy.getType() == Type::Can)
				{
					const auto &enemyCircles = enemy.getHitboxes();
					for (const auto &pCircle : playerCircles)
					{
						for (const auto &eCircle : enemyCircles)
						{
							float dx = pCircle.center.x - eCircle.center.x;
							float dy = pCircle.center.y - eCircle.center.y;
							float distanceSqr = dx * dx + dy * dy;
							float radiusSum = pCircle.radius + eCircle.radius;

							if (distanceSqr < radiusSum * radiusSum)
							{
								isHit = true;
								break;
							}
						}
						if (isHit)
							break;
					}
				}
				else
				{
					sf::FloatRect enemyBounds = enemy.getBounds();

					for (const auto &pCircle : playerCircles)
					{
						float closeX = max(enemyBounds.position.x, min(pCircle.center.x, enemyBounds.position.x + enemyBounds.size.x));
						float closeY = max(enemyBounds.position.y, min(pCircle.center.y, enemyBounds.position.y + enemyBounds.size.y));

						float dx = pCircle.center.x - closeX;
						float dy = pCircle.center.y - closeY;

						if ((dx * dx + dy * dy) < (pCircle.radius * pCircle.radius))
						{
							isHit = true;
							break;
						}
					}
				}

				if (isHit)
					break;
			}

			if (isHit)
			{
				m_player.loseHealth(dt);
				if (m_player.getHealth() <= 0)
				{
					m_currentGameState = GameState::GameOver;
					m_player.updateSound();
					m_deathSound.play();
					m_menu.setupMenuButtons(m_currentGameState, currentW, currentH, m_gameSettings, m_lastGameState);
					m_gameMusic.stop();
				}
			}

			break;
		}

		case GameState::MainMenu:
		{
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

		//* === DRAW LOGIC ===
		m_gameWindow.clear(Color::Black);

		if (m_currentGameState == GameState::GameOver)
		{
			m_player.draw(m_gameWindow, m_gameSettings);
			m_menu.draw(m_gameWindow, m_currentGameState);
		}
		else
		{
			m_gameWindow.draw(m_bgSprite);

			if (m_currentGameState == GameState::Playing || m_currentGameState == GameState::Paused || m_currentGameState == GameState::Settings)
			{
				m_player.draw(m_gameWindow, m_gameSettings);
				for (auto &enemy : m_enemies)
				{
					enemy.draw(m_gameWindow, m_gameSettings);
				}
			}

			if (m_currentGameState == GameState::Paused || m_currentGameState == GameState::MainMenu || m_currentGameState == GameState::Settings)
			{
				m_menu.draw(m_gameWindow, m_currentGameState);
			}
			if (!m_fpsFontIsLoaded)
				m_gameWindow.draw(m_fpsErrorRect);
			else if (m_gameSettings.showFps)
				m_gameWindow.draw(m_fpsText);
		}
		m_gameWindow.display();
	}
};

void Game::startNewGame()
{
	int difficultyHP = 3;
	float difficultyPlayerSpeed = 550.0f;
	float difficultyInvincibility = 2.0f;

	switch (m_gameSettings.gameDifficulty)
	{
	case GameDifficulty::Easy:
	{
		difficultyHP = 4;
		difficultyPlayerSpeed = 650.0f;
		difficultyInvincibility = 3.0f;
		break;
	}
	case GameDifficulty::Normal:
	{
		break;
	}
	case GameDifficulty::Hard:
	{
		difficultyHP = 1;
		difficultyPlayerSpeed = 500.0f;
		difficultyInvincibility = 0.0f;
		break;
	}
	}

	m_player.resetGame(m_startPosX, m_startPosY, difficultyHP, difficultyPlayerSpeed, difficultyInvincibility, m_gameSettings.gameDifficulty);

	m_gameClock.restart();
	m_spawnTimer = 0.0f;
	m_enemies.clear();
}