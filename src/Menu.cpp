#include "Menu.hpp"
#include "Settings.hpp"
#include <cmath>
#include <iostream>

//* Constructor
Menu::Menu(float winWidth, float winHeight)
    : m_menuBG({winWidth, winHeight}), m_menuBGSprite(m_menuBGTexture),
      m_gameNameLogo(m_menuFont), m_playButtonText(m_menuFont),
      m_settingsButtonText(m_menuFont), m_exitButtonText(m_menuFont),
      m_settingsTitle(m_menuFont), m_difficultyText(m_menuFont),
      m_difficultyValueText(m_menuFont), m_musicText(m_menuFont),
      m_musicValueText(m_menuFont), m_vsyncText(m_menuFont),
      m_vsyncValueText(m_menuFont), m_FPSCounterText(m_menuFont),
      m_FPSCounterValueText(m_menuFont), m_hitboxText(m_menuFont),
      m_hitboxValueText(m_menuFont), m_languageText(m_menuFont),
      m_languageValueText(m_menuFont), m_controlsButtonText(m_menuFont),
      m_backButtonText(m_menuFont), m_controlsTitle(m_menuFont),
      m_controlsContent(m_menuFont), m_controlsBackText(m_menuFont),
      m_sfxText(m_menuFont), m_sfxValueText(m_menuFont),
      m_menuMusic(m_menuMusicBuffer) {
  m_playButtonText.setCharacterSize(60);
  m_playButtonText.setFillColor(sf::Color(255, 220, 0));
  m_playButtonText.setOutlineColor(sf::Color::Black);
  m_playButtonText.setOutlineThickness(4.0f);

  auto initBtn = [](sf::RectangleShape &btn,
                    sf::Color fill = sf::Color(0, 255, 0, 100)) {
    btn.setFillColor(fill);
    btn.setOutlineColor(sf::Color::White);
    btn.setOutlineThickness(5.0f);
  };
  initBtn(m_playButton);
  initBtn(m_settingsButton);
  initBtn(m_exitButton);

  m_menuMusic.setLooping(true);
  m_isDifficultyLocked = false;
  m_menuMusic.setVolume(AudioConfig::MENU_MUSIC);
}

//* Assets
void Menu::loadAssets(float winWidth, float winHeight) {
  if (!m_menuBGTexture.loadFromFile("assets/images/fizz_bg.png"))
    std::cerr << "Menu BG error!" << std::endl;
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
  if (!m_menuFontUA.openFromFile("assets/fonts/Russo_One.ttf"))
    std::cerr << "Ukrainian font error!" << std::endl;
  if (!m_menuMusicBuffer.loadFromFile("assets/sound/menu_music.ogg"))
    std::cerr << "Menu music error!" << std::endl;
}

//* Music helpers
void Menu::updateMusicVolume(bool isPlaying) {
  if (isPlaying) {
    m_menuMusic.setVolume(AudioConfig::MENU_MUSIC);
    if (m_menuMusic.getStatus() != sf::Sound::Status::Playing)
      m_menuMusic.play();
  } else {
    m_menuMusic.setVolume(0);
  }
}

void Menu::fadeOutMusic(float dtSeconds) {
  if (m_menuMusic.getStatus() == sf::Sound::Status::Playing) {
    float v = m_menuMusic.getVolume();
    if (v > 0.0f) {
      v -= dtSeconds * 80.0f;
      if (v < 0.0f)
        v = 0.0f;
      m_menuMusic.setVolume(v);
      if (v == 0.0f)
        m_menuMusic.stop();
    }
  }
}

//* Input mode
void Menu::setInputMode(InputMode mode) { m_inputMode = mode; }
InputMode Menu::getInputMode() const { return m_inputMode; }

void Menu::updatePulse(float dtSeconds) {
  m_pulseTimer += dtSeconds;
  m_lastDt = dtSeconds;
}

//? How many focusable buttons each screen has (ordered by visual position)
int Menu::buttonCount(GameState state) const {
  if (state == GameState::MainMenu || state == GameState::Paused)
    return 3;
  if (state == GameState::GameOver)
    return 2;
  //? Settings: Language(0), Difficulty(1), VSync(2), Music(3), ShowFPS(4),
  // Hitbox(5), Dummy(6->7), Back(7->8)
  if (state == GameState::Settings)
    return 9;
  if (state == GameState::Controls)
    return 1;
  return 0;
}

void Menu::resetFocus(GameState state) {
  m_focusedButtonIndex = 0;
  m_hoveredButtonIndex = -1;
}

void Menu::syncFocusFromHover(GameState state) {
  if (m_hoveredButtonIndex != -1) {
    m_focusedButtonIndex = m_hoveredButtonIndex;
  } else {
    m_focusedButtonIndex = 0;
  }
}

bool Menu::moveFocus2D(int dx, int dy, GameState state) {
  int oldIndex = m_focusedButtonIndex;
  int count = buttonCount(state);
  if (count == 0)
    return false;

  if (state == GameState::Settings) {
    if (dx != 0) {
      if (m_focusedButtonIndex < 4) {
        m_focusedButtonIndex += 4;
      } else if (m_focusedButtonIndex >= 4 && m_focusedButtonIndex < 8) {
        m_focusedButtonIndex -= 4;
      }
    }
    if (dy != 0) {
      if (dy < 0) { // UP
        if (m_focusedButtonIndex == 0 || m_focusedButtonIndex == 4) {
          m_lastSettingsColumn = (m_focusedButtonIndex == 0) ? 0 : 1;
          m_focusedButtonIndex = 8;
        } else if (m_focusedButtonIndex == 8) {
          m_focusedButtonIndex = (m_lastSettingsColumn == 0) ? 3 : 7;
        } else {
          m_focusedButtonIndex--;
        }
      } else { // DOWN
        if (m_focusedButtonIndex == 3 || m_focusedButtonIndex == 7) {
          m_lastSettingsColumn = (m_focusedButtonIndex == 3) ? 0 : 1;
          m_focusedButtonIndex = 8;
        } else if (m_focusedButtonIndex == 8) {
          m_focusedButtonIndex = (m_lastSettingsColumn == 0) ? 0 : 4;
        } else {
          m_focusedButtonIndex++;
        }
      }
    }
  } else {
    int dir = dy != 0 ? dy : dx;
    m_focusedButtonIndex = (m_focusedButtonIndex + dir + count) % count;
  }
  return m_focusedButtonIndex != oldIndex;
}

