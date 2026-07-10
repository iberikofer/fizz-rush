#include <iostream>
#include <cmath>
#include <cstdlib>
#include <SFML/Audio.hpp>
#include "Enemies.hpp"
using namespace std;
using namespace sf;

Vector2f normalizeVector(Vector2f source)
{
	float length = sqrt((source.x * source.x) + (source.y * source.y));
	if (length != 0)
		return Vector2f(source.x / length, source.y / length);
	else
		return source;
}

Enemy::Enemy(float enemyStartX, float enemyStartY) : m_enemySprite(m_enemyTexture),
																										 m_type(Type::Can)
{
	m_enemySprite.setPosition({enemyStartX, enemyStartY});
	m_enemySprite.setScale({0.053f, 0.053f});
	m_enemySpeed = 1000.0f;
}

	//* === ASSETS LOADING ===
void Enemy::loadAssets()
{
	if (!m_enemyTexture.loadFromFile("assets/images/enemy_can1.png"))
		cerr << "Enemy sprite error!" << endl;
	else
		m_enemySprite.setTexture(m_enemyTexture, true);
}

	//* === INITIAL SETUP ===
void Enemy::spawn(float m_startPosX, float m_startPosY, float m_difficultyEnemySpeed, const Texture &texture, Type type, Vector2f playerPos, const sf::Texture *particleTexture)
{
	m_type = type;
	m_enemySpeed = m_difficultyEnemySpeed;

	m_enemySprite.setPosition({m_startPosX, m_startPosY});
	m_enemySprite.setTexture(texture, true);

	if (m_type == Type::Box)
	{
		m_hasShot = false;
		m_shootHeight = 100.0f + (rand() % 100);

		m_particleTexturePtr = particleTexture;

		m_cookies.clear();
		m_enemySprite.setScale({0.25f, 0.25f});
	}
	else
	{
		m_enemySprite.setScale({0.2f, 0.2f});
	}

	FloatRect bounds = m_enemySprite.getLocalBounds();
	m_enemySprite.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});

	m_chaseTimer = 5.0f;

	if (m_type == Type::HandStraight)
	{
		Vector2f direction = playerPos - Vector2f(m_startPosX, m_startPosY);
		m_velocity = normalizeVector(direction) * m_enemySpeed;

		float angle = atan2(direction.y, direction.x) * 180 / 3.14159f;
		m_enemySprite.setRotation(degrees(angle));
	}
	else if (m_type == Type::HandChaser)
	{
		Vector2f direction = playerPos - Vector2f(m_startPosX, m_startPosY);
		m_velocity = normalizeVector(direction) * m_enemySpeed;
	}
	else
	{
		m_velocity = {0.0f, m_enemySpeed};
	}
}

	//* === UPDATE LOGIC ===
void Enemy::update(Time dt, float winWidth, float winHeight, float m_machineLeftWall, float m_machineRightWall, Vector2f playerPos, Sound &cookieSound)
{
	if (m_type == Type::HandStraight)
	{
		m_enemySprite.move(m_velocity * dt.asSeconds());
	}
	else if (m_type == Type::HandChaser)
	{
		m_chaseTimer -= dt.asSeconds();
		if (m_chaseTimer > 0)
		{
			Vector2f direction = playerPos - m_enemySprite.getPosition();
			m_velocity = normalizeVector(direction) * m_enemySpeed;
			float angle = atan2(direction.y, direction.x) * 180 / 3.14159f;
			m_enemySprite.setRotation(degrees(angle));
		}
		m_enemySprite.move(m_velocity * dt.asSeconds());
	}
	else
	{
		m_enemySprite.move({0.0f, m_enemySpeed * dt.asSeconds()});
	}

	if (m_type == Type::Box)
	{
		if (!m_hasShot && m_enemySprite.getPosition().y >= m_shootHeight)
		{
			m_hasShot = true;

			cookieSound.play();
			for (int i = 0; i < 4; i++)
			{
				if (m_particleTexturePtr)
				{
					CookieParticle cookie(*m_particleTexturePtr);
					cookie.sprite.setTexture(*m_particleTexturePtr, true);
					cookie.sprite.setPosition(m_enemySprite.getPosition());
					cookie.sprite.setScale({0.15f, 0.15f});

					FloatRect b = cookie.sprite.getLocalBounds();
					cookie.sprite.setOrigin({b.size.x / 2, b.size.y / 2});

					cookie.active = true;

					bool isLeft = (m_enemySprite.getPosition().x < winWidth / 2);

					float velX = isLeft ? (250.0f + rand() % 150) : -(250.0f + rand() % 150);
					float velY = -300.0f - (rand() % 200);

					velX += i * 40.0f * (isLeft ? 1 : -1);

					cookie.velocity = {velX, velY};
					m_cookies.push_back(cookie);
				}
			}
		}

		float gravity = 1200.0f;
		for (auto &cookie : m_cookies)
		{
			if (cookie.active)
			{
				cookie.velocity.y += gravity * dt.asSeconds();
				cookie.sprite.move(cookie.velocity * dt.asSeconds());
				cookie.sprite.rotate(degrees(360.0f * dt.asSeconds()));

				if (cookie.sprite.getPosition().y > winHeight + 100)
				{
					cookie.active = false;
				}
			}
		}
	}
}

void Enemy::setDifficultyParams(float enemySpeed, float enemyScale)
{
}

	//* === DRAW LOGIC ===
void Enemy::draw(RenderWindow &window, const GameSettings &gameSettings)
{
	window.draw(m_enemySprite);

	if (m_type == Type::Box)
	{
		for (const auto &cookie : m_cookies)
		{
			if (cookie.active)
				window.draw(cookie.sprite);
			if (gameSettings.debugMode)
			{
				FloatRect b = cookie.sprite.getGlobalBounds();
				RectangleShape r;
				r.setPosition({b.position.x, b.position.y});
				r.setSize({b.size.x, b.size.y});
				r.setFillColor(Color::Transparent);
				r.setOutlineColor(Color::Cyan);
				r.setOutlineThickness(2.0f);
				window.draw(r);
			}
		}
	}

	if (gameSettings.debugMode)
	{
		if (m_type == Type::Can)
		{
			const auto &debugCircles = getHitboxes();
			CircleShape debugShape;
			debugShape.setFillColor(Color::Transparent);
			debugShape.setOutlineColor(Color::Cyan);
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
			FloatRect bounds = getBounds();
			RectangleShape debugRect;

			debugRect.setPosition({bounds.position.x, bounds.position.y});
			debugRect.setSize({bounds.size.x, bounds.size.y});

			debugRect.setFillColor(Color::Transparent);
			debugRect.setOutlineColor(Color::Cyan);
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

const std::vector<CookieParticle> &Enemy::getCookies()
{
	return m_cookies;
}