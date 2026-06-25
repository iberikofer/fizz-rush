#include <iostream>
#include <cmath>
#include <cstdlib>
#include "Enemy.hpp"
using namespace std;
using namespace sf;

Enemy::Enemy(float enemyStartX, float enemyStartY) : m_enemySprite(m_enemyTexture)
{
	m_enemySprite.setPosition({enemyStartX, enemyStartY});
	m_enemySprite.setScale({0.053f, 0.053f});
	m_enemySpeed = 1400.0f;
}

void Enemy::loadAssets()
{
	if (!m_enemyTexture.loadFromFile("assets/images/enemy_1.png"))
		cerr << "Enemy sprite error!" << endl;
	else
		m_enemySprite.setTexture(m_enemyTexture, true);
}

void Enemy::update(Time dt, float winWidth, float winHeight, float m_machineLeftWall, float m_machineRightWall)
{
	if (getHitbox().position.y > winHeight)
	{
		float m_maxSpawnX = winWidth - m_machineRightWall - m_enemySprite.getGlobalBounds().size.x;
		int m_enemySpawnRange = static_cast<int>(m_maxSpawnX - m_machineLeftWall);

		if (m_enemySpawnRange > 0)
		{
			float randomX = m_machineLeftWall + (rand() % m_enemySpawnRange);
			m_enemySprite.setPosition({randomX, -200.0f});
		}
		else
			m_enemySprite.setPosition({m_machineLeftWall, -200.0f});
	}

	m_enemySprite.move({0.0f, m_enemySpeed * dt.asSeconds()});
}

void Enemy::setDifficultyParams(float enemySpeed, float enemyScale)
{
}

void Enemy::reset(float m_startPosX, float m_sstartPosY, float m_difficultyEnemySpeed)
{
	m_enemySprite.setPosition({m_startPosX, m_sstartPosY});
	m_enemySpeed = m_difficultyEnemySpeed;
}

void Enemy::draw(RenderWindow &window, const GameSettings &gameSettings)
{
	window.draw(m_enemySprite);

	if (gameSettings.debugMode)
	{
		FloatRect enemyHitbox = getHitbox();
		RectangleShape enemyDebugRect({enemyHitbox.size.x, enemyHitbox.size.y});
		enemyDebugRect.setPosition({enemyHitbox.position.x, enemyHitbox.position.y});
		enemyDebugRect.setFillColor(Color::Transparent);
		enemyDebugRect.setOutlineColor(Color::Cyan);
		enemyDebugRect.setOutlineThickness(2.0f);
		window.draw(enemyDebugRect);
	}
}

FloatRect Enemy::getHitbox()
{
	return m_enemySprite.getGlobalBounds();
}
