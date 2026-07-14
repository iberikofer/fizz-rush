#pragma once
#include <SFML/Graphics.hpp>
#include "Menu.hpp"
#include "Settings.hpp"
#include "Player.hpp"
#include "Enemies.hpp"
#include "Boss.hpp"

enum class GameEpisode
{
	VendingMachine,
	Survival,
	BossFight,
	Victory
};

struct HeartPickup
{
	HeartPickup(const sf::Texture &texture) : healSprite(texture) {}

	sf::Sprite healSprite;
	float animationTime = 0.0f;
	bool isActive = false;
};

class Game
{
public:
	Game();
	void run();

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

	//* SYSTEM OBJECTS
	sf::Font m_loadingFont;
	sf::Text m_loadingText;
	sf::RectangleShape m_loadingTextBG;
	sf::Texture m_bgTexture;
	sf::Sprite m_bgSprite;
	sf::Texture m_enemyBarTexture;
	sf::Sprite m_enemyBar;
	sf::Texture m_enemyHand1Texture;
	sf::Sprite m_enemyHand1;
	sf::Texture m_enemyBar2Texture;
	sf::Sprite m_enemyBar2;
	sf::Texture m_enemyHand2Texture;
	sf::Sprite m_enemyHand2;
	sf::Texture m_enemyBoxTexture;
	sf::Sprite m_enemyBox;
	sf::Texture m_enemyCookieTexture;
	sf::Sprite m_enemyCookie;
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
	float m_machineUpWall;
	float m_WallPushBack;
	sf::SoundBuffer m_menuButtonSoundBuffer;
	sf::Sound m_menuButtonSound;
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
	sf::SoundBuffer m_deathSoundBuffer;
	sf::Sound m_deathSound;
	sf::SoundBuffer m_healSoundBuffer;
	sf::Sound m_healSound;
	float m_enemySpawnTimer;
	float m_currentEpisodeTime;
	float m_currentLeftWall;
	float m_currentRightWall;
	float m_currentUpWall;
	float difficultyPlayerSpeed;
	float m_episodeDuration;
	float m_transitionTimer;
	GameEpisode m_nextEpisode;
	void startTransition(GameEpisode nextEpisode);
	sf::RectangleShape m_fadeRect;
	float m_fadeAlpha;
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
	bool isHit;

	//* === GAME OBJETS ===
	Menu m_menu;
	Player m_player;
	Boss m_boss;
	std::vector<sf::Texture> m_currentEnemyTextures;
	std::vector<sf::Texture> m_enemyCanTextures;
	std::vector<Enemy> m_enemies;
};