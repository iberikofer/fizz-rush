#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include "Settings.hpp"

class Player
{
public:
	Player(float winWidth, float winHeight);
	void loadAssets(float startPosX, float startPosY);
	void update(sf::Time dt, float winWidth, float winHeight, float m_machineLeftWall, float m_machineRightWall, float m_machineUpWall, float m_WallPushBack, bool playWallSound, int currentEpisode);
	void stopSound();
	void checkWorldCollision(float winWidth, float winHeight, float leftWall, float rightWall, float topWall, float m_WallPushBack, bool playWallSound);
	const std::vector<CollisionCircle> &getHitboxes();
	sf::FloatRect getPlayerHitbox();
	bool hasPlayerMoved();
	int getHealth();
	void gainHealth(int amount);
	int loseHealth();
	void resetGame(float m_startPosX, float m_startPosY, int maxHP, float difficultySpeed, float invincibilityDuration, GameDifficulty difficulty);
	sf::Vector2f getPosition();
	void startNextEpisode(float startX, float startY);
	void draw(sf::RenderWindow &window, const GameSettings &gameSettings);
	void updateCarpetSpeed(bool isOnCarpet);

private:
	//* === GRAPHICS ===
	sf::Texture m_canTexture;
	sf::Texture m_canRollTexture;
	sf::Texture m_canSlowtexture;
	sf::Texture m_canRollSlowTexture;
	sf::Texture m_auraTexture1;
	sf::Texture m_auraTexture2;
	sf::Texture m_arrowTexture;
	sf::Sprite m_player;
	sf::Sprite m_aura;
	sf::Sprite m_arrow;
	sf::Texture m_fullHeartTexture;
	sf::Texture m_emptyHeartTexture;
	sf::Texture m_emptyHeartTexture2;
	sf::Sprite m_HealthSprite;
	float m_waveTotalTime;
	float m_rollAnimTimer;
	bool m_isRollTexture;

	//* === PHYSICS ===
	float m_playerWidth;
	float m_playerHeight;
	float m_speed;
	float m_basicSpeed;
	float m_carpetMultiplier;
	float m_invincibilityMultiplier;
	bool m_playerMoved;
	bool m_isCanMoving;
	float m_machineLeftWall;
	float m_machineUpWall;
	float m_machineRightWall;
	std::vector<CollisionCircle> m_hitboxes;
	sf::Vector2f m_velocity;
	float m_acceleration;
	float m_friction;

	//* === SOUND ===
	sf::SoundBuffer m_wallSoundBuffer;
	sf::Sound m_wallSound;
	sf::SoundBuffer m_hitSoundBuffer;
	sf::Sound m_hitSound;
	sf::SoundBuffer m_rollSoundBuffer;
	sf::Sound m_rollSound;
	float m_wallSoundTimer;
	float m_wallSoundInterval;

	//* === GAMING PROCESS ===
	int m_HP;
	int m_maxHP;
	bool m_showAura;
	bool m_isInvincible;
	float m_invincibilityTimer;
	float m_maxInvincibilityTime;
	GameDifficulty m_difficulty;
};
