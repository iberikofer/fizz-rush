#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include "Settings.hpp"

enum class BossState
{
	Starting,
	Phase1,
	Phase2,
	Phase3,
	Death
};

class Boss
{
public:
	Boss();
	void loadAssets();
	void spawn(float startX, float startY, const GameSettings &settings);
	void update(sf::Time dt, sf::Vector2f playerPos, float windowWidth, float Windowlength);
	void draw(sf::RenderWindow &window, const GameSettings &settings);

	const std::vector<CollisionCircle> &getHandleHitboxes();
	sf::FloatRect getBrushHitbox();
	sf::FloatRect getBounds();
	void loseHealth();
	int getHealth() const;
	bool isAlive() const;

private:
	sf::Texture m_bossTexture;
	sf::Sprite m_boss;
	sf::Texture m_bossWarningTexture;
	sf::Sprite m_bossWarning;
	std::vector<CollisionCircle> m_handleHitboxes;
	int m_maxHP;
	int m_currentHP;
	float m_speed;
	bool m_active;
	sf::SoundBuffer deathSoundBuffer;
	sf::Sound deathSound;
	sf::SoundBuffer attackSoundBuffer;
	sf::Sound attackSound;
	sf::SoundBuffer warningSoundBuffer;
	sf::Sound warningSound;
	BossState m_bossPhase;
	float m_phaseTimer;
	float m_spawnColorTimer;
	sf::Vector2f m_targetPos;
	float m_warningScale;
	float m_currentIdleDuration;

	enum class AttackState
	{
		Idle,
		Warning,
		Attacking,
		Returning
	};

	AttackState m_attackState;

	float m_attackTimer;
	float m_warningDuration;
	float m_attacksCooldown;

	float m_safeZoneY;
	sf::Vector2f m_attackDir;
	sf::Vector2f m_startPos;
};