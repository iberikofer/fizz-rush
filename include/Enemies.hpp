#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include "Settings.hpp"

enum class Type
{
	Can,
	Bar,
	Bar2,
	Box,
	HandStraight,
	HandChaser
};

struct CookieParticle
{
	CookieParticle(const sf::Texture &tex) : sprite(tex) {}

	sf::Sprite sprite;
	sf::Vector2f velocity;
	bool active = false;
};

class Enemy
{
public:
	Enemy(const sf::Texture &texture, float enemyStartX, float enemyStartY);
	void spawn(float m_startPosX, float m_startPosY, float m_difficultyEnemySpeed, const sf::Texture &texture, Type type, sf::Vector2f playerPos, const sf::Texture *particleTexture = nullptr);
	void update(sf::Time dt, float winWidth, float winHeight, float m_machineLeftWall, float m_machineRightWall, sf::Vector2f playerPos, sf::Sound &shootSound);
	void setDifficultyParams(float enemySpeed, float enemyScale);
	void draw(sf::RenderWindow &window, const GameSettings &gameSettings);
	Type getType() const;
	const std::vector<CollisionCircle> &getHitboxes();
	const std::vector<CookieParticle> &getCookies();
	sf::FloatRect getBounds();
	sf::Vector2f getPosition();
	bool checkCollision(const CollisionCircle &circle) const;

private:
	sf::Sprite m_enemySprite;
	float m_enemySpeed;
	std::vector<CollisionCircle> m_hitboxes;
	bool m_hasShot;
	float m_shootHeight;
	std::vector<CookieParticle> m_cookies;
	const sf::Texture *m_particleTexturePtr;
	Type m_type;
	sf::Vector2f m_velocity;
	float m_chaseTimer;
};