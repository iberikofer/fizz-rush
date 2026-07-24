#include "Game.hpp"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>

#ifdef _WIN32
#include <stdint.h>
extern "C" {
struct XINPUT_VIBRATION {
  uint16_t wLeftMotorSpeed;
  uint16_t wRightMotorSpeed;
};
uint32_t __stdcall XInputSetState(uint32_t dwUserIndex,
                                  XINPUT_VIBRATION *pVibration);
}
#endif

//! INITIAL SETUP
Game::Game()
    : m_loadingText(m_loadingFont), m_bgSprite(m_bgTexture),
      m_cookieSound(m_enemyCookieBuffer), m_fpsText(m_fpsFont),
      m_fpsErrorRect({100.f, 100.f}),
      m_menuButtonSound(m_menuButtonSoundBuffer),
      m_menuSwitchSound(m_menuSwitchSoundBuffer),
      m_Episode1Music(m_Episode1MusicBuffer),
      m_Episode2Music(m_Episode2MusicBuffer),
      m_Episode3Music(m_Episode3MusicBuffer),
      m_transitionSound(m_transitionSoundBuffer), m_winSound(m_winSoundBuffer),
      m_deathFizzSound(m_deathFizzSoundBuffer), m_winMusic(m_winMusicBuffer),
      m_deathSound(m_deathSoundBuffer), m_healSound(m_healSoundBuffer),
      m_heartSpawnSound(m_heartSpawnSoundBuffer),
      m_gameLoadingSound(m_gameLoadingSoundBuffer),
      m_gameStartSound(m_gameStartSoundBuffer),
      m_menu(static_cast<float>(sf::VideoMode::getDesktopMode().size.x),
             static_cast<float>(sf::VideoMode::getDesktopMode().size.y)),
      m_player(static_cast<float>(sf::VideoMode::getDesktopMode().size.x),
               static_cast<float>(sf::VideoMode::getDesktopMode().size.y)) {
  m_gameSettings.loadFromFile("settings.ini");
  m_gameWindow.create(sf::VideoMode::getDesktopMode(), "Fizz Rush!",
                      sf::Style::None, sf::State::Fullscreen);
  m_gameWindow.requestFocus();
  m_gameWindow.clear(sf::Color::Black);
  m_gameWindow.display();
  if (m_gameSettings.VSync) {
    m_gameWindow.setVerticalSyncEnabled(true);
    m_gameWindow.setFramerateLimit(0);
  } else {
    m_gameWindow.setVerticalSyncEnabled(false);
    m_gameWindow.setFramerateLimit(120);
  }

  //! CONFIG
  if (!m_loadingFont.openFromFile("assets/fonts/Lilita_One.ttf"))
    std::cerr << "Menu font error!" << std::endl;
  if (!m_loadingFontUA.openFromFile("assets/fonts/Russo_One.ttf"))
    std::cerr << "Menu font UA error!" << std::endl;
  m_loadingText.setFont(m_gameSettings.ukrainianLanguage ? m_loadingFontUA
                                                         : m_loadingFont);
  m_loadingText.setString(m_gameSettings.ukrainianLanguage
                              ? U"\u0417\u0410\u0412\u0410\u041D\u0422\u0410"
                                U"\u0416\u0415\u041D\u041D\u042F... =)"
                              : U"LOADING... =)");
  m_loadingText.setCharacterSize(m_gameSettings.ukrainianLanguage ? 120 : 175);
  m_loadingText.setFillColor(sf::Color::White);
  sf::FloatRect m_loadingTextPos = m_loadingText.getLocalBounds();
  m_loadingText.setOrigin(
      {m_loadingTextPos.position.x + m_loadingTextPos.size.x / 2.0f,
       m_loadingTextPos.position.y + m_loadingTextPos.size.y / 2.0f});
  m_loadingText.setPosition(
      {m_gameWindow.getSize().x / 2.0f, m_gameWindow.getSize().y / 2.0f});
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
  if (!m_bgTexture.loadFromFile("assets/images/episode_1.jpg"))
    std::cerr << "Background texture error!" << std::endl;
  if (!m_Episode1MusicBuffer.loadFromFile("assets/sound/episode_1.ogg"))
    std::cerr << "Episode 1 music error!" << std::endl;
  if (!m_Episode2MusicBuffer.loadFromFile("assets/sound/episode_2.ogg"))
    std::cerr << "Episode 2 music error!" << std::endl;
  if (!m_Episode3MusicBuffer.loadFromFile("assets/sound/boss_music.ogg"))
    std::cerr << "Boss music error!" << std::endl;
  if (!m_transitionSoundBuffer.loadFromFile(
          "assets/sound/episode_transition.ogg"))
    std::cerr << "Transition sound error!" << std::endl;
  if (!m_deathFizzSoundBuffer.loadFromFile("assets/sound/death_fizz.ogg"))
    std::cerr << "Death Fizz sound error!" << std::endl;
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
  if (!m_enemyCookieTexture.loadFromFile(
          "assets/images/enemy_box1_particle.png"))
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
  m_cookieSound.setVolume(AudioConfig::COOKIE);
  m_healSound.setVolume(AudioConfig::HEAL);
  m_menuButtonSound.setVolume(AudioConfig::BUTTON_SELECT);
  m_menuSwitchSound.setVolume(AudioConfig::BUTTON_SWITCH);
  m_winSound.setVolume(AudioConfig::WIN_SOUND);
  m_gameStartSound.setVolume(AudioConfig::GAME_START);
  m_transitionSound.setVolume(AudioConfig::TRANSITION);
  m_winMusic.setVolume(AudioConfig::WIN_MUSIC);
  m_deathSound.setVolume(AudioConfig::DEATH);
  m_heartSpawnSound.setVolume(AudioConfig::HEART_SPAWN);
  m_gameLoadingSound.setVolume(AudioConfig::GAME_LOADING);
  m_deathFizzSound.setVolume(AudioConfig::DEATH_FIZZ);
  m_bgSprite.setTexture(m_bgTexture, true);
  float m_bgScaleX =
      m_gameWindow.getSize().x / static_cast<float>(m_bgTexture.getSize().x);
  float m_bgScaleY =
      m_gameWindow.getSize().y / static_cast<float>(m_bgTexture.getSize().y);
  m_bgSprite.setScale({m_bgScaleX, m_bgScaleY});
  m_startPosX = m_gameWindow.getSize().x / 2.0f + 103.5f;
  m_startPosY = m_gameWindow.getSize().y / 2.0f - 185.0f;
  m_currentWindowSize = m_gameWindow.getSize();
  m_currentGameState = GameState::MainMenu;
  m_fpsText.setCharacterSize(40);
  m_fpsText.setFillColor(sf::Color(255, 220, 0));
  m_fpsText.setOutlineColor(sf::Color(150, 100, 0));
  m_fpsText.setOutlineThickness(4.0f);
  if (m_fpsFont.openFromFile("assets/fonts/Nabla.ttf")) {
    m_fpsFontIsLoaded = true;
    m_fpsText.setPosition({m_currentWindowSize.x - m_fpsMargin - 10.0f, 5.0f});
    m_fpsText.setString("FPS: -");
  } else {
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
  m_menu.setupMenuButtons(m_currentGameState, m_currentWindowSize.x,
                          m_currentWindowSize.y, m_gameSettings,
                          m_lastGameState);
  if (!m_menuButtonSoundBuffer.loadFromFile("assets/sound/button_select.ogg"))
    std::cerr << "Menu sound error!" << std::endl;
  if (!m_menuSwitchSoundBuffer.loadFromFile("assets/sound/button_switch.ogg"))
    std::cerr << "Menu switch sound error!" << std::endl;
  if (!m_winSoundBuffer.loadFromFile("assets/sound/victory.ogg"))
    std::cerr << "Victory sound error!" << std::endl;
  else
    m_winSound.setBuffer(m_winSoundBuffer);
  if (!m_winMusicBuffer.loadFromFile("assets/sound/victory_music.ogg"))
    std::cerr << "Victory music error!" << std::endl;
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
  m_fadeRect.setSize(sf::Vector2f(static_cast<float>(m_currentWindowSize.x),
                                  static_cast<float>(m_currentWindowSize.y)));
  m_fadeRect.setFillColor(sf::Color(0, 0, 0, 0));
  m_fadeAlpha = 0.0f;
  m_isFadingOut = false;
  m_goToMenuAfterFade = false;
  m_fadeRect.setFillColor(sf::Color(0, 0, 0, 255));
  m_fadeAlpha = 255.0f;
  m_isFadingIn = true;
  m_isFadingOut = false;

  updateSfxVolume(m_gameSettings.playSfx);
  m_player.updateSfxVolume(m_gameSettings.playSfx);
  m_boss.updateSfxVolume(m_gameSettings.playSfx);

  m_gameClock.restart();
}

//! GAME LOOP
void Game::run() {
  m_gameWindow.requestFocus();
  while (m_gameWindow.isOpen()) {
    float currentW = static_cast<float>(m_currentWindowSize.x);
    float currentH = static_cast<float>(m_currentWindowSize.y);
    sf::Time dt = m_gameClock.restart();
    bool isHit = false;

    if (m_pendingMenuAction != -1) {
      m_pendingMenuTimer -= dt.asSeconds();
      if (m_pendingMenuTimer <= 0.f) {
        int action = m_pendingMenuAction;
        m_pendingMenuAction = -1;
        handleMenuAction(action, currentW, currentH);
      }
    }

    if (m_menuSwitchSoundTimer > 0.0f) {
      m_menuSwitchSoundTimer -= dt.asSeconds();
      if (m_menuSwitchSoundTimer <= 0.0f) {
        m_menuSwitchSound.play();
        m_menuSwitchSoundTimer = 0.0f;
      }
    }

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
      } else if (const auto *resized = event->getIf<sf::Event::Resized>()) {
        unsigned int minW = 800;
        unsigned int minH = 600;
        if (resized->size.x < minW || resized->size.y < minH) {
          m_gameWindow.setSize({std::max(minW, resized->size.x),
                                std::max(minH, resized->size.y)});
        }
      } else if (const auto *mouseMoved =
                     event->getIf<sf::Event::MouseMoved>()) {
        sf::Vector2i newPos(mouseMoved->position.x, mouseMoved->position.y);
        int dx = newPos.x - m_lastMousePos.x;
        int dy = newPos.y - m_lastMousePos.y;
        float dist = std::sqrt((float)(dx * dx + dy * dy));
        if (dist >= 4.f && m_mouseSwitchCooldown <= 0.f &&
            m_inputMode != InputMode::Mouse) {
          m_inputMode = InputMode::Mouse;
          m_menu.setInputMode(InputMode::Mouse);
          m_gameWindow.setMouseCursorVisible(true);
          //? Snap mouse to focused button's position
          sf::Vector2f focused = m_menu.getButtonPosition(
              m_menu.getFocusedButtonIndex(), m_currentGameState);
          sf::Vector2i screenPos = m_gameWindow.mapCoordsToPixel(focused);
          sf::Mouse::setPosition(screenPos, m_gameWindow);
        }
        m_lastMousePos = newPos;
        //* Update hover
        sf::Vector2f worldPos =
            m_gameWindow.mapPixelToCoords(mouseMoved->position);
        if (m_menu.updateMouseHover(worldPos.x, worldPos.y,
                                    m_currentGameState)) {
          if (m_currentGameState != GameState::Controls)
            m_menuSwitchSoundTimer = 0.08f;
        }

      } else if (const auto *mouseClick =
                     event->getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseClick->button == sf::Mouse::Button::Left) {
          //? Mouse click -> instantly switch to Mouse mode
          if (m_inputMode != InputMode::Mouse) {
            m_inputMode = InputMode::Mouse;
            m_menu.setInputMode(InputMode::Mouse);
            m_gameWindow.setMouseCursorVisible(true);
            m_mouseSwitchCooldown = 0.3f;
          }
          sf::Vector2f worldPos =
              m_gameWindow.mapPixelToCoords(mouseClick->position);
          int clickType = m_menu.mouseClickPos(
              worldPos.x, worldPos.y, m_currentGameState, m_gameSettings);
          if (clickType > 0 && m_pendingMenuAction == -1) {
            m_pendingMenuAction = clickType;
            m_pendingMenuTimer = 0.15f;
            m_menuButtonSound.play();
          }
        }
      } else if (const auto *keyPressed =
                     event->getIf<sf::Event::KeyPressed>()) {

        bool isNavKey = (keyPressed->code == sf::Keyboard::Key::Up ||
                         keyPressed->code == sf::Keyboard::Key::Down ||
                         keyPressed->code == sf::Keyboard::Key::Left ||
                         keyPressed->code == sf::Keyboard::Key::Right ||
                         keyPressed->code == sf::Keyboard::Key::W ||
                         keyPressed->code == sf::Keyboard::Key::A ||
                         keyPressed->code == sf::Keyboard::Key::S ||
                         keyPressed->code == sf::Keyboard::Key::D);
        if (isNavKey && m_inputMode != InputMode::Gamepad) {
          m_inputMode = InputMode::Gamepad;
          m_menu.setInputMode(InputMode::Gamepad);
          m_gameWindow.setMouseCursorVisible(false);
          //? Warp cursor to corner
          sf::Mouse::setPosition(
              {(int)m_currentWindowSize.x, (int)m_currentWindowSize.y},
              m_gameWindow);
          m_mouseSwitchCooldown = 0.3f;
          m_menu.syncFocusFromHover(m_currentGameState);
        }

        bool isMenuScreen = (m_currentGameState == GameState::MainMenu ||
                             m_currentGameState == GameState::Paused ||
                             m_currentGameState == GameState::Settings ||
                             m_currentGameState == GameState::Controls ||
                             m_currentGameState == GameState::GameOver);

        if (isMenuScreen) {
          bool focusChanged = false;
          if (keyPressed->code == sf::Keyboard::Key::Up ||
              keyPressed->code == sf::Keyboard::Key::W)
            focusChanged = m_menu.moveFocus2D(0, -1, m_currentGameState);
          else if (keyPressed->code == sf::Keyboard::Key::Down ||
                   keyPressed->code == sf::Keyboard::Key::S)
            focusChanged = m_menu.moveFocus2D(0, 1, m_currentGameState);
          else if (keyPressed->code == sf::Keyboard::Key::Left ||
                   keyPressed->code == sf::Keyboard::Key::A) {
            if (m_currentGameState == GameState::Settings ||
                m_currentGameState == GameState::GameOver)
              focusChanged = m_menu.moveFocus2D(-1, 0, m_currentGameState);
          } else if (keyPressed->code == sf::Keyboard::Key::Right ||
                     keyPressed->code == sf::Keyboard::Key::D) {
            if (m_currentGameState == GameState::Settings ||
                m_currentGameState == GameState::GameOver)
              focusChanged = m_menu.moveFocus2D(1, 0, m_currentGameState);
          }
          if (focusChanged && m_currentGameState != GameState::Controls) {
            m_menuSwitchSoundTimer = 0.08f;
          }
        }

        if (keyPressed->code == sf::Keyboard::Key::Escape) {
          if (m_currentGameState == GameState::Playing) {
            m_currentGameState = GameState::Paused;
            m_menu.resetFocus(GameState::Paused);
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
            m_Episode1Music.setVolume(0);
            m_Episode2Music.setVolume(0);
            m_Episode3Music.setVolume(0);
            m_player.stopSound();
            m_boss.stopSound();
            if (m_isPlayerDying) {
              if (m_deathFizzSound.getStatus() == sf::Sound::Status::Playing)
                m_deathFizzSound.pause();
              if (m_deathSound.getStatus() == sf::Sound::Status::Playing)
                m_deathSound.pause();
              if (m_cookieSound.getStatus() == sf::Sound::Status::Playing)
                m_cookieSound.pause();
            }
          } else if (m_currentGameState == GameState::Paused) {
            m_currentGameState = GameState::Playing;
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
            if (m_gameSettings.playMusic) {
              m_Episode1Music.setVolume(30.0f);
              m_Episode2Music.setVolume(30.0f);
              m_Episode3Music.setVolume(45.0f);
            }
            if (m_isPlayerDying) {
              if (m_deathFizzSound.getStatus() == sf::Sound::Status::Paused)
                m_deathFizzSound.play();
              if (m_deathSound.getStatus() == sf::Sound::Status::Paused)
                m_deathSound.play();
              if (m_cookieSound.getStatus() == sf::Sound::Status::Paused)
                m_cookieSound.play();
            }
          } else if (m_currentGameState == GameState::Settings) {
            m_currentGameState = m_lastGameState;
            m_menu.resetFocus(m_currentGameState);
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
          } else if (m_currentGameState == GameState::Controls) {
            m_currentGameState = GameState::Settings;
            m_menu.resetFocus(GameState::Settings);
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
          } else if (m_currentGameState == GameState::GameOver) {
            if (!m_isFadingOut) {
              m_isFadingOut = true;
              m_fadeAlpha = 0.0f;
              m_goToMenuAfterFade = true;
            }
          }
        }

        if (keyPressed->code == sf::Keyboard::Key::Enter) {
          if (m_currentGameState == GameState::EpisodeTransition &&
              m_nextEpisode == GameEpisode::Victory) {
            playCutscene(4);
            m_winMusic.stop();
            m_winMusic.setVolume(0);
            m_currentEpisode = GameEpisode::VendingMachine;
            m_currentGameState = GameState::MainMenu;
            m_menu.setupMenuButtons(m_currentGameState, m_currentWindowSize.x,
                                    m_currentWindowSize.y, m_gameSettings,
                                    m_lastGameState);
            if (m_gameSettings.playMusic)
              m_menu.updateMusicVolume(true);
            m_isFadingIn = true;
            m_fadeAlpha = 255.0f;
          }
          if (isMenuScreen) {
            int action = m_menu.getFocusedButtonClickType(m_currentGameState);
            if (action > 0 && m_pendingMenuAction == -1) {
              m_pendingMenuAction = action;
              m_pendingMenuTimer = 0.15f;
              m_menuButtonSound.play();
            }
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

      } else if (const auto *joyBtn =
                     event->getIf<sf::Event::JoystickButtonPressed>()) {
        if (m_inputMode != InputMode::Gamepad) {
          m_inputMode = InputMode::Gamepad;
          m_menu.setInputMode(InputMode::Gamepad);
          m_gameWindow.setMouseCursorVisible(false);
          sf::Mouse::setPosition(
              {(int)m_currentWindowSize.x, (int)m_currentWindowSize.y},
              m_gameWindow);
          m_mouseSwitchCooldown = 0.3f;
          m_menu.syncFocusFromHover(m_currentGameState);
        }

        PadBtn btn = static_cast<PadBtn>(joyBtn->button);

        //? A button (0) -> Confirm (like Enter)
        if (btn == PadBtn::A) {
          bool isMenuScreen2 = (m_currentGameState == GameState::MainMenu ||
                                m_currentGameState == GameState::Paused ||
                                m_currentGameState == GameState::Settings ||
                                m_currentGameState == GameState::Controls ||
                                m_currentGameState == GameState::GameOver);
          if (isMenuScreen2) {
            int action = m_menu.getFocusedButtonClickType(m_currentGameState);
            if (action > 0 && m_pendingMenuAction == -1) {
              m_pendingMenuAction = action;
              m_pendingMenuTimer = 0.15f;
              m_menuButtonSound.play();
            }
          }
        }

        //? B button (1) -> Back (like Escape)
        if (btn == PadBtn::B) {
          if (m_currentGameState == GameState::Paused) {
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
            m_menu.resetFocus(m_currentGameState);
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
          } else if (m_currentGameState == GameState::Controls) {
            m_currentGameState = GameState::Settings;
            m_menu.resetFocus(GameState::Settings);
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
          } else if (m_currentGameState == GameState::GameOver) {
            m_currentGameState = GameState::MainMenu;
            m_menu.resetFocus(GameState::MainMenu);
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
            if (m_gameSettings.playMusic)
              m_menu.updateMusicVolume(true);
          }
        }

        //? Start button (7) -> Pause/Resume during gameplay
        if (btn == PadBtn::Start) {
          if (m_currentGameState == GameState::Playing) {
            m_currentGameState = GameState::Paused;
            m_menu.resetFocus(GameState::Paused);
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
          }
        }

      } else if (event->is<sf::Event::FocusLost>()) {
        if (m_currentGameState == GameState::Playing) {
          bool canPause = true;
          if (m_currentEpisode == GameEpisode::BossFight &&
              m_boss.getPhase() == BossPhase::Death)
            canPause = false;

          if (canPause) {
            m_currentGameState = GameState::Paused;
            m_menu.resetFocus(GameState::Paused);
            m_menu.setupMenuButtons(m_currentGameState, m_currentWindowSize.x,
                                    m_currentWindowSize.y, m_gameSettings,
                                    m_lastGameState);
            m_Episode1Music.setVolume(0);
            m_Episode2Music.setVolume(0);
            m_Episode3Music.setVolume(0);
            m_player.stopSound();
            m_boss.stopSound();
          }
        }
      } else if (event->is<sf::Event::FocusGained>()) {
        m_gameClock.restart();
      }
    }

    bool isMenuActiveForNav = (m_currentGameState == GameState::MainMenu ||
                               m_currentGameState == GameState::Paused ||
                               m_currentGameState == GameState::Settings ||
                               m_currentGameState == GameState::Controls ||
                               m_currentGameState == GameState::GameOver);
    if (isMenuActiveForNav && sf::Joystick::isConnected(0)) {
      float stickY = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y);
      float stickX = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X);
      float povY = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY);
      float povX = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX);
      float deadzone = 20.f;

      bool navUp = (stickY < -deadzone) || (povY > deadzone);
      bool navDown = (stickY > deadzone) || (povY < -deadzone);
      bool navLeft = (stickX < -deadzone) || (povX < -deadzone);
      bool navRight = (stickX > deadzone) || (povX > deadzone);
      bool anyNav = navUp || navDown || navLeft || navRight;

      //? Switch to Gamepad if stick/dpad active (past deadzone)
      if (anyNav && m_inputMode != InputMode::Gamepad) {
        m_inputMode = InputMode::Gamepad;
        m_menu.setInputMode(InputMode::Gamepad);
        m_gameWindow.setMouseCursorVisible(false);
        sf::Mouse::setPosition(
            {(int)m_currentWindowSize.x, (int)m_currentWindowSize.y},
            m_gameWindow);
        m_mouseSwitchCooldown = 0.3f;
        m_menu.syncFocusFromHover(m_currentGameState);
      }

      m_stickNavTimer += dt.asSeconds();
      float navRepeatDelay = 0.2f;
      if (anyNav && m_stickNavTimer >= navRepeatDelay) {
        m_stickNavTimer = 0.f;
        bool focusChanged = false;
        if (navUp)
          focusChanged = m_menu.moveFocus2D(0, -1, m_currentGameState);
        else if (navDown)
          focusChanged = m_menu.moveFocus2D(0, 1, m_currentGameState);
        else if (navLeft) {
          if (m_currentGameState == GameState::Settings ||
              m_currentGameState == GameState::GameOver)
            focusChanged = m_menu.moveFocus2D(-1, 0, m_currentGameState);
        } else if (navRight) {
          if (m_currentGameState == GameState::Settings ||
              m_currentGameState == GameState::GameOver)
            focusChanged = m_menu.moveFocus2D(1, 0, m_currentGameState);
        }
        if (focusChanged && m_currentGameState != GameState::Controls) {
          m_menuSwitchSoundTimer = 0.08f;
        }
      }
      if (!anyNav)
        m_stickNavTimer = navRepeatDelay; //? allow immediate first move
    }

    if (m_mouseSwitchCooldown > 0.f)
      m_mouseSwitchCooldown -= dt.asSeconds();

    m_menu.updatePulse(dt.asSeconds());

    if (m_rumbleTimer > 0.f) {
      m_rumbleTimer -= dt.asSeconds();
      if (m_rumbleTimer <= 0.f) {
        m_rumbleTimer = 0.f;
        if (!m_bossRumbleActive)
          setVibration(0.f, 0.f);
      }
    }
    //? Stop boss rumble when boss phase leaves Death
    if (m_bossRumbleActive && m_boss.getPhase() != BossPhase::Death) {
      m_bossRumbleActive = false;
      setVibration(0.f, 0.f);
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

      m_menu.fadeOutMusic(dt.asSeconds());

      if (m_fadeAlpha >= 255.0f) {
        m_fadeAlpha = 255.0f;

        bool waitingForSound = false;
        if ((m_currentGameState == GameState::MainMenu ||
             m_currentGameState == GameState::GameOver) &&
            !m_goToMenuAfterFade) {
          if (m_gameStartSound.getStatus() == sf::Sound::Status::Playing) {
            waitingForSound = true;
          }
        }

        if (!waitingForSound) {
          m_isFadingOut = false;

          if (m_goToMenuAfterFade) {
            m_currentGameState = GameState::MainMenu;
            m_winMusic.stop();
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
      }
      m_fadeRect.setFillColor(
          sf::Color(0, 0, 0, static_cast<int>(m_fadeAlpha)));
    }

    if (m_isPlayerDying && m_currentGameState != GameState::Paused) {
      m_deathAnimTimer += dt.asSeconds();
      sf::Sprite &pSprite = m_player.getSprite();

      if (m_Episode1Music.getVolume() > 0)
        m_Episode1Music.setVolume(std::max(0.f, m_Episode1Music.getVolume() -
                                                    dt.asSeconds() * 100.f));
      if (m_Episode2Music.getVolume() > 0)
        m_Episode2Music.setVolume(std::max(0.f, m_Episode2Music.getVolume() -
                                                    dt.asSeconds() * 100.f));
      if (m_Episode3Music.getVolume() > 0)
        m_Episode3Music.setVolume(std::max(0.f, m_Episode3Music.getVolume() -
                                                    dt.asSeconds() * 100.f));

      if (m_deathAnimWhitePhase) {
        if (m_deathFizzSound.getStatus() != sf::Sound::Status::Playing) {
          m_deathAnimWhitePhase = false;
          m_deathAnimLaunched = true;
          pSprite.setColor(sf::Color::White);
          pSprite.setPosition(m_deathAnimVelocity);

          m_deathAnimVelocity.y = -600.0f;
          float leftSpace = pSprite.getPosition().x - m_currentLeftWall;
          float rightSpace =
              (currentW - m_currentRightWall) - pSprite.getPosition().x;
          m_deathAnimVelocity.x = (leftSpace > rightSpace) ? -200.0f : 200.0f;

          setVibration(0.f, 0.f);
          m_rumbleTimer = 0.f;
        } else {
          if (m_deathAnimTimer <= dt.asSeconds()) {
            m_deathAnimVelocity = pSprite.getPosition();
          }
          float shakeX = std::sin(m_deathAnimTimer * 200.f) * 20.f +
                         std::sin(m_deathAnimTimer * 137.f) * 15.f;
          float shakeY = std::cos(m_deathAnimTimer * 180.f) * 20.f +
                         std::cos(m_deathAnimTimer * 151.f) * 15.f;
          pSprite.setPosition(sf::Vector2f(m_deathAnimVelocity.x + shakeX,
                                           m_deathAnimVelocity.y + shakeY));
          pSprite.setColor(
              sf::Color(255, 255, 255, (rand() % 100 > 30) ? 255 : 150));
        }
      } else if (m_deathAnimLaunched) {
        m_deathAnimVelocity.y += m_deathAnimGravity * dt.asSeconds();
        pSprite.move(m_deathAnimVelocity * dt.asSeconds());

        if (m_deathAnimBounceCount == 0) {
          float spinDir = (m_deathAnimVelocity.x > 0) ? 1.0f : -1.0f;
          pSprite.rotate(sf::degrees(spinDir * 800.0f * dt.asSeconds()));
        }

        float floorY = m_currentWindowSize.y - 120.0f;
        if (pSprite.getPosition().y > floorY) {
          pSprite.setPosition(sf::Vector2f(pSprite.getPosition().x, floorY));

          float rot = pSprite.getRotation().asDegrees();
          while (rot < 0.f)
            rot += 360.f;
          while (rot >= 360.f)
            rot -= 360.f;
          if (rot > 0.f && rot <= 180.f) {
            pSprite.setRotation(sf::degrees(90.0f));
          } else {
            pSprite.setRotation(sf::degrees(270.0f));
          }

          if (m_deathAnimBounceCount == 0) {
            m_deathSound.play();
            m_deathAnimVelocity.y = -250.0f;
            m_deathAnimVelocity.x *= 0.5f;
            m_deathAnimBounceCount++;
            setVibration(0.8f, 0.8f);
            m_rumbleTimer = 0.1f;

            m_currentGameState = GameState::GameOver;
            m_menu.resetFocus(GameState::GameOver);
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
            m_lastGameState = GameState::GameOver;
          } else if (m_deathAnimBounceCount == 1) {
            m_deathAnimVelocity.y = -120.0f;
            m_deathAnimBounceCount++;
            setVibration(0.5f, 0.5f);
            m_rumbleTimer = 0.1f;
          } else if (m_deathAnimBounceCount < 7) {
            m_deathAnimVelocity.y = -60.0f;
            m_deathAnimBounceCount++;
            setVibration(0.2f, 0.2f);
            m_rumbleTimer = 0.05f;
          } else {
            m_deathAnimLaunched = false;
            m_deathAnimVelocity.x = pSprite.getPosition().x;
          }
        }
      } else if (!m_deathAnimLaunched && !m_deathAnimWhitePhase) {
        float t = std::fmod(m_deathAnimTimer, 3.0f);
        if (t > 2.8f) {
          float shakeX = ((std::rand() % 100) / 100.0f - 0.5f) * 10.0f;
          float shakeY = ((std::rand() % 100) / 100.0f - 0.5f) * 10.0f;
          pSprite.setPosition(
              sf::Vector2f(m_deathAnimVelocity.x + shakeX,
                           m_currentWindowSize.y - 120.0f + shakeY));
        } else {
          pSprite.setPosition(sf::Vector2f(m_deathAnimVelocity.x,
                                           m_currentWindowSize.y - 120.0f));
        }
      }
    }

    switch (m_currentGameState) {
    case GameState::Playing: {

      bool victoryInvincibility = false;
      if (m_boss.getPhase() == BossPhase::Death) {
        victoryInvincibility = true;
        //? Continuous rumble during boss death
        if (!m_bossRumbleActive) {
          m_bossRumbleActive = true;
          setVibration(0.9f, 0.9f);
        }

        float currentBossVol = m_Episode3Music.getVolume();
        if (currentBossVol > 0.0f) {
          float newVol = currentBossVol - (dt.asSeconds() * 12.5f);
          if (newVol < 0.0f)
            newVol = 0.0f;
          m_Episode3Music.setVolume(newVol);
        }

        if (m_winMusic.getStatus() != sf::Sound::Status::Playing) {
          m_winMusic.setVolume(0.0f);
          m_winMusic.play();
        }
        float currentWinVol = m_winMusic.getVolume();
        if (currentWinVol < 90.0f) {
          float newWinVol = currentWinVol + (dt.asSeconds() * 10.0f);
          if (newWinVol > 90.0f)
            newWinVol = 90.0f;
          m_winMusic.setVolume(newWinVol);
        }
      }

      bool shouldPlayWallSound =
          (m_currentEpisode == GameEpisode::VendingMachine);
      if (!m_isPlayerDying) {
        m_player.update(dt, currentW, currentH, m_currentLeftWall,
                        m_currentRightWall, m_currentTopWall, m_WallPushBack,
                        shouldPlayWallSound,
                        static_cast<int>(m_currentEpisode));
      }

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
           m_currentEpisodeTime > 0.0f) ||
          m_isPlayerDying) {
        m_enemySpawnTimer -= dt.asSeconds();
        if (!m_isPlayerDying) {
          m_currentEpisodeTime += dt.asSeconds();
          m_heartSpawnTimer += dt.asSeconds();
          if ((m_gameSettings.gameDifficulty == GameDifficulty::Easy) &&
              m_heartSpawnTimer >= 25.0f) {
            spawnHeart();
            m_heartSpawnTimer = 0.0f;
          } else if ((m_gameSettings.gameDifficulty ==
                      GameDifficulty::Normal) &&
                     m_heartSpawnTimer >= 20.0f) {
            spawnHeart();
            m_heartSpawnTimer = 0.0f;
          } else if ((m_gameSettings.gameDifficulty == GameDifficulty::Hard) &&
                     m_heartSpawnTimer >= 5.0f) {
            spawnHeart();
            m_heartSpawnTimer = 0.0f;
          }
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
          for (auto &enemy : m_enemies) {
            enemy.update(dt, currentW, currentH, m_currentLeftWall,
                         m_currentRightWall, m_player.getPosition(),
                         m_cookieSound);
          }
          if (!m_isPlayerDying) {
            m_boss.update(dt, m_player.getPosition(), currentW, currentH,
                          m_gameSettings);
          }
          isHit = false;

          const auto &handleCircles = m_boss.getHandleHitboxes();
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
            if (!victoryInvincibility && !m_player.isInvincible()) {
              m_player.loseHealth();
              setVibration(0.4f, 0.4f);
              m_rumbleTimer = 0.45f;
            }

            if (m_player.getHealth() <= 0) {
              triggerPlayerDeath();
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

                static bool lastSpawnLeft = false;
                bool spawnLeft = !lastSpawnLeft;
                lastSpawnLeft = spawnLeft;

                float spawnX = spawnLeft
                                   ? (m_machineLeftWall + 130.0f)
                                   : (currentW - m_machineRightWall - 105.0f);
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

              //* SPAWN SIDE
              int side = std::rand() % 4;
              float offset = 100.0f;
              float spawnX = 0, spawnY = 0;

              if (side == 0) //* UP
              {
                spawnX =
                    static_cast<float>(std::rand() % m_gameWindow.getSize().x);
                spawnY = -offset;
              } else if (side == 1) //* RIGHT
              {
                spawnX = m_gameWindow.getSize().x + offset;
                spawnY =
                    static_cast<float>(std::rand() % m_gameWindow.getSize().y);
              } else if (side == 2) //* DOWN
              {
                spawnX =
                    static_cast<float>(std::rand() % m_gameWindow.getSize().x);
                spawnY = m_gameWindow.getSize().y + offset;
              } else //* LEFT
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

          if (m_isPlayerDying && !m_deathAnimWhitePhase) {
            m_cookieSound.stop();
          } else {
            for (auto &enemy : m_enemies) {
              enemy.update(dt, currentW, currentH, m_machineLeftWall,
                           m_machineRightWall, m_player.getPosition(),
                           m_cookieSound);
            }
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

      bool isHit = false; //? reset per-frame for enemy collision

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

          if (!isHit) {
            for (const auto &pCircle : playerCircles) {
              if (enemy.checkCollision(pCircle)) {
                isHit = true;
                break;
              }
            }
          }
        } else {
          for (const auto &pCircle : playerCircles) {
            if (enemy.checkCollision(pCircle)) {
              isHit = true;
              break;
            }
          }
        }

        if (isHit)
          break;
      }

      if (isHit && !m_isPlayerDying) {
        if (!victoryInvincibility && !m_player.isInvincible()) {
          m_player.loseHealth();
          setVibration(0.4f, 0.4f);
          m_rumbleTimer = 0.45f;
        }
        if (m_player.getHealth() <= 0) {
          triggerPlayerDeath();
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
      break;
    }

    case GameState::Paused:
    case GameState::Settings:
    case GameState::Controls:
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

        float totalTime = 10.0f;
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

          if (m_nextEpisode == GameEpisode::VendingMachine)
            playCutscene(1);
          else if (m_nextEpisode == GameEpisode::Survival)
            playCutscene(2);
          else if (m_nextEpisode == GameEpisode::BossFight)
            playCutscene(3);

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

    //! DRAW LOGIC
    m_gameWindow.clear(sf::Color::Black);

    if (m_currentGameState == GameState::Controls) {
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
          m_currentGameState == GameState::Settings ||
          m_currentGameState == GameState::GameOver) {
        for (auto &enemy : m_enemies) {
          enemy.draw(m_gameWindow, m_gameSettings);
        }
        if (m_currentEpisode == GameEpisode::BossFight) {
          m_boss.draw(m_gameWindow, m_gameSettings);
        }

        if (m_isPlayerDying) {
          uint8_t alpha = 0;
          if (!m_deathAnimWhitePhase) {
            alpha =
                static_cast<uint8_t>(std::min(m_deathAnimTimer * 200.f, 255.f));
          }
          if (alpha > 0) {
            sf::RectangleShape dimRect((sf::Vector2f)m_gameWindow.getSize());
            dimRect.setFillColor(sf::Color(0, 0, 0, alpha));
            m_gameWindow.draw(dimRect);
          }
        }

        if (m_currentEpisode == GameEpisode::Survival &&
            m_gameSettings.showHitbox == true) {
          m_gameWindow.draw(m_slowSafeZoneRect);
        }

        for (auto &heart : m_hearts) {
          m_gameWindow.draw(heart.healSprite);
        }

        if (m_isPlayerDying && m_deathAnimWhitePhase) {
          sf::Vector2f center = m_deathAnimVelocity;
          float rayShakeX = ((rand() % 100) / 100.0f - 0.5f) * 15.0f;
          float rayShakeY = ((rand() % 100) / 100.0f - 0.5f) * 15.0f;
          center.x += rayShakeX;
          center.y += rayShakeY;

          float totalFizzTime =
              m_deathFizzSoundBuffer.getDuration().asSeconds();
          float progress = std::min(m_deathAnimTimer / totalFizzTime, 1.0f);

          sf::Sprite whiteJar = m_player.getSprite();
          uint8_t alpha = static_cast<uint8_t>(progress * 255.f);
          whiteJar.setColor(sf::Color(255, 255, 255, alpha));
          m_gameWindow.draw(whiteJar, sf::BlendAdd);
          m_gameWindow.draw(whiteJar, sf::BlendAdd);
          m_gameWindow.draw(whiteJar, sf::BlendAdd);

          float blinkFactor = (std::sin(m_deathAnimTimer * 5.f) + 1.f) * 0.5f;
          uint8_t blueComp = static_cast<uint8_t>(100.f + blinkFactor * 155.f);
          sf::Color effectColor(255, 255, blueComp, 180);

          float maxRadius = 10.f + progress * progress * 400.f;
          float currentRadius = maxRadius;
          sf::CircleShape circle(currentRadius);
          circle.setOrigin(sf::Vector2f(currentRadius, currentRadius));
          circle.setPosition(center);
          circle.setFillColor(effectColor);
          m_gameWindow.draw(circle);

          float rayLength = std::min(m_deathAnimTimer * 2857.f, 2000.f) +
                            progress * progress * 200.f;
          float rayThickness = 40.f + progress * progress * 450.f;

          sf::ConvexShape ray(5);
          float arrowHead = std::min(rayThickness * 1.5f, rayLength * 0.8f);
          ray.setPoint(0, sf::Vector2f(0, -rayThickness / 2.f));
          ray.setPoint(
              1, sf::Vector2f(rayLength - arrowHead, -rayThickness / 2.f));
          ray.setPoint(2, sf::Vector2f(rayLength, 0));
          ray.setPoint(3,
                       sf::Vector2f(rayLength - arrowHead, rayThickness / 2.f));
          ray.setPoint(4, sf::Vector2f(0, rayThickness / 2.f));

          ray.setFillColor(effectColor);
          ray.setPosition(center);

          sf::ConvexShape sideRay = ray;
          sf::Color sideColor = effectColor;
          sideColor.a = 120;
          sideRay.setFillColor(sideColor);
          sideRay.setPosition(center);

          for (int i = 0; i < 8; i++) {
            ray.setRotation(sf::degrees(i * 45.f + m_deathAnimTimer * 120.f));
            m_gameWindow.draw(ray);

            sideRay.setRotation(
                sf::degrees(i * 45.f + 22.5f - m_deathAnimTimer * 80.f));
            m_gameWindow.draw(sideRay);
          }
        }
        m_player.draw(m_gameWindow, m_gameSettings);

        if (m_isPlayerDying && m_deathAnimWhitePhase) {
          float totalFizzTime =
              m_deathFizzSoundBuffer.getDuration().asSeconds();
          float progress = std::min(m_deathAnimTimer / totalFizzTime, 1.0f);
          sf::Sprite whiteJar = m_player.getSprite();
          uint8_t alpha = static_cast<uint8_t>(progress * 255.f);
          whiteJar.setColor(sf::Color(255, 255, 255, alpha));
          m_gameWindow.draw(whiteJar, sf::BlendAdd);
        }
      }

      if (m_currentGameState == GameState::Paused ||
          m_currentGameState == GameState::MainMenu ||
          m_currentGameState == GameState::Settings ||
          m_currentGameState == GameState::GameOver) {

        sf::View oldView = m_gameWindow.getView();
        if (m_currentGameState == GameState::GameOver) {
          m_gameOverAnimTimer += dt.asSeconds();
          float t = std::min(m_gameOverAnimTimer * 1.5f, 1.0f);
          float scale = 1.0f;
          if (t < 1.0f) {
            scale =
                std::pow(2.0f, -10.0f * t) *
                    std::sin((t * 10.0f - 0.75f) * ((2.0f * 3.14159f) / 3.0f)) +
                1.0f;
            scale = std::max(0.01f, scale);
          }
          uint8_t alpha =
              static_cast<uint8_t>(std::min(t * 1.5f, 1.0f) * 255.f);
          m_menu.setGlobalAlpha(alpha);

          sf::View menuView = m_gameWindow.getDefaultView();
          menuView.zoom(1.f / scale);
          m_gameWindow.setView(menuView);
        } else {
          m_menu.setGlobalAlpha(255);
        }

        m_menu.draw(m_gameWindow, m_currentGameState);

        if (m_currentGameState == GameState::GameOver) {
          m_gameWindow.setView(oldView);
        }
      }
      if (!m_fpsFontIsLoaded)
        m_gameWindow.draw(m_fpsErrorRect);
      else if (m_gameSettings.showFps)
        m_gameWindow.draw(m_fpsText);
    }

    if (m_fadeAlpha > 0.0f) {
      m_gameWindow.draw(m_fadeRect);
    }

    m_gameWindow.display();
  }
}

void Game::spawnHeart() {
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

void Game::triggerPlayerDeath() {
  m_isPlayerDying = true;
  m_player.prepareForDeathAnim();
  m_boss.stopSound();

  if (m_deathFizzSound.getStatus() != sf::Sound::Status::Playing)
    m_deathFizzSound.play();

  updateSfxVolume(m_gameSettings.playSfx);

  m_deathAnimTimer = 0.0f;
  m_gameOverAnimTimer = 0.0f;
  m_deathAnimBounceCount = 0;
  m_deathAnimLaunched = false;
  m_deathAnimWhitePhase = true;
  m_rumbleTimer = 999.0f;
  setVibration(0.2f, 0.2f);
}

void Game::startNewGame(bool useTransition) {
  GameEpisode startingEpisode = GameEpisode::VendingMachine;
  m_isPlayerDying = false;
  updateSfxVolume(m_gameSettings.playSfx);

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
    m_episodeDuration = 55.0f;

    if (!m_bgTexture.loadFromFile("assets/images/episode_1.jpg"))
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
    m_episodeDuration = 120.0f;
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

    if (!m_bgTexture.loadFromFile("assets/images/episode_2.png"))
      std::cerr << "Failed to load episode 2 background!" << std::endl;
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

    if (!m_bgTexture.loadFromFile("assets/images/episode_3.png"))
      std::cerr << "Failed to load episode 3 background!" << std::endl;
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
  m_transitionTimer = 10.0f;
  m_player.stopSound();
  m_boss.stopSound();
  m_menu.updateMusicVolume(false);
  m_transitionSound.setVolume(50.0f);

  if (m_transitionSound.getStatus() != sf::Sound::Status::Playing)
    m_transitionSound.play();

  if (m_nextEpisode == GameEpisode::VendingMachine) {
    m_loadingText.setFont(m_gameSettings.ukrainianLanguage ? m_loadingFontUA
                                                           : m_loadingFont);
    if (m_gameSettings.ukrainianLanguage)
      m_loadingText.setString(
          U"\u0415\u043F\u0456\u0437\u043E\u0434 1 - "
          U"\u041D\u0430\u0440\u043E\u0434\u0436\u0435\u043D\u043D\u044F\n\n"
          U"\u0412\u0456\u0434\u0435\u043E "
          U"\u043E\u0441\u044C-\u043E\u0441\u044C "
          U"\u043F\u043E\u0447\u043D\u0435\u0442\u044C\u0441\u044F...\n("
          U"\u0413\u0440\u0430 "
          U"\u043F\u043E\u0441\u0442\u0430\u0432\u0438\u0442\u044C\u0441\u044F "
          U"\u043D\u0430 \u043F\u0430\u0443\u0437\u0443)");
    else
      m_loadingText.setString("Episode 1 - Birth\n\nVideo starts "
                              "soon...\n(Game will pause automatically)");
  } else if (m_nextEpisode == GameEpisode::Survival) {
    m_loadingText.setFont(m_gameSettings.ukrainianLanguage ? m_loadingFontUA
                                                           : m_loadingFont);
    if (m_gameSettings.ukrainianLanguage)
      m_loadingText.setString(
          U"\u0415\u043F\u0456\u0437\u043E\u0434 2 - "
          U"\u0421\u043F\u0440\u0430\u0433\u0430\n\n\u0412\u0456\u0434\u0435"
          U"\u043E \u043E\u0441\u044C-\u043E\u0441\u044C "
          U"\u043F\u043E\u0447\u043D\u0435\u0442\u044C\u0441\u044F...\n("
          U"\u0413\u0440\u0430 "
          U"\u043F\u043E\u0441\u0442\u0430\u0432\u0438\u0442\u044C\u0441\u044F "
          U"\u043D\u0430 \u043F\u0430\u0443\u0437\u0443)");
    else
      m_loadingText.setString("Episode 2 - Thirst\n\nVideo starts "
                              "soon...\n(Game will pause automatically)");
    m_transitionSound.play();
  } else if (m_nextEpisode == GameEpisode::BossFight) {
    m_loadingText.setFont(m_gameSettings.ukrainianLanguage ? m_loadingFontUA
                                                           : m_loadingFont);
    if (m_gameSettings.ukrainianLanguage)
      m_loadingText.setString(
          U"\u0415\u043F\u0456\u0437\u043E\u0434 3 - "
          U"\u041F\u0420\u0418\u0411\u0418\u0420\u0410\u041D\u041D\u042F!"
          U"\n\n\u0412\u0456\u0434\u0435\u043E "
          U"\u043E\u0441\u044C-\u043E\u0441\u044C "
          U"\u043F\u043E\u0447\u043D\u0435\u0442\u044C\u0441\u044F...\n("
          U"\u0413\u0440\u0430 "
          U"\u043F\u043E\u0441\u0442\u0430\u0432\u0438\u0442\u044C\u0441\u044F "
          U"\u043D\u0430 \u043F\u0430\u0443\u0437\u0443)");
    else
      m_loadingText.setString("Episode 3 - CLEANING!\n\nVideo starts "
                              "soon...\n(Game will pause automatically)");
    m_transitionSound.play();
  } else if (m_nextEpisode == GameEpisode::Victory) {
    if (!m_bgTexture.loadFromFile("assets/images/Victory_bg.jpg"))
      std::cerr << "Victory BG error" << std::endl;

    m_bgSprite.setTexture(m_bgTexture, true);
    float scaleX =
        static_cast<float>(m_currentWindowSize.x) / m_bgTexture.getSize().x;
    float scaleY =
        static_cast<float>(m_currentWindowSize.y) / m_bgTexture.getSize().y;
    m_bgSprite.setScale({scaleX, scaleY});

    m_loadingText.setCharacterSize(200);
    m_loadingText.setFont(m_gameSettings.ukrainianLanguage ? m_loadingFontUA
                                                           : m_loadingFont);
    if (m_gameSettings.ukrainianLanguage)
      m_loadingText.setString(
          U"\u0412\u0438 "
          U"\u0432\u0438\u0436\u0438\u043B\u0438!\n\u0414\u0430\u043B\u0456 "
          U"\u0431\u0443\u0434\u0435...\n\n(\u0411\u0443\u0434\u0435 "
          U"\u0437\u0456\u0433\u0440\u0430\u043D\u043E "
          U"\u043A\u0430\u0442\u0441\u0446\u0435\u043D\u0443 "
          U"4)\n\u041D\u0430\u0442\u0438\u0441\u043D\u0438 Enter "
          U"\u0449\u043E\u0431 "
          U"\u043F\u0440\u043E\u0434\u043E\u0432\u0436\u0438\u0442\u0438");
    else
      m_loadingText.setString("You Survived!\nTo be continued...\n\n(Cutscene "
                              "4 will be played)\nPress Enter to proceed");

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

//! CUTSCENE
void Game::playCutscene(int episodeNumber) {
  std::string videoPath;
  switch (episodeNumber) {
  case 1:
    videoPath = "assets/videos/Cutscene_1.mp4";
    break;
  case 2:
    videoPath = "assets/videos/Cutscene_2.mp4";
    break;
  case 3:
    videoPath = "assets/videos/Cutscene_3.mp4";
    break;
  case 4:
    videoPath = "assets/videos/Cutscene_4.mp4";
    break;
  default:
    return;
  }
  std::string command = "start " + videoPath;
  std::system(command.c_str());
}

//? handleMenuAction -> unified action dispatch (shared by mouse, keyboard, pad)
void Game::handleMenuAction(int actionId, float currentW, float currentH) {
  if (actionId == 0)
    return;

  if (actionId == 1) {
    if (m_currentGameState == GameState::MainMenu) {
      if (!m_isFadingIn && !m_isFadingOut) {
        if (m_gameStartSound.getStatus() != sf::Sound::Status::Playing)
          m_gameStartSound.play();
        startFadeOut(GameEpisode::VendingMachine);
      }
    } else if (m_currentGameState == GameState::GameOver) {
      if (!m_isFadingIn && !m_isFadingOut) {
        if (m_gameStartSound.getStatus() != sf::Sound::Status::Playing)
          m_gameStartSound.play();
        startNewGame(false);
      }
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
  } else if (actionId == 2) {
    m_lastGameState = m_currentGameState;
    m_currentGameState = GameState::Settings;
    m_menu.resetFocus(GameState::Settings);
    m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                            m_gameSettings, m_lastGameState);
  } else if (actionId == 3) {
    if (m_gameSettings.gameDifficulty == GameDifficulty::Easy)
      m_gameSettings.gameDifficulty = GameDifficulty::Normal;
    else if (m_gameSettings.gameDifficulty == GameDifficulty::Normal)
      m_gameSettings.gameDifficulty = GameDifficulty::Hard;
    else
      m_gameSettings.gameDifficulty = GameDifficulty::Easy;
    m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                            m_gameSettings, m_lastGameState);
  } else if (actionId == 4) {
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
    m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                            m_gameSettings, m_lastGameState);
  } else if (actionId == 5) {
    if (m_gameSettings.VSync) {
      m_gameWindow.setVerticalSyncEnabled(false);
      m_gameWindow.setFramerateLimit(120);
    } else {
      m_gameWindow.setVerticalSyncEnabled(true);
      m_gameWindow.setFramerateLimit(0);
    }
    m_gameSettings.VSync = !m_gameSettings.VSync;
    m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                            m_gameSettings, m_lastGameState);
  } else if (actionId == 6) {
    m_gameSettings.showFps = !m_gameSettings.showFps;
    m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                            m_gameSettings, m_lastGameState);
  } else if (actionId == 7) {
    m_gameSettings.showHitbox = !m_gameSettings.showHitbox;
    m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                            m_gameSettings, m_lastGameState);
  } else if (actionId == 8) {
    if (m_currentGameState == GameState::Controls) {
      m_currentGameState = GameState::Settings;
      m_menu.resetFocus(GameState::Settings);
    } else {
      m_currentGameState = m_lastGameState;
      m_menu.resetFocus(m_currentGameState);
      m_menu.setFocusedButtonIndex(1);
    }
    m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                            m_gameSettings, m_lastGameState);
  } else if (actionId == 9) {
    if (!m_isFadingIn && !m_isFadingOut) {
      if (m_currentGameState == GameState::MainMenu) {
        m_gameWindow.clear(sf::Color::Black);
        m_menu.draw(m_gameWindow, m_currentGameState);
        m_gameWindow.display();

        while (m_menuButtonSound.getStatus() == sf::Sound::Status::Playing) {
          sf::sleep(sf::milliseconds(5));
        }
        m_gameWindow.close();
      } else {
        startFadeOutToMenu();
      }
    }
  } else if (actionId == 10) {
    m_gameSettings.ukrainianLanguage = !m_gameSettings.ukrainianLanguage;
    m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                            m_gameSettings, m_lastGameState);
  } else if (actionId == 11) {
    m_currentGameState = GameState::Controls;
    m_menu.resetFocus(GameState::Controls);
    m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                            m_gameSettings, m_lastGameState);
  } else if (actionId == 12) {
    m_gameSettings.playSfx = !m_gameSettings.playSfx;
    updateSfxVolume(m_gameSettings.playSfx);
    m_player.updateSfxVolume(m_gameSettings.playSfx);
    m_boss.updateSfxVolume(m_gameSettings.playSfx);
    m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                            m_gameSettings, m_lastGameState);
  }
}

