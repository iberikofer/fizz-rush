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
	void update(sf::Time dt, float winWidth, float winHeight, float m_machineLeftWall, float m_machineRightWall);
	void updateSound();
	void setDifficultyParams(int maxHP, float playerSpeed, float invincibilityDur);
	void checkWorldCollision(float winWidth, float winHeight);
	std::vector<CollisionCircle> getHitboxes();
	bool hasPlayerMoved();
	int getHealth();
	int loseHealth(sf::Time dt);
	void resetGame(float m_startPosX, float m_startPosY, int maxHP, float difficultySpeed, float invincibilityDuration, GameDifficulty difficulty);
	void draw(sf::RenderWindow &window, const GameSettings &gameSettings);

private:
	//* === GRAPHICS ===
	sf::Texture m_playerTexture;
	sf::Texture m_playerAuraTexture1;
	sf::Texture m_playerAuraTexture2;
	sf::Texture m_arrowTexture;
	sf::Sprite m_player;
	sf::Sprite m_playerAura;
	sf::Sprite m_arrow;
	float m_waveTotalTime;

	//* === PHYSICS ===
	float m_playerWidth;
	float m_playerHeight;
	float m_playerSpeed;
	bool m_playerMoved;
	bool m_isCanMoving;
	float m_machineLeftWall;
	float m_machineUpWall;
	float m_machineRightWall;
	GameDifficulty m_difficulty;

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
	bool m_showAura;
	bool m_isInvincible;
	float m_invincibilityTimer;
	float m_maxInvincibilityTime;
};
