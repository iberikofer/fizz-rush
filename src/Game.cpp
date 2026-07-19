#include "Game.hpp"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>

//* === INITIAL SETUP ===
Game::Game() : m_loadingText(m_loadingFont),
							 m_bgSprite(m_bgTexture),
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
							 m_heartSpawnSound(m_heartSpawnSoundBuffer),
							 m_gameLoadingSound(m_gameLoadingSoundBuffer),
							 m_gameStartSound(m_gameStartSoundBuffer),
							 m_menu(static_cast<float>(sf::VideoMode::getDesktopMode().size.x), static_cast<float>(sf::VideoMode::getDesktopMode().size.y)),
							 m_player(static_cast<float>(sf::VideoMode::getDesktopMode().size.x), static_cast<float>(sf::VideoMode::getDesktopMode().size.y))
{
	m_gameWindow.create(sf::VideoMode::getDesktopMode(), "Fizz Rush!", sf::Style::None, sf::State::Fullscreen);
	m_gameWindow.requestFocus();
	m_gameWindow.clear(sf::Color::Black);
	m_gameWindow.display();
	if (m_gameSettings.VSync)
	{
		m_gameWindow.setVerticalSyncEnabled(true);
		m_gameWindow.setFramerateLimit(0);
	}
	else
	{
		m_gameWindow.setVerticalSyncEnabled(false);
		m_gameWindow.setFramerateLimit(120);
	}

	//! CONFIG
	if (!m_loadingFont.openFromFile("assets/fonts/Lilita_One.ttf"))
		std::cerr << "Menu font error!" << std::endl;
	m_loadingText.setString("LOADING...=)");
	m_loadingText.setCharacterSize(175);
	m_loadingText.setFillColor(sf::Color::White);
	sf::FloatRect m_loadingTextPos = m_loadingText.getLocalBounds();
	m_loadingText.setOrigin({m_loadingTextPos.position.x + m_loadingTextPos.size.x / 2.0f,
													 m_loadingTextPos.position.y + m_loadingTextPos.size.y / 2.0f});
	m_loadingText.setPosition({m_gameWindow.getSize().x / 2.0f, m_gameWindow.getSize().y / 2.0f});
	if (!m_gameLoadingSoundBuffer.loadFromFile("assets/sound/loading_screen.ogg"))
		std::cerr << "Loading screen sound error!" << std::endl;
	m_gameLoadingSound.setVolume(60.0f);
	m_gameLoadingSound.play();
	if (!m_gameStartSoundBuffer.loadFromFile("assets/sound/game_start_sound.ogg"))
		std::cerr << "Game start sound error!" << std::endl;

	m_gameWindow.clear(sf::Color::Black);
	m_gameWindow.draw(m_loadingText);
	m_gameWindow.display();
	std::srand(std::time(0));

	//! SYSTEM OBJECTS
	if (m_gameWindowIcon.loadFromFile("assets/images/app_icon.png"))
		m_gameWindow.setIcon(m_gameWindowIcon);
	else
		std::cerr << "Window icon error!" << std::endl;
	if (!m_bgTexture.loadFromFile("assets/images/Episode_1.jpg"))
		std::cerr << "Background error!" << std::endl;
	if (!m_Episode1MusicBuffer.loadFromFile("assets/sound/Episode_1.ogg"))
		std::cerr << "Game music 1 error!" << std::endl;
	if (!m_Episode2MusicBuffer.loadFromFile("assets/sound/Episode_2.ogg"))
		std::cerr << "Game music 2 error!" << std::endl;
	if (!m_Episode3MusicBuffer.loadFromFile("assets/sound/boss_music.ogg"))
		std::cerr << "Boss music error!" << std::endl;
	if (!m_transitionSoundBuffer.loadFromFile("assets/sound/Episode_transition.ogg"))
		std::cerr << "Transition sound error!" << std::endl;
	if (!m_heartPickupTexture.loadFromFile("assets/images/heart_heal.png"))
		std::cerr << "Heart pickup texture error!" << std::endl;
	if (!m_enemyBarTexture.loadFromFile("assets/images/enemy_bar1.png"))
		std::cerr << "Bar texture error!" << std::endl;
	if (!m_enemyBar2Texture.loadFromFile("assets/images/enemy_bar2.png"))
		std::cerr << "Bar2 texture error!" << std::endl;
	sf::Texture tempTexture;
	if (tempTexture.loadFromFile("assets/images/enemy_can1.png"))
		m_enemyCanTextures.push_back(tempTexture);
	else
		std::cerr << "Enemy 1 texture error!" << std::endl;
	if (tempTexture.loadFromFile("assets/images/enemy_can2.png"))
		m_enemyCanTextures.push_back(tempTexture);
	else
		std::cerr << "Enemy 2 texture error!" << std::endl;
	if (tempTexture.loadFromFile("assets/images/enemy_can3.png"))
		m_enemyCanTextures.push_back(tempTexture);
	else
		std::cerr << "Enemy 3 texture error!" << std::endl;
	if (tempTexture.loadFromFile("assets/images/enemy_can4.png"))
		m_enemyCanTextures.push_back(tempTexture);
	else
		std::cerr << "Enemy 4 texture error!" << std::endl;
	if (tempTexture.loadFromFile("assets/images/enemy_can5.png"))
		m_enemyCanTextures.push_back(tempTexture);
	else
		std::cerr << "Enemy 5 texture error!" << std::endl;
	if (!m_enemyBoxTexture.loadFromFile("assets/images/enemy_box1.png"))
		std::cerr << "Box texture error\n";
	if (!m_enemyCookieTexture.loadFromFile("assets/images/enemy_box1_particle.png"))
		std::cerr << "Cookie texture error\n";
	if (!m_enemyCookieBuffer.loadFromFile("assets/sound/box_particle.ogg"))
		std::cerr << "Cookie Sound error!" << std::endl;
	else
		m_cookieSound.setBuffer(m_enemyCookieBuffer);
	if (!m_enemyHand1Texture.loadFromFile("assets/images/enemy_hand1.png"))
		std::cerr << "Enemy Hand 1 Texture error!" << std::endl;
	if (!m_enemyHand2Texture.loadFromFile("assets/images/enemy_hand2.png"))
		std::cerr << "Enemy Hand 2 Texture error!" << std::endl;
	m_heartSpawnTimer = 0.0f;
	m_Episode1Music.setLooping(true);
	m_Episode2Music.setLooping(true);
	m_Episode3Music.setLooping(true);
	m_menuButtonSound.setVolume(50.0f);
	m_cookieSound.setVolume(20.0f);
	m_healSound.setVolume(50.0f);
	m_bgSprite.setTexture(m_bgTexture, true);
	float m_bgScaleX = m_gameWindow.getSize().x / static_cast<float>(m_bgTexture.getSize().x);
	float m_bgScaleY = m_gameWindow.getSize().y / static_cast<float>(m_bgTexture.getSize().y);
	m_bgSprite.setScale({m_bgScaleX, m_bgScaleY});
	m_startPosX = m_gameWindow.getSize().x / 2.0f + 103.5f;
	m_startPosY = m_gameWindow.getSize().y / 2.0f - 185.0f;
	m_currentWindowSize = m_gameWindow.getSize();
	m_currentGameState = GameState::MainMenu;
	m_fpsText.setCharacterSize(40);
	m_fpsText.setFillColor(sf::Color(255, 220, 0));
	m_fpsText.setOutlineColor(sf::Color(150, 100, 0));
	m_fpsText.setOutlineThickness(4.0f);
	if (m_fpsFont.openFromFile("assets/fonts/Nabla.ttf"))
	{
		m_fpsFontIsLoaded = true;
		m_fpsText.setPosition({m_currentWindowSize.x - m_fpsMargin - 10.0f, 5.0f});
		m_fpsText.setString("FPS: -");
	}
	else
	{
		std::cerr << "Font error!" << std::endl;
		m_fpsFontIsLoaded = false;
	}
	m_fpsErrorRect.setFillColor(sf::Color::Red);
	m_fpsErrorRect.setPosition({m_currentWindowSize.x - m_fpsMargin, 50.0f});
	m_player.loadAssets(m_startPosX, m_startPosY);
	m_boss.loadAssets();
	m_machineLeftWall = 68.5f;
	m_machineRightWall = 135.0f;
	m_machineTopWall = 16.0f;
	m_menu.loadAssets(m_currentWindowSize.x, m_currentWindowSize.y);
	m_menu.setupMenuButtons(m_currentGameState, m_currentWindowSize.x, m_currentWindowSize.y, m_gameSettings, m_lastGameState);
	if (!m_menuButtonSoundBuffer.loadFromFile("assets/sound/button.ogg"))
		std::cerr << "Menu sound error!" << std::endl;
	if (!m_winSoundBuffer.loadFromFile("assets/sound/victory.ogg"))
		std::cerr << "Victory sound error!" << std::endl;
	else
		m_winSound.setBuffer(m_winSoundBuffer);
	if (!m_deathSoundBuffer.loadFromFile("assets/sound/death.ogg"))
		std::cerr << "Death sound error!" << std::endl;
	m_deathSound.setVolume(50.0f);
	if (!m_healSoundBuffer.loadFromFile("assets/sound/heal.ogg"))
		std::cerr << "Heal sound error!" << std::endl;
	m_healSound.setVolume(75.0f);
	if (!m_heartSpawnSoundBuffer.loadFromFile("assets/sound/heart_spawn.ogg"))
		std::cerr << "Heart spawn sound error!" << std::endl;
	m_heartSpawnSound.setVolume(25.0f);
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

//* === GAME LOOP ===
void Game::run() {
  m_gameWindow.requestFocus();
  while (m_gameWindow.isOpen()) {
    float currentW = static_cast<float>(m_currentWindowSize.x);
    float currentH = static_cast<float>(m_currentWindowSize.y);
    sf::Time dt = m_gameClock.restart();
    bool isHit = false;
    m_fpsUpdateTimer += dt.asSeconds();
    float FPS = (dt.asSeconds() > 0) ? 1.0f / dt.asSeconds() : 0.0f;
    if (m_fpsUpdateTimer >= 0.3f) {
      if (m_fpsFontIsLoaded) {
        m_fpsText.setString("FPS: " + std::to_string(int(FPS)));
      }
      m_fpsUpdateTimer = 0.0f;
    }

    while (const std::optional event = m_gameWindow.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        m_gameWindow.close();
      } else if (const auto *mouseClick =
                     event->getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseClick->button == sf::Mouse::Button::Left) {
          int m_menuClickType =
              m_menu.mouseClickPos(mouseClick->position.x,
                                   mouseClick->position.y, m_currentGameState);

          if (m_menuClickType == 1) {
            m_menuButtonSound.play();

            if (m_currentGameState == GameState::MainMenu) {
              if (!m_isFadingIn && !m_isFadingOut) {
                if (m_gameStartSound.getStatus() != sf::Sound::Status::Playing)
                  m_gameStartSound.play();
                startFadeOut(GameEpisode::VendingMachine);
              }
            } else if (m_currentGameState == GameState::GameOver) {
              if (m_gameStartSound.getStatus() != sf::Sound::Status::Playing)
                m_gameStartSound.play();
              startNewGame(false);
            } else if (m_currentGameState == GameState::Paused) {
              m_currentGameState = GameState::Playing;
            }

            if (m_gameSettings.playMusic) {
              m_Episode1Music.setVolume(30.0f);
              m_Episode2Music.setVolume(30.0f);
              m_Episode3Music.setVolume(45.0f);
            } else {
              m_Episode1Music.setVolume(0);
              m_Episode2Music.setVolume(0);
              m_Episode3Music.setVolume(0);
            }

            m_menu.setupMenuButtons(m_currentGameState, m_currentWindowSize.x,
                                    m_currentWindowSize.y, m_gameSettings,
                                    m_lastGameState);
          } else if (m_menuClickType == 2) {
            m_menuButtonSound.play();
            m_lastGameState = m_currentGameState;
            m_currentGameState = GameState::Settings;
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
          } else if (m_menuClickType == 3) {
            m_menuButtonSound.play();
            if (m_gameSettings.gameDifficulty == GameDifficulty::Easy)
              m_gameSettings.gameDifficulty = GameDifficulty::Normal;
            else if (m_gameSettings.gameDifficulty == GameDifficulty::Normal)
              m_gameSettings.gameDifficulty = GameDifficulty::Hard;
            else
              m_gameSettings.gameDifficulty = GameDifficulty::Easy;
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
          } else if (m_menuClickType == 4) {
            m_gameSettings.playMusic = !m_gameSettings.playMusic;
            if (m_gameSettings.playMusic) {
              if (m_currentGameState == GameState::MainMenu ||
                  (m_currentGameState == GameState::Settings &&
                   m_lastGameState == GameState::MainMenu)) {
                m_menu.updateMusicVolume(true);
                m_Episode1Music.setVolume(0);
                m_Episode2Music.setVolume(0);
                m_Episode3Music.setVolume(0);
              } else
                m_menu.updateMusicVolume(false);
            } else {
              m_Episode1Music.setVolume(0);
              m_Episode2Music.setVolume(0);
              m_Episode3Music.setVolume(0);
              m_menu.updateMusicVolume(false);
            }

            m_menuButtonSound.play();
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
          } else if (m_menuClickType == 5) {
            if (m_gameSettings.VSync == true) {
              m_gameWindow.setVerticalSyncEnabled(false);
              m_gameWindow.setFramerateLimit(120);
              m_gameSettings.VSync = !m_gameSettings.VSync;
              m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                      m_gameSettings, m_lastGameState);
            } else if (m_gameSettings.VSync == false) {
              m_gameWindow.setVerticalSyncEnabled(true);
              m_gameWindow.setFramerateLimit(0);
              m_gameSettings.VSync = !m_gameSettings.VSync;
              m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                      m_gameSettings, m_lastGameState);
            }
          } else if (m_menuClickType == 6) {
            m_menuButtonSound.play();
            m_gameSettings.showFps = !m_gameSettings.showFps;
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
          } else if (m_menuClickType == 7) {
            m_menuButtonSound.play();
            m_gameSettings.showHitbox = !m_gameSettings.showHitbox;
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
          } else if (m_menuClickType == 8) {
            m_menuButtonSound.play();
            m_currentGameState = m_lastGameState;
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
          } else if (m_menuClickType == 9) {
            m_menuButtonSound.play();
            if (!m_isFadingIn && !m_isFadingOut) {
              m_menuButtonSound.play();
              if (m_currentGameState == GameState::MainMenu)
                m_gameWindow.close();
              else {
                startFadeOutToMenu();
              }
            }
          }
        }
      } else if (const auto *keyPressed =
                     event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Tab) {
          m_gameWindow.close();
        }

        if (keyPressed->code == sf::Keyboard::Key::Escape) {
          //* === UPDATE LOGIC ===
          if (m_currentGameState == GameState::Playing) {
            m_currentGameState = GameState::Paused;
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
            m_Episode1Music.setVolume(0);
            m_Episode2Music.setVolume(0);
            m_Episode3Music.setVolume(0);
            m_player.stopSound();
            m_boss.stopSound();
          } else if (m_currentGameState == GameState::Paused) {
            m_currentGameState = GameState::Playing;
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);

            if (m_gameSettings.playMusic) {
              m_Episode1Music.setVolume(30.0f);
              m_Episode2Music.setVolume(30.0f);
              m_Episode3Music.setVolume(45.0f);
            }
          } else if (m_currentGameState == GameState::Settings) {
            m_currentGameState = m_lastGameState;
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
          } else if (m_currentGameState == GameState::GameOver) {
            m_currentGameState = GameState::MainMenu;
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);

            if (m_gameSettings.playMusic)
              m_menu.updateMusicVolume(true);
          }
        }

        if (keyPressed->code == sf::Keyboard::Key::Enter) {
          if (m_currentGameState == GameState::EpisodeTransition &&
              m_nextEpisode == GameEpisode::Victory) {
            m_currentGameState = GameState::MainMenu;
            m_menu.setupMenuButtons(m_currentGameState, m_currentWindowSize.x,
                                    m_currentWindowSize.y, m_gameSettings,
                                    m_lastGameState);

            if (m_gameSettings.playMusic)
              m_menu.updateMusicVolume(true);
            return;
          }
          if (m_currentGameState == GameState::MainMenu ||
              m_currentGameState == GameState::GameOver) {
            if (m_currentGameState == GameState::MainMenu &&
                m_lastGameState == GameState::GameOver)
              m_currentEpisode = GameEpisode::VendingMachine;

            if (m_currentGameState == GameState::MainMenu) {
              if (!m_isFadingIn && !m_isFadingOut) {
                if (m_gameStartSound.getStatus() != sf::Sound::Status::Playing)
                  m_gameStartSound.play();
                startFadeOut(GameEpisode::VendingMachine);
              }
            } else if (m_currentGameState == GameState::GameOver) {
              if (m_gameStartSound.getStatus() != sf::Sound::Status::Playing)
                m_gameStartSound.play();
              startNewGame(false);
            }

            if (m_gameSettings.playMusic) {
              m_Episode1Music.setVolume(30.0f);
              m_Episode2Music.setVolume(30.0f);
              m_Episode3Music.setVolume(45.0f);
            } else {
              m_Episode1Music.setVolume(0);
              m_Episode2Music.setVolume(0);
              m_Episode3Music.setVolume(0);
            }
            m_menu.setupMenuButtons(m_currentGameState, m_currentWindowSize.x,
                                    m_currentWindowSize.y, m_gameSettings,
                                    m_lastGameState);
          } else if (m_currentGameState == GameState::Paused) {
            m_currentGameState = GameState::Playing;
            if (m_gameSettings.playMusic) {
              m_Episode1Music.setVolume(30.0f);
              m_Episode2Music.setVolume(30.0f);
              m_Episode3Music.setVolume(45.0f);
            }
          }
        }

        if (keyPressed->code == sf::Keyboard::Key::F11) {
          m_isFullscreen = !m_isFullscreen;
          if (m_isFullscreen) {
            m_gameWindow.create(sf::VideoMode::getDesktopMode(),
                                m_gameWindowName, sf::Style::Default);
            m_gameWindow.setPosition({0, 0});
            m_currentWindowSize = m_gameWindow.getSize();
          } else {
            m_gameWindow.create(sf::VideoMode::getDesktopMode(),
                                m_gameWindowName, sf::State::Fullscreen);
          }
          m_currentWindowSize = m_gameWindow.getSize();
          if (m_gameSettings.VSync == true) {
            m_gameWindow.setVerticalSyncEnabled(true);
            m_gameWindow.setFramerateLimit(0);
          } else if (m_gameSettings.VSync == false) {
            m_gameWindow.setVerticalSyncEnabled(false);
            m_gameWindow.setFramerateLimit(120);
          }
          if (m_gameWindowIcon.getSize().x > 0)
            m_gameWindow.setIcon(m_gameWindowIcon);
          float sx = static_cast<float>(m_currentWindowSize.x) /
                     static_cast<float>(m_bgTexture.getSize().x);
          float sy = static_cast<float>(m_currentWindowSize.y) /
                     static_cast<float>(m_bgTexture.getSize().y);
          m_bgSprite.setScale({sx, sy});

          m_fpsText.setPosition({m_currentWindowSize.x - m_fpsMargin, 0.0f});
          m_fpsErrorRect.setPosition(
              {m_currentWindowSize.x - m_fpsMargin, 0.0f});
        }
      } else if (event->is<sf::Event::FocusLost>()) {
        //* === UPDATE LOGIC ===
        if (m_currentGameState == GameState::Playing) {
          m_currentGameState = GameState::Paused;

          m_menu.setupMenuButtons(m_currentGameState, m_currentWindowSize.x,
                                  m_currentWindowSize.y, m_gameSettings,
                                  m_lastGameState);

          m_Episode1Music.setVolume(0);
          m_Episode2Music.setVolume(0);
          m_Episode3Music.setVolume(0);
          m_player.stopSound();
          m_boss.stopSound();
        }
      } else if (event->is<sf::Event::FocusGained>()) {
        m_gameClock.restart();
      }
    }

    if (m_isFadingIn) {
      float fadeSpeed = 700.0f;
      m_fadeAlpha -= fadeSpeed * dt.asSeconds();

      if (m_fadeAlpha <= 0.0f) {
        m_fadeAlpha = 0.0f;
        m_isFadingIn = false;
      }

      m_fadeRect.setFillColor(
          sf::Color(0, 0, 0, static_cast<int>(m_fadeAlpha)));
    }

    if (m_isFadingOut) {
      float fadeSpeed = 300.0f;
      m_fadeAlpha += fadeSpeed * dt.asSeconds();

      if (m_fadeAlpha >= 255.0f) {
        m_fadeAlpha = 255.0f;
        m_isFadingOut = false;

        if (m_goToMenuAfterFade) {
          m_currentGameState = GameState::MainMenu;
          m_Episode1Music.stop();
          m_Episode2Music.stop();
          m_Episode3Music.stop();
          m_player.stopSound();
          m_boss.stopSound();

          m_menu.setupMenuButtons(m_currentGameState,
                                  static_cast<float>(m_currentWindowSize.x),
                                  static_cast<float>(m_currentWindowSize.y),
                                  m_gameSettings, m_lastGameState);

          if (m_gameSettings.playMusic)
            m_menu.updateMusicVolume(true);

          m_isFadingIn = true;
          m_fadeAlpha = 255.0f;

          m_goToMenuAfterFade = false;
        } else if (m_currentGameState == GameState::MainMenu ||
                   m_currentGameState == GameState::GameOver) {
          startNewGame(true);
          m_isFadingIn = true;
        } else {
          startTransition(m_nextEpisodeAfterFade);
        }
      }
      m_fadeRect.setFillColor(
          sf::Color(0, 0, 0, static_cast<int>(m_fadeAlpha)));
    }

    switch (m_currentGameState) {
    case GameState::Playing: {

      bool victoryInvincibility = false;
      if (m_boss.getPhase() == BossPhase::Death) {
        victoryInvincibility = true;
      }

      bool shouldPlayWallSound =
          (m_currentEpisode == GameEpisode::VendingMachine);
      m_player.update(dt, currentW, currentH, m_currentLeftWall,
                      m_currentRightWall, m_currentTopWall, m_WallPushBack,
                      shouldPlayWallSound, static_cast<int>(m_currentEpisode));

      if (m_currentEpisode == GameEpisode::VendingMachine &&
          m_player.hasPlayerMoved() &&
          m_Episode1Music.getStatus() != sf::Sound::Status::Playing &&
          m_gameSettings.playMusic) {
        m_Episode1Music.setVolume(30.0f);
        m_Episode1Music.play();
      } else if (m_currentEpisode == GameEpisode::Survival &&
                 m_player.hasPlayerMoved() &&
                 m_Episode2Music.getStatus() != sf::Sound::Status::Playing &&
                 m_gameSettings.playMusic) {
        m_Episode2Music.setVolume(30.0f);
        m_Episode2Music.play();
      } else if (m_currentEpisode == GameEpisode::BossFight &&
                 m_player.hasPlayerMoved() &&
                 m_Episode3Music.getStatus() != sf::Sound::Status::Playing &&
                 m_gameSettings.playMusic) {
        m_Episode3Music.setVolume(15.0f);
        m_Episode3Music.play();
      }

      if (m_player.hasPlayerMoved() ||
          (m_currentEpisode != GameEpisode::VendingMachine &&
           m_currentEpisodeTime > 0.0f)) {
        m_enemySpawnTimer -= dt.asSeconds();
        m_currentEpisodeTime += dt.asSeconds();
        m_heartSpawnTimer += dt.asSeconds();
        if ((m_gameSettings.gameDifficulty == GameDifficulty::Easy) &&
            m_heartSpawnTimer >= 25.0f) {
          spawnHeart();
          m_heartSpawnTimer = 0.0f;
        } else if ((m_gameSettings.gameDifficulty == GameDifficulty::Normal) &&
                   m_heartSpawnTimer >= 20.0f) {
          spawnHeart();
          m_heartSpawnTimer = 0.0f;
        } else if ((m_gameSettings.gameDifficulty == GameDifficulty::Hard) &&
                   m_heartSpawnTimer >= 5.0f) {
          spawnHeart();
          m_heartSpawnTimer = 0.0f;
        }

        if (m_currentEpisode == GameEpisode::VendingMachine &&
            m_currentEpisodeTime >= m_episodeDuration) {
          if (!m_isFadingIn && !m_isFadingOut)
            startFadeOut(GameEpisode::Survival);
          m_Episode1Music.stop();
          m_player.stopSound();
          m_boss.stopSound();
          break;
        } else if (m_currentEpisode == GameEpisode::Survival &&
                   m_currentEpisodeTime >= m_episodeDuration) {
          if (!m_isFadingIn && !m_isFadingOut)
            startFadeOut(GameEpisode::BossFight);
          break;
        } else if (m_currentEpisode == GameEpisode::BossFight &&
                   m_currentEpisodeTime >= m_episodeDuration) {
          if (!m_isFadingIn && !m_isFadingOut)
            startFadeOut(GameEpisode::Victory);
          break;
        }

        if (m_currentEpisode == GameEpisode::VendingMachine &&
            m_Episode1Music.getStatus() != sf::Sound::Status::Playing &&
            m_gameSettings.playMusic) {
          m_Episode1Music.setVolume(30.0f);
          m_Episode1Music.play();
        } else if (m_currentEpisode == GameEpisode::Survival &&
                   m_Episode2Music.getStatus() != sf::Sound::Status::Playing &&
                   m_gameSettings.playMusic) {
          m_Episode2Music.setVolume(30.0f);
          m_Episode2Music.play();
        } else if (m_currentEpisode == GameEpisode::BossFight &&
                   m_Episode3Music.getStatus() != sf::Sound::Status::Playing &&
                   m_gameSettings.playMusic) {
          m_Episode3Music.setVolume(45.0f);
          m_Episode3Music.play();
        }

        if (m_currentEpisode == GameEpisode::BossFight) {
          m_boss.update(dt, m_player.getPosition(), currentW, currentH,
                        m_gameSettings);
          isHit = false;

          const auto &handleCircles = m_boss.getHandleHitboxes();
          const auto &brushCircles = m_boss.getBrushHitboxes();
          const auto &playerCircles = m_player.getHitboxes();

          for (const auto &bCircle : handleCircles) {
            for (const auto &pCircle : playerCircles) {
              float dx = bCircle.center.x - pCircle.center.x;
              float dy = bCircle.center.y - pCircle.center.y;
              float distanceSqr = dx * dx + dy * dy;
              float radiusSum = bCircle.radius + pCircle.radius;

              if (distanceSqr < (radiusSum * radiusSum)) {
                isHit = true;
                break;
              }
            }
            if (isHit)
              break;
          }
          if (!isHit) {
            const auto &brushCircles = m_boss.getBrushHitboxes();
            const auto &playerCircles = m_player.getHitboxes();

            for (const auto &bCircle : brushCircles) {
              for (const auto &pCircle : playerCircles) {
                float dx = bCircle.center.x - pCircle.center.x;
                float dy = bCircle.center.y - pCircle.center.y;
                float distanceSqr = dx * dx + dy * dy;
                float radiusSum = bCircle.radius + pCircle.radius;

                if (distanceSqr < (radiusSum * radiusSum)) {
                  isHit = true;
                  break;
                }
              }
              if (isHit)
                break;
            }
          }
          if (isHit) {
            if (!victoryInvincibility) {
              m_player.loseHealth();
            }

            if (m_player.getHealth() <= 0) {
              m_currentGameState = GameState::GameOver;
              m_player.stopSound();
              m_boss.stopSound();
              m_deathSound.play();
              m_menu.setupMenuButtons(m_currentGameState,
                                      static_cast<float>(m_currentWindowSize.x),
                                      static_cast<float>(m_currentWindowSize.y),
                                      m_gameSettings, m_lastGameState);

              m_Episode1Music.stop();
              m_Episode2Music.stop();
              m_Episode3Music.stop();

              m_lastGameState = GameState::GameOver;
            }
          }

          if (!m_boss.isAlive()) {
            startFadeOut(GameEpisode::Victory);
          }
        } else {

          if (m_currentEpisode == GameEpisode::Survival) {
            sf::FloatRect playerBounds = m_player.getPlayerHitbox();
            float playerArea = playerBounds.size.x * playerBounds.size.y;

            auto intersection = m_slowSafeZone.findIntersection(playerBounds);
            float intersectionArea = 0.0f;

            if (intersection.has_value()) {
              intersectionArea = intersection->size.x * intersection->size.y;
            }

            bool isOnCarpet = (intersectionArea / playerArea) >= 0.5f;
            m_player.updateCarpetSpeed(isOnCarpet);
          }

          if (m_enemySpawnTimer < 0) {
            Enemy tempEnemy(0, 0);

            if (m_currentEpisode == GameEpisode::VendingMachine) {
              int spawnChance = std::rand() % 100;
              if (spawnChance <= 20) {
                Type currentType = Type::Box;
                const sf::Texture *currentEnemyTexture = &m_enemyBoxTexture;
                float finalSpeed = 350.0f;

                bool spawnLeft = (std::rand() % 2 == 0);
                float spawnX = spawnLeft
                                   ? (m_machineLeftWall + 50.0f)
                                   : (currentW - m_machineRightWall - 50.0f);
                float spawnY = -150.0f;

                tempEnemy.spawn(spawnX, spawnY, finalSpeed,
                                *currentEnemyTexture, currentType,
                                m_player.getPosition(), &m_enemyCookieTexture);
                m_enemies.push_back(tempEnemy);
              } else {
                Type currentType;
                const sf::Texture *currentEnemyTexture = nullptr;

                if (std::rand() % 100 <= 35) {
                  currentType = Type::Bar;
                  if (std::rand() % 100 <= 49)
                    currentEnemyTexture = &m_enemyBarTexture;
                  else {
                    currentEnemyTexture = &m_enemyBar2Texture;
                    currentType = Type::Bar2;
                  }
                } else {
                  currentType = Type::Can;
                  if (!m_enemyCanTextures.empty())
                    currentEnemyTexture =
                        &m_enemyCanTextures[std::rand() %
                                            m_enemyCanTextures.size()];
                }

                if (currentEnemyTexture) {
                  float finalSpeed = 0.0f;

                  if (m_gameSettings.gameDifficulty == GameDifficulty::Easy) {
                    finalSpeed =
                        (currentType == Type::Bar || currentType == Type::Bar2)
                            ? -500.0f
                            : 900.0f;
                    m_enemySpawnTimer = 0.5f;
                  } else if (m_gameSettings.gameDifficulty ==
                             GameDifficulty::Normal) {
                    finalSpeed =
                        (currentType == Type::Bar || currentType == Type::Bar2)
                            ? -700.0f
                            : 900.0f;
                    m_enemySpawnTimer = 0.3f;
                  } else {
                    finalSpeed =
                        (currentType == Type::Bar || currentType == Type::Bar2)
                            ? -800.0f
                            : 950.0f;
                    m_enemySpawnTimer = 0.2f;
                  }

                  float spawnY =
                      (currentType == Type::Bar || currentType == Type::Bar2)
                          ? currentH + 100.0f
                          : -300.0f;

                  float enemyScale = 0.2f;
                  float texWidth = currentEnemyTexture->getSize().x;
                  float halfWidth = (texWidth * enemyScale) / 2.0f;
                  float minX = m_machineLeftWall + halfWidth;
                  float maxX = currentW - m_machineRightWall - halfWidth;
                  float spawnX =
                      minX + (std::rand() % static_cast<int>(maxX - minX));

                  tempEnemy.spawn(spawnX, spawnY, finalSpeed,
                                  *currentEnemyTexture, currentType,
                                  m_player.getPosition());
                  m_enemies.push_back(tempEnemy);
                }
              }
            } else if (m_currentEpisode == GameEpisode::Survival) {
              Type currentType;
              const sf::Texture *currentEnemyTexture = nullptr;
              float finalSpeed = 0.0f;

              if (std::rand() % 2 == 0) {
                currentType = Type::HandStraight;
                currentEnemyTexture = &m_enemyHand1Texture;
                finalSpeed = 800.0f;
              } else {
                currentType = Type::HandChaser;
                currentEnemyTexture = &m_enemyHand2Texture;
                if (m_gameSettings.gameDifficulty == GameDifficulty::Easy) {
                  finalSpeed = 100.0f;
                }
                if (m_gameSettings.gameDifficulty == GameDifficulty::Normal) {
                  finalSpeed = 150.0f;
                }
                if (m_gameSettings.gameDifficulty == GameDifficulty::Hard) {
                  finalSpeed = 200.0f;
                }
              }

              if (m_gameSettings.gameDifficulty == GameDifficulty::Easy) {
                m_enemySpawnTimer = 1.0f;
              }
              if (m_gameSettings.gameDifficulty == GameDifficulty::Normal) {
                m_enemySpawnTimer = 0.75f;
              }
              if (m_gameSettings.gameDifficulty == GameDifficulty::Hard) {
                m_enemySpawnTimer = 0.5f;
              }

              // SPAWN SIDE
              int side = std::rand() % 4;
              float offset = 100.0f;
              float spawnX = 0, spawnY = 0;

              if (side == 0) // UP
              {
                spawnX =
                    static_cast<float>(std::rand() % m_gameWindow.getSize().x);
                spawnY = -offset;
              } else if (side == 1) // RIGHT
              {
                spawnX = m_gameWindow.getSize().x + offset;
                spawnY =
                    static_cast<float>(std::rand() % m_gameWindow.getSize().y);
              } else if (side == 2) // DOWN
              {
                spawnX =
                    static_cast<float>(std::rand() % m_gameWindow.getSize().x);
                spawnY = m_gameWindow.getSize().y + offset;
              } else // LEFT
              {
                spawnX = -offset;
                spawnY =
                    static_cast<float>(std::rand() % m_gameWindow.getSize().y);
              }

              if (currentEnemyTexture) {
                tempEnemy.spawn(spawnX, spawnY, finalSpeed,
                                *currentEnemyTexture, currentType,
                                m_player.getPosition());
                m_enemies.push_back(tempEnemy);
              }
            }
          }

          for (auto &enemy : m_enemies) {
            enemy.update(dt, currentW, currentH, m_machineLeftWall,
                         m_machineRightWall, m_player.getPosition(),
                         m_cookieSound);
          }

          for (auto it = m_enemies.begin(); it != m_enemies.end();) {
            if (it->getPosition().y > currentH + 200.0f ||
                it->getPosition().y < -400.0f) {
              it = m_enemies.erase(it);
            } else {
              ++it;
            }
          }
        }
      }

      const auto &playerCircles = m_player.getHitboxes();

      bool isHit = false; // reset per-frame for enemy collision

      for (auto &enemy : m_enemies) {
        if (enemy.getType() == Type::Can) {
          const auto &enemyCircles = enemy.getHitboxes();
          for (const auto &pCircle : playerCircles) {
            for (const auto &eCircle : enemyCircles) {
              float dx = pCircle.center.x - eCircle.center.x;
              float dy = pCircle.center.y - eCircle.center.y;
              float distanceSqr = dx * dx + dy * dy;
              float radiusSum = pCircle.radius + eCircle.radius;

              if (distanceSqr < radiusSum * radiusSum) {
                isHit = true;
                break;
              }
            }

            if (isHit)
              break;
          }
        } else if (enemy.getType() == Type::Box) {
          const auto &cookies = enemy.getCookies();
          for (const auto &cookie : cookies) {
            if (cookie.active) {
              if (cookie.sprite.getGlobalBounds().findIntersection(
                      m_player.getPlayerHitbox())) {
                isHit = true;
                break;
              }
            }
          }
        } else {
          sf::FloatRect enemyBounds = enemy.getBounds();

          for (const auto &pCircle : playerCircles) {
            float closeX =
                std::max(enemyBounds.position.x,
                         std::min(pCircle.center.x,
                                  enemyBounds.position.x + enemyBounds.size.x));
            float closeY =
                std::max(enemyBounds.position.y,
                         std::min(pCircle.center.y,
                                  enemyBounds.position.y + enemyBounds.size.y));

            float dx = pCircle.center.x - closeX;
            float dy = pCircle.center.y - closeY;

            if ((dx * dx + dy * dy) < (pCircle.radius * pCircle.radius)) {
              isHit = true;
              break;
            }
          }
        }

        if (isHit)
          break;
      }

      if (isHit) {
        if (!victoryInvincibility) {
          m_player.loseHealth();
        }
        if (m_player.getHealth() <= 0) {
          m_currentGameState = GameState::GameOver;
          m_player.stopSound();
          m_boss.stopSound();
          m_deathSound.play();
          m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                  m_gameSettings, m_lastGameState);
          m_Episode1Music.stop();
          m_Episode2Music.stop();
          m_lastGameState = GameState::GameOver;
        }
      }

      for (auto it = m_hearts.begin(); it != m_hearts.end();) {
        it->animationTime += dt.asSeconds();

        float colorFactor = (-sin(it->animationTime * 5.0f) + 1.0f) / 2.0f;
        sf::Color colorA = sf::Color(255, 255, 255);
        sf::Color colorB = sf::Color(255, 20, 20);
        float heartR = static_cast<uint8_t>(
            colorA.r + colorFactor * (colorB.r - colorA.r));
        float heartG = static_cast<uint8_t>(
            colorA.g + colorFactor * (colorB.g - colorA.g));
        float heartB = static_cast<uint8_t>(
            colorA.b + colorFactor * (colorB.b - colorA.b));

        it->healSprite.setColor(sf::Color(heartR, heartG, heartB));

        float scaleBase = 0.15f;
        float scaleWave = 0.02f * sin(it->animationTime * 5.0f);
        it->healSprite.setScale({scaleBase + scaleWave, scaleBase + scaleWave});

        if (it->healSprite.getGlobalBounds().findIntersection(
                m_player.getPlayerHitbox())) {
          m_healSound.play();
          m_player.gainHealth(1);
          it = m_hearts.erase(it);
          continue;
        }

        ++it;
      }

      break;
    }

    case GameState::MainMenu: {
    }

    case GameState::Paused:
    case GameState::Settings:
    case GameState::GameOver: {
      break;
    }

    case GameState::EpisodeTransition: {
      if (m_nextEpisode == GameEpisode::Victory) {
        sf::Color textColor = m_loadingText.getFillColor();
        textColor.a = 255;
        m_loadingText.setFillColor(textColor);
        m_loadingText.setScale({1.0f, 1.0f});
      } else {
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

        if (m_transitionTimer <= 0.0f) {
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
    }

    //* === DRAW LOGIC ===
    m_gameWindow.clear(sf::Color::Black);

    if (m_currentGameState == GameState::GameOver) {
      m_player.stopSound();
      m_boss.stopSound();
      m_player.draw(m_gameWindow, m_gameSettings);
      m_menu.draw(m_gameWindow, m_currentGameState);
    } else if (m_currentGameState == GameState::EpisodeTransition) {
      if (m_nextEpisode == GameEpisode::Victory) {
        m_gameWindow.draw(m_bgSprite);
        m_gameWindow.draw(m_loadingTextBG);
      }

      m_gameWindow.draw(m_loadingText);
    } else {
      m_gameWindow.draw(m_bgSprite);

      if (m_currentGameState == GameState::Playing ||
          m_currentGameState == GameState::Paused ||
          m_currentGameState == GameState::Settings) {
        m_player.draw(m_gameWindow, m_gameSettings);
        for (auto &enemy : m_enemies) {
          enemy.draw(m_gameWindow, m_gameSettings);
        }
        if (m_currentEpisode == GameEpisode::Survival &&
            m_gameSettings.showHitbox == true) {
          m_gameWindow.draw(m_slowSafeZoneRect);
        }
        if (m_currentEpisode == GameEpisode::BossFight) {
          m_boss.draw(m_gameWindow, m_gameSettings);
        }
        for (auto &heart : m_hearts) {
          m_gameWindow.draw(heart.healSprite);
        }
      }

      if (m_currentGameState == GameState::Paused ||
          m_currentGameState == GameState::MainMenu ||
          m_currentGameState == GameState::Settings) {
        m_menu.draw(m_gameWindow, m_currentGameState);
      }
      if (!m_fpsFontIsLoaded)
        m_gameWindow.draw(m_fpsErrorRect);
      else if (m_gameSettings.showFps)
        m_gameWindow.draw(m_fpsText);

      if (m_fadeAlpha > 0.0f) {
        m_gameWindow.draw(m_fadeRect);
      }
    }
    m_gameWindow.display();
  }
}

