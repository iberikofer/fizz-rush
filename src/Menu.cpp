#include "Menu.hpp"
#include "Settings.hpp"
#include <iostream>

Menu::Menu(float winWidth, float winHeight)
    : m_menuBG({winWidth, winHeight}), m_menuBGSprite(m_menuBGTexture),
      m_gameNameLogo(m_menuFont), m_playButtonText(m_menuFont),
      m_settingsButtonText(m_menuFont), m_exitButtonText(m_menuFont),
      m_settingsTitle(m_menuFont), m_difficultyText(m_menuFont),
      m_musicText(m_menuFont), m_vsyncText(m_menuFont),
      m_FPSCounterText(m_menuFont), m_hitboxText(m_menuFont),
      m_backButtonText(m_menuFont), m_menuMusic(m_menuMusicBuffer) {
  // Початкове налаштування (загальне)
  m_playButtonText.setCharacterSize(60);
  m_playButtonText.setFillColor(sf::Color(255, 220, 0));
  m_playButtonText.setOutlineColor(sf::Color::Black);
  m_playButtonText.setOutlineThickness(4.0f);

  // Кнопки (зелені)
  m_playButton.setFillColor(sf::Color(0, 255, 0, 100));
  m_playButton.setOutlineColor(sf::Color::White);
  m_playButton.setOutlineThickness(5.0f);

  m_settingsButton.setFillColor(sf::Color(0, 255, 0, 100));
  m_settingsButton.setOutlineColor(sf::Color::White);
  m_settingsButton.setOutlineThickness(5.0f);

  m_exitButton.setFillColor(sf::Color(0, 255, 0, 100));
  m_exitButton.setOutlineColor(sf::Color::White);
  m_exitButton.setOutlineThickness(5.0f);

  m_menuMusic.setLooping(true);
  m_isDifficultyLocked = false;
  m_menuMusic.setVolume(40.0f);
}

void Menu::loadAssets(float winWidth, float winHeight) {
  if (!m_menuBGTexture.loadFromFile("assets/images/fizz_bg.png"))
    std::cerr << "Menu font error!" << std::endl;
  else
    m_menuBGSprite.setTexture(m_menuBGTexture, true);
  m_menuBGSprite.setColor(sf::Color(255, 255, 255, 150));
  sf::Vector2u textureSize = m_menuBGTexture.getSize();
  float scaleX = static_cast<float>(winWidth) / textureSize.x;
  float scaleY = static_cast<float>(winHeight) / textureSize.y;
  m_menuBGSprite.setScale({scaleX, scaleY});
  m_menuBGSprite.setPosition({0.0f, 0.0f});
  if (!m_menuFont.openFromFile("assets/fonts/Lilita_One.ttf"))
    std::cerr << "Menu font error!" << std::endl;
  if (!m_menuMusicBuffer.loadFromFile("assets/sound/menu_music.ogg"))
    std::cerr << "Menu music error!" << std::endl;
}

void Menu::updateMusicVolume(bool isPlaying) {
  if (isPlaying) {
    m_menuMusic.setVolume(40.0f);
    if (m_menuMusic.getStatus() != sf::Sound::Status::Playing) {
      m_menuMusic.play();
    }
  } else {
    m_menuMusic.setVolume(0);
  }
}

