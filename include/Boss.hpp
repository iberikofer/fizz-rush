#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include "Settings.hpp"

enum class BossPhase
{
	Starting,
	Phase1,
	Phase2,
	Phase3,
	Death
};

enum class AttackState
{
	Idle,
	Warning,
	Attacking,
	Returning,
	PhaseTransition
};

class Boss
{
public:
	Boss();
	void loadAssets();
	void spawn(float startX, float startY, const GameSettings &settings);
	void update(sf::Time dt, sf::Vector2f playerPos, float windowWidth, float Windowlength, const GameSettings &settings);
	void draw(sf::RenderWindow &window, const GameSettings &settings);
	void stopSound();

	const std::vector<CollisionCircle> &getHandleHitboxes();
	const std::vector<CollisionCircle> &getBrushHitboxes();
	sf::FloatRect getBounds();
	void loseHealth();
	int getHealth() const;
	bool isAlive() const;
	BossPhase getPhase();
	AttackState getAttackState() const { return m_attackState; }
	bool getIsBossCentered() const { return isBossCentered; }

private:
	sf::Texture m_bossTexture;
	sf::Sprite m_boss;
	sf::Texture m_bossWarningTexture;
	sf::Sprite m_bossWarning;
	std::vector<CollisionCircle>
			m_handleHitboxes;
	std::vector<CollisionCircle> m_brushHitboxes;
	int m_maxHP;
	int m_currentHP;
	float m_speed;
	bool m_active;
	bool isBossCentered;
	bool isDescentFinished;
	sf::SoundBuffer deathSoundBuffer;
	sf::Sound m_deathSound;
	sf::SoundBuffer attackSoundBuffer;
	sf::Sound m_attackSound;
	sf::SoundBuffer warningSoundBuffer;
	sf::Sound m_warningSound;
	BossPhase m_bossPhase;
	AttackState m_attackState;
	float m_phaseTimer;
	float transitionTimer;
	float m_spawnColorTimer;
	sf::Vector2f m_targetPos;
	sf::Vector2f m_attackDirection;
	float m_warningScaleX;
	float m_warningScaleY;
	float m_currentIdleDuration;
	float m_attackTimer;
	float attackSpeed;
	float spinSpeed;
	float m_warningDuration;
	float m_phase1Duration;
	float m_phase2Duration;
	float m_phase3Duration;
	float m_currentBossScaleX;
	float newAlpha;
	float m_antiCheatTimer;
	bool m_isSpinning360;
	float m_spinProgress;
	int m_bounceCount;
	sf::Vector2f m_dashVelocity;
	float m_currentDashSpeed;
};