//? Maps focused index → button action ID
//? Action IDs mirror mouseClickPos return values
int Menu::getFocusedButtonClickType(GameState state) {
  if (state == GameState::MainMenu || state == GameState::Paused) {
    //* 0=Play(1), 1=Settings(2), 2=Exit(9)
    int ids[] = {1, 2, 9};
    return ids[m_focusedButtonIndex];
  }
  if (state == GameState::GameOver) {
    //* 0=StartNew(1), 1=ExitMenu(9)
    int ids[] = {1, 9};
    return ids[m_focusedButtonIndex];
  }
  if (state == GameState::Settings) {
    //? Language(0)→10, Controls(1)→11, Difficulty(2)→3, VSync(3)→5, Music(4)→4,
    // SFX(5)→12, FPS(6)→6, Hitbox(7)→7, Back(8)→8
    int ids[] = {10, 11, 3, 5, 4, 12, 6, 7, 8};
    if (m_focusedButtonIndex == 2 && m_isDifficultyLocked)
      return 0;
    return ids[m_focusedButtonIndex];
  }
  if (state == GameState::Controls) {
    return 8; //* Back
  }
  return 0;
}

//? Returns center position of the focused button for mouse snapping
sf::Vector2f Menu::getButtonPosition(int index, GameState state) {
  if (state == GameState::MainMenu || state == GameState::Paused) {
    sf::RectangleShape *btns[] = {&m_playButton, &m_settingsButton,
                                  &m_exitButton};
    if (index >= 0 && index < 3)
      return btns[index]->getPosition();
  }
  if (state == GameState::GameOver) {
    sf::RectangleShape *btns[] = {&m_playButton, &m_exitButton};
    if (index >= 0 && index < 2)
      return btns[index]->getPosition();
  }
  if (state == GameState::Settings) {
    sf::RectangleShape *btns[] = {
        &m_languageButton,   &m_controlsButton, &m_difficultyButton,
        &m_vsyncButton,      &m_musicButton,    &m_sfxButton,
        &m_FPSCounterButton, &m_hitboxButton,   &m_backButton};
    if (index >= 0 && index < 9)
      return btns[index]->getPosition();
  }
  if (state == GameState::Controls) {
    return m_controlsBackButton.getPosition();
  }
  return {0.f, 0.f};
}

bool Menu::updateMouseHover(float x, float y, GameState state) {
  int oldHover = m_hoveredButtonIndex;
  m_hoveredButtonIndex = -1;
  sf::Vector2f pos(x, y);

  auto check = [&](sf::RectangleShape &btn, int idx) {
    if (btn.getGlobalBounds().contains(pos))
      m_hoveredButtonIndex = idx;
  };

  if (state == GameState::MainMenu || state == GameState::Paused) {
    check(m_playButton, 0);
    check(m_settingsButton, 1);
    check(m_exitButton, 2);
  } else if (state == GameState::GameOver) {
    check(m_playButton, 0);
    check(m_exitButton, 1);
  } else if (state == GameState::Settings) {
    check(m_languageButton, 0);
    check(m_controlsButton, 1);
    check(m_difficultyButton, 2);
    check(m_vsyncButton, 3);
    check(m_musicButton, 4);
    check(m_sfxButton, 5);
    check(m_FPSCounterButton, 6);
    check(m_hitboxButton, 7);
    check(m_backButton, 8);
  } else if (state == GameState::Controls) {
    check(m_controlsBackButton, 0);
  }
  return m_hoveredButtonIndex != oldHover && m_hoveredButtonIndex != -1;
}

//? Helper: apply yellow pulsing or white normal outline
void Menu::applyFocusOutline(sf::RectangleShape &btn, bool active,
                             bool isPressed) {
  float &hoverFactor = m_hoverFactors[&btn];
  float &pressFactor = m_pressFactors[&btn];
  float &pulseTimer = m_pulseTimers[&btn];
  float hoverSpeed = 10.0f;
  float pressSpeed = 20.0f;

  if (active) {
    if (hoverFactor == 0.0f) {
      pulseTimer = 0.0f;
    }
    pulseTimer += m_lastDt;
    hoverFactor += m_lastDt * hoverSpeed;
    if (hoverFactor > 1.0f)
      hoverFactor = 1.0f;
  } else {
    hoverFactor -= m_lastDt * hoverSpeed;
    if (hoverFactor < 0.0f)
      hoverFactor = 0.0f;
  }

  if (active && isPressed) {
    pressFactor += m_lastDt * pressSpeed;
    if (pressFactor > 1.0f)
      pressFactor = 1.0f;
  } else {
    pressFactor -= m_lastDt * pressSpeed;
    if (pressFactor < 0.0f)
      pressFactor = 0.0f;
  }

  sf::Color outColor = sf::Color::White;
  float thickness = 3.0f;
  float scale = 1.0f;

  if (hoverFactor > 0.0f) {
    float pulse = 0.5f + 0.5f * std::cos(pulseTimer * 5.0f);

    sf::Color activeColor(255, 220, 0, 255);
    float activeThickness = 2.0f + 4.0f * pulse;
    float activeScale = 1.05f;

    outColor.r = static_cast<uint8_t>(
        outColor.r + hoverFactor * (activeColor.r - outColor.r));
    outColor.g = static_cast<uint8_t>(
        outColor.g + hoverFactor * (activeColor.g - outColor.g));
    outColor.b = static_cast<uint8_t>(
        outColor.b + hoverFactor * (activeColor.b - outColor.b));
    outColor.a = static_cast<uint8_t>(
        (uint8_t)(outColor.a + hoverFactor * (activeColor.a - outColor.a)) *
        m_globalAlpha / 255);

    thickness = thickness + hoverFactor * (activeThickness - thickness);
    scale = scale + hoverFactor * (activeScale - scale);
  } else {
    outColor.a = (uint8_t)(outColor.a * m_globalAlpha / 255);
  }

  if (pressFactor > 0.0f) {
    sf::Color pressColor(150, 100, 0, 255);
    float pThickness = 4.0f;
    float pScale = 0.95f;

    outColor.r = static_cast<uint8_t>(
        outColor.r + pressFactor * (pressColor.r - outColor.r));
    outColor.g = static_cast<uint8_t>(
        outColor.g + pressFactor * (pressColor.g - outColor.g));
    outColor.b = static_cast<uint8_t>(
        outColor.b + pressFactor * (pressColor.b - outColor.b));
    outColor.a = static_cast<uint8_t>(
        (uint8_t)(outColor.a + pressFactor * (pressColor.a - outColor.a)) *
        m_globalAlpha / 255);

    thickness = thickness + pressFactor * (pThickness - thickness);
    scale = scale + pressFactor * (pScale - scale);
  }

  btn.setOutlineColor(outColor);
  btn.setOutlineThickness(thickness);
  btn.setScale({scale, scale});
}