//* === BUTTONS SETUP LOGIC ===
void Menu::setupMenuButtons(GameState m_gameState, float winWidth,
                            float winHeight, GameSettings &settings,
                            GameState m_lastGameState) {
  if (m_gameState == GameState::Settings) {
    m_gameNameLogo.setString("SETTINGS:");
    m_gameNameLogo.setCharacterSize(150);
    m_gameNameLogo.setFillColor(sf::Color(255, 220, 0));
    m_gameNameLogo.setOutlineColor(sf::Color(150, 100, 0));
    m_gameNameLogo.setOutlineThickness(4.0f);

    sf::FloatRect logoRect = m_gameNameLogo.getLocalBounds();
    m_gameNameLogo.setOrigin({logoRect.position.x + logoRect.size.x / 2.0f,
                              logoRect.position.y + logoRect.size.y / 2.0f});
    m_gameNameLogo.setPosition({winWidth / 2.0f, winHeight / 2.0f - 350.0f});

    if (m_lastGameState == GameState::MainMenu)
      m_menuBG.setFillColor(sf::Color(0, 0, 120, 255));
    else {
      m_menuBG.setFillColor(sf::Color(0, 0, 50, 220));
      m_menuMusic.stop();
    }

    std::string diffString = "Difficulty: ";
    if (settings.gameDifficulty == GameDifficulty::Easy) {
      diffString += "Easy (^-^)";
      m_difficultyText.setFillColor(sf::Color(0, 168, 0));
    } else if (settings.gameDifficulty == GameDifficulty::Normal) {
      diffString += "Normal (._.)";
      m_difficultyText.setFillColor(sf::Color(255, 100, 0));
    } else {
      diffString += "Hard (0_0)";
      m_difficultyText.setFillColor(sf::Color(160, 0, 0));
    }

    m_difficultyText.setString(diffString);
    m_difficultyText.setCharacterSize(50);
    m_difficultyText.setOutlineColor(sf::Color::Black);
    m_difficultyText.setOutlineThickness(3.0f);

    sf::FloatRect diffRect = m_difficultyText.getLocalBounds();
    m_difficultyText.setOrigin(
        {diffRect.size.x / 2.0f, diffRect.size.y / 2.0f + 10.0f});
    m_difficultyText.setPosition({winWidth / 2.0f, winHeight / 2.0f - 160.5f});

    m_difficultyButton.setSize({510.0f, 70.0f});
    m_difficultyButton.setOrigin({m_difficultyButton.getSize().x / 2.0f,
                                  m_difficultyButton.getSize().y / 2.0f});
    m_difficultyButton.setPosition(m_difficultyText.getPosition());

    if (m_lastGameState == GameState::Paused) {
      m_difficultyButton.setFillColor(sf::Color(100, 100, 100, 150));
      m_isDifficultyLocked = true;
    } else {
      m_difficultyButton.setFillColor(sf::Color(0, 100, 255, 150));
      m_isDifficultyLocked = false;
    }
    m_difficultyButton.setOutlineColor(sf::Color::White);
    m_difficultyButton.setOutlineThickness(5.0f);

    std::string musicString = "Music: ";
    musicString += (settings.playMusic ? "ON" : "OFF");
    m_musicText.setString(musicString);
    m_musicText.setCharacterSize(50);
    m_musicText.setFillColor(settings.playMusic ? sf::Color::Green
                                                : sf::Color::Red);
    m_musicText.setOutlineColor(sf::Color::Black);
    m_musicText.setOutlineThickness(3.0f);

    sf::FloatRect musicRect = m_musicText.getLocalBounds();
    m_musicText.setOrigin(
        {musicRect.size.x / 2.0f, musicRect.size.y / 2.0f + 10.0f});
    m_musicText.setPosition({winWidth / 2.0f, winHeight / 2.0f - 60.5f});

    m_musicButton.setSize({510.0f, 70.0f});
    m_musicButton.setOrigin(
        {m_musicButton.getSize().x / 2.0f, m_musicButton.getSize().y / 2.0f});
    m_musicButton.setPosition(m_musicText.getPosition());
    m_musicButton.setFillColor(sf::Color(0, 100, 255, 150));
    m_musicButton.setOutlineColor(sf::Color::White);
    m_musicButton.setOutlineThickness(5.0f);

    std::string vsyncString = "VSync: ";
    vsyncString += (settings.VSync ? "ON" : "OFF");
    m_vsyncText.setString(vsyncString);
    m_vsyncText.setCharacterSize(50);
    m_vsyncText.setFillColor(settings.VSync ? sf::Color::Green
                                            : sf::Color::Red);
    m_vsyncText.setOutlineColor(sf::Color::Black);
    m_vsyncText.setOutlineThickness(3.0f);

    sf::FloatRect vsyncRect = m_vsyncText.getLocalBounds();
    m_vsyncText.setOrigin(
        {vsyncRect.size.x / 2.0f, vsyncRect.size.y / 2.0f + 10.0f});
    m_vsyncText.setPosition({winWidth / 2.0f, winHeight / 2.0f + 39.5f});

    m_vsyncButton.setSize({510.0f, 70.0f});
    m_vsyncButton.setOrigin(
        {m_vsyncButton.getSize().x / 2.0f, m_vsyncButton.getSize().y / 2.0f});
    m_vsyncButton.setPosition(m_vsyncText.getPosition());
    m_vsyncButton.setFillColor(sf::Color(0, 100, 255, 150));
    m_vsyncButton.setOutlineColor(sf::Color::White);
    m_vsyncButton.setOutlineThickness(5.0f);

    std::string fpsString = "SHOW FPS: ";
    fpsString += (settings.showFps ? "ON" : "OFF");
    m_FPSCounterText.setString(fpsString);
    m_FPSCounterText.setCharacterSize(50);
    m_FPSCounterText.setFillColor(settings.showFps ? sf::Color::Green
                                                   : sf::Color::Red);
    m_FPSCounterText.setOutlineColor(sf::Color::Black);
    m_FPSCounterText.setOutlineThickness(3.0f);

    sf::FloatRect FPSRect = m_FPSCounterText.getLocalBounds();
    m_FPSCounterText.setOrigin(
        {FPSRect.size.x / 2.0f, FPSRect.size.y / 2.0f + 10.0f});
    m_FPSCounterText.setPosition({winWidth / 2.0f, winHeight / 2.0f + 139.5f});

    m_FPSCounterButton.setSize({510.0f, 70.0f});
    m_FPSCounterButton.setOrigin({m_FPSCounterButton.getSize().x / 2.0f,
                                  m_FPSCounterButton.getSize().y / 2.0f});
    m_FPSCounterButton.setPosition(m_FPSCounterText.getPosition());
    m_FPSCounterButton.setFillColor(sf::Color(0, 100, 255, 150));
    m_FPSCounterButton.setOutlineColor(sf::Color::White);
    m_FPSCounterButton.setOutlineThickness(5.0f);

    std::string debugString = "Show Hitboxes: ";
    debugString += (settings.showHitbox ? "ON" : "OFF");
    m_hitboxText.setString(debugString);
    m_hitboxText.setCharacterSize(50);
    m_hitboxText.setFillColor(settings.showHitbox ? sf::Color::Green
                                                  : sf::Color::Red);
    m_hitboxText.setOutlineColor(sf::Color::Black);
    m_hitboxText.setOutlineThickness(3.0f);

    sf::FloatRect hitboxRect = m_hitboxText.getLocalBounds();
    m_hitboxText.setOrigin(
        {hitboxRect.size.x / 2.0f, hitboxRect.size.y / 2.0f + 10.0f});
    m_hitboxText.setPosition({winWidth / 2.0f, winHeight / 2.0f + 239.5f});

    m_hitboxButton.setSize({510.0f, 70.0f});
    m_hitboxButton.setOrigin(
        {m_hitboxButton.getSize().x / 2.0f, m_hitboxButton.getSize().y / 2.0f});
    m_hitboxButton.setPosition(m_hitboxText.getPosition());
    m_hitboxButton.setFillColor(sf::Color(0, 100, 255, 150));
    m_hitboxButton.setOutlineColor(sf::Color::White);
    m_hitboxButton.setOutlineThickness(5.0f);

    m_backButtonText.setString("Back");
    m_backButtonText.setCharacterSize(50);
    m_backButtonText.setFillColor(sf::Color::Yellow);
    m_backButtonText.setOutlineColor(sf::Color::Black);
    m_backButtonText.setOutlineThickness(3.0f);

    sf::FloatRect backRect = m_backButtonText.getLocalBounds();
    m_backButtonText.setOrigin(
        {backRect.size.x / 2.0f, backRect.size.y / 2.0f + 10.0f});
    m_backButtonText.setPosition({winWidth / 2.0f, winHeight / 2.0f + 420.0f});

    m_backButton.setSize({backRect.size.x + 40.0f, backRect.size.y + 20.0f});
    m_backButton.setOrigin(
        {m_backButton.getSize().x / 2.0f, m_backButton.getSize().y / 2.0f});
    m_backButton.setPosition(m_backButtonText.getPosition());
    m_backButton.setFillColor(sf::Color(255, 50, 50, 150));
    m_backButton.setOutlineColor(sf::Color::White);
    m_backButton.setOutlineThickness(5.0f);
  } else if (m_gameState == GameState::GameOver) {
    m_menuBG.setFillColor(sf::Color(0, 0, 0, 255));

    m_gameNameLogo.setString("GAME OVER :(");
    m_gameNameLogo.setCharacterSize(150);
    m_gameNameLogo.setFillColor(sf::Color(255, 0, 0));
    m_gameNameLogo.setOutlineColor(sf::Color::Yellow);
    m_gameNameLogo.setOutlineThickness(4.0f);

    sf::FloatRect logoRect = m_gameNameLogo.getLocalBounds();
    m_gameNameLogo.setOrigin({logoRect.position.x + logoRect.size.x / 2.0f,
                              logoRect.position.y + logoRect.size.y / 2.0f});
    m_gameNameLogo.setPosition({winWidth / 2.0f, winHeight / 2.0f - 200.0f});

    m_playButtonText.setString("Start new game");
    m_playButtonText.setFillColor(sf::Color(255, 220, 0));
    sf::FloatRect m_playRect = m_playButtonText.getLocalBounds();
    m_playButtonText.setOrigin(
        {m_playRect.position.x + m_playRect.size.x / 2.0f,
         m_playRect.position.y + m_playRect.size.y / 2.0f});

    m_playButtonText.setPosition(
        {winWidth / 2.0f - 190.0f, winHeight / 2.0f + 100.0f});

    m_playButton.setSize(
        {m_playRect.size.x + 30.0f, m_playRect.size.y + 20.0f});
    m_playButton.setOrigin(
        {m_playButton.getSize().x / 2.0f, m_playButton.getSize().y / 2.0f});
    m_playButton.setPosition(m_playButtonText.getPosition());

    m_exitButtonText.setString("Exit to menu");
    m_exitButtonText.setFillColor(sf::Color(255, 220, 0));
    sf::FloatRect m_exitRect = m_exitButtonText.getLocalBounds();
    m_exitButtonText.setOrigin(
        {m_exitRect.position.x + m_exitRect.size.x / 2.0f,
         m_exitRect.position.y + m_exitRect.size.y / 2.0f});

    m_exitButtonText.setPosition(
        {winWidth / 2.0f + 230.0f, winHeight / 2.0f + 100.0f});

    m_exitButton.setSize(
        {m_exitRect.size.x + 30.0f, m_exitRect.size.y + 31.0f});
    m_exitButton.setOrigin(
        {m_exitButton.getSize().x / 2.0f, m_exitButton.getSize().y / 2.0f});
    m_exitButton.setPosition(m_exitButtonText.getPosition());
  } else {
    m_gameNameLogo.setString("FIZZ RUSH!");
    m_gameNameLogo.setCharacterSize(200);
    m_gameNameLogo.setFillColor(sf::Color(255, 220, 0));
    m_gameNameLogo.setOutlineColor(sf::Color(150, 100, 0));
    m_gameNameLogo.setOutlineThickness(4.0f);

    sf::FloatRect gameNameText = m_gameNameLogo.getLocalBounds();
    m_gameNameLogo.setOrigin(
        {gameNameText.position.x + gameNameText.size.x / 2.0f,
         gameNameText.position.y + gameNameText.size.y / 2});
    m_gameNameLogo.setPosition({winWidth / 2.0f, winHeight / 2.0f - 350.0f});

    if (m_gameState == GameState::MainMenu) {
      if (m_menuMusic.getStatus() != sf::Sound::Status::Playing &&
          settings.playMusic) {
        m_menuMusic.play();
      }
      m_playButtonText.setString("Start the game");
      m_menuBG.setFillColor(sf::Color(0, 0, 120, 255));
      m_exitButtonText.setString("Exit the game");
    } else if (m_gameState == GameState::Playing) {
      m_menuMusic.stop();
    } else if (m_gameState == GameState::Paused) {
      m_playButtonText.setString("Resume");
      m_menuBG.setFillColor(sf::Color(0, 0, 120, 180));
      m_exitButtonText.setString("Exit to menu");
    }

    m_playButtonText.setFillColor(sf::Color(255, 220, 0));

    sf::FloatRect m_playButtonPos = m_playButtonText.getLocalBounds();
    m_playButtonText.setOrigin(
        {m_playButtonPos.position.x + m_playButtonPos.size.x / 2.0f,
         m_playButtonPos.position.y + m_playButtonPos.size.y / 2.0f});

    m_playButtonText.setPosition({winWidth / 2.0f, winHeight / 2.0f - 100.0f});

    m_playButton.setSize(
        {m_playButtonPos.size.x + 20.0f, m_playButtonPos.size.y + 20.0f});
    m_playButton.setOrigin(
        {m_playButton.getSize().x / 2.0f, m_playButton.getSize().y / 2.0f});
    m_playButton.setPosition(m_playButtonText.getPosition());

    m_settingsButtonText.setString("Settings");
    m_settingsButtonText.setCharacterSize(60);
    m_settingsButtonText.setFillColor(sf::Color(255, 220, 0));
    m_settingsButtonText.setOutlineColor(sf::Color::Black);
    m_settingsButtonText.setOutlineThickness(4.0f);

    sf::FloatRect m_menuSettingsPos = m_settingsButtonText.getLocalBounds();
    m_settingsButtonText.setOrigin(
        {m_menuSettingsPos.position.x + m_menuSettingsPos.size.x / 2.0f,
         m_menuSettingsPos.position.y + m_menuSettingsPos.size.y / 2.0f});
    m_settingsButtonText.setPosition(
        {winWidth / 2.0f, winHeight / 2.0f + 75.0f});

    m_settingsButton.setSize(
        {m_menuSettingsPos.size.x + 20.0f, m_menuSettingsPos.size.y + 20.0f});
    m_settingsButton.setOrigin({m_settingsButton.getSize().x / 2.0f,
                                m_settingsButton.getSize().y / 2.0f});
    m_settingsButton.setPosition(m_settingsButtonText.getPosition());

    m_exitButtonText.setCharacterSize(60);
    m_exitButtonText.setFillColor(sf::Color(255, 220, 0));
    m_exitButtonText.setOutlineColor(sf::Color::Black);
    m_exitButtonText.setOutlineThickness(4.0f);

    sf::FloatRect m_menuExitText = m_exitButtonText.getLocalBounds();
    m_exitButtonText.setOrigin(
        {m_menuExitText.position.x + m_menuExitText.size.x / 2.0f,
         m_menuExitText.position.y + m_menuExitText.size.y / 2.0f});
    m_exitButtonText.setPosition({winWidth / 2.0f, winHeight / 2.0f + 250.0f});

    m_exitButton.setSize(
        {m_menuExitText.size.x + 20.0f, m_menuExitText.size.y + 20.0f});
    m_exitButton.setOrigin(
        {m_exitButton.getSize().x / 2.0f, m_exitButton.getSize().y / 2.0f});
    m_exitButton.setPosition(m_exitButtonText.getPosition());
  }
}

