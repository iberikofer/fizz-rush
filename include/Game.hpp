#pragma once
#include "Boss.hpp"
#include "Enemies.hpp"
#include "Menu.hpp"
#include "Player.hpp"
#include "Settings.hpp"
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

enum class GameEpisode { VendingMachine, Survival, BossFight, Victory };

enum class PadBtn { A = 0, B = 1, X = 2, Y = 3, Select = 6, Start = 7 };

struct HeartPickup {
  HeartPickup(const sf::Texture &texture) : healSprite(texture) {}

  sf::Sprite healSprite;
  float animationTime = 0.0f;
  bool isActive = false;
};

class Game {
public:
  Game();
  void run();

  PadBtn mapJoystickButtonToPadBtn(unsigned int joystickId,
                                   unsigned int buttonId);
  bool isPadButtonPressed(unsigned int joystickId, PadBtn btn);

private:
  //* SETTINGS
  std::string m_gameWindowName = "Fizz Rush!";
  sf::RenderWindow m_gameWindow;
  sf::Vector2u m_currentWindowSize;
  sf::Image m_gameWindowIcon;
  GameSettings m_gameSettings;
  GameState m_currentGameState;
  GameState m_lastGameState;
  GameEpisode m_currentEpisode;
  void startNewGame(bool useTransition = true);
  void setupEpisode(GameEpisode episode);
  void playCutscene(int episodeNumber);

  //* SYSTEM OBJECTS
  sf::Font m_loadingFont;
  sf::Font m_loadingFontUA;
  sf::Text m_loadingText;
  sf::RectangleShape m_loadingTextBG;
  sf::Texture m_bgTexture;
  sf::Sprite m_bgSprite;
  sf::Texture m_enemyBarTexture;
  sf::Texture m_enemyHand1Texture;
  sf::Texture m_enemyBar2Texture;
  sf::Texture m_enemyHand2Texture;
  sf::Texture m_enemyBoxTexture;
  sf::Texture m_enemyCookieTexture;
  sf::SoundBuffer m_enemyCookieBuffer;
  sf::Sound m_cookieSound;
  float m_startPosX;
  float m_startPosY;
  sf::Clock m_gameClock;
  sf::Font m_fpsFont;
  sf::Text m_fpsText;
  sf::RectangleShape m_fpsErrorRect;
  float m_fpsUpdateTimer = 0.0f;
  float m_fpsMargin = 135.0f;
  bool m_fpsFontIsLoaded = false;
  bool m_isFullscreen = false;
  int m_menuClickType;
  float m_machineLeftWall;
  float m_machineRightWall;
  float m_machineTopWall;
  float m_WallPushBack;
  sf::SoundBuffer m_menuButtonSoundBuffer;
  sf::Sound m_menuButtonSound;
  sf::SoundBuffer m_menuSwitchSoundBuffer;
  sf::Sound m_menuSwitchSound;
  float m_menuSwitchSoundTimer = 0.0f;
  sf::SoundBuffer m_Episode1MusicBuffer;
  sf::Sound m_Episode1Music;
  sf::SoundBuffer m_Episode2MusicBuffer;
  sf::Sound m_Episode2Music;
  sf::SoundBuffer m_Episode3MusicBuffer;
  sf::Sound m_Episode3Music;
  sf::SoundBuffer m_transitionSoundBuffer;
  sf::Sound m_transitionSound;
  sf::SoundBuffer m_winSoundBuffer;
  sf::Sound m_winSound;
  sf::SoundBuffer m_winMusicBuffer;
  sf::Sound m_winMusic;
  sf::SoundBuffer m_deathSoundBuffer;
  sf::Sound m_deathSound;
  sf::SoundBuffer m_healSoundBuffer;
  sf::Sound m_healSound;
  sf::SoundBuffer m_heartSpawnSoundBuffer;
  sf::Sound m_heartSpawnSound;
  sf::SoundBuffer m_gameLoadingSoundBuffer;
  sf::Sound m_gameLoadingSound;
  sf::SoundBuffer m_gameStartSoundBuffer;
  sf::Sound m_gameStartSound;
  float m_enemySpawnTimer;
  float m_currentEpisodeTime;
  float m_currentLeftWall;
  float m_currentRightWall;
  float m_currentTopWall;
  float difficultyPlayerSpeed;
  float m_episodeDuration;
  float m_transitionTimer;
  GameEpisode m_nextEpisode;
  void startTransition(GameEpisode nextEpisode);
  void updateSfxVolume(bool playSfx);
  sf::RectangleShape m_fadeRect;
  float m_fadeAlpha;
  float m_blackScreenDelay = 0.0f;
  bool m_isFadingOut;
  GameEpisode m_nextEpisodeAfterFade;
  void startFadeOut(GameEpisode nextEpisode);
  bool m_goToMenuAfterFade;
  void startFadeOutToMenu();
  bool m_isFadingIn;
  sf::Texture m_heartPickupTexture;
  std::vector<HeartPickup> m_hearts;
  float m_heartSpawnTimer;
  void spawnHeart();
  sf::FloatRect m_slowZone;
  sf::FloatRect m_slowSafeZone;
  sf::RectangleShape m_slowSafeZoneRect;

