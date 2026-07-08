#include <iostream>
#include <cmath>
#include <cstdlib>
#include "Enemy.hpp"
using namespace std;
using namespace sf;

Enemy::Enemy(float enemyStartX, float enemyStartY) : m_enemySprite(m_enemyTexture), 
m_type(Type::Can)
{
	m_enemySprite.setPosition({enemyStartX, enemyStartY});
	m_enemySprite.setScale({0.053f, 0.053f});
	m_enemySpeed = 1400.0f;
}

	//* === ASSETS LOADING ===
void Enemy::loadAssets()
{
	if (!m_enemyTexture.loadFromFile("assets/images/enemy_can1.png"))
		cerr << "Enemy sprite error!" << endl;
	else
		m_enemySprite.setTexture(m_enemyTexture, true);
}

	//* === UPDATE LOGIC ===
void Enemy::update(Time dt, float winWidth, float winHeight, float m_machineLeftWall, float m_machineRightWall)
{
	m_enemySprite.move({0.0f, m_enemySpeed * dt.asSeconds()});
}

void Enemy::setDifficultyParams(float enemySpeed, float enemyScale)
{
}

void Enemy::spawn(float m_startPosX, float m_sstartPosY, float m_difficultyEnemySpeed, const sf::Texture &texture, Type type)
{
	m_enemySprite.setPosition({m_startPosX, m_sstartPosY});
	m_enemySpeed = m_difficultyEnemySpeed;

	m_type = type;
	m_enemySprite.setTexture(texture, true);
	m_enemySprite.setScale({0.2f, 0.2f});
}

	//* === DRAW LOGIC ===
void Enemy::draw(RenderWindow &window, const GameSettings &gameSettings)
{
	window.draw(m_enemySprite);

	if (gameSettings.debugMode)
	{
		if (m_type == Type::Can)
		{
			const auto &debugCircles = getHitboxes();
			sf::CircleShape debugShape;
			debugShape.setFillColor(sf::Color::Transparent);
			debugShape.setOutlineColor(sf::Color::Cyan);
			debugShape.setOutlineThickness(3.0f);

			for (const auto &circle : debugCircles)
			{
				debugShape.setRadius(circle.radius);
				debugShape.setOrigin({circle.radius, circle.radius});
				debugShape.setPosition(circle.center);
				window.draw(debugShape);
			}
		}
		else
		{
			sf::FloatRect bounds = getBounds();
			sf::RectangleShape debugRect;

			debugRect.setPosition({bounds.position.x, bounds.position.y});
			debugRect.setSize({bounds.size.x, bounds.size.y});

			debugRect.setFillColor(sf::Color::Transparent);
			debugRect.setOutlineColor(sf::Color::Cyan);
			debugRect.setOutlineThickness(3.0f);

			window.draw(debugRect);
		}
	}
}

const vector<CollisionCircle> &Enemy::getHitboxes()
{
	m_hitboxes.clear();

	FloatRect bounds = m_enemySprite.getGlobalBounds();

	float radius = (bounds.size.x / 2.0f) * 0.8f;

	float centerX = bounds.position.x + bounds.size.x / 2.0f;

	if (m_type == Type::Can)
	{
		m_hitboxes.push_back({{centerX, bounds.position.y + radius}, radius});
		m_hitboxes.push_back({{centerX, bounds.position.y + bounds.size.y - radius}, radius});
	}
	else if (m_type == Type::Bar || m_type == Type::Bar2)
	{
		float barRadius;
		float offset;

		if (m_type == Type::Bar2)
		{
			barRadius = (bounds.size.y / 2.0f) * 0.2f;
			offset = (bounds.size.x / 2.0f) * 0.3f;
		}
		else
		{
			barRadius = (bounds.size.y / 2.0f) * 0.9f;
			offset = bounds.size.x * 0.35f;
		}

		float centerY = bounds.position.y + bounds.size.y / 2.0f;
		float centerX = bounds.position.x + bounds.size.x / 2.0f;

		m_hitboxes.push_back({{centerX - offset, centerY}, barRadius});
		m_hitboxes.push_back({{centerX, centerY}, barRadius});
		m_hitboxes.push_back({{centerX + offset, centerY}, barRadius});
	}

	return m_hitboxes;
}

FloatRect Enemy::getBounds()
{
	return m_enemySprite.getGlobalBounds();
}

Type Enemy::getType() const
{
	return m_type;
}

Vector2f Enemy::getPosition()
{
	return m_enemySprite.getPosition();
}