//* setupMenuButtons
void Menu::setupMenuButtons(GameState m_gameState, float winWidth,
                            float winHeight, GameSettings &settings,
                            GameState m_lastGameState) {
  settings.saveToFile("settings.ini");
  bool ua = settings.ukrainianLanguage;
  sf::Font &activeFont = ua ? m_menuFontUA : m_menuFont;

  //* Apply fonts
  m_gameNameLogo.setFont(m_menuFontUA);
  m_playButtonText.setFont(activeFont);
  m_settingsButtonText.setFont(activeFont);
  m_exitButtonText.setFont(activeFont);
  m_difficultyText.setFont(activeFont);
  m_difficultyValueText.setFont(activeFont);
  m_musicText.setFont(activeFont);
  m_musicValueText.setFont(activeFont);
  m_vsyncText.setFont(activeFont);
  m_vsyncValueText.setFont(activeFont);
  m_FPSCounterText.setFont(activeFont);
  m_FPSCounterValueText.setFont(activeFont);
  m_hitboxText.setFont(activeFont);
  m_hitboxValueText.setFont(activeFont);
  m_sfxText.setFont(activeFont);
  m_sfxValueText.setFont(activeFont);
  m_languageText.setFont(activeFont);
  m_languageValueText.setFont(activeFont);
  m_controlsButtonText.setFont(activeFont);
  m_backButtonText.setFont(activeFont);
  m_controlsTitle.setFont(activeFont);
  m_controlsContent.setFont(activeFont);
  m_controlsBackText.setFont(activeFont);

  auto alignTexts = [](sf::Text &nameText, sf::Text &valText, float centerX,
                       float centerY, float gap = -1.0f) {
    sf::FloatRect nRect = nameText.getLocalBounds();
    sf::FloatRect vRect = valText.getLocalBounds();
    float totalW = nRect.size.x + gap + vRect.size.x;
    float startX = centerX - totalW / 2.0f;
    nameText.setOrigin({nRect.position.x, 0});
    valText.setOrigin({vRect.position.x, 0});
    float baselineY = centerY - (nRect.position.y + nRect.size.y / 2.0f);
    nameText.setPosition({startX, baselineY});
    valText.setPosition({startX + nRect.size.x + gap, baselineY});
  };

  if (m_gameState == GameState::Controls) {
    m_menuBG.setFillColor(sf::Color(0, 0, 80, 255));

    m_controlsTitle.setString(
        ua ? U"\u041a\u0415\u0420\u0423\u0412\u0410\u041d\u041d\u042f"
           : U"CONTROLS");
    m_controlsTitle.setCharacterSize(130);
    m_controlsTitle.setFillColor(sf::Color(255, 220, 0));
    m_controlsTitle.setOutlineColor(sf::Color(150, 100, 0));
    m_controlsTitle.setOutlineThickness(4.0f);
    sf::FloatRect tr = m_controlsTitle.getLocalBounds();
    m_controlsTitle.setOrigin(
        {tr.position.x + tr.size.x / 2.f, tr.position.y + tr.size.y / 2.f});
    m_controlsTitle.setPosition({winWidth / 2.f, winHeight / 2.f - 340.f});

    sf::String content;
    if (ua) {
      content =
          U"\u0420\u0443\u0445 \u0443 \u0433\u0440\u0456: WASD / "
          U"\u0421\u0442\u0440\u0456\u043b\u043a\u0438 / "
          U"\u041b\u0456\u0432\u0438\u0439 \u0441\u0442\u0456\u043a\n"
          U"\u041d\u0430\u0432\u0456\u0433\u0430\u0446\u0456\u044f "
          U"\u043c\u0435\u043d\u044e: \u041c\u0438\u0448\u0430 / "
          U"\u0421\u0442\u0440\u0456\u043b\u043a\u0438 / "
          U"\u041b\u0456\u0432\u0438\u0439 \u0441\u0442\u0456\u043a / "
          U"\u0425\u0440\u0435\u0441\u0442\u043e\u0432\u0438\u043d\u0430\n"
          U"\u041f\u0456\u0434\u0442\u0432\u0435\u0440\u0434\u0438\u0442\u0438:"
          U" \u041a\u043b\u0456\u043a \u043c\u0438\u0448\u0456 / Enter / "
          U"\u041a\u043d\u043e\u043f\u043a\u0430 A\n"
          U"\u041d\u0430\u0437\u0430\u0434 / \u041f\u0430\u0443\u0437\u0430: "
          U"Esc / \u041a\u043d\u043e\u043f\u043a\u0430 B / Start\n"
          U"\u041f\u043e\u0432\u043d\u0438\u0439 "
          U"\u0435\u043a\u0440\u0430\u043d: F11\n\n"
          U"\u2139 \u0423\u0441\u0456 \u0435\u043a\u0440\u0430\u043d\u0438 "
          U"\u043c\u0435\u043d\u044e "
          U"\u043f\u0456\u0434\u0442\u0440\u0438\u043c\u0443\u044e\u0442\u044c"
          U"\n"
          U"  \u043c\u0438\u0448\u043a\u0443, "
          U"\u043a\u043b\u0430\u0432\u0456\u0430\u0442\u0443\u0440\u0443 "
          U"\u0442\u0430 \u0434\u0436\u043e\u0439\u0441\u0442\u0438\u043a.";
    } else {
      content = U"Movement: WASD / Arrow Keys / Left Stick\n"
                U"Menu Navigation: Mouse / Arrows / Left Stick / D-Pad\n"
                U"Confirm/Select: Mouse Click / Enter / Button A\n"
                U"Back / Pause: Esc / Button B / Start\n"
                U"Fullscreen: F11\n\n"
                U"\u2139 All menu screens support Mouse,\n"
                U"  Keyboard and Gamepad navigation.";
    }
    m_controlsContent.setString(content);
    m_controlsContent.setCharacterSize(42);
    m_controlsContent.setFillColor(sf::Color::White);
    m_controlsContent.setOutlineColor(sf::Color::Black);
    m_controlsContent.setOutlineThickness(2.5f);
    sf::FloatRect cr = m_controlsContent.getLocalBounds();
    m_controlsContent.setOrigin(
        {cr.position.x + cr.size.x / 2.f, cr.position.y});
    m_controlsContent.setPosition({winWidth / 2.f, winHeight / 2.f - 220.f});

    //* Back button
    m_controlsBackText.setString(ua ? U"\u041d\u0430\u0437\u0430\u0434"
                                    : U"Back");
    m_controlsBackText.setCharacterSize(55);
    m_controlsBackText.setFillColor(sf::Color::Yellow);
    m_controlsBackText.setOutlineColor(sf::Color::Black);
    m_controlsBackText.setOutlineThickness(3.f);
    sf::FloatRect br = m_controlsBackText.getLocalBounds();
    m_controlsBackText.setOrigin(
        {br.position.x + br.size.x / 2.f, br.position.y + br.size.y / 2.f});
    m_controlsBackText.setPosition({winWidth / 2.f, winHeight / 2.f + 360.f});

    m_controlsBackButton.setSize({br.size.x + 40.f, br.size.y + 20.f});
    m_controlsBackButton.setOrigin({m_controlsBackButton.getSize().x / 2.f,
                                    m_controlsBackButton.getSize().y / 2.f});
    m_controlsBackButton.setPosition(m_controlsBackText.getPosition());
    m_controlsBackButton.setFillColor(sf::Color(255, 50, 50, 150));
    m_controlsBackButton.setOutlineColor(sf::Color::White);
    m_controlsBackButton.setOutlineThickness(5.f);
    return;
  }

  if (m_gameState == GameState::Settings) {
    m_gameNameLogo.setString(ua ? U"\u041d\u0410\u041b\u0410\u0428\u0422\u0423"
                                  U"\u0412\u0410\u041d\u041d\u042f:"
                                : U"SETTINGS:");
    m_gameNameLogo.setLetterSpacing(ua ? 1.6f : 1.3f);
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

    float uniformWidth = ua ? 820.0f : 550.0f;

    //* 2-Column layout
    float startY = winHeight / 2.0f - 140.0f;
    float gap = 120.0f;
    float yRow0 = startY + gap * 0;
    float yRow1 = startY + gap * 1;
    float yRow2 = startY + gap * 2;
    float yRow3 = startY + gap * 3;
    float yBack = startY + gap * 4.2f;

    float leftX = winWidth / 2.0f - (ua ? 430.0f : 340.0f);
    float rightX = winWidth / 2.0f + (ua ? 430.0f : 340.0f);

    auto makeSettingsBtn = [&](sf::RectangleShape &btn, float cx, float cy,
                               bool isLocked = false) {
      btn.setSize({uniformWidth, 70.f});
      btn.setOrigin({btn.getSize().x / 2.f, btn.getSize().y / 2.f});
      btn.setPosition({cx, cy});
      btn.setFillColor(isLocked ? sf::Color(100, 100, 100, 150)
                                : sf::Color(0, 100, 255, 150));
      btn.setOutlineColor(sf::Color::White);
      btn.setOutlineThickness(5.f);
    };

    //* Language
    m_languageText.setString(ua ? U"\u041c\u043e\u0432\u0430" : U"Language");
    m_languageValueText.setString(
        ua ? U": \u0423\u041a\u0420/\u0410\u041d\u0413\u041b" : U": ENG/UKR");
    m_languageText.setCharacterSize(50);
    m_languageText.setFillColor(sf::Color::White);
    m_languageText.setOutlineColor(sf::Color::Black);
    m_languageText.setOutlineThickness(3.f);
    m_languageValueText.setCharacterSize(50);
    m_languageValueText.setFillColor(sf::Color::White);
    m_languageValueText.setOutlineColor(sf::Color::Black);
    m_languageValueText.setOutlineThickness(3.f);
    alignTexts(m_languageText, m_languageValueText, leftX, yRow0);
    makeSettingsBtn(m_languageButton, leftX, yRow0);

    //* Difficulty
    m_difficultyText.setString(
        ua ? U"\u0421\u043a\u043b\u0430\u0434\u043d\u0456\u0441\u0442\u044c"
           : U"Difficulty");
    sf::String diffValue = U": ";
    sf::Color diffColor;
    if (settings.gameDifficulty == GameDifficulty::Easy) {
      diffValue += ua ? U"\u041b\u0435\u0433\u043a\u043e (^-^)" : U"Easy (^-^)";
      diffColor = sf::Color(0, 168, 0);
    } else if (settings.gameDifficulty == GameDifficulty::Normal) {
      diffValue +=
          ua ? U"\u041d\u043e\u0440\u043c\u0430\u043b\u044c\u043d\u043e (._.)"
             : U"Normal (._.)";
      diffColor = sf::Color(255, 100, 0);
    } else {
      diffValue += ua ? U"\u0412\u0430\u0436\u043a\u043e (0_0)" : U"Hard (0_0)";
      diffColor = sf::Color(160, 0, 0);
    }
    m_difficultyValueText.setString(diffValue);
    m_difficultyText.setCharacterSize(50);
    m_difficultyText.setFillColor(diffColor);
    m_difficultyText.setOutlineColor(sf::Color::Black);
    m_difficultyText.setOutlineThickness(3.f);
    m_difficultyValueText.setCharacterSize(50);
    m_difficultyValueText.setFillColor(diffColor);
    m_difficultyValueText.setOutlineColor(sf::Color::Black);
    m_difficultyValueText.setOutlineThickness(3.f);
    alignTexts(m_difficultyText, m_difficultyValueText, leftX, yRow2);
    m_isDifficultyLocked = (m_lastGameState == GameState::Paused);
    makeSettingsBtn(m_difficultyButton, leftX, yRow2, m_isDifficultyLocked);

    //* VSync
    m_vsyncText.setString("VSync");
    m_vsyncValueText.setString(
        sf::String(U": ") +
        (settings.VSync
             ? (ua ? U"\u0423\u0412\u0406\u041c\u041a\u041d\u0415\u041d\u041e"
                   : U"ON")
             : (ua ? U"\u0412\u0418\u041c\u041a\u041d\u0415\u041d\u041e"
                   : U"OFF")));
    sf::Color vsyncColor = settings.VSync ? sf::Color::Green : sf::Color::Red;
    m_vsyncText.setCharacterSize(50);
    m_vsyncText.setFillColor(vsyncColor);
    m_vsyncText.setOutlineColor(sf::Color::Black);
    m_vsyncText.setOutlineThickness(3.f);
    m_vsyncValueText.setCharacterSize(50);
    m_vsyncValueText.setFillColor(vsyncColor);
    m_vsyncValueText.setOutlineColor(sf::Color::Black);
    m_vsyncValueText.setOutlineThickness(3.f);
    alignTexts(m_vsyncText, m_vsyncValueText, leftX, yRow3);
    makeSettingsBtn(m_vsyncButton, leftX, yRow3);

    //* Music
    m_musicText.setString(ua ? U"\u041c\u0443\u0437\u0438\u043a\u0430"
                             : U"Music");
    m_musicValueText.setString(
        sf::String(U": ") +
        (settings.playMusic
             ? (ua ? U"\u0423\u0412\u0406\u041c\u041a\u041d\u0415\u041d\u041e"
                   : U"ON")
             : (ua ? U"\u0412\u0418\u041c\u041a\u041d\u0415\u041d\u041e"
                   : U"OFF")));
    sf::Color musicColor =
        settings.playMusic ? sf::Color::Green : sf::Color::Red;
    m_musicText.setCharacterSize(50);
    m_musicText.setFillColor(musicColor);
    m_musicText.setOutlineColor(sf::Color::Black);
    m_musicText.setOutlineThickness(3.f);
    m_musicValueText.setCharacterSize(50);
    m_musicValueText.setFillColor(musicColor);
    m_musicValueText.setOutlineColor(sf::Color::Black);
    m_musicValueText.setOutlineThickness(3.f);
    alignTexts(m_musicText, m_musicValueText, rightX, yRow0);
    makeSettingsBtn(m_musicButton, rightX, yRow0);

    //* SFX
    m_sfxText.setString(ua ? U"\u0417\u0432\u0443\u043a\u0438" : U"SOUND");
    m_sfxValueText.setString(
        sf::String(U": ") +
        (settings.playSfx
             ? (ua ? U"\u0423\u0412\u0406\u041c\u041a\u041d\u0415\u041d\u041e"
                   : U"ON")
             : (ua ? U"\u0412\u0418\u041c\u041a\u041d\u0415\u041d\u041e"
                   : U"OFF")));
    sf::Color sfxColor = settings.playSfx ? sf::Color::Green : sf::Color::Red;
    m_sfxText.setCharacterSize(50);
    m_sfxText.setFillColor(sfxColor);
    m_sfxText.setOutlineColor(sf::Color::Black);
    m_sfxText.setOutlineThickness(3.f);
    m_sfxValueText.setCharacterSize(50);
    m_sfxValueText.setFillColor(sfxColor);
    m_sfxValueText.setOutlineColor(sf::Color::Black);
    m_sfxValueText.setOutlineThickness(3.f);
    alignTexts(m_sfxText, m_sfxValueText, rightX, yRow1);
    makeSettingsBtn(m_sfxButton, rightX, yRow1);

    //* FPS
    m_FPSCounterText.setString(
        ua ? U"\u041f\u043e\u043a\u0430\u0437\u0430\u0442\u0438 FPS"
           : U"Show FPS");
    m_FPSCounterValueText.setString(
        sf::String(U": ") +
        (settings.showFps
             ? (ua ? U"\u0423\u0412\u0406\u041c\u041a\u041d\u0415\u041d\u041e"
                   : U"ON")
             : (ua ? U"\u0412\u0418\u041c\u041a\u041d\u0415\u041d\u041e"
                   : U"OFF")));
    sf::Color fpsColor = settings.showFps ? sf::Color::Green : sf::Color::Red;
    m_FPSCounterText.setCharacterSize(50);
    m_FPSCounterText.setFillColor(fpsColor);
    m_FPSCounterText.setOutlineColor(sf::Color::Black);
    m_FPSCounterText.setOutlineThickness(3.f);
    m_FPSCounterValueText.setCharacterSize(50);
    m_FPSCounterValueText.setFillColor(fpsColor);
    m_FPSCounterValueText.setOutlineColor(sf::Color::Black);
    m_FPSCounterValueText.setOutlineThickness(3.f);
    alignTexts(m_FPSCounterText, m_FPSCounterValueText, rightX, yRow2);
    makeSettingsBtn(m_FPSCounterButton, rightX, yRow2);

    //* Hitbox
    m_hitboxText.setString(
        ua ? U"\u0425\u0456\u0442\u0431\u043e\u043a\u0441\u0438"
           : U"Show Hitboxes");
    m_hitboxValueText.setString(
        sf::String(U": ") +
        (settings.showHitbox
             ? (ua ? U"\u0423\u0412\u0406\u041c\u041a\u041d\u0415\u041d\u041e"
                   : U"ON")
             : (ua ? U"\u0412\u0418\u041c\u041a\u041d\u0415\u041d\u041e"
                   : U"OFF")));
    sf::Color hitboxColor =
        settings.showHitbox ? sf::Color::Green : sf::Color::Red;
    m_hitboxText.setCharacterSize(50);
    m_hitboxText.setFillColor(hitboxColor);
    m_hitboxText.setOutlineColor(sf::Color::Black);
    m_hitboxText.setOutlineThickness(3.f);
    m_hitboxValueText.setCharacterSize(50);
    m_hitboxValueText.setFillColor(hitboxColor);
    m_hitboxValueText.setOutlineColor(sf::Color::Black);
    m_hitboxValueText.setOutlineThickness(3.f);
    alignTexts(m_hitboxText, m_hitboxValueText, rightX, yRow3);
    makeSettingsBtn(m_hitboxButton, rightX, yRow3);

    //* Controls button
    m_controlsButtonText.setString(
        ua ? U"\u041a\u0435\u0440\u0443\u0432\u0430\u043d\u043d\u044f"
           : U"Controls");
    m_controlsButtonText.setCharacterSize(50);
    m_controlsButtonText.setFillColor(sf::Color::White);
    m_controlsButtonText.setOutlineColor(sf::Color::Black);
    m_controlsButtonText.setOutlineThickness(3.f);
    sf::FloatRect ctrlRect = m_controlsButtonText.getLocalBounds();
    m_controlsButtonText.setOrigin(
        {ctrlRect.position.x + ctrlRect.size.x / 2.f,
         ctrlRect.position.y + ctrlRect.size.y / 2.f});
    m_controlsButtonText.setPosition({leftX, yRow1});
    makeSettingsBtn(m_controlsButton, leftX, yRow1);

    //* Back
    m_backButtonText.setString(ua ? U"\u041d\u0430\u0437\u0430\u0434"
                                  : U"Back");
    m_backButtonText.setCharacterSize(50);
    m_backButtonText.setFillColor(sf::Color::Yellow);
    m_backButtonText.setOutlineColor(sf::Color::Black);
    m_backButtonText.setOutlineThickness(3.f);
    sf::FloatRect backRect = m_backButtonText.getLocalBounds();
    m_backButtonText.setOrigin({backRect.position.x + backRect.size.x / 2.f,
                                backRect.position.y + backRect.size.y / 2.f});
    m_backButtonText.setPosition({winWidth / 2.f, yBack});
    m_backButton.setSize({backRect.size.x + 40.f, backRect.size.y + 20.f});
    m_backButton.setOrigin(
        {m_backButton.getSize().x / 2.f, m_backButton.getSize().y / 2.f});
    m_backButton.setPosition(m_backButtonText.getPosition());
    m_backButton.setFillColor(sf::Color(255, 50, 50, 150));
    m_backButton.setOutlineColor(sf::Color::White);
    m_backButton.setOutlineThickness(5.f);
    return;
  }

  if (m_gameState == GameState::GameOver) {
    m_menuBG.setFillColor(sf::Color::Transparent);
    m_gameNameLogo.setString(
        ua ? U"\u0412\u0410\u041c \u0413\u0410\u041f\u041b\u0418\u041a :("
           : U"GAME OVER :(");
    m_gameNameLogo.setCharacterSize(150);
    m_gameNameLogo.setFillColor(sf::Color(255, 0, 0));
    m_gameNameLogo.setOutlineColor(sf::Color::Yellow);
    m_gameNameLogo.setOutlineThickness(4.0f);
    sf::FloatRect logoRect = m_gameNameLogo.getLocalBounds();
    m_gameNameLogo.setOrigin({logoRect.position.x + logoRect.size.x / 2.0f,
                              logoRect.position.y + logoRect.size.y / 2.0f});
    m_gameNameLogo.setPosition({winWidth / 2.0f, winHeight / 2.0f - 200.0f});

    m_playButtonText.setString(
        ua ? U"\u041d\u043e\u0432\u0430 \u0433\u0440\u0430"
           : U"Start new game");
    m_playButtonText.setCharacterSize(60);
    m_playButtonText.setFillColor(sf::Color(255, 220, 0));
    m_playButtonText.setOutlineColor(sf::Color::Black);
    m_playButtonText.setOutlineThickness(4.0f);
    sf::FloatRect m_playRect = m_playButtonText.getLocalBounds();
    m_playButtonText.setOrigin(
        {m_playRect.position.x + m_playRect.size.x / 2.f,
         m_playRect.position.y + m_playRect.size.y / 2.f});

    m_exitButtonText.setString(ua ? U"\u0412\u0438\u0439\u0442\u0438 "
                                    U"\u0434\u043e \u043c\u0435\u043d\u044e"
                                  : U"Exit to menu");
    m_exitButtonText.setCharacterSize(60);
    m_exitButtonText.setFillColor(sf::Color(255, 220, 0));
    m_exitButtonText.setOutlineColor(sf::Color::Black);
    m_exitButtonText.setOutlineThickness(4.0f);
    sf::FloatRect m_exitRect = m_exitButtonText.getLocalBounds();
    m_exitButtonText.setOrigin(
        {m_exitRect.position.x + m_exitRect.size.x / 2.f,
         m_exitRect.position.y + m_exitRect.size.y / 2.f});

    float maxBtnWidth = std::max(m_playRect.size.x, m_exitRect.size.x) + 40.f;
    float maxBtnHeight = std::max(m_playRect.size.y, m_exitRect.size.y) + 40.f;
    float offsetX = maxBtnWidth / 2.f + 20.f;

    m_playButtonText.setPosition(
        {winWidth / 2.f - offsetX, winHeight / 2.f + 100.f});
    m_exitButtonText.setPosition(
        {winWidth / 2.f + offsetX, winHeight / 2.f + 100.f});

    m_playButton.setSize({maxBtnWidth, maxBtnHeight});
    m_playButton.setOrigin(
        {m_playButton.getSize().x / 2.f, m_playButton.getSize().y / 2.f});
    m_playButton.setPosition(m_playButtonText.getPosition());
    m_playButton.setFillColor(sf::Color(0, 255, 0, 100));
    m_playButton.setOutlineColor(sf::Color::White);
    m_playButton.setOutlineThickness(5.f);

    m_exitButton.setSize({maxBtnWidth, maxBtnHeight});
    m_exitButton.setOrigin(
        {m_exitButton.getSize().x / 2.f, m_exitButton.getSize().y / 2.f});
    m_exitButton.setPosition(m_exitButtonText.getPosition());
    m_exitButton.setFillColor(sf::Color(0, 255, 0, 100));
    m_exitButton.setOutlineColor(sf::Color::White);
    m_exitButton.setOutlineThickness(5.f);
    return;
  }

  m_gameNameLogo.setString("FIZZ RUSH!");
  m_gameNameLogo.setLetterSpacing(1.3f);
  m_gameNameLogo.setCharacterSize(200);
  m_gameNameLogo.setFillColor(sf::Color(255, 220, 0));
  m_gameNameLogo.setOutlineColor(sf::Color(150, 100, 0));
  m_gameNameLogo.setOutlineThickness(4.0f);
  sf::FloatRect gameNameText = m_gameNameLogo.getLocalBounds();
  m_gameNameLogo.setOrigin(
      {gameNameText.position.x + gameNameText.size.x / 2.f,
       gameNameText.position.y + gameNameText.size.y / 2.f});
  m_gameNameLogo.setPosition({winWidth / 2.f, winHeight / 2.f - 350.f});

  if (m_gameState == GameState::MainMenu) {
    if (m_menuMusic.getStatus() != sf::Sound::Status::Playing &&
        settings.playMusic)
      m_menuMusic.play();
    m_playButtonText.setString(ua ? U"\u0420\u043e\u0437\u043f\u043e\u0447"
                                    U"\u0430\u0442\u0438 \u0433\u0440\u0443"
                                  : U"Start the game");
    m_menuBG.setFillColor(sf::Color(0, 0, 120, 255));
    m_exitButtonText.setString(
        ua ? U"\u0412\u0438\u0439\u0442\u0438 \u0437 \u0433\u0440\u0438"
           : U"Exit the game");
  } else if (m_gameState == GameState::Playing) {
    m_menuMusic.stop();
  } else if (m_gameState == GameState::Paused) {
    m_playButtonText.setString(
        ua ? U"\u041f\u0440\u043e\u0434\u043e\u0432\u0436\u0438\u0442\u0438"
           : U"Resume");
    m_menuBG.setFillColor(sf::Color(0, 0, 120, 180));
    m_exitButtonText.setString(ua ? U"\u0412\u0438\u0439\u0442\u0438 "
                                    U"\u0434\u043e \u043c\u0435\u043d\u044e"
                                  : U"Exit to menu");
  }

  m_playButtonText.setFillColor(sf::Color(255, 220, 0));
  sf::FloatRect m_playButtonPos = m_playButtonText.getLocalBounds();
  m_playButtonText.setOrigin(
      {m_playButtonPos.position.x + m_playButtonPos.size.x / 2.f,
       m_playButtonPos.position.y + m_playButtonPos.size.y / 2.f});
  m_playButtonText.setPosition({winWidth / 2.f, winHeight / 2.f - 100.f});
  m_playButton.setSize(
      {m_playButtonPos.size.x + 20.f, m_playButtonPos.size.y + 20.f});
  m_playButton.setOrigin(
      {m_playButton.getSize().x / 2.f, m_playButton.getSize().y / 2.f});
  m_playButton.setPosition(m_playButtonText.getPosition());
  m_playButton.setFillColor(sf::Color(0, 255, 0, 100));
  m_playButton.setOutlineColor(sf::Color::White);
  m_playButton.setOutlineThickness(5.f);

  m_settingsButtonText.setString(ua ? U"\u041d\u0430\u043b\u0430\u0448\u0442"
                                      U"\u0443\u0432\u0430\u043d\u043d\u044f"
                                    : U"Settings");
  m_settingsButtonText.setCharacterSize(60);
  m_settingsButtonText.setFillColor(sf::Color(255, 220, 0));
  m_settingsButtonText.setOutlineColor(sf::Color::Black);
  m_settingsButtonText.setOutlineThickness(4.0f);
  sf::FloatRect m_menuSettingsPos = m_settingsButtonText.getLocalBounds();
  m_settingsButtonText.setOrigin(
      {m_menuSettingsPos.position.x + m_menuSettingsPos.size.x / 2.f,
       m_menuSettingsPos.position.y + m_menuSettingsPos.size.y / 2.f});
  m_settingsButtonText.setPosition({winWidth / 2.f, winHeight / 2.f + 75.f});
  m_settingsButton.setSize(
      {m_menuSettingsPos.size.x + 20.f, m_menuSettingsPos.size.y + 20.f});
  m_settingsButton.setOrigin(
      {m_settingsButton.getSize().x / 2.f, m_settingsButton.getSize().y / 2.f});
  m_settingsButton.setPosition(m_settingsButtonText.getPosition());
  m_settingsButton.setFillColor(sf::Color(0, 255, 0, 100));
  m_settingsButton.setOutlineColor(sf::Color::White);
  m_settingsButton.setOutlineThickness(5.f);

  m_exitButtonText.setCharacterSize(60);
  m_exitButtonText.setFillColor(sf::Color(255, 220, 0));
  m_exitButtonText.setOutlineColor(sf::Color::Black);
  m_exitButtonText.setOutlineThickness(4.0f);
  sf::FloatRect m_menuExitText = m_exitButtonText.getLocalBounds();
  m_exitButtonText.setOrigin(
      {m_menuExitText.position.x + m_menuExitText.size.x / 2.f,
       m_menuExitText.position.y + m_menuExitText.size.y / 2.f});
  m_exitButtonText.setPosition({winWidth / 2.f, winHeight / 2.f + 250.f});
  m_exitButton.setSize(
      {m_menuExitText.size.x + 20.f, m_menuExitText.size.y + 20.f});
  m_exitButton.setOrigin(
      {m_exitButton.getSize().x / 2.f, m_exitButton.getSize().y / 2.f});
  m_exitButton.setPosition(m_exitButtonText.getPosition());
  m_exitButton.setFillColor(sf::Color(0, 255, 0, 100));
  m_exitButton.setOutlineColor(sf::Color::White);
  m_exitButton.setOutlineThickness(5.f);
}