void Game::spawnHeart()
{
	m_heartSpawnSound.play();

	HeartPickup heart(m_heartPickupTexture);
  heart.healSprite.setTexture(m_heartPickupTexture, true);

  sf::FloatRect bounds = heart.healSprite.getLocalBounds();
  heart.healSprite.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});

  heart.healSprite.setScale({0.1f, 0.1f});

  float heartX =
      m_machineLeftWall +
      static_cast<float>(std::rand()) /
          (static_cast<float>(RAND_MAX /
                              (m_currentWindowSize.x - m_machineRightWall -
                               m_machineLeftWall - 100.0f)));
  float heartY =
      m_machineTopWall +
      static_cast<float>(std::rand()) /
          (static_cast<float>(
              RAND_MAX / (m_currentWindowSize.y - m_machineTopWall - 100.0f)));

  heart.healSprite.setPosition({heartX, heartY});
  heart.isActive = true;

  m_hearts.push_back(heart);
}

void Game::startNewGame(bool useTransition) {
  GameEpisode startingEpisode = GameEpisode::VendingMachine;

  setupEpisode(startingEpisode);
  int difficultyHP = 3;
  float difficultyInvincibility = 3.0f;

  if (m_gameSettings.gameDifficulty == GameDifficulty::Easy) {
    difficultyHP = 4;
    difficultyInvincibility = 4.0f;
  } else if (m_gameSettings.gameDifficulty == GameDifficulty::Hard) {
    difficultyHP = 2;
    difficultyInvincibility = 2.0f;
  }

  m_player.resetGame(m_startPosX, m_startPosY, difficultyHP,
                     difficultyPlayerSpeed, difficultyInvincibility,
                     m_gameSettings.gameDifficulty);

  m_gameClock.restart();
  m_enemySpawnTimer = 0.0f;
  m_enemies.clear();

  if (useTransition) {
    startTransition(GameEpisode::VendingMachine);
  } else {
    setupEpisode(GameEpisode::VendingMachine);
    m_currentGameState = GameState::Playing;
    m_gameClock.restart();
  }
}

