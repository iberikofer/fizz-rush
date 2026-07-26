#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include "Settings.hpp"

enum class InputMode
{
	Mouse,
	Keyboard,
	Gamepad
};

class Menu
{
public:
	Menu(float winWidth, float winHeight);
	void loadAssets(float winWidth, float winHeight);
	void updateMusicVolume(bool isPlaying);
	void fadeOutMusic(float dtSeconds);
	void setupMenuButtons(GameState m_gameState, float winWidth, float winHeight, GameSettings &settings, GameState m_lastGameState);
	int mouseClickPos(float mouseX, float mouseY, GameState m_gameState, const GameSettings &settings);

	void setGlobalAlpha(uint8_t alpha) { m_globalAlpha = alpha; }

	//* Navigation
	bool moveFocus2D(int dx, int dy, GameState state);
	int  getFocusedButtonClickType(GameState state);
	int  getFocusedButtonIndex() const { return m_focusedButtonIndex; }
	int  getHoveredButtonIndex() const { return m_hoveredButtonIndex; }
	void setFocusedButtonIndex(int index) { m_focusedButtonIndex = index; }
	sf::Vector2f getButtonPosition(int index, GameState state);
	bool updateMouseHover(float x, float y, GameState state);
	void setInputMode(InputMode mode);
	InputMode getInputMode() const;
	void resetFocus(GameState state);
	void syncFocusFromHover(GameState state);

	const sf::Font& getFont() const { return m_menuFont; }
	const sf::Font& getFontUA() const { return m_menuFontUA; }
	void updatePulse(float dtSeconds);
	bool consumeHoverSoundFlag();

	void draw(sf::RenderWindow &window, GameState m_gameState);

private:
	int  buttonCount(GameState state) const;
	void applyFocusOutline(sf::RectangleShape &btn, bool active, bool isPressed = false);

	InputMode m_inputMode         = InputMode::Mouse;
	int       m_focusedButtonIndex = 0;
	int       m_hoveredButtonIndex = -1;
	int       m_lastSettingsColumn = 0;
	float     m_pulseTimer         = 0.f;
	float     m_lastDt             = 0.016f;
	std::unordered_map<const sf::RectangleShape*, float> m_hoverFactors;
	std::unordered_map<const sf::RectangleShape*, float> m_pressFactors;
	std::unordered_map<const sf::RectangleShape*, float> m_pulseTimers;

	sf::RectangleShape m_menuBG;
	sf::Texture        m_menuBGTexture;
	sf::Sprite         m_menuBGSprite;
	sf::Font           m_menuFont;
	sf::Font           m_menuFontUA;
	sf::Text           m_gameNameLogo;

	sf::Text           m_playButtonText;
	sf::RectangleShape m_playButton;
	sf::Text           m_settingsButtonText;
	sf::RectangleShape m_settingsButton;
	sf::Text           m_exitButtonText;
	sf::RectangleShape m_exitButton;

	sf::Text           m_settingsTitle;
	sf::Text           m_difficultyText;
	sf::Text           m_difficultyValueText;
	sf::RectangleShape m_difficultyButton;
	sf::Text           m_musicText;
	sf::Text           m_musicValueText;
	sf::RectangleShape m_musicButton;
	sf::Text           m_vsyncText;
	sf::Text           m_vsyncValueText;
	sf::RectangleShape m_vsyncButton;
	sf::Text           m_FPSCounterText;
	sf::Text           m_FPSCounterValueText;
	sf::RectangleShape m_FPSCounterButton;
	sf::Text           m_hitboxText;
	sf::Text           m_hitboxValueText;
	sf::RectangleShape m_hitboxButton;
	sf::Text           m_languageText;
	sf::Text           m_languageValueText;
	sf::RectangleShape m_languageButton;
	sf::Text           m_controlsButtonText;
	sf::RectangleShape m_controlsButton;
	sf::Text           m_backButtonText;
	sf::RectangleShape m_backButton;
	sf::Text           m_sfxText;
	sf::Text           m_sfxValueText;
	sf::RectangleShape m_sfxButton;

	sf::Text           m_controlsTitle;
	sf::Text           m_controlsContent;
	sf::Text           m_controlsBackText;
	sf::RectangleShape m_controlsBackButton;

	sf::SoundBuffer    m_menuMusicBuffer;
	sf::Sound          m_menuMusic;

	bool m_isDifficultyLocked;
	uint8_t m_globalAlpha = 255;
	bool m_wantsToPlayHoverSound = false;
};