  //* === GAME OBJECTS ===
  Menu m_menu;
  Player m_player;
  Boss m_boss;
  std::vector<sf::Texture> m_currentEnemyTextures;
  std::vector<sf::Texture> m_enemyCanTextures;
  std::vector<Enemy> m_enemies;

  //* === INPUT / NAVIGATION ===
  InputMode m_inputMode = InputMode::Mouse;
  sf::Vector2i m_lastMousePos;
  float m_mouseSwitchCooldown = 0.f; //? 0.15s cooldown
  bool m_stickNavActive = false;     //? debounce for stick nav
  float m_stickNavTimer = 0.f;       //? time since last nav move

  //* Rumble
  void setVibration(float left, float right);
  float m_rumbleTimer = 0.f;
  bool m_bossRumbleActive = false;

  //* Player Death Animation State
  sf::SoundBuffer m_deathFizzSoundBuffer;
  sf::Sound m_deathFizzSound;
  float m_deathAnimTimer = 0.f;
  float m_gameOverAnimTimer = 0.f;
  sf::Vector2f m_deathAnimVelocity;
  float m_deathAnimGravity = 1500.0f;
  int m_deathAnimBounceCount = 0;
  bool m_deathAnimLaunched = false;
  bool m_deathAnimWhitePhase = false;
  bool m_isPlayerDying = false;
  void triggerPlayerDeath();

  //? Helper to execute a menu action ID (reused for both mouse and
  //? keyboard/pad)
  void handleMenuAction(int actionId, float currentW, float currentH);

  //? Pending menu action for smooth animations
  int m_pendingMenuAction = -1;
  float m_pendingMenuTimer = 0.f;

  //* === SCROLLING BACKGROUND (Episode 1) ===
  std::optional<sf::Sprite> m_bgSprite2;
  float m_bgScrollSpeed = 40.0f;

  //* === DISCLAIMER SCREEN ===
  void drawDisclaimerScreen(float currentW, float currentH);

  //* === STORY SCREEN ===
  void drawStoryScreen(float currentW, float currentH);
  sf::String m_storyTextFull;
  sf::String m_storyTextVisible;
  float m_typewriterTimer = 0.f;
  float m_typewriterSpeed = 0.05f;
  size_t m_typewriterIndex = 0;
  bool m_typewriterDone = false;
  bool m_isStoryActive = false;

  //* === EPISODE TITLE & CUTSCENE SKIP ===
  float m_cutsceneSkipHoldTimer = 0.f;
  bool m_cutsceneSkipped = false;
  bool m_skipTextAnimating = false;
  float m_skipTextAnimTimer = 0.f;
  bool m_waitingForCutsceneReturn;
  float m_cutsceneCooldown = 0.0f;
  bool m_goToStoryAfterFade = false;
  bool m_fromGameOver = false;
};