//* === EVENT LOGIC ===
int Menu::mouseClickPos(int mouseX, int mouseY, GameState m_gameState) {
  if (m_gameState == GameState::MainMenu || m_gameState == GameState::Paused) {
    if (m_playButton.getGlobalBounds().contains(
            {static_cast<float>(mouseX), static_cast<float>(mouseY)}))
      return 1;
    else if (m_settingsButton.getGlobalBounds().contains(
                 {static_cast<float>(mouseX), static_cast<float>(mouseY)}))
      return 2;
    else if (m_exitButton.getGlobalBounds().contains(
                 {static_cast<float>(mouseX), static_cast<float>(mouseY)}))
      return 9;
    else
      return 0;
  } else if (m_gameState == GameState::Settings) {
    if (m_difficultyButton.getGlobalBounds().contains(
            {static_cast<float>(mouseX), static_cast<float>(mouseY)})) {
      if (m_isDifficultyLocked)
        return 0;
      else
        return 3;
    } else if (m_musicButton.getGlobalBounds().contains(
                   {static_cast<float>(mouseX), static_cast<float>(mouseY)}))
      return 4;
    else if (m_vsyncButton.getGlobalBounds().contains(
                 {static_cast<float>(mouseX), static_cast<float>(mouseY)}))
      return 5;
    else if (m_FPSCounterButton.getGlobalBounds().contains(
                 {static_cast<float>(mouseX), static_cast<float>(mouseY)}))
      return 6;
    else if (m_hitboxButton.getGlobalBounds().contains(
                 {static_cast<float>(mouseX), static_cast<float>(mouseY)}))
      return 7;
    else if (m_backButton.getGlobalBounds().contains(
                 {static_cast<float>(mouseX), static_cast<float>(mouseY)}))
      return 8;
    else
      return 0;
  } else if (m_gameState == GameState::GameOver) {
    if (m_playButton.getGlobalBounds().contains(
            {static_cast<float>(mouseX), static_cast<float>(mouseY)}))
      return 1;
    else if (m_exitButton.getGlobalBounds().contains(
                 {static_cast<float>(mouseX), static_cast<float>(mouseY)}))
      return 9;
  }

  return 0;
}

