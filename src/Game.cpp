#include <iostream>
#include <ctime>
#include <cmath>
#include <algorithm>
#include "Game.hpp"
using namespace std;
using namespace sf;

	//* === INITIAL SETUP ===
Game::Game() : m_loadingText(m_loadingFont),
							 m_bgSprite(m_bgTexture),
							 m_enemyBar(m_enemyBarTexture),
							 m_enemyHand1(m_enemyHand1Texture),
							 m_enemyBar2(m_enemyBar2Texture),
							 m_enemyHand2(m_enemyHand2Texture),
							 m_enemyBox(m_enemyBoxTexture),
							 m_enemyCookie(m_enemyCookieTexture),
							 m_cookieSound(m_enemyCookieBuffer),
							 m_fpsText(m_fpsFont),
							 m_fpsErrorRect({100.f, 100.f}),
							 m_menuButtonSound(m_menuButtonSoundBuffer),
							 m_Episode1Music(m_Episode1MusicBuffer),
							 m_Episode2Music(m_Episode2MusicBuffer),
							 m_Episode3Music(m_Episode3MusicBuffer),
							 m_transitionSound(m_transitionSoundBuffer),
							 m_winSound(m_winSoundBuffer),
							 m_deathSound(m_deathSoundBuffer),
							 m_healSound(m_healSoundBuffer),
							 m_menu(static_cast<float>(VideoMode::getDesktopMode().size.x), static_cast<float>(VideoMode::getDesktopMode().size.y)),
							 m_player(static_cast<float>(VideoMode::getDesktopMode().size.x), static_cast<float>(VideoMode::getDesktopMode().size.y))
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
		cerr << "Window icon error!" << endl;
	if (!m_bgTexture.loadFromFile("assets/images/Episode_1.jpg"))
		cerr << "Background error!" << endl;
	if (!m_Episode1MusicBuffer.loadFromFile("assets/sound/Episode_1.ogg"))
		cerr << "Game music 1 error!" << endl;
	if (!m_Episode2MusicBuffer.loadFromFile("assets/sound/Episode_2.ogg"))
		cerr << "Game music 2 error!" << endl;
	if (!m_Episode3MusicBuffer.loadFromFile("assets/sound/boss_music.ogg"))
		cerr << "Boss music error!" << endl;
	if (!m_transitionSoundBuffer.loadFromFile("assets/sound/Episode_transition.ogg"))
		cerr << "Transition sound error!" << endl;
	if (!m_heartPickupTexture.loadFromFile("assets/images/heart_heal.png"))
		cerr << "Heart pickup texture error!" << endl;
	m_heartSpawnTimer = 0.0f;
	m_gameWindow.setVerticalSyncEnabled(true);
	m_Episode1Music.setLooping(true);
	m_Episode2Music.setLooping(true);
	m_Episode3Music.setLooping(true);
	m_menuButtonSound.setVolume(50.0f);
	m_cookieSound.setVolume(20.0f);
	m_bgSprite.setTexture(m_bgTexture, true);
	float m_bgScaleX = m_gameWindow.getSize().x / static_cast<float>(m_bgTexture.getSize().x);
	float m_bgScaleY = m_gameWindow.getSize().y / static_cast<float>(m_bgTexture.getSize().y);
	m_bgSprite.setScale({m_bgScaleX, m_bgScaleY});
	m_startPosX = m_gameWindow.getSize().x / 2.0f + 103.5f;
	m_startPosY = m_gameWindow.getSize().y / 2.0f - 185.0f;
	m_currentWindowSize = m_gameWindow.getSize();
	m_currentGameState = GameState::MainMenu;
	m_fpsText.setCharacterSize(40);
	m_fpsText.setFillColor(Color(255, 220, 0));
	m_fpsText.setOutlineColor(Color(150, 100, 0));
	m_fpsText.setOutlineThickness(4.0f);
	if (m_fpsFont.openFromFile("assets/fonts/Nabla.ttf"))
	{
		m_fpsFontIsLoaded = true;
		m_fpsText.setPosition({m_currentWindowSize.x - m_fpsMargin - 10.0f, 5.0f});
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
	m_boss.loadAssets();
	m_machineLeftWall = 68.5f;
	m_machineRightWall = 135.0f;
	m_machineTopWall = 16.0f;
	m_menu.loadAssets();
	m_menu.setupMenuButtons(m_currentGameState, m_currentWindowSize.x, m_currentWindowSize.y, m_gameSettings, m_lastGameState);
	if (!m_menuButtonSoundBuffer.loadFromFile("assets/sound/button.ogg"))
		cerr << "Menu sound error!" << endl;
	if (!m_winSoundBuffer.loadFromFile("assets/sound/victory.ogg"))
		cerr << "Victory sound error!" << endl;
	else
		m_winSound.setBuffer(m_winSoundBuffer);
	if (!m_deathSoundBuffer.loadFromFile("assets/sound/death.ogg"))
		cerr << "Death sound error!" << endl;
	m_deathSound.setVolume(50.0f);
	if (!m_healSoundBuffer.loadFromFile("assets/sound/heal.ogg"))
		cerr << "Death sound error!" << endl;
	m_healSound.setVolume(75.0f);
	m_gameClock.restart();
	setupEpisode(GameEpisode::VendingMachine);
	m_fadeRect.setSize(sf::Vector2f(static_cast<float>(m_currentWindowSize.x), static_cast<float>(m_currentWindowSize.y)));
	m_fadeRect.setFillColor(sf::Color(0, 0, 0, 0));
	m_fadeAlpha = 0.0f;
	m_isFadingOut = false;
	m_goToMenuAfterFade = false;
	m_fadeRect.setFillColor(sf::Color(0, 0, 0, 255));
	m_fadeAlpha = 255.0f;
	m_isFadingIn = true;
	m_isFadingOut = false;
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

						if (m_currentGameState == GameState::MainMenu)
						{
							if (!m_isFadingIn && !m_isFadingOut)
								startFadeOut(GameEpisode::BossFight);
						}
						else if (m_currentGameState == GameState::GameOver)
							startNewGame(false);
						else if (m_currentGameState == GameState::Paused)
						{
							m_currentGameState = GameState::Playing;
						}

						if (m_gameSettings.playMusic)
						{
							m_Episode1Music.setVolume(30.0f);
							m_Episode2Music.setVolume(30.0f);
							m_Episode3Music.setVolume(30.0f);
						}
						else
						{
							m_Episode1Music.setVolume(0);
							m_Episode2Music.setVolume(0);
							m_Episode3Music.setVolume(0);
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
								m_Episode1Music.setVolume(0);
								m_Episode2Music.setVolume(0);
								m_Episode3Music.setVolume(0);
							}
							else
								m_menu.updateMusicVolume(false);
						}
						else
						{
							m_Episode1Music.setVolume(0);
							m_Episode2Music.setVolume(0);
							m_Episode3Music.setVolume(0);
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
						if (!m_isFadingIn && !m_isFadingOut)
						{
							m_menuButtonSound.play();
							if (m_currentGameState == GameState::MainMenu)
								m_gameWindow.close();
							else
							{
								startFadeOutToMenu();
							}
						}
					}
				}
			}
			else if (const auto *keyPressed = event->getIf<Event::KeyPressed>())
			{
				if (keyPressed->code == Keyboard::Key::Tab)
				{
					m_gameWindow.close();
				}

				if (keyPressed->code == Keyboard::Key::Escape)
				{
					if (m_currentGameState == GameState::Playing)
					{
						m_currentGameState = GameState::Paused;
						m_menu.setupMenuButtons(m_currentGameState, currentW, currentH, m_gameSettings, m_lastGameState);
						m_Episode1Music.setVolume(0);
						m_Episode2Music.setVolume(0);
						m_Episode3Music.setVolume(0);
						m_player.updateSound();
					}
					else if (m_currentGameState == GameState::Paused)
					{
						m_currentGameState = GameState::Playing;
						m_menu.setupMenuButtons(m_currentGameState, currentW, currentH, m_gameSettings, m_lastGameState);

						if (m_gameSettings.playMusic)
						{
							m_Episode1Music.setVolume(30.0f);
							m_Episode2Music.setVolume(30.0f);
							m_Episode3Music.setVolume(30.0f);
						}
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

						if (m_gameSettings.playMusic)
							m_menu.updateMusicVolume(true);
					}
				}

				if (keyPressed->code == Keyboard::Key::Enter)
				{
					if (m_currentGameState == GameState::EpisodeTransition && m_nextEpisode == GameEpisode::Victory)
					{
						m_currentGameState = GameState::MainMenu;
						m_menu.setupMenuButtons(m_currentGameState, m_currentWindowSize.x, m_currentWindowSize.y, m_gameSettings, m_lastGameState);

						if (m_gameSettings.playMusic)
							m_menu.updateMusicVolume(true);
						return;
					}
					if (m_currentGameState == GameState::MainMenu || m_currentGameState == GameState::GameOver)
					{
						if (m_currentGameState == GameState::MainMenu && m_lastGameState == GameState::GameOver)
							m_currentEpisode = GameEpisode::VendingMachine;

						if (m_currentGameState == GameState::MainMenu)
						{
							if (!m_isFadingIn)
								startFadeOut(GameEpisode::VendingMachine);
						}
						else if (m_currentGameState == GameState::GameOver)
							startNewGame(false);

						if (m_gameSettings.playMusic)
						{
							m_Episode1Music.setVolume(30.0f);
							m_Episode2Music.setVolume(30.0f);
							m_Episode3Music.setVolume(30.0f);
						}
						else
						{
							m_Episode1Music.setVolume(0);
							m_Episode2Music.setVolume(0);
							m_Episode3Music.setVolume(0);
						}
						m_menu.setupMenuButtons(m_currentGameState, m_currentWindowSize.x, m_currentWindowSize.y, m_gameSettings, m_lastGameState);
					}
					else if (m_currentGameState == GameState::Paused)
					{
						m_currentGameState = GameState::Playing;
						if (m_gameSettings.playMusic)
						{
							m_Episode1Music.setVolume(30.0f);
							m_Episode2Music.setVolume(30.0f);
							m_Episode3Music.setVolume(30.0f);
						}
					}
				}

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
					float sx = static_cast<float>(m_currentWindowSize.x) / static_cast<float>(m_bgTexture.getSize().x);
					float sy = static_cast<float>(m_currentWindowSize.y) / static_cast<float>(m_bgTexture.getSize().y);
					m_bgSprite.setScale({sx, sy});

					m_fpsText.setPosition({m_currentWindowSize.x - m_fpsMargin, 0.0f});
					m_fpsErrorRect.setPosition({m_currentWindowSize.x - m_fpsMargin, 0.0f});
				}
			}
		}

		if (m_isFadingIn)
		{
			float fadeSpeed = 700.0f;
			m_fadeAlpha -= fadeSpeed * dt.asSeconds();

			if (m_fadeAlpha <= 0.0f)
			{
				m_fadeAlpha = 0.0f;
				m_isFadingIn = false;
			}

			m_fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<int>(m_fadeAlpha)));
		}

		if (m_isFadingOut)
		{
			float fadeSpeed = 300.0f;
			m_fadeAlpha += fadeSpeed * dt.asSeconds();

			if (m_fadeAlpha >= 255.0f)
			{
				m_fadeAlpha = 255.0f;
				m_isFadingOut = false;

				if (m_goToMenuAfterFade)
				{
					m_currentGameState = GameState::MainMenu;
					m_Episode1Music.stop();
					m_Episode2Music.stop();
					m_Episode3Music.stop();
					m_player.updateSound();

					m_menu.setupMenuButtons(m_currentGameState, static_cast<float>(m_currentWindowSize.x), static_cast<float>(m_currentWindowSize.y), m_gameSettings, m_lastGameState);

					if (m_gameSettings.playMusic)
						m_menu.updateMusicVolume(true);

					m_isFadingIn = true;
					m_fadeAlpha = 255.0f;

					m_goToMenuAfterFade = false;
				}
				else if (m_currentGameState == GameState::MainMenu || m_currentGameState == GameState::GameOver)
				{
					startNewGame(true);
					m_isFadingIn = true;
				}
				else
				{
					startTransition(m_nextEpisodeAfterFade);
				}
			}
			m_fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<int>(m_fadeAlpha)));
		}

		//* === UPDATE LOGIC ===
		switch (m_currentGameState)
		{
		case GameState::Playing:
		{

			bool shouldPlayWallSound = (m_currentEpisode == GameEpisode::VendingMachine);
			m_player.update(dt, currentW, currentH, m_currentLeftWall, m_currentRightWall, m_currentTopWall, m_WallPushBack, shouldPlayWallSound, static_cast<int>(m_currentEpisode));

			if (m_currentEpisode == GameEpisode::VendingMachine && m_player.hasPlayerMoved() &&
					m_Episode1Music.getStatus() != Sound::Status::Playing &&
					m_gameSettings.playMusic)
			{
				m_Episode1Music.setVolume(30.0f);
				m_Episode1Music.play();
			}
			else if (m_currentEpisode == GameEpisode::Survival && m_player.hasPlayerMoved() &&
							 m_Episode2Music.getStatus() != Sound::Status::Playing &&
							 m_gameSettings.playMusic)
			{
				m_Episode2Music.setVolume(30.0f);
				m_Episode2Music.play();
			}
			else if (m_currentEpisode == GameEpisode::BossFight && m_player.hasPlayerMoved() &&
							 m_Episode3Music.getStatus() != Sound::Status::Playing &&
							 m_gameSettings.playMusic)
			{
				m_Episode3Music.setVolume(15.0f);
				m_Episode3Music.play();
			}

			if (m_player.hasPlayerMoved() || (m_currentEpisode != GameEpisode::VendingMachine && m_currentEpisodeTime > 0.0f))
			{
				m_spawnTimer -= dt.asSeconds();
				m_currentEpisodeTime += dt.asSeconds();
				m_heartSpawnTimer += dt.asSeconds();
				if (m_heartSpawnTimer >= 10.0f)
				{
					spawnHeart();
					m_heartSpawnTimer = 0.0f;
				}

				if (m_currentEpisode == GameEpisode::VendingMachine && m_currentEpisodeTime >= m_episodeDuration)
				{
					if (!m_isFadingIn && !m_isFadingOut)
						startFadeOut(GameEpisode::Survival);
					m_Episode1Music.stop();
					m_player.updateSound();
					break;
				}
				else if (m_currentEpisode == GameEpisode::Survival && m_currentEpisodeTime >= m_episodeDuration)
				{
					if (!m_isFadingIn && !m_isFadingOut)
						startFadeOut(GameEpisode::BossFight);
					break;
				}
				else if (m_currentEpisode == GameEpisode::BossFight && m_currentEpisodeTime >= m_episodeDuration)
				{
					if (!m_isFadingIn && !m_isFadingOut)
						startFadeOut(GameEpisode::Victory);
					break;
				}

				if (m_currentEpisode == GameEpisode::VendingMachine && m_Episode1Music.getStatus() != Sound::Status::Playing && m_gameSettings.playMusic)
				{
					m_Episode1Music.setVolume(30.0f);
					m_Episode1Music.play();
				}
				else if (m_currentEpisode == GameEpisode::Survival && m_Episode2Music.getStatus() != Sound::Status::Playing && m_gameSettings.playMusic)
				{
					m_Episode2Music.setVolume(30.0f);
					m_Episode2Music.play();
				}
				else if (m_currentEpisode == GameEpisode::BossFight && m_Episode3Music.getStatus() != Sound::Status::Playing && m_gameSettings.playMusic)
				{
					m_Episode3Music.setVolume(30.0f);
					m_Episode3Music.play();
				}
				if (m_currentEpisode == GameEpisode::BossFight)
				{
					m_boss.update(dt, m_player.getPosition(), currentW, currentH);

					if (!m_boss.isAlive())
					{
						startTransition(GameEpisode::Victory);
					}

					bool isHit = false;

					FloatRect playerRect = m_player.getHeartHitboxRect();

					const auto &handleCircles = m_boss.getHandleHitboxes();
					for (const auto &circle : handleCircles)
					{
						float closeX = max(playerRect.position.x, min(circle.center.x, playerRect.position.x + playerRect.size.x));
						float closeY = max(playerRect.position.y, min(circle.center.y, playerRect.position.y + playerRect.size.y));

						float dx = circle.center.x - closeX;
						float dy = circle.center.y - closeY;

						if ((dx * dx + dy * dy) < (circle.radius * circle.radius))
						{
							isHit = true;
							break;
						}
					}

					if (!isHit)
					{
						FloatRect brushRect = m_boss.getBrushHitbox();
						if (brushRect.findIntersection(playerRect))
						{
							isHit = true;
						}
					}

					if (isHit)
					{
						m_player.loseHealth(dt);
						if (m_player.getHealth() <= 0)
						{
							m_currentGameState = GameState::GameOver;
							m_player.updateSound();
							m_deathSound.play();
							m_menu.setupMenuButtons(m_currentGameState, static_cast<float>(m_currentWindowSize.x), static_cast<float>(m_currentWindowSize.y), m_gameSettings, m_lastGameState);

							m_Episode1Music.stop();
							m_Episode2Music.stop();
							m_Episode3Music.stop();

							m_lastGameState = GameState::GameOver;
						}
					}

					if (!m_boss.isAlive())
					{
						startFadeOut(GameEpisode::Victory);
					}
				}
				else
				{
					if (m_spawnTimer < 0)
					{
						Enemy tempEnemy(0, 0);

						if (m_currentEpisode == GameEpisode::VendingMachine)
						{
							int spawnChance = rand() % 100;

							if (spawnChance <= 20)
							{
								Type currentType = Type::Box;
								const Texture *currentEnemyTexture = &m_enemyBoxTexture;
								float finalSpeed = 350.0f;

								bool spawnLeft = (rand() % 2 == 0);
								float spawnX = spawnLeft ? (m_machineLeftWall + 50.0f) : (currentW - m_machineRightWall - 50.0f);
								float spawnY = -150.0f;

								tempEnemy.spawn(spawnX, spawnY, finalSpeed, *currentEnemyTexture, currentType, m_player.getPosition(), &m_enemyCookieTexture);
								m_enemies.push_back(tempEnemy);
							}
							else
							{
								Type currentType;
								const Texture *currentEnemyTexture = nullptr;

								if (rand() % 100 <= 35)
								{
									currentType = Type::Bar;
									if (rand() % 100 <= 49)
										currentEnemyTexture = &m_enemyBarTexture;
									else
									{
										currentEnemyTexture = &m_enemyBar2Texture;
										currentType = Type::Bar2;
									}
								}
								else
								{
									currentType = Type::Can;
									if (!m_enemyCanTextures.empty())
										currentEnemyTexture = &m_enemyCanTextures[rand() % m_enemyCanTextures.size()];
								}

								if (currentEnemyTexture)
								{
									float finalSpeed = 0.0f;

									if (m_gameSettings.gameDifficulty == GameDifficulty::Easy)
									{
										finalSpeed = (currentType == Type::Bar || currentType == Type::Bar2) ? -500.0f : 900.0f;
										m_spawnTimer = 0.5f;
									}
									else if (m_gameSettings.gameDifficulty == GameDifficulty::Normal)
									{
										finalSpeed = (currentType == Type::Bar || currentType == Type::Bar2) ? -700.0f : 900.0f;
										m_spawnTimer = 0.3f;
									}
									else
									{
										finalSpeed = (currentType == Type::Bar || currentType == Type::Bar2) ? -800.0f : 950.0f;
										m_spawnTimer = 0.2f;
									}

									float spawnY = (currentType == Type::Bar || currentType == Type::Bar2) ? currentH + 100.0f : -300.0f;

									float enemyScale = 0.2f;
									float texWidth = currentEnemyTexture->getSize().x;
									float halfWidth = (texWidth * enemyScale) / 2.0f;
									float minX = m_machineLeftWall + halfWidth;
									float maxX = currentW - m_machineRightWall - halfWidth;
									float spawnX = minX + (rand() % static_cast<int>(maxX - minX));

									tempEnemy.spawn(spawnX, spawnY, finalSpeed, *currentEnemyTexture, currentType, m_player.getPosition());
									m_enemies.push_back(tempEnemy);
								}
							}
						}
						else if (m_currentEpisode == GameEpisode::Survival)
						{
							Type currentType;
							const Texture *currentEnemyTexture = nullptr;
							float finalSpeed = 0.0f;

							if (rand() % 2 == 0)
							{
								currentType = Type::HandStraight;
								currentEnemyTexture = &m_enemyHand1Texture;
								finalSpeed = 800.0f;
							}
							else
							{
								currentType = Type::HandChaser;
								currentEnemyTexture = &m_enemyHand2Texture;
								if (m_gameSettings.gameDifficulty == GameDifficulty::Easy)
								{
									finalSpeed = 100.0f;
								}
								if (m_gameSettings.gameDifficulty == GameDifficulty::Normal)
								{
									finalSpeed = 150.0f;
								}
								if (m_gameSettings.gameDifficulty == GameDifficulty::Hard)
								{
									finalSpeed = 200.0f;
								}
							}

							if (m_gameSettings.gameDifficulty == GameDifficulty::Easy)
							{
								m_spawnTimer = 1.0f;
							}
							if (m_gameSettings.gameDifficulty == GameDifficulty::Normal)
							{
								m_spawnTimer = 0.75f;
							}
							if (m_gameSettings.gameDifficulty == GameDifficulty::Hard)
							{
								m_spawnTimer = 0.5f;
							}

							// SPAWN SIDE
							int side = rand() % 4;
							float offset = 100.0f;
							float spawnX = 0, spawnY = 0;

							if (side == 0) // UP
							{
								spawnX = static_cast<float>(rand() % m_gameWindow.getSize().x);
								spawnY = -offset;
							}
							else if (side == 1) // RIGHT
							{
								spawnX = m_gameWindow.getSize().x + offset;
								spawnY = static_cast<float>(rand() % m_gameWindow.getSize().y);
							}
							else if (side == 2) // DOWN
							{
								spawnX = static_cast<float>(rand() % m_gameWindow.getSize().x);
								spawnY = m_gameWindow.getSize().y + offset;
							}
							else // LEFT
							{
								spawnX = -offset;
								spawnY = static_cast<float>(rand() % m_gameWindow.getSize().y);
							}

							if (currentEnemyTexture)
							{
								tempEnemy.spawn(spawnX, spawnY, finalSpeed, *currentEnemyTexture, currentType, m_player.getPosition());
								m_enemies.push_back(tempEnemy);
							}
						}
					}

					for (auto &enemy : m_enemies)
					{
						enemy.update(dt, currentW, currentH, m_machineLeftWall, m_machineRightWall, m_player.getPosition(), m_cookieSound);
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
				else if (enemy.getType() == Type::Box)
				{
					const auto &cookies = enemy.getCookies();
					for (const auto &cookie : cookies)
					{
						if (cookie.active)
						{
							if (cookie.sprite.getGlobalBounds().findIntersection(m_player.getHeartHitboxRect()))
							{
								isHit = true;
								break;
							}
						}
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
					m_Episode1Music.stop();
					m_Episode2Music.stop();
					m_lastGameState = GameState::GameOver;
				}
			}

			for (auto it = m_hearts.begin(); it != m_hearts.end();)
			{
				it->animationTime += dt.asSeconds();
				float scaleBase = 0.1f;
				float scaleWave = 0.02f * sin(it->animationTime * 5.0f);
				it->healSprite.setScale({scaleBase + scaleWave, scaleBase + scaleWave});

				if (it->healSprite.getGlobalBounds().findIntersection(m_player.getHeartHitboxRect()))
				{
					sf::Vector2f pPos = m_player.getPosition();
					sf::Vector2f hPos = it->healSprite.getPosition();
					float dx = pPos.x - hPos.x;
					float dy = pPos.y - hPos.y;
					float dist = sqrt(dx * dx + dy * dy);

					if (dist < 50.0f)
					{
						m_healSound.play();
						m_player.gainHealth(1);
						it = m_hearts.erase(it);
						continue;
					}
				}

				++it;
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

		case GameState::EpisodeTransition:
		{
			if (m_nextEpisode == GameEpisode::Victory)
			{
				sf::Color textColor = m_loadingText.getFillColor();
				textColor.a = 255;
				m_loadingText.setFillColor(textColor);
				m_loadingText.setScale({1.0f, 1.0f});
			}
			else
			{
				m_transitionTimer -= dt.asSeconds();

				float totalTime = 5.0f;
				float timePassed = totalTime - m_transitionTimer;

				int alpha = 255;

				if (timePassed < 0.5f)
					alpha = static_cast<int>((timePassed / 0.5f) * 255);
				else if (m_transitionTimer < 0.5f)
					alpha = static_cast<int>((m_transitionTimer / 0.5f) * 255);

				if (alpha < 0)
					alpha = 0;
				if (alpha > 255)
					alpha = 255;

				sf::Color textColor = m_loadingText.getFillColor();
				textColor.a = alpha;
				m_loadingText.setFillColor(textColor);

				float zoomSpeed = 0.05f;
				float currentScale = 1.0f + (timePassed * zoomSpeed);
				m_loadingText.setScale({currentScale, currentScale});

				if (m_transitionTimer <= 0.0f)
				{
					setupEpisode(m_nextEpisode);
					m_currentGameState = GameState::Playing;

					m_isFadingIn = true;
					m_fadeAlpha = 255.0f;
					m_isFadingOut = false;

					m_loadingText.setScale({1.0f, 1.0f});
					textColor.a = 255;
					m_loadingText.setFillColor(textColor);

					m_gameClock.restart();
				}
			}
			break;
		}
		case GameState::Settings:
			break;
		case GameState::GameOver:
			break;
		}

		//* === DRAW LOGIC ===
		m_gameWindow.clear(Color::Black);

		if (m_currentGameState == GameState::GameOver)
		{
			m_player.updateSound();
			m_player.draw(m_gameWindow, m_gameSettings);
			m_menu.draw(m_gameWindow, m_currentGameState);
		}
		else if (m_currentGameState == GameState::EpisodeTransition)
		{
			if (m_nextEpisode == GameEpisode::Victory)
			{
				m_gameWindow.draw(m_bgSprite);
				m_gameWindow.draw(m_loadingTextBG);
			}

			m_gameWindow.draw(m_loadingText);
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
				if (m_currentEpisode == GameEpisode::BossFight)
				{
					m_boss.draw(m_gameWindow, m_gameSettings);
				}
				for (auto &heart : m_hearts)
				{
					m_gameWindow.draw(heart.healSprite);
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

			if (m_fadeAlpha > 0.0f)
			{
				m_gameWindow.draw(m_fadeRect);
			}
		}
		m_gameWindow.display();
	}
}

void Game::spawnHeart()
{
	HeartPickup heart(m_heartPickupTexture);
	heart.healSprite.setTexture(m_heartPickupTexture, true);

	sf::FloatRect bounds = heart.healSprite.getLocalBounds();
	heart.healSprite.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});

	heart.healSprite.setScale({0.1f, 0.1f});

	float minX = 100.0f;
	float maxX = m_currentWindowSize.x - 100.0f;
	float minY = 100.0f;
	float maxY = m_currentWindowSize.y - 100.0f;

	float x = minX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX - minX)));
	float y = minY + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxY - minY)));

	heart.healSprite.setPosition({x, y});
	heart.isActive = true;

	m_hearts.push_back(heart);
}