//* mouseClickPos
int Menu::mouseClickPos(float mouseX, float mouseY, GameState m_gameState,
                        const GameSettings &settings) {
  sf::Vector2f pos(mouseX, mouseY);
  if (m_gameState == GameState::MainMenu || m_gameState == GameState::Paused) {
    if (m_playButton.getGlobalBounds().contains(pos))
      return 1;
    if (m_settingsButton.getGlobalBounds().contains(pos))
      return 2;
    if (m_exitButton.getGlobalBounds().contains(pos))
      return 9;
    return 0;
  }
  if (m_gameState == GameState::Settings) {
    if (m_languageButton.getGlobalBounds().contains(pos))
      return 10;
    if (m_controlsButton.getGlobalBounds().contains(pos))
      return 11;
    if (m_difficultyButton.getGlobalBounds().contains(pos))
      return m_isDifficultyLocked ? 0 : 3;
    if (m_vsyncButton.getGlobalBounds().contains(pos))
      return 5;
    if (m_musicButton.getGlobalBounds().contains(pos))
      return 4;
    if (m_FPSCounterButton.getGlobalBounds().contains(pos))
      return 6;
    if (m_hitboxButton.getGlobalBounds().contains(pos))
      return 7;
    if (m_sfxButton.getGlobalBounds().contains(pos))
      return 12;
    if (m_backButton.getGlobalBounds().contains(pos))
      return 8;
    return 0;
  }
  if (m_gameState == GameState::GameOver) {
    if (m_playButton.getGlobalBounds().contains(pos))
      return 1;
    if (m_exitButton.getGlobalBounds().contains(pos))
      return 9;
    return 0;
  }
  if (m_gameState == GameState::Controls) {
    if (m_controlsBackButton.getGlobalBounds().contains(pos))
      return 8;
    return 0;
  }
  return 0;
}

