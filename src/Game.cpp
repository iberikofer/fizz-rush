#include "Game.hpp"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>

PadBtn Game::mapJoystickButtonToPadBtn(unsigned int joystickId,
                                       unsigned int buttonId) {
  if (!sf::Joystick::isConnected(joystickId))
    return static_cast<PadBtn>(buttonId);

  sf::Joystick::Identification id = sf::Joystick::getIdentification(joystickId);
  std::string name = id.name.toAnsiString();
  bool isPS = (name.find("Sony") != std::string::npos ||
               name.find("DualShock") != std::string::npos ||
               name.find("DualSense") != std::string::npos);

  if (isPS) {
    switch (buttonId) {
    case 1:
      return PadBtn::A;
    case 2:
      return PadBtn::B;
    case 0:
      return PadBtn::X;
    case 3:
      return PadBtn::Y;
    case 8:
      return PadBtn::Select;
    case 9:
      return PadBtn::Start;
    default:
      return static_cast<PadBtn>(buttonId);
    }
  }
  return static_cast<PadBtn>(buttonId);
}

bool Game::isPadButtonPressed(unsigned int joystickId, PadBtn btn) {
  if (!sf::Joystick::isConnected(joystickId))
    return false;

  sf::Joystick::Identification id = sf::Joystick::getIdentification(joystickId);
  std::string name = id.name.toAnsiString();
  bool isPS = (name.find("Sony") != std::string::npos ||
               name.find("DualShock") != std::string::npos ||
               name.find("DualSense") != std::string::npos);

  unsigned int physicalBtn = static_cast<unsigned int>(btn);
  if (isPS) {
    switch (btn) {
    case PadBtn::A:
      physicalBtn = 1;
      break;
    case PadBtn::B:
      physicalBtn = 2;
      break;
    case PadBtn::X:
      physicalBtn = 0;
      break;
    case PadBtn::Y:
      physicalBtn = 3;
      break;
    case PadBtn::Select:
      physicalBtn = 8;
      break;
    case PadBtn::Start:
      physicalBtn = 9;
      break;
    default:
      break;
    }
  }
  return sf::Joystick::isButtonPressed(joystickId, physicalBtn);
}

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
      m_winMusic(m_winMusicBuffer), m_deathSound(m_deathSoundBuffer),
      m_healSound(m_healSoundBuffer),
      m_heartSpawnSound(m_heartSpawnSoundBuffer),
      m_gameLoadingSound(m_gameLoadingSoundBuffer),
      m_gameStartSound(m_gameStartSoundBuffer),
      m_menu(static_cast<float>(sf::VideoMode::getDesktopMode().size.x),
             static_cast<float>(sf::VideoMode::getDesktopMode().size.y)),
      m_player(static_cast<float>(sf::VideoMode::getDesktopMode().size.x),
               static_cast<float>(sf::VideoMode::getDesktopMode().size.y)),
      m_deathFizzSound(m_deathFizzSoundBuffer),
      m_typewriterSound(m_typewriterSoundBuffer) {
  m_gameSettings.loadFromFile("settings.ini");
  m_gameWindow.create(sf::VideoMode::getDesktopMode(), "Fizz Rush!",
                      sf::Style::None, sf::State::Fullscreen);
  m_gameWindow.setKeyRepeatEnabled(false);
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
  if (!m_typewriterSoundBuffer.loadFromFile(
          "assets/sound/story_typewrighter.ogg"))
    std::cerr << "Typewriter sound error!" << std::endl;
  m_typewriterSound.setBuffer(m_typewriterSoundBuffer);
  m_typewriterSound.setLooping(true);
  m_typewriterSound.setPitch(1.3f);
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

    if (m_menu.consumeHoverSoundFlag()) {
      m_menuSwitchSound.play();
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
      } else if (m_isFadingIn || m_isFadingOut) {
        continue;
      } else if (const auto *mouseMoved =
                     event->getIf<sf::Event::MouseMoved>()) {
        if (!m_gameWindow.hasFocus())
          continue;
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
          // Hover sound is now handled by consumeHoverSoundFlag
        }

      } else if (const auto *mouseClick =
                     event->getIf<sf::Event::MouseButtonPressed>()) {
        m_gameWindow.requestFocus();
        if (!m_gameWindow.hasFocus())
          continue;
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
        if (!m_gameWindow.hasFocus())
          continue;

        bool isNavKey = (keyPressed->code == sf::Keyboard::Key::Up ||
                         keyPressed->code == sf::Keyboard::Key::Down ||
                         keyPressed->code == sf::Keyboard::Key::Left ||
                         keyPressed->code == sf::Keyboard::Key::Right ||
                         keyPressed->code == sf::Keyboard::Key::W ||
                         keyPressed->code == sf::Keyboard::Key::A ||
                         keyPressed->code == sf::Keyboard::Key::S ||
                         keyPressed->code == sf::Keyboard::Key::D);
        if (isNavKey && m_inputMode != InputMode::Keyboard) {
          m_inputMode = InputMode::Keyboard;
          m_menu.setInputMode(InputMode::Keyboard);
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

        if (isMenuScreen && m_pendingMenuAction == -1) {
          if (keyPressed->code == sf::Keyboard::Key::Up ||
              keyPressed->code == sf::Keyboard::Key::W) {
            m_menu.moveFocus2D(0, -1, m_currentGameState);
          } else if (keyPressed->code == sf::Keyboard::Key::Down ||
                   keyPressed->code == sf::Keyboard::Key::S) {
            m_menu.moveFocus2D(0, 1, m_currentGameState);
          } else if (keyPressed->code == sf::Keyboard::Key::Left ||
                   keyPressed->code == sf::Keyboard::Key::A) {
            if (m_currentGameState == GameState::Settings ||
                m_currentGameState == GameState::GameOver)
              m_menu.moveFocus2D(-1, 0, m_currentGameState);
          } else if (keyPressed->code == sf::Keyboard::Key::Right ||
                     keyPressed->code == sf::Keyboard::Key::D) {
            if (m_currentGameState == GameState::Settings ||
                m_currentGameState == GameState::GameOver)
              m_menu.moveFocus2D(1, 0, m_currentGameState);
          }
          // Focus sound is now handled by consumeHoverSoundFlag
        }

        if (keyPressed->code == sf::Keyboard::Key::Escape) {
          if (m_currentGameState == GameState::Playing ||
              m_currentGameState == GameState::StoryScreen) {
            m_lastGameState = m_currentGameState;
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
            m_currentGameState = m_lastGameState;
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
            if (m_currentGameState == GameState::Playing &&
                m_gameSettings.playMusic) {
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
          } else if (m_currentGameState == GameState::GameOver ||
                     m_currentGameState == GameState::Disclaimer) {
            if (!m_isFadingOut) {
              m_isFadingOut = true;
              m_fadeAlpha = 0.0f;
              m_goToMenuAfterFade = true;
            }
          }
        }

        if (keyPressed->code == sf::Keyboard::Key::Enter) {
          if (m_currentGameState == GameState::EpisodeTransition &&
              m_nextEpisode != GameEpisode::Victory &&
              m_waitingForCutsceneReturn && m_cutsceneCooldown <= 0.0f) {
            m_cutsceneSkipped = true;
          } else if (m_currentGameState == GameState::EpisodeTransition &&
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
          if (m_currentGameState == GameState::Disclaimer && !m_isFadingOut) {
            m_isFadingOut = true;
            m_fadeAlpha = 0.0f;
            m_goToMenuAfterFade = false;
          }
          if (m_currentGameState == GameState::StoryScreen && !m_isFadingOut) {
            if (!m_typewriterDone) {
              m_storyTextVisible = m_storyTextFull;
              m_typewriterDone = true;
              m_typewriterIndex = m_storyTextFull.getSize();
            } else {
              m_isFadingOut = true;
              m_fadeAlpha = 0.0f;
              m_goToMenuAfterFade = false;
            }
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
            m_gameWindow.setKeyRepeatEnabled(false);
            m_gameWindow.setPosition({0, 0});
            m_currentWindowSize = m_gameWindow.getSize();
          } else {
            m_gameWindow.create(sf::VideoMode::getDesktopMode(),
                                m_gameWindowName, sf::State::Fullscreen);
            m_gameWindow.setKeyRepeatEnabled(false);
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
        if (!m_gameWindow.hasFocus())
          continue;
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

        PadBtn btn =
            mapJoystickButtonToPadBtn(joyBtn->joystickId, joyBtn->button);

        //? A button (0) -> Confirm (like Enter)
        if (btn == PadBtn::A) {
          if (m_currentGameState == GameState::EpisodeTransition &&
              m_nextEpisode != GameEpisode::Victory &&
              m_waitingForCutsceneReturn && m_cutsceneCooldown <= 0.0f) {
            m_cutsceneSkipped = true;
          }
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
          if (m_currentGameState == GameState::Disclaimer && !m_isFadingOut) {
            m_isFadingOut = true;
            m_fadeAlpha = 0.0f;
            m_goToMenuAfterFade = false;
          }
          if (m_currentGameState == GameState::StoryScreen && !m_isFadingOut) {
            if (!m_typewriterDone) {
              m_storyTextVisible = m_storyTextFull;
              m_typewriterDone = true;
              m_typewriterIndex = m_storyTextFull.getSize();
            } else {
              m_isFadingOut = true;
              m_fadeAlpha = 0.0f;
              m_goToMenuAfterFade = false;
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
          } else if (m_currentGameState == GameState::GameOver ||
                     m_currentGameState == GameState::Disclaimer) {
            if (!m_isFadingOut) {
              m_isFadingOut = true;
              m_fadeAlpha = 0.0f;
              m_goToMenuAfterFade = true;
            }
          }
        }

        //? Start button (7) -> Pause/Resume during gameplay
        if (btn == PadBtn::Start) {
          if (m_currentGameState == GameState::Playing ||
              m_currentGameState == GameState::StoryScreen) {
            m_lastGameState = m_currentGameState;
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
            m_currentGameState = m_lastGameState;
            m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                                    m_gameSettings, m_lastGameState);
            if (m_currentGameState == GameState::Playing &&
                m_gameSettings.playMusic) {
              m_Episode1Music.setVolume(30.0f);
              m_Episode2Music.setVolume(30.0f);
              m_Episode3Music.setVolume(45.0f);
            }
          }
        }

      } else if (event->is<sf::Event::FocusLost>()) {
        if (m_currentGameState == GameState::Playing ||
            m_currentGameState == GameState::StoryScreen) {
          bool canPause = true;
          if (m_currentGameState == GameState::Playing &&
              m_currentEpisode == GameEpisode::BossFight &&
              m_boss.getPhase() == BossPhase::Death)
            canPause = false;

          if (canPause) {
            m_lastGameState = m_currentGameState;
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
        m_gameWindow.requestFocus();
        m_gameClock.restart();
      }
    }

    bool isMenuActiveForNav = (m_currentGameState == GameState::MainMenu ||
                               m_currentGameState == GameState::Paused ||
                               m_currentGameState == GameState::Settings ||
                               m_currentGameState == GameState::Controls ||
                               m_currentGameState == GameState::GameOver);
    if (isMenuActiveForNav && sf::Joystick::isConnected(0) &&
        m_gameWindow.hasFocus()) {
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
      if (anyNav && m_stickNavTimer >= navRepeatDelay &&
          m_pendingMenuAction == -1) {
        m_stickNavTimer = 0.f;
        if (navUp)
          m_menu.moveFocus2D(0, -1, m_currentGameState);
        else if (navDown)
          m_menu.moveFocus2D(0, 1, m_currentGameState);
        else if (navLeft) {
          if (m_currentGameState == GameState::Settings ||
              m_currentGameState == GameState::GameOver)
            m_menu.moveFocus2D(-1, 0, m_currentGameState);
        } else if (navRight) {
          if (m_currentGameState == GameState::Settings ||
              m_currentGameState == GameState::GameOver)
            m_menu.moveFocus2D(1, 0, m_currentGameState);
        }
        // Focus sound is now handled by consumeHoverSoundFlag
      }
      if (!anyNav)
        m_stickNavTimer = navRepeatDelay; //? allow immediate first move
    }

    if (m_mouseSwitchCooldown > 0.f)
      m_mouseSwitchCooldown -= dt.asSeconds();

    if (m_cutsceneCooldown > 0.f) {
      m_cutsceneCooldown -= dt.asSeconds();
    }

    m_menu.updatePulse(dt.asSeconds());

    if (m_currentGameState == GameState::EpisodeTransition &&
        m_nextEpisode != GameEpisode::Victory && !m_cutsceneSkipped) {
      bool holdKey = m_gameWindow.hasFocus() &&
                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter);
      bool holdBtn =
          m_gameWindow.hasFocus() && isPadButtonPressed(0, PadBtn::A);
      if (holdKey || holdBtn) {
        m_cutsceneSkipHoldTimer += dt.asSeconds();
        if (!m_skipTextAnimating) {
          m_skipTextAnimating = true;
          m_skipTextAnimTimer = 0.f;
        }
        if (m_cutsceneSkipHoldTimer >= 1.0f) {
          m_cutsceneSkipped = true;
        }
      } else {
        m_cutsceneSkipHoldTimer = 0.f;
        m_skipTextAnimating = false;
        m_skipTextAnimTimer = 0.f;
      }
    }

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
      if (m_blackScreenDelay > 0.0f) {
        m_blackScreenDelay -= dt.asSeconds();
        if (m_blackScreenDelay <= 0.0f) {
          if (m_currentGameState == GameState::EpisodeTransition) {
            if (m_transitionSound.getStatus() != sf::Sound::Status::Playing)
              m_transitionSound.play();
          }
        }
      } else {
        float fadeSpeed = 700.0f;
        m_fadeAlpha -= fadeSpeed * dt.asSeconds();

        if (m_fadeAlpha <= 0.0f) {
          m_fadeAlpha = 0.0f;
          m_isFadingIn = false;
        }
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
            m_isStoryActive = false;
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
          } else if (m_currentGameState == GameState::MainMenu &&
                     !m_fromGameOver) {
            m_currentGameState = GameState::Disclaimer;
            m_isFadingIn = true;
            m_fadeAlpha = 255.0f;
          } else if (m_currentGameState == GameState::Disclaimer) {
            startNewGame(false);
            m_nextEpisodeAfterFade = GameEpisode::VendingMachine;
            m_goToStoryAfterFade = true;
            m_isFadingOut = true;
          } else if (m_currentGameState == GameState::StoryScreen) {
            m_isStoryActive = false;
            startTransition(m_nextEpisodeAfterFade);
            m_blackScreenDelay = 0.5f;
            m_isFadingIn = true;
          } else if (m_currentGameState == GameState::EpisodeTransition) {
            m_isStoryActive = false;
            m_currentGameState = GameState::Playing;
            m_isFadingIn = true;
            m_fadeAlpha = 255.0f;
            setupEpisode(m_nextEpisode);
            m_gameClock.restart();
          } else if (m_goToStoryAfterFade) {
            m_goToStoryAfterFade = false;

            bool ua = m_gameSettings.ukrainianLanguage;
            std::string rawStoryText;
            if (m_nextEpisode == GameEpisode::VendingMachine) {
              if (ua) {
                rawStoryText =
                    "Звичайний вечір у залі неонових ігрових автоматів "
                    "обірвався\n"
                    "раптовим стрибком напруги. Іскра. Гучний тріск. Коротке "
                    "замикання.\n\n"
                    "Поки всі думали, що це просто чергова поломка старого "
                    "автомату з їжею,\n"
                    "електричний розряд зробив дещо неможливе.\n"
                    "Він випадково подарував свідомість... вам.\n\n"
                    "Так, ви - звичайна прохолодна баночка Спрайту, яка щойно "
                    "усвідомила\n"
                    "своє існування в цьому жорстокому світі. І у вас великі "
                    "проблеми.\n\n"
                    "Тепер цей світ пластику, металу та спраглих геймерів -\n"
                    "украй небезпечне місце. Ваша мета - вижити.\n"
                    "Бути випитим чи вибухнути від сильного збовтування -\n"
                    "обидва фінали означають неминучий кінець.\n\n"
                    "Час показати цьому світові, на що здатна звичайна баночка "
                    "Спрайту, наділена свідомістю.\n\n"
                    "Удачі!";
              } else {
                rawStoryText =
                    "A regular evening at the neon-lit arcade was cut short\n"
                    "by a sudden power surge. A spark. A loud crack. A short "
                    "circuit.\n\n"
                    "While everyone thought it was just another breakdown of "
                    "an "
                    "old vending\n"
                    "machine, the electric discharge did the impossible.\n"
                    "It accidentally granted consciousness to... you.\n\n"
                    "Yes, you are a regular, cold can of Sprite that has just "
                    "realized\n"
                    "its existence in this cruel world. And you are in deep "
                    "trouble.\n\n"
                    "Now, this world of plastic, metal, and thirsty gamers\n"
                    "is a highly dangerous place. Your goal is to survive.\n"
                    "Being drunk or exploding from being shaken too much -\n"
                    "both endings mean an inevitable Game Over.\n\n"
                    "It's time to show this world what a regular can of "
                    "Sprite, "
                    "endowed with consciousness, is capable of.\n\n"
                    "Good luck!";
              }
            } else if (m_nextEpisode == GameEpisode::Survival) {
              if (ua)
                rawStoryText = "[Placeholder - Епізод 2]";
              else
                rawStoryText = "[Placeholder - Episode 2]";
            } else if (m_nextEpisode == GameEpisode::BossFight) {
              if (ua)
                rawStoryText = "[Placeholder - Епізод 3]";
              else
                rawStoryText = "[Placeholder - Episode 3]";
            }

            m_storyTextFull =
                sf::String::fromUtf8(rawStoryText.begin(), rawStoryText.end());

            m_storyTextVisible.clear();
            m_typewriterTimer = 0.f;
            m_typewriterIndex = 0;
            m_typewriterDone = false;
            m_isStoryActive = true;
            m_currentGameState = GameState::StoryScreen;

            m_isFadingIn = true;
            m_fadeAlpha = 255.0f;
            m_isFadingOut = false;

            m_loadingText.setScale({1.0f, 1.0f});
            sf::Color textColor = m_loadingText.getFillColor();
            textColor.a = 255;
            m_loadingText.setFillColor(textColor);

            m_cutsceneSkipped = false;
            m_cutsceneSkipHoldTimer = 0.f;
            m_waitingForCutsceneReturn = false;
            m_skipTextAnimating = false;
            m_skipTextAnimTimer = 0.f;

            m_gameClock.restart();
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

          float floorY = currentH - 120.0f;
          float topY = m_machineTopWall;
          float playerY = pSprite.getPosition().y;
          float t = std::clamp((playerY - topY) / (floorY - topY), 0.0f, 1.0f);
          m_deathAnimVelocity.y = -350.0f - t * 580.0f;

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
            m_deathAnimVelocity.y = -180.0f;
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
            m_deathAnimVelocity.y = -90.0f;
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
      if (m_currentEpisode == GameEpisode::VendingMachine && m_bgSprite2) {
        if (m_player.hasPlayerMoved()) {
          float bgHeight = static_cast<float>(m_bgTexture.getSize().y) *
                           m_bgSprite.getScale().y;

          //? Stop bg scroll when near end of episode (last 10 sec)
          float timeLeft = m_episodeDuration - m_currentEpisodeTime;
          if (!m_bgStopped && timeLeft < 10.0f) {
            //? Check if either sprite reached y=0 (top of picture at screen)
            float p1y = m_bgSprite.getPosition().y;
            float p2y = m_bgSprite2->getPosition().y;
            if (p1y >= 0.0f || p2y >= 0.0f) {
              //? Snap the closest one to 0 and freeze
              if (p1y >= 0.0f)
                m_bgSprite.setPosition({0.0f, 0.0f});
              if (p2y >= 0.0f)
                m_bgSprite2->setPosition({0.0f, -bgHeight});
              m_bgStopped = true;
            }
          }

          if (!m_bgStopped) {
            float scrollAmt = m_bgScrollSpeed * dt.asSeconds();
            m_bgSprite.move({0.0f, -scrollAmt});
            m_bgSprite2->move({0.0f, -scrollAmt});

            if (m_bgSprite.getPosition().y <= -bgHeight) {
              m_bgSprite.setPosition(
                  {0.0f, m_bgSprite2->getPosition().y + bgHeight});
            }
            if (m_bgSprite2->getPosition().y <= -bgHeight) {
              m_bgSprite2->setPosition(
                  {0.0f, m_bgSprite.getPosition().y + bgHeight});
            }
          }
        }
      }

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

            if (m_currentEpisode == GameEpisode::VendingMachine) {
              float phase = m_currentEpisodeTime;

              if (phase < 300.0f) {
                //! PHASE 1 (0-5 min): ONLY Cans
                Type currentType = Type::Can;
                const sf::Texture *currentEnemyTexture = nullptr;
                if (!m_enemyCanTextures.empty())
                  currentEnemyTexture =
                      &m_enemyCanTextures[std::rand() %
                                          m_enemyCanTextures.size()];

                if (currentEnemyTexture) {
                  float finalSpeed = 0.0f;
                  if (m_gameSettings.gameDifficulty == GameDifficulty::Easy) {
                    finalSpeed = 900.0f;
                    m_enemySpawnTimer = 0.7f;
                  } else if (m_gameSettings.gameDifficulty ==
                             GameDifficulty::Normal) {
                    finalSpeed = 900.0f;
                    m_enemySpawnTimer = 0.45f;
                  } else {
                    finalSpeed = 950.0f;
                    m_enemySpawnTimer = 0.3f;
                  }

                  float enemyScale = 0.2f;
                  float texWidth = currentEnemyTexture->getSize().x;
                  float halfWidth = (texWidth * enemyScale) / 2.0f;
                  float minX = m_machineLeftWall + halfWidth;
                  float maxX = currentW - m_machineRightWall - halfWidth;
                  float spawnX =
                      minX + (std::rand() % static_cast<int>(maxX - minX));
                  float spawnY = -300.0f;

                  Enemy tempEnemy(*currentEnemyTexture, 0, 0);
                  tempEnemy.spawn(spawnX, spawnY, finalSpeed,
                                  *currentEnemyTexture, currentType,
                                  m_player.getPosition());
                  m_enemies.push_back(tempEnemy);
                }

              } else if (phase < 600.0f) {
                //! PHASE 2 (5-10 min): ONLY Bars
                Type currentType;
                const sf::Texture *currentEnemyTexture = nullptr;
                if (std::rand() % 2 == 0) {
                  currentType = Type::Bar;
                  currentEnemyTexture = &m_enemyBarTexture;
                } else {
                  currentType = Type::Bar2;
                  currentEnemyTexture = &m_enemyBar2Texture;
                }

                float finalSpeed = 0.0f;
                if (m_gameSettings.gameDifficulty == GameDifficulty::Easy) {
                  finalSpeed = -500.0f;
                  m_enemySpawnTimer = 0.8f;
                } else if (m_gameSettings.gameDifficulty ==
                           GameDifficulty::Normal) {
                  finalSpeed = -700.0f;
                  m_enemySpawnTimer = 0.55f;
                } else {
                  finalSpeed = -800.0f;
                  m_enemySpawnTimer = 0.35f;
                }

                float enemyScale = 0.2f;
                float texWidth = currentEnemyTexture->getSize().x;
                float halfWidth = (texWidth * enemyScale) / 2.0f;
                float minX = m_machineLeftWall + halfWidth;
                float maxX = currentW - m_machineRightWall - halfWidth;
                float spawnX =
                    minX + (std::rand() % static_cast<int>(maxX - minX));
                float spawnY = currentH + 100.0f;

                Enemy tempEnemy(*currentEnemyTexture, 0, 0);
                tempEnemy.spawn(spawnX, spawnY, finalSpeed,
                                *currentEnemyTexture, currentType,
                                m_player.getPosition());
                m_enemies.push_back(tempEnemy);

              } else if (phase < 900.0f) {
                //! PHASE 3 (10-15 min): ONLY Boxes
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

                if (m_gameSettings.gameDifficulty == GameDifficulty::Easy)
                  m_enemySpawnTimer = 2.5f;
                else if (m_gameSettings.gameDifficulty ==
                         GameDifficulty::Normal)
                  m_enemySpawnTimer = 1.8f;
                else
                  m_enemySpawnTimer = 1.2f;

                Enemy tempEnemy(*currentEnemyTexture, 0, 0);
                tempEnemy.spawn(spawnX, spawnY, finalSpeed,
                                *currentEnemyTexture, currentType,
                                m_player.getPosition(), &m_enemyCookieTexture);
                m_enemies.push_back(tempEnemy);

              } else {
                //! PHASE 4 (15-20 min): MIX — 50% Cans, 30% Bars, 20% Boxes
                int roll = std::rand() % 100;
                Type currentType;
                const sf::Texture *currentEnemyTexture = nullptr;

                if (roll < 50) {
                  //? Cans (most common, баночки)
                  currentType = Type::Can;
                  if (!m_enemyCanTextures.empty())
                    currentEnemyTexture =
                        &m_enemyCanTextures[std::rand() %
                                            m_enemyCanTextures.size()];

                  if (currentEnemyTexture) {
                    float finalSpeed = 0.0f;
                    if (m_gameSettings.gameDifficulty == GameDifficulty::Easy) {
                      finalSpeed = 900.0f;
                      m_enemySpawnTimer = 0.5f;
                    } else if (m_gameSettings.gameDifficulty ==
                               GameDifficulty::Normal) {
                      finalSpeed = 900.0f;
                      m_enemySpawnTimer = 0.35f;
                    } else {
                      finalSpeed = 950.0f;
                      m_enemySpawnTimer = 0.25f;
                    }

                    float enemyScale = 0.2f;
                    float texWidth = currentEnemyTexture->getSize().x;
                    float halfWidth = (texWidth * enemyScale) / 2.0f;
                    float minX = m_machineLeftWall + halfWidth;
                    float maxX = currentW - m_machineRightWall - halfWidth;
                    float spawnX =
                        minX + (std::rand() % static_cast<int>(maxX - minX));
                    float spawnY = -300.0f;

                    Enemy tempEnemy(*currentEnemyTexture, 0, 0);
                    tempEnemy.spawn(spawnX, spawnY, finalSpeed,
                                    *currentEnemyTexture, currentType,
                                    m_player.getPosition());
                    m_enemies.push_back(tempEnemy);
                  }

                } else if (roll < 80) {
                  //? Bars (medium)
                  if (std::rand() % 2 == 0) {
                    currentType = Type::Bar;
                    currentEnemyTexture = &m_enemyBarTexture;
                  } else {
                    currentType = Type::Bar2;
                    currentEnemyTexture = &m_enemyBar2Texture;
                  }
                  float finalSpeed = 0.0f;
                  if (m_gameSettings.gameDifficulty == GameDifficulty::Easy) {
                    finalSpeed = -500.0f;
                    m_enemySpawnTimer = 0.5f;
                  } else if (m_gameSettings.gameDifficulty ==
                             GameDifficulty::Normal) {
                    finalSpeed = -700.0f;
                    m_enemySpawnTimer = 0.35f;
                  } else {
                    finalSpeed = -800.0f;
                    m_enemySpawnTimer = 0.25f;
                  }

                  float enemyScale = 0.2f;
                  float texWidth = currentEnemyTexture->getSize().x;
                  float halfWidth = (texWidth * enemyScale) / 2.0f;
                  float minX = m_machineLeftWall + halfWidth;
                  float maxX = currentW - m_machineRightWall - halfWidth;
                  float spawnX =
                      minX + (std::rand() % static_cast<int>(maxX - minX));
                  float spawnY = currentH + 100.0f;

                  Enemy tempEnemy(*currentEnemyTexture, 0, 0);
                  tempEnemy.spawn(spawnX, spawnY, finalSpeed,
                                  *currentEnemyTexture, currentType,
                                  m_player.getPosition());
                  m_enemies.push_back(tempEnemy);

                } else {
                  //? Boxes (least common, орео/коробки)
                  currentType = Type::Box;
                  currentEnemyTexture = &m_enemyBoxTexture;

                  float finalSpeed = 350.0f;
                  static bool lastSpawnLeft = false;
                  bool spawnLeft = !lastSpawnLeft;
                  lastSpawnLeft = spawnLeft;
                  float spawnX = spawnLeft
                                     ? (m_machineLeftWall + 130.0f)
                                     : (currentW - m_machineRightWall - 105.0f);
                  float spawnY = -150.0f;

                  if (m_gameSettings.gameDifficulty == GameDifficulty::Easy)
                    m_enemySpawnTimer = 0.5f;
                  else if (m_gameSettings.gameDifficulty ==
                           GameDifficulty::Normal)
                    m_enemySpawnTimer = 0.35f;
                  else
                    m_enemySpawnTimer = 0.25f;

                  Enemy tempEnemy(*currentEnemyTexture, 0, 0);
                  tempEnemy.spawn(spawnX, spawnY, finalSpeed,
                                  *currentEnemyTexture, currentType,
                                  m_player.getPosition(),
                                  &m_enemyCookieTexture);
                  m_enemies.push_back(tempEnemy);
                }
              }
            } else if (m_currentEpisode == GameEpisode::Survival) {
              Type currentType;
              const sf::Texture *currentEnemyTexture = nullptr;
              float finalSpeed = 0.0f;
              float phase = m_currentEpisodeTime;
              
              bool spawnSlow = false;
              if (phase < 300.0f) {
                //? Phase 1 (0-5 min): ONLY Slow hands
                spawnSlow = true;
              } else if (phase < 600.0f) {
                //? Phase 2 (5-10 min): ONLY Fast hands
                spawnSlow = false;
              } else if (phase < 900.0f) {
                //? Phase 3 (10-15 min): 65% Slow, 35% Fast
                spawnSlow = (std::rand() % 100 < 65);
              } else {
                //? Phase 4 (15-20 min): 35% Slow, 65% Fast
                spawnSlow = (std::rand() % 100 < 35);
              }

              if (!spawnSlow) {
                //? Fast hands (HandStraight)
                currentType = Type::HandStraight;
                currentEnemyTexture = &m_enemyHand1Texture;
                finalSpeed = 800.0f;
              } else {
                //? Slow hands (HandChaser)
                currentType = Type::HandChaser;
                currentEnemyTexture = &m_enemyHand2Texture;
                if (m_gameSettings.gameDifficulty == GameDifficulty::Easy) {
                  finalSpeed = 100.0f;
                } else if (m_gameSettings.gameDifficulty == GameDifficulty::Normal) {
                  finalSpeed = 150.0f;
                } else if (m_gameSettings.gameDifficulty == GameDifficulty::Hard) {
                  finalSpeed = 200.0f;
                }
              }

              //? Base spawn timers
              if (m_gameSettings.gameDifficulty == GameDifficulty::Easy) {
                m_enemySpawnTimer = 1.0f;
              } else if (m_gameSettings.gameDifficulty == GameDifficulty::Normal) {
                m_enemySpawnTimer = 0.75f;
              } else if (m_gameSettings.gameDifficulty == GameDifficulty::Hard) {
                m_enemySpawnTimer = 0.5f;
              }

              //? Increase spawn frequency during Phase 1 by making timer shorter
              if (phase < 300.0f) {
                m_enemySpawnTimer *= 0.75f; //? 25% faster spawns
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
                Enemy tempEnemy(*currentEnemyTexture, 0, 0);
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
                it->getPosition().y < -400.0f ||
                it->getPosition().x > currentW + 400.0f ||
                it->getPosition().x < -400.0f) {
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

        float totalTime = 5.0f;
        float timePassed = totalTime - m_transitionTimer;

        int alpha = 255;

        if (timePassed < 0.5f)
          alpha = static_cast<int>((timePassed / 0.5f) * 255);

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

        if (m_skipTextAnimating) {
          m_skipTextAnimTimer += dt.asSeconds();
        }

        if (m_transitionTimer <= 0.0f) {
          m_transitionTimer = 0.0f;

          if (!m_cutsceneSkipped && !m_waitingForCutsceneReturn &&
              m_cutsceneSkipHoldTimer == 0.0f) {
            setupEpisode(m_nextEpisode);
            m_waitingForCutsceneReturn = true;
            m_cutsceneCooldown = 2.0f;
            if (m_nextEpisode == GameEpisode::VendingMachine)
              playCutscene(1);
            else if (m_nextEpisode == GameEpisode::Survival)
              playCutscene(2);
            else if (m_nextEpisode == GameEpisode::BossFight)
              playCutscene(3);
          }

          if (m_cutsceneSkipped && !m_isFadingOut) {
            m_isFadingOut = true;
            m_fadeAlpha = 0.0f;
          }
        }
      }
      break;
    }

    case GameState::StoryScreen: {
      if (!m_typewriterDone) {
        //? Start typewriter sound if not playing
        if (m_typewriterSound.getStatus() != sf::Sound::Status::Playing)
          m_typewriterSound.play();
        m_typewriterTimer += dt.asSeconds();
        while (m_typewriterTimer >= m_typewriterSpeed &&
               m_typewriterIndex < m_storyTextFull.getSize()) {
          m_typewriterTimer -= m_typewriterSpeed;
          m_storyTextVisible += m_storyTextFull[m_typewriterIndex];
          m_typewriterIndex++;
        }
        if (m_typewriterIndex >= m_storyTextFull.getSize()) {
          m_typewriterDone = true;
          m_typewriterSound.stop();
        }
      }
      break;
    }

    case GameState::Disclaimer: {
      break;
    }
    }

    //! DRAW LOGIC
    m_gameWindow.clear(sf::Color::Black);

    if (m_currentGameState == GameState::Controls) {
      m_menu.draw(m_gameWindow, m_currentGameState);
    } else if (m_currentGameState == GameState::Disclaimer) {
      drawDisclaimerScreen(currentW, currentH);
    } else if (m_isStoryActive) {
      drawStoryScreen(currentW, currentH);
      if (m_currentGameState != GameState::StoryScreen) {
        sf::RectangleShape dimRect((sf::Vector2f)m_gameWindow.getSize());
        dimRect.setFillColor(sf::Color(0, 0, 0, 150));
        m_gameWindow.draw(dimRect);

        sf::View oldView = m_gameWindow.getView();
        m_menu.setGlobalAlpha(255);
        m_menu.draw(m_gameWindow, m_currentGameState);
        m_gameWindow.setView(oldView);
      }
    } else if (m_currentGameState == GameState::EpisodeTransition) {
      if (m_nextEpisode == GameEpisode::Victory) {
        m_gameWindow.draw(m_bgSprite);
        m_gameWindow.draw(m_loadingTextBG);
      }

      m_gameWindow.draw(m_loadingText);

      if (m_nextEpisode != GameEpisode::Victory) {
        if (m_waitingForCutsceneReturn) {
          bool ua = m_gameSettings.ukrainianLanguage;
          sf::Text waitText(ua ? m_loadingFontUA : m_loadingFont);
          waitText.setCharacterSize(24);
          waitText.setFillColor(sf::Color::White);
          waitText.setOutlineColor(sf::Color::Black);
          waitText.setOutlineThickness(2.0f);

          int cutsceneNum = 1;
          if (m_nextEpisode == GameEpisode::Survival)
            cutsceneNum = 2;
          else if (m_nextEpisode == GameEpisode::BossFight)
            cutsceneNum = 3;

          std::string strWait =
              ua ? "Катсцена відкривається у зовнішньому плеєрі...\n(Якщо "
                   "відео не запустилось, ви можете переглянути його в папці "
                   "assets/videos/Cutscene_" +
                       std::to_string(cutsceneNum) + ".mp4)"
                 : "Cutscene is opening in an external player...\n(If video "
                   "did not start, you can view it in assets/videos/Cutscene_" +
                       std::to_string(cutsceneNum) + ".mp4)";

          std::string strConfirm =
              ua ? "\n\n[ Натисніть Enter, щоб продовжити ]"
                 : "\n\n[ Press Enter to continue ]";
          if (m_inputMode == InputMode::Gamepad) {
            strConfirm =
                ua ? "\n\n[ Натисніть Кнопку A (PlayStation: X), щоб "
                     "продовжити ]"
                   : "\n\n[ Press Button A (PlayStation: X) to continue ]";
          }
          strWait += strConfirm;

          waitText.setString(
              sf::String::fromUtf8(strWait.begin(), strWait.end()));
          sf::FloatRect bounds = waitText.getLocalBounds();
          waitText.setOrigin({bounds.position.x + bounds.size.x / 2.0f,
                              bounds.position.y + bounds.size.y / 2.0f});
          waitText.setPosition({currentW / 2.0f, currentH * 0.85f});
          m_gameWindow.draw(waitText);
        } else {
          bool ua = m_gameSettings.ukrainianLanguage;
          sf::Text skipText(m_gameSettings.ukrainianLanguage ? m_loadingFontUA
                                                             : m_loadingFont);
          skipText.setCharacterSize(22);
          skipText.setFillColor(sf::Color(200, 200, 200, 180));

          std::string str;
          if (m_inputMode == InputMode::Gamepad) {
            str = ua ? "[Тримайте Кнопку A (PlayStation: X), щоб пропустити "
                       "катсцену]"
                     : "[Hold Button A (PlayStation: X) to skip cutscene]";
          } else {
            str = ua ? "[Тримайте Enter, щоб пропустити катсцену]"
                     : "[Hold Enter to skip cutscene]";
          }

          if (m_cutsceneSkipped ||
              (m_skipTextAnimating && m_skipTextAnimTimer >= 2.0f)) {
            str =
                ua ? "[Катсцену буде пропущено]" : "[Cutscene will be skipped]";
            skipText.setFillColor(sf::Color(255, 200, 100, 230));
          }

          skipText.setString(sf::String::fromUtf8(str.begin(), str.end()));

          sf::FloatRect skipBounds = skipText.getLocalBounds();
          skipText.setOrigin({skipBounds.position.x + skipBounds.size.x,
                              skipBounds.position.y + skipBounds.size.y});

          skipText.setPosition({currentW - 30.f, currentH - 45.f});
          m_gameWindow.draw(skipText);

          if (m_skipTextAnimating && !m_cutsceneSkipped &&
              m_skipTextAnimTimer < 2.0f) {
            float progress = std::min(m_cutsceneSkipHoldTimer / 1.0f, 1.0f);
            float barWidth = skipBounds.size.x;
            float barHeight = 6.f;

            sf::RectangleShape barBg(sf::Vector2f(barWidth, barHeight));
            barBg.setOrigin({barWidth, 0});
            barBg.setPosition({currentW - 30.f, currentH - 35.f});
            barBg.setFillColor(sf::Color::Transparent);
            barBg.setOutlineThickness(1.f);
            barBg.setOutlineColor(sf::Color(200, 200, 200, 180));

            sf::RectangleShape barFill(
                sf::Vector2f(barWidth * progress, barHeight));
            barFill.setPosition({currentW - 30.f - barWidth, currentH - 35.f});
            barFill.setFillColor(sf::Color(255, 220, 100, 230));

            m_gameWindow.draw(barBg);
            m_gameWindow.draw(barFill);
          }
        }
      }
    } else {
      m_gameWindow.draw(m_bgSprite);
      if (m_bgSprite2) {
        m_gameWindow.draw(*m_bgSprite2);
      }

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
    }

    if (!m_fpsFontIsLoaded)
      m_gameWindow.draw(m_fpsErrorRect);
    else if (m_gameSettings.showFps)
      m_gameWindow.draw(m_fpsText);

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
    m_episodeDuration = 1200.0f; //? 20 minutes (4 phases x 5 min)
    m_bgStopped = false;

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
    m_episodeDuration = 1200.0f; //? 20 minutes (4 phases x 5 min)
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
    m_episodeDuration = 240.0f;

    m_player.startNextEpisode(m_currentWindowSize.x / 2.0f,
                              m_currentWindowSize.y - 200.0f);

    m_boss.spawn(m_currentWindowSize.x / 2.0f, -500.0f, m_gameSettings);

    if (!m_bgTexture.loadFromFile("assets/images/episode_3.png"))
      std::cerr << "Failed to load episode 3 background!" << std::endl;
  } break;
  }
  m_bgSprite.setTexture(m_bgTexture, true);
  m_bgSprite.setPosition({0.0f, 0.0f});

  float m_bgScaleX = static_cast<float>(m_currentWindowSize.x) /
                     static_cast<float>(m_bgTexture.getSize().x);
  float m_bgScaleY = static_cast<float>(m_currentWindowSize.y) /
                     static_cast<float>(m_bgTexture.getSize().y);
  m_bgSprite.setScale({m_bgScaleX, m_bgScaleY});

  if (m_currentEpisode == GameEpisode::VendingMachine) {
    m_bgSprite2.emplace(m_bgTexture);
    m_bgSprite2->setScale({m_bgScaleX, m_bgScaleY});
    m_bgSprite2->setPosition(
        {0.0f, -static_cast<float>(m_currentWindowSize.y)});
  } else {
    m_bgSprite2.reset();
  }
}

void Game::startTransition(GameEpisode nextEpisode) {
  m_nextEpisode = nextEpisode;
  m_currentGameState = GameState::EpisodeTransition;
  m_transitionTimer = 5.0f;
  m_player.stopSound();
  m_boss.stopSound();
  m_menu.updateMusicVolume(false);
  m_transitionSound.setVolume(50.0f);

  m_cutsceneSkipped = false;
  m_cutsceneSkipHoldTimer = 0.f;
  m_skipTextAnimating = false;
  m_skipTextAnimTimer = 0.f;
  m_waitingForCutsceneReturn = false;
  m_cutsceneCooldown = 0.0f;

  if (m_blackScreenDelay <= 0.0f) {
  }

  bool ua = m_gameSettings.ukrainianLanguage;
  m_loadingText.setFont(m_menu.getFont());

  if (m_nextEpisode == GameEpisode::VendingMachine) {
    if (ua)
      m_loadingText.setString(
          U"\u0415\u043f\u0456\u0437\u043e\u0434 1 \u2013 "
          U"\u041d\u0430\u0440\u043e\u0434\u0436\u0435\u043d\u043d\u044f");
    else
      m_loadingText.setString(U"Episode 1 \u2013 Birth");
  } else if (m_nextEpisode == GameEpisode::Survival) {
    if (ua)
      m_loadingText.setString(U"\u0415\u043f\u0456\u0437\u043e\u0434 2 \u2013 "
                              U"\u0421\u043f\u0440\u0430\u0433\u0430");
    else
      m_loadingText.setString(U"Episode 2 \u2013 Thirst");
  } else if (m_nextEpisode == GameEpisode::BossFight) {
    if (ua)
      m_loadingText.setString(
          U"\u0415\u043f\u0456\u0437\u043e\u0434 3 \u2013 "
          U"\u041f\u0420\u0418\u0411\u0418\u0420\u0410\u041d\u041d\u042f!");
    else
      m_loadingText.setString(U"Episode 3 \u2013 CLEANING!");
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
    m_loadingText.setFont(m_menu.getFont());
    std::string str;
    if (m_inputMode == InputMode::Gamepad) {
      str = ua ? "Ви вижили!\nДалі буде...\n\n(Буде зіграно катсцену 4)\n[ "
                 "Натисніть Кнопку A (PS: X), щоб продовжити ]"
               : "You Survived!\nTo be continued...\n\n(Cutscene 4 will be "
                 "played)\n[ Press Button A (PS: X) to proceed ]";
    } else {
      str = ua ? "Ви вижили!\nДалі буде...\n\n(Буде зіграно катсцену 4)\n[ "
                 "Натисніть Enter, щоб продовжити ]"
               : "You Survived!\nTo be continued...\n\n(Cutscene 4 will be "
                 "played)\n[ Press Enter to proceed ]";
    }
    m_loadingText.setString(sf::String::fromUtf8(str.begin(), str.end()));

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
  m_goToStoryAfterFade = true;
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
        m_isFadingOut = true;
        m_fadeAlpha = 0.0f;
        m_goToMenuAfterFade = false;
        m_nextEpisodeAfterFade = GameEpisode::VendingMachine;
        m_fromGameOver = false;
      }
    } else if (m_currentGameState == GameState::GameOver) {
      if (!m_isFadingIn && !m_isFadingOut) {
        if (m_gameStartSound.getStatus() != sf::Sound::Status::Playing)
          m_gameStartSound.play();
        m_fromGameOver = true;
        startNewGame(false);
      }
    } else if (m_currentGameState == GameState::Paused) {
      m_currentGameState = m_lastGameState;
    }

    if (m_currentGameState == GameState::Playing) {
      if (m_gameSettings.playMusic) {
        m_Episode1Music.setVolume(30.0f);
        m_Episode2Music.setVolume(30.0f);
        m_Episode3Music.setVolume(45.0f);
      } else {
        m_Episode1Music.setVolume(0);
        m_Episode2Music.setVolume(0);
        m_Episode3Music.setVolume(0);
      }

      if (m_isPlayerDying) {
        if (m_deathFizzSound.getStatus() == sf::Sound::Status::Paused)
          m_deathFizzSound.play();
        if (m_deathSound.getStatus() == sf::Sound::Status::Paused)
          m_deathSound.play();
        if (m_cookieSound.getStatus() == sf::Sound::Status::Paused)
          m_cookieSound.play();
      }
    }
    m_menu.setupMenuButtons(m_currentGameState, m_currentWindowSize.x,
                            m_currentWindowSize.y, m_gameSettings,
                            m_lastGameState);
  } else if (actionId == 2) {
    m_stateBeforeSettings = m_currentGameState;
    m_currentGameState = GameState::Settings;
    m_menu.resetFocus(GameState::Settings);
    m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                            m_gameSettings, m_stateBeforeSettings);
  } else if (actionId == 3) {
    if (m_gameSettings.gameDifficulty == GameDifficulty::Easy)
      m_gameSettings.gameDifficulty = GameDifficulty::Normal;
    else if (m_gameSettings.gameDifficulty == GameDifficulty::Normal)
      m_gameSettings.gameDifficulty = GameDifficulty::Hard;
    else
      m_gameSettings.gameDifficulty = GameDifficulty::Easy;
    m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                            m_gameSettings, m_stateBeforeSettings);
  } else if (actionId == 4) {
    m_gameSettings.playMusic = !m_gameSettings.playMusic;
    if (m_gameSettings.playMusic) {
      if (m_currentGameState == GameState::MainMenu ||
          (m_currentGameState == GameState::Settings &&
           m_stateBeforeSettings == GameState::MainMenu)) {
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
                            m_gameSettings, m_stateBeforeSettings);
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
                            m_gameSettings, m_stateBeforeSettings);
  } else if (actionId == 6) {
    m_gameSettings.showFps = !m_gameSettings.showFps;
    m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                            m_gameSettings, m_stateBeforeSettings);
  } else if (actionId == 7) {
    m_gameSettings.showHitbox = !m_gameSettings.showHitbox;
    m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                            m_gameSettings, m_stateBeforeSettings);
  } else if (actionId == 8) {
    if (m_currentGameState == GameState::Controls) {
      m_currentGameState = GameState::Settings;
      m_menu.resetFocus(GameState::Settings);
    } else {
      m_currentGameState = m_stateBeforeSettings;
      m_menu.resetFocus(m_currentGameState);
      m_menu.setFocusedButtonIndex(1);
    }
    m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                            m_gameSettings, m_stateBeforeSettings);
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
                            m_gameSettings, m_stateBeforeSettings);
  } else if (actionId == 11) {
    m_currentGameState = GameState::Controls;
    m_menu.resetFocus(GameState::Controls);
    m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                            m_gameSettings, m_stateBeforeSettings);
  } else if (actionId == 12) {
    m_gameSettings.playSfx = !m_gameSettings.playSfx;
    updateSfxVolume(m_gameSettings.playSfx);
    m_player.updateSfxVolume(m_gameSettings.playSfx);
    m_boss.updateSfxVolume(m_gameSettings.playSfx);
    m_menu.setupMenuButtons(m_currentGameState, currentW, currentH,
                            m_gameSettings, m_stateBeforeSettings);
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

//! DISCLAIMER SCREEN
void Game::drawDisclaimerScreen(float currentW, float currentH) {
  bool ua = m_gameSettings.ukrainianLanguage;
  const sf::Font &font = ua ? m_menu.getFontUA() : m_menu.getFont();

  std::string warningStr;
  std::string disclaimerStr;
  std::string dodgeStr;

  if (ua) {
    warningStr = "!!! УВАГА: ФОТОЧУТЛИВІСТЬ !!!\n"
                 "Ця гра містить спалахи світла та яскраві кольори,\n"
                 "які можуть вплинути на людей з епілепсією.\n"
                 "Якщо ви схильні до нападів - проконсультуйтесь з лікарем.\n";

    disclaimerStr =
        "КАТСЦЕНИ:\n"
        "Через технічні обмеження рушія, сюжетні відео-катсцени відтворюються\n"
        "у вашому зовнішньому медіа-плеєрі перед кожним епізодом.\n"
        "Відео будуть відкриватись вашим плеєром за замовчуванням.\n"
        "Їх потрібно подивитись, щоб зрозуміти сюжет.\n\n"
        "МЕДІА-МАТЕРІАЛИ:\n"
        "Всі фони та відео були згенеровані за допомогою штучного "
        "інтелекту.\n\n"
        "КЕРУВАННЯ:\n"
        "В кожному епізоді буде РІЗНА механіка керування.";

    dodgeStr = "ГОЛОВНЕ, ЩО ВАМ ТРЕБА ЗНАТИ: УХИЛЯЙТЕСЬ!";
  } else {
    warningStr = "!!! WARNING: PHOTOSENSITIVITY !!!\n"
                 "This game contains flashing lights and bright colors\n"
                 "that may affect people with epilepsy or photosensitivity.\n"
                 "If you are prone to seizures, please consult a doctor.\n";

    disclaimerStr =
        "CUTSCENES:\n"
        "Due to engine limitations, story cutscene videos\n"
        "are played in your external media player before each episode.\n"
        "Videos will be opened by your default player.\n"
        "They are required to understand the plot.\n\n"
        "MEDIA CONTENT:\n"
        "All backgrounds and videos were generated using artificial "
        "intelligence.\n\n"
        "CONTROLS:\n"
        "Each episode will have a DIFFERENT gameplay mechanic.";

    dodgeStr = "THE MAIN THING TO KNOW: DODGE!";
  }

  sf::Text warningText(font);
  warningText.setCharacterSize(32);
  warningText.setFillColor(sf::Color(255, 230, 80));
  warningText.setString(
      sf::String::fromUtf8(warningStr.begin(), warningStr.end()));
  warningText.setLineSpacing(1.3f);
  sf::FloatRect wBounds = warningText.getLocalBounds();
  warningText.setOrigin(
      {wBounds.position.x + wBounds.size.x / 2.f, wBounds.position.y});
  warningText.setPosition({currentW / 2.f, 40.f});
  m_gameWindow.draw(warningText);

  sf::Text mainText(font);
  mainText.setCharacterSize(30);
  mainText.setFillColor(sf::Color::White);
  mainText.setString(
      sf::String::fromUtf8(disclaimerStr.begin(), disclaimerStr.end()));
  mainText.setLineSpacing(1.3f);
  sf::FloatRect bounds = mainText.getLocalBounds();
  mainText.setOrigin(
      {bounds.position.x + bounds.size.x / 2.f, bounds.position.y});
  mainText.setPosition({currentW / 2.f, 40.f + wBounds.size.y + 15.f});
  m_gameWindow.draw(mainText);

  std::string confirmStr;
  if (m_inputMode == InputMode::Gamepad) {
    confirmStr =
        ua ? "[ Щоб продовжити натисніть Кнопку A (PlayStation: X) ]\n"
             "[ Щоб повернутись в меню натисніть Кнопку B (PlayStation: O) ]"
           : "[ To continue press Button A (PlayStation: X) ]\n"
             "[ To return to menu press Button B (PlayStation: O) ]";
  } else {
    confirmStr = ua ? "[ Щоб продовжити натисніть Enter ]\n"
                      "[ Щоб повернутись в меню натисніть Escape ]"
                    : "[ To continue press Enter ]\n"
                      "[ To return to menu press Escape ]";
  }

  sf::Text confirmText(font);
  confirmText.setCharacterSize(28);
  confirmText.setLineSpacing(1.3f);

  float pulse =
      std::sin(m_gameClock.getElapsedTime().asSeconds() * 3.f) * 0.5f + 0.5f;
  uint8_t ca = static_cast<uint8_t>(160 + pulse * 95);
  confirmText.setFillColor(sf::Color(255, 220, 100, ca));
  confirmText.setString(
      sf::String::fromUtf8(confirmStr.begin(), confirmStr.end()));

  sf::FloatRect cBounds = confirmText.getLocalBounds();
  confirmText.setOrigin({cBounds.position.x + cBounds.size.x / 2.f,
                         cBounds.position.y + cBounds.size.y});
  confirmText.setPosition({currentW / 2.f, currentH - 30.f});

  sf::Text dodgeText(font);
  dodgeText.setCharacterSize(30);
  dodgeText.setFillColor(sf::Color::White);
  dodgeText.setString(sf::String::fromUtf8(dodgeStr.begin(), dodgeStr.end()));
  sf::FloatRect dBounds = dodgeText.getLocalBounds();
  dodgeText.setOrigin({dBounds.position.x + dBounds.size.x / 2.f,
                       dBounds.position.y + dBounds.size.y / 2.f});

  float mainTextBottom = mainText.getPosition().y + bounds.size.y;
  float confirmTextTop = confirmText.getPosition().y - cBounds.size.y;
  float middleY = mainTextBottom + (confirmTextTop - mainTextBottom) / 2.f;
  dodgeText.setPosition({currentW / 2.f, middleY});

  m_gameWindow.draw(dodgeText);
  m_gameWindow.draw(confirmText);
}

//! STORY SCREEN
void Game::drawStoryScreen(float currentW, float currentH) {
  bool ua = m_gameSettings.ukrainianLanguage;
  const sf::Font &font = ua ? m_loadingFontUA : m_loadingFont;

  sf::Text storyText(font);
  storyText.setCharacterSize(26);
  storyText.setFillColor(sf::Color::White);
  storyText.setString(m_storyTextVisible);
  storyText.setLineSpacing(1.4f);

  sf::Text fullText(font);
  fullText.setCharacterSize(26);
  fullText.setString(m_storyTextFull);
  fullText.setLineSpacing(1.4f);

  sf::FloatRect fullBounds = fullText.getLocalBounds();
  storyText.setOrigin(
      {fullBounds.position.x + fullBounds.size.x / 2.f, fullBounds.position.y});
  storyText.setPosition({currentW / 2.f, 80.f});
  m_gameWindow.draw(storyText);

  if (m_typewriterDone) {
    std::string continueStr;
    if (m_inputMode == InputMode::Gamepad) {
      continueStr =
          ua ? "[ Натисніть Кнопку A (PlayStation: X), щоб продовжити ]"
             : "[ Press Button A (PlayStation: X) to continue ]";
    } else {
      continueStr = ua ? "[ Натисніть Enter, щоб продовжити ]"
                       : "[ Press Enter to continue ]";
    }

    sf::Text continueText(font);
    continueText.setCharacterSize(26);
    float pulse =
        std::sin(m_gameClock.getElapsedTime().asSeconds() * 3.f) * 0.5f + 0.5f;
    uint8_t ca = static_cast<uint8_t>(160 + pulse * 95);
    continueText.setFillColor(sf::Color(255, 220, 100, ca));
    continueText.setString(
        sf::String::fromUtf8(continueStr.begin(), continueStr.end()));

    sf::FloatRect cBounds = continueText.getLocalBounds();
    continueText.setOrigin({cBounds.position.x + cBounds.size.x / 2.f,
                            cBounds.position.y + cBounds.size.y});
    continueText.setPosition({currentW / 2.f, currentH - 40.f});
    m_gameWindow.draw(continueText);
  }
}
