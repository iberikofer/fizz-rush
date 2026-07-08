#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Settings.hpp"

enum class Type
{
	Can,
	Bar,
	Bar2
};

class Enemy
{
public:
	Enemy(float enemyStartX, float enemyStartY);
	void loadAssets();
	void update(sf::Time dt, float winWidth, float winHeight, float m_machineLeftWall, float m_machineRightWall);
	void setDifficultyParams(float enemySpeed, float enemyScale);
	void spawn(float m_startPosX, float m_sstartPosY, float m_difficultyEnemySpeed, const sf::Texture &texture, Type type);
	void draw(sf::RenderWindow &window, const GameSettings &gameSettings);
	Type getType() const;
	const std::vector<CollisionCircle> &getHitboxes();
	sf::FloatRect getBounds();
	sf::Vector2f getPosition();

private:
	sf::Texture m_enemyTexture;
	sf::Sprite m_enemySprite;
	float m_enemySpeed;
	std::vector<CollisionCircle> m_hitboxes;
	Type m_type;
};