void Game::setupEpisode(GameEpisode episode) {
  m_currentEpisode = episode;
  m_enemies.clear();
  m_hearts.clear();
  m_heartSpawnTimer = 0.0f;

  switch (m_gameSettings.gameDifficulty) {
  case GameDifficulty::Easy:
    m_currentEpisode == GameEpisode::Survival ? difficultyPlayerSpeed = 450.0f
                                              : difficultyPlayerSpeed = 650.0f;
    break;
  case GameDifficulty::Normal:
    m_currentEpisode == GameEpisode::Survival ? difficultyPlayerSpeed = 350.0f
                                              : difficultyPlayerSpeed = 550.0f;
    break;
  case GameDifficulty::Hard:
    m_currentEpisode == GameEpisode::Survival ? difficultyPlayerSpeed = 400.0f
                                              : difficultyPlayerSpeed = 500.0f;
    break;
  }

  switch (m_currentEpisode) {
  case GameEpisode::VendingMachine: {
    m_currentLeftWall = m_machineLeftWall;
    m_currentRightWall = m_machineRightWall;
    m_currentTopWall = m_machineTopWall;
    m_WallPushBack = 15.0f;
    m_currentEpisodeTime = 0.0f;
    // !
    m_episodeDuration = 90.0f;
    // !

    if (!m_bgTexture.loadFromFile("assets/images/Episode_1.jpg"))
      std::cerr << "Error bg" << std::endl;
    m_currentEnemyTextures.clear();

    break;
  }
  case GameEpisode::Survival: {
    m_currentEpisodeTime = 0.0f;
    m_currentLeftWall = 0.0f;
    m_currentRightWall = 0.0f;
    m_currentTopWall = 0.0f;
    m_WallPushBack = 0.0f;
    // !
    m_episodeDuration = 120.0f;
    // !
    m_slowZone.size.x = m_currentWindowSize.x;
    m_slowZone.size.y = m_currentWindowSize.y;
    m_slowSafeZone.size.x = m_currentWindowSize.x / 1.7;
    m_slowSafeZone.size.y = m_currentWindowSize.y / 1.7 + 30.0f;
    m_slowSafeZone.position.x =
        (m_currentWindowSize.x / 2.0f - m_slowSafeZone.size.x / 2.0f) + 2.0f;
    m_slowSafeZone.position.y =
        (m_currentWindowSize.y / 2.0f - m_slowSafeZone.size.y / 2.0f) - 2.0f;
    m_slowSafeZoneRect.setSize({m_slowSafeZone.size.x, m_slowSafeZone.size.y});
    m_slowSafeZoneRect.setOrigin(
        {m_slowSafeZoneRect.getLocalBounds().size.x / 2.0f,
         m_slowSafeZoneRect.getLocalBounds().size.y / 2.0f});
    float slowSafeZoneCenterX =
        m_slowSafeZone.position.x + m_slowSafeZone.size.x / 2.0f;
    float slowSafeZoneCenterY =
        m_slowSafeZone.position.y + m_slowSafeZone.size.y / 2.0f;
    m_slowSafeZoneRect.setPosition({slowSafeZoneCenterX, slowSafeZoneCenterY});
    m_slowSafeZoneRect.setFillColor(sf::Color::Transparent);
    m_slowSafeZoneRect.setOutlineColor(sf::Color::Green);
    m_slowSafeZoneRect.setOutlineThickness(2.0f);

    m_player.startNextEpisode(m_currentWindowSize.x / 2.0f,
                              m_currentWindowSize.y / 2.0f);

    if (!m_bgTexture.loadFromFile("assets/images/Episode_2.png"))
      std::cerr << "Error bg" << std::endl;
    break;
  }
  case GameEpisode::Victory:
    break;

  case GameEpisode::BossFight: {
    m_currentEpisodeTime = 0.0f;
    m_currentLeftWall = 0.0f;
    m_currentRightWall = 0.0f;
    m_currentTopWall = 0.0f;
    //!
    m_episodeDuration = 240.0f;
    //!

    m_player.startNextEpisode(m_currentWindowSize.x / 2.0f,
                              m_currentWindowSize.y - 200.0f);

    m_boss.spawn(m_currentWindowSize.x / 2.0f, -500.0f, m_gameSettings);

    if (!m_bgTexture.loadFromFile("assets/images/Episode_3.png"))
      std::cerr << "Error bg" << std::endl;
  } break;
  }
  m_bgSprite.setTexture(m_bgTexture, true);

  float m_bgScaleX = static_cast<float>(m_currentWindowSize.x) /
                     static_cast<float>(m_bgTexture.getSize().x);
  float m_bgScaleY = static_cast<float>(m_currentWindowSize.y) /
                     static_cast<float>(m_bgTexture.getSize().y);
  m_bgSprite.setScale({m_bgScaleX, m_bgScaleY});
}