//* === DRAW LOGIC ===
void Menu::draw(sf::RenderWindow &window, GameState m_gameState) {
  window.draw(m_menuBG);
  if (m_gameState == GameState::MainMenu || m_gameState == GameState::Paused ||
      m_gameState == GameState::Settings) {
    window.draw(m_menuBGSprite);
  }

  window.draw(m_gameNameLogo);

  if (m_gameState == GameState::Settings) {
    window.draw(m_difficultyButton);
    window.draw(m_difficultyText);

    window.draw(m_musicButton);
    window.draw(m_musicText);

    window.draw(m_vsyncButton);
    window.draw(m_vsyncText);

    window.draw(m_FPSCounterButton);
    window.draw(m_FPSCounterText);

    window.draw(m_hitboxButton);
    window.draw(m_hitboxText);

    window.draw(m_backButton);
    window.draw(m_backButtonText);
  } else if (m_gameState == GameState::GameOver) {
    window.draw(m_playButton);
    window.draw(m_playButtonText);

    window.draw(m_exitButton);
    window.draw(m_exitButtonText);
  } else {
    window.draw(m_playButton);
    window.draw(m_playButtonText);

    window.draw(m_settingsButton);
    window.draw(m_settingsButtonText);

    window.draw(m_exitButton);
    window.draw(m_exitButtonText);
  }
}