void Game::updateSfxVolume(bool playSfx) {
  float enemyMult = m_isPlayerDying ? 0.25f : 1.0f;
  m_cookieSound.setVolume(playSfx ? AudioConfig::COOKIE * enemyMult : 0.f);
  m_menuButtonSound.setVolume(playSfx ? AudioConfig::BUTTON_SELECT : 0.f);
  m_menuSwitchSound.setVolume(playSfx ? AudioConfig::BUTTON_SWITCH : 0.f);
  m_transitionSound.setVolume(playSfx ? AudioConfig::TRANSITION : 0.f);
  m_winSound.setVolume(playSfx ? AudioConfig::WIN_SOUND : 0.f);
  m_deathSound.setVolume(playSfx ? AudioConfig::DEATH : 0.f);
  m_deathFizzSound.setVolume(playSfx ? AudioConfig::DEATH_FIZZ : 0.f);
  m_healSound.setVolume(playSfx ? AudioConfig::HEAL : 0.f);
  m_heartSpawnSound.setVolume(playSfx ? AudioConfig::HEART_SPAWN : 0.f);
  m_gameLoadingSound.setVolume(playSfx ? AudioConfig::GAME_LOADING : 0.f);
  m_gameStartSound.setVolume(playSfx ? AudioConfig::GAME_START : 0.f);
}

//? setVibration -> XInput rumble (Windows only)
void Game::setVibration(float left, float right) {
#ifdef _WIN32
  XINPUT_VIBRATION vibration = {0};
  vibration.wLeftMotorSpeed = static_cast<uint16_t>(left * 65535.f);
  vibration.wRightMotorSpeed = static_cast<uint16_t>(right * 65535.f);
  XInputSetState(0, &vibration);
#else
  (void)left;
  (void)right;
#endif
}