void Game::startNewGame(bool useTransition)
{
	int difficultyHP = 3;
	float difficultyPlayerSpeed = 550.0f;
	float difficultyInvincibility = 3.0f;

	switch (m_gameSettings.gameDifficulty)
	{
	case GameDifficulty::Easy:
	{
		difficultyHP = 4;
		difficultyPlayerSpeed = 650.0f;
		difficultyInvincibility = 4.0f;
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

	if (useTransition)
	{
		startTransition(GameEpisode::BossFight);
	}
	else
	{
		setupEpisode(GameEpisode::VendingMachine);
		m_currentGameState = GameState::Playing;

		m_gameClock.restart();
	}
}

void Game::setupEpisode(GameEpisode episode)
{
	m_currentEpisode = episode;
	m_enemies.clear();
	m_hearts.clear();
	m_heartSpawnTimer = 0.0f;

	switch (m_currentEpisode)
	{
	case GameEpisode::VendingMachine:
	{
		m_currentLeftWall = m_machineLeftWall;
		m_currentRightWall = m_machineRightWall;
		m_currentTopWall = m_machineTopWall;
		m_WallPushBack = 15.0f;
		m_currentEpisodeTime = 0.0f;
		m_episodeDuration = 0.01f;

		if (!m_bgTexture.loadFromFile("assets/images/Episode_1.jpg"))
			cerr << "Error bg" << endl;

		m_currentEnemyTextures.clear();
		m_enemyCanTextures.clear();

		if (!m_enemyBarTexture.loadFromFile("assets/images/enemy_bar1.png"))
			cerr << "Bar texture error!" << endl;
		if (!m_enemyBar2Texture.loadFromFile("assets/images/enemy_bar2.png"))
			cerr << "Bar2 texture error!" << endl;
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
		// TODO зробити 1 тип нового рядка а не 2
		if (!m_enemyBoxTexture.loadFromFile("assets/images/enemy_box1.png"))
			cerr << "Box tex error\n";
		if (!m_enemyCookieTexture.loadFromFile("assets/images/enemy_box1_particle.png"))
			cerr << "Cookie tex error\n";
		if (!m_enemyCookieBuffer.loadFromFile("assets/sound/box_particle.ogg"))
			cerr << "Cookie Sound error!" << endl;
		else
			m_cookieSound.setBuffer(m_enemyCookieBuffer);

		break;
	}
	case GameEpisode::Survival:
		m_currentEpisodeTime = 0.0f;
		m_currentLeftWall = 0.0f;
		m_currentRightWall = 0.0f;
		m_currentTopWall = 0.0f;
		m_WallPushBack = 0.0f;
		m_episodeDuration = 0.01f;

		m_player.startNextEpisode(m_currentWindowSize.x / 2.0f, m_currentWindowSize.y / 2.0f);

		if (!m_bgTexture.loadFromFile("assets/images/Episode_2.jpg"))
			cerr << "Error bg" << endl;
		if (m_enemyHand1Texture.loadFromFile("assets/images/enemy_hand1.png"))
			m_enemyHand1.setTexture(m_enemyHand1Texture, true);
		else
			cerr << "Enemy Hand texture error!" << endl;
		if (m_enemyHand2Texture.loadFromFile("assets/images/enemy_hand2.png"))
			m_enemyHand2.setTexture(m_enemyHand2Texture, true);
		else
			cerr << "Enemy Hand 2 texture error!" << endl;
		break;

	case GameEpisode::BossFight:
	{
		m_currentEpisodeTime = 0.0f;
		m_currentLeftWall = 0.0f;
		m_currentRightWall = 0.0f;
		m_currentTopWall = 0.0f;
		m_episodeDuration = 90.0f;

		m_player.startNextEpisode(m_currentWindowSize.x / 2.0f, m_currentWindowSize.y - 200.0f);

		m_boss.spawn(m_currentWindowSize.x / 2.0f, -500.0f, m_gameSettings);

		if (!m_bgTexture.loadFromFile("assets/images/Episode_3.png"))
			cerr << "Error bg" << endl;
	}
	break;
	case GameEpisode::Victory:
		break;
	}
	m_bgSprite.setTexture(m_bgTexture, true);

	float m_bgScaleX = static_cast<float>(m_currentWindowSize.x) / static_cast<float>(m_bgTexture.getSize().x);
	float m_bgScaleY = static_cast<float>(m_currentWindowSize.y) / static_cast<float>(m_bgTexture.getSize().y);
	m_bgSprite.setScale({m_bgScaleX, m_bgScaleY});
}

void Game::startTransition(GameEpisode nextEpisode)
{
	m_nextEpisode = nextEpisode;
	m_currentGameState = GameState::EpisodeTransition;
	m_transitionTimer = 5.0f;
	m_player.updateSound();
	m_menu.updateMusicVolume(false);
	m_transitionSound.setVolume(50.0f);

	if (m_transitionSound.getStatus() != Sound::Status::Playing)
		m_transitionSound.play();

	if (m_nextEpisode == GameEpisode::VendingMachine)
		m_loadingText.setString("Episode 1\nThe Birth");
	else if (m_nextEpisode == GameEpisode::Survival)
		m_loadingText.setString("Episode 2\nThirst");
	else if (m_nextEpisode == GameEpisode::BossFight)
		m_loadingText.setString("Episode 3\nCLEANING!");
	else if (m_nextEpisode == GameEpisode::Victory)
	{
		if (!m_bgTexture.loadFromFile("assets/images/victory_bg.jpg"))
			cerr << "Victory BG error" << endl;

		m_bgSprite.setTexture(m_bgTexture, true);
		float scaleX = static_cast<float>(m_currentWindowSize.x) / m_bgTexture.getSize().x;
		float scaleY = static_cast<float>(m_currentWindowSize.y) / m_bgTexture.getSize().y;
		m_bgSprite.setScale({scaleX, scaleY});

		m_loadingText.setCharacterSize(200);
		m_loadingText.setString("VICTORY!\n\nTo be continued...");

		sf::FloatRect textRect = m_loadingText.getLocalBounds();
		m_loadingText.setOrigin({textRect.position.x + textRect.size.x / 2.0f,
														 textRect.position.y + textRect.size.y / 2.0f});
		m_loadingText.setPosition({static_cast<float>(m_currentWindowSize.x) * 0.3f,
															 static_cast<float>(m_currentWindowSize.y) / 2.0f});

		m_loadingTextBG.setSize({600.0f, 300.0f});
		m_loadingTextBG.setOrigin({m_loadingTextBG.getSize().x / 2.0f, m_loadingTextBG.getSize().y / 2.0f});
		m_loadingTextBG.setPosition(m_loadingText.getPosition());
		m_loadingTextBG.setFillColor(Color(0, 255, 0, 185));
		m_loadingTextBG.setOutlineColor(Color::White);
		m_loadingTextBG.setOutlineThickness(5.0f);
		m_winSound.play();
	}
	m_loadingText.setCharacterSize(50);
	sf::FloatRect textRect = m_loadingText.getLocalBounds();
	m_loadingText.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
	if (m_nextEpisode != GameEpisode::Victory)
	{
		m_loadingText.setPosition({static_cast<float>(m_currentWindowSize.x) / 2.0f, static_cast<float>(m_currentWindowSize.y) / 2.0f});
	}

	m_Episode1Music.stop();
	m_Episode2Music.stop();
	m_Episode3Music.stop();
}

void Game::startFadeOut(GameEpisode nextEpisode)
{
	m_nextEpisodeAfterFade = nextEpisode;
	m_isFadingOut = true;
	m_fadeAlpha = 0.0f;
	m_goToMenuAfterFade = false;
}

void Game::startFadeOutToMenu()
{
	m_isFadingOut = true;
	m_fadeAlpha = 0.0f;
	m_goToMenuAfterFade = true;
}