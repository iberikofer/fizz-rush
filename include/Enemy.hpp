#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Settings.hpp"

class Enemy
{
public:
	Enemy(float enemyStartX, float enemyStartY);
	void loadAssets();
	void update(sf::Time dt, float winWidth, float winHeight, float m_machineLeftWall, float m_machineRightWall);
	void setDifficultyParams(float enemySpeed, float enemyScale);
	void reset(float m_startPosX, float m_sstartPosY, float m_difficultyEnemySpeed);
	void draw(sf::RenderWindow &window, const GameSettings &gameSettings);
	std::vector<CollisionCircle> getHitboxes();

private:
	sf::Texture m_enemyTexture;
	sf::Sprite m_enemySprite;
	float m_enemySpeed;
};