void Game::startTransition(GameEpisode nextEpisode) {
  m_nextEpisode = nextEpisode;
  m_currentGameState = GameState::EpisodeTransition;
  m_transitionTimer = 5.0f;
  m_player.stopSound();
  m_boss.stopSound();
  m_menu.updateMusicVolume(false);
  m_transitionSound.setVolume(50.0f);

  if (m_transitionSound.getStatus() != sf::Sound::Status::Playing)
    m_transitionSound.play();

	if (m_nextEpisode == GameEpisode::VendingMachine)
		m_loadingText.setString("Episode 1 - Birth\n(Watch \"Cutscene 1\")");
	else if (m_nextEpisode == GameEpisode::Survival)
	{
		m_loadingText.setString("Episode 2 - Thirst\n(Watch \"Cutscene 2\")");
		m_transitionSound.play();
	}
	else if (m_nextEpisode == GameEpisode::BossFight)
	{
		m_loadingText.setString("Episode 3 - CLEANING!\n(Watch \"Cutscene 3\")");
		m_transitionSound.play();
	}
	else if (m_nextEpisode == GameEpisode::Victory)
	{
		if (!m_bgTexture.loadFromFile("assets/images/victory_bg.jpg"))
      std::cerr << "Victory BG error" << std::endl;

    m_bgSprite.setTexture(m_bgTexture, true);
    float scaleX =
        static_cast<float>(m_currentWindowSize.x) / m_bgTexture.getSize().x;
    float scaleY =
        static_cast<float>(m_currentWindowSize.y) / m_bgTexture.getSize().y;
    m_bgSprite.setScale({scaleX, scaleY});

    m_loadingText.setCharacterSize(200);
    m_loadingText.setString("You Survived!\n\nTo be continued...");

    sf::FloatRect textRect = m_loadingText.getLocalBounds();
    m_loadingText.setOrigin({textRect.position.x + textRect.size.x / 2.0f,
                             textRect.position.y + textRect.size.y / 2.0f});
    m_loadingText.setPosition(
        {static_cast<float>(m_currentWindowSize.x) * 0.3f,
         static_cast<float>(m_currentWindowSize.y) / 2.0f});

    m_loadingTextBG.setSize({600.0f, 300.0f});
    m_loadingTextBG.setOrigin({m_loadingTextBG.getSize().x / 2.0f,
                               m_loadingTextBG.getSize().y / 2.0f});
    m_loadingTextBG.setPosition(m_loadingText.getPosition());
    m_loadingTextBG.setFillColor(sf::Color(0, 255, 0, 185));
    m_loadingTextBG.setOutlineColor(sf::Color::White);
    m_loadingTextBG.setOutlineThickness(5.0f);
    m_winSound.play();
  }
  m_loadingText.setCharacterSize(50);
  sf::FloatRect textRect = m_loadingText.getLocalBounds();
  m_loadingText.setOrigin({textRect.position.x + textRect.size.x / 2.0f,
                           textRect.position.y + textRect.size.y / 2.0f});
  if (m_nextEpisode != GameEpisode::Victory) {
    m_loadingText.setPosition(
        {static_cast<float>(m_currentWindowSize.x) / 2.0f,
         static_cast<float>(m_currentWindowSize.y) / 2.0f});
  }

  m_Episode1Music.stop();
  m_Episode2Music.stop();
  m_Episode3Music.stop();
}

void Game::startFadeOut(GameEpisode nextEpisode) {
  m_nextEpisodeAfterFade = nextEpisode;
  m_isFadingOut = true;
  m_fadeAlpha = 0.0f;
  m_goToMenuAfterFade = false;
}

void Game::startFadeOutToMenu() {
  m_isFadingOut = true;
  m_fadeAlpha = 0.0f;
  m_goToMenuAfterFade = true;
}