//* draw
void Menu::draw(sf::RenderWindow &window, GameState m_gameState) {
  window.draw(m_menuBG);

  bool showBGSprite =
      (m_gameState == GameState::MainMenu || m_gameState == GameState::Paused ||
       m_gameState == GameState::Settings ||
       m_gameState == GameState::Controls);
  if (showBGSprite)
    window.draw(m_menuBGSprite);

  //? Determine which button index is "active" (hover trumps focus in Mouse
  //? mode)
  int activeIdx = -1;
  if (m_inputMode == InputMode::Mouse)
    activeIdx = m_hoveredButtonIndex;
  else
    activeIdx = m_focusedButtonIndex;

  bool isPressed = false;
  if (m_inputMode == InputMode::Mouse) {
    isPressed = window.hasFocus() &&
                sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
  } else {
    isPressed = window.hasFocus() &&
                (sf::Joystick::isButtonPressed(0, 0) ||
                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter));
  }

  auto drawBtn = [&](sf::RectangleShape &btn, bool isActive,
                     bool isBtnPressed) {
    applyFocusOutline(btn, isActive, isBtnPressed);

    sf::Color origFill = btn.getFillColor();
    sf::Color fill = origFill;
    fill.a = static_cast<uint8_t>((fill.a * m_globalAlpha) / 255);
    btn.setFillColor(fill);

    sf::Color origOut = btn.getOutlineColor();
    sf::Color out = origOut;
    out.a = static_cast<uint8_t>((out.a * m_globalAlpha) / 255);
    btn.setOutlineColor(out);

    window.draw(btn);
    if (isActive && isBtnPressed) {
      sf::RectangleShape overlay = btn;
      overlay.setFillColor(
          sf::Color(0, 0, 0, (uint8_t)(100 * m_globalAlpha / 255)));
      overlay.setOutlineColor(sf::Color::Transparent);
      window.draw(overlay);
    }

    btn.setFillColor(origFill);
    btn.setOutlineColor(origOut);
  };

  auto drawBtnText = [&](const sf::RectangleShape &btn, sf::Text &t1,
                         sf::Text *t2 = nullptr) {
    sf::Color origC1 = t1.getFillColor();
    sf::Color c1 = origC1;
    c1.a = static_cast<uint8_t>((c1.a * m_globalAlpha) / 255);
    t1.setFillColor(c1);

    sf::Color origOc1 = t1.getOutlineColor();
    sf::Color oc1 = origOc1;
    oc1.a = static_cast<uint8_t>((oc1.a * m_globalAlpha) / 255);
    t1.setOutlineColor(oc1);

    sf::Color origC2, origOc2;
    if (t2) {
      origC2 = t2->getFillColor();
      sf::Color c2 = origC2;
      c2.a = static_cast<uint8_t>((c2.a * m_globalAlpha) / 255);
      t2->setFillColor(c2);

      origOc2 = t2->getOutlineColor();
      sf::Color oc2 = origOc2;
      oc2.a = static_cast<uint8_t>((oc2.a * m_globalAlpha) / 255);
      t2->setOutlineColor(oc2);
    }

    sf::Vector2f pos = btn.getPosition();
    sf::Vector2f s = btn.getScale();
    sf::RenderStates states;
    states.transform.translate(pos).scale(s).translate(-pos);
    window.draw(t1, states);
    if (t2)
      window.draw(*t2, states);

    t1.setFillColor(origC1);
    t1.setOutlineColor(origOc1);
    if (t2) {
      t2->setFillColor(origC2);
      t2->setOutlineColor(origOc2);
    }
  };

  if (m_gameState == GameState::Controls) {
    window.draw(m_controlsTitle);
    window.draw(m_controlsContent);
    drawBtn(m_controlsBackButton, activeIdx == 0, isPressed);
    drawBtnText(m_controlsBackButton, m_controlsBackText);
    return;
  }

  window.draw(m_gameNameLogo);

  if (m_gameState == GameState::Settings) {
    //? Settings buttons with focus outlines
    //? Order: Lang(0), Controls(1), Diff(2), VSync(3), Music(4), FPS(5),
    // Hitbox(6), Back(7)
    struct {
      sf::RectangleShape *btn;
      sf::Text *t1;
      sf::Text *t2;
      int idx;
    } items[] = {
        {&m_languageButton, &m_languageText, &m_languageValueText, 0},
        {&m_controlsButton, &m_controlsButtonText, nullptr, 1},
        {&m_difficultyButton, &m_difficultyText, &m_difficultyValueText, 2},
        {&m_vsyncButton, &m_vsyncText, &m_vsyncValueText, 3},
        {&m_musicButton, &m_musicText, &m_musicValueText, 4},
        {&m_FPSCounterButton, &m_FPSCounterText, &m_FPSCounterValueText, 6},
        {&m_hitboxButton, &m_hitboxText, &m_hitboxValueText, 7},
        {&m_sfxButton, &m_sfxText, &m_sfxValueText, 5},
        {&m_backButton, &m_backButtonText, nullptr, 8},
    };
    for (auto &item : items) {
      drawBtn(*item.btn, activeIdx == item.idx, isPressed);
      drawBtnText(*item.btn, *item.t1, item.t2);
    }
  } else if (m_gameState == GameState::GameOver) {
    drawBtn(m_playButton, activeIdx == 0, isPressed);
    drawBtn(m_exitButton, activeIdx == 1, isPressed);
    drawBtnText(m_playButton, m_playButtonText);
    drawBtnText(m_exitButton, m_exitButtonText);
  } else {
    //* MainMenu / Paused
    drawBtn(m_playButton, activeIdx == 0, isPressed);
    drawBtn(m_settingsButton, activeIdx == 1, isPressed);
    drawBtn(m_exitButton, activeIdx == 2, isPressed);
    drawBtnText(m_playButton, m_playButtonText);
    drawBtnText(m_settingsButton, m_settingsButtonText);
    drawBtnText(m_exitButton, m_exitButtonText);
  }
}