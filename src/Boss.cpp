#include "Boss.hpp"
#include <iostream>
#include <cmath>

using namespace sf;
using namespace std;

Boss::Boss() : m_boss(m_bossTexture),
							 m_bossWarning(m_bossWarningTexture),
							 deathSound(deathSoundBuffer),
							 attackSound(attackSoundBuffer),
							 warningSound(warningSoundBuffer)
{
	m_active = false;
	m_maxHP = 50;
	m_currentHP = m_maxHP;
	m_speed = 150.0f;
	m_attackState = AttackState::Idle;
	m_currentIdleDuration = 1.5f;
	m_attackTimer = 0.0f;
	m_warningDuration = 0.0f;
	m_warningScale = 1.0f;
	m_attacksCooldown = 0.0f;
	m_safeZoneY = 0.0f;
	attackSound.setVolume(100.0f);
	warningSound.setVolume(65.0f);
}

	//* === ASSETS LOADING ===
void Boss::loadAssets()
{
	if (!m_bossTexture.loadFromFile("assets/images/boss_1.png"))
		cerr << "Boss texture error!" << endl;
	if (!m_bossWarningTexture.loadFromFile("assets/images/boss_warning.png"))
		cerr << "Boss warning texture error!" << endl;
	if (!attackSoundBuffer.loadFromFile("assets/sound/boss_attack1.ogg"))
		cerr << "Boss attack sound error!" << endl;
	if (!warningSoundBuffer.loadFromFile("assets/sound/boss_warning.ogg"))
		cerr << "Boss attack sound error!" << endl;
	if (!deathSoundBuffer.loadFromFile("assets/sound/boss_death.ogg"))
		cerr << "Boss death sound error!" << endl;

	m_boss.setTexture(m_bossTexture, true);
	m_boss.setScale({1.0f, 1.0f});

	FloatRect bounds = m_boss.getLocalBounds();
	m_boss.setOrigin({bounds.size.x / 2, bounds.size.y / 2});
}

	//* === INITIAL SETUP ===
void Boss::spawn(float startX, float startY, const GameSettings &settings)
{
	m_bossPhase = BossState::Starting;
	m_phaseTimer = 0.0f;
	m_spawnColorTimer = 0.0f;
	m_active = true;
	m_boss.setPosition({startX, startY});
	m_targetPos = {startX, 350.0f};

	if (settings.gameDifficulty == GameDifficulty::Easy)
	{
		m_currentHP = 50;
		m_speed = 100.0f;
	}
	else if (settings.gameDifficulty == GameDifficulty::Normal)
	{
		m_currentHP = 100;
		m_speed = 150.0f;
	}
	else if (settings.gameDifficulty == GameDifficulty::Hard)
	{
		m_currentHP = 150;
		m_speed = 200.0f;
	}
}

	//* === UPDATE LOGIC ===
void Boss::update(Time dt, Vector2f playerPos, float windowWidth, float windowHeight)
{
	if (!m_active)
		return;

	m_phaseTimer += dt.asSeconds();
	m_spawnColorTimer += dt.asSeconds();

	if (m_bossPhase != BossState::Starting)
	{
		if (m_phaseTimer < 60.0f)
		{
			m_bossPhase = BossState::Phase1;
		}
		else if (m_phaseTimer < 120.0f)
		{
			m_bossPhase = BossState::Phase2;
		}
		else
		{
			m_active = false;
			return;
		}
	}

	switch (m_bossPhase)
	{
	case BossState::Starting:
	{
		if (static_cast<int>(m_spawnColorTimer * 5.0f) % 2 == 0)
			m_boss.setColor(Color(255, 0, 0));
		else
			m_boss.setColor(Color::White);

		float distance = m_targetPos.y - m_boss.getPosition().y;

		float step = m_speed * dt.asSeconds();

		if (std::abs(distance) < step)
		{
			m_boss.setPosition(m_targetPos);
			m_boss.setColor(Color::White);
			m_bossPhase = BossState::Phase1;
			m_phaseTimer = 0.0f;
		}
		else
		{
			m_boss.move({0.0f, step});
		}
	}
	break;
	case BossState::Phase1:
		switch (m_attackState)
		{
		case AttackState::Idle:
		{
			m_attackTimer += dt.asSeconds();

			m_boss.setColor(Color(Color::White));

			float targetX = playerPos.x;
			float currentX = m_boss.getPosition().x;
			float smoothFactor = 3.0f;

			float newX = currentX + (targetX - currentX) * smoothFactor * dt.asSeconds();

			m_boss.setPosition({newX, 350.0f});

			if (m_attackTimer >= m_currentIdleDuration)
			{
				m_attackTimer = 0.0f;
				m_attackState = AttackState::Warning;
				m_bossWarning.setPosition({newX, -windowHeight + m_bossWarning.getGlobalBounds().size.y + 200.0f});
				warningSound.play();
			}
		}
		break;
		case AttackState::Warning:
		{
			m_attackTimer += dt.asSeconds();
			// (m_bossWarning) logic

			if (m_attackTimer >= 0.5f)
			{
				m_attackTimer = 0.0f;
				m_boss.setColor(Color(255, 0, 0));
				m_attackState = AttackState::Attacking;
				m_currentIdleDuration = 1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0f));
				attackSound.play();
			}
		}
		break;
		case AttackState::Attacking:
		{
			m_boss.setColor(Color(Color(255, 0, 0)));

			float attackSpeed = m_speed * 5.0f;
			m_boss.move({0.0f, attackSpeed * dt.asSeconds()});

			FloatRect bounds = m_boss.getLocalBounds();
			if (m_boss.getPosition().y > windowHeight + bounds.size.y / 2)
			{
				m_boss.setPosition({m_boss.getPosition().x, -600.0f});
				m_attackState = AttackState::Returning;
			}
		}
		break;
		case AttackState::Returning:
		{
			float targetY = 350.0f;
			if (m_boss.getPosition().y < targetY)
			{
				m_boss.move({0.0f, m_speed * dt.asSeconds() * 4.0f});
			}
			else
			{
				m_boss.setPosition({m_boss.getPosition().x, targetY});
				m_attackState = AttackState::Idle;
				m_attackTimer = 0.0f;
				m_currentIdleDuration = 1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 5.5f));
			}
			break;
		}
		}
		break;
	case BossState::Phase2:
		switch (m_attackState)
		{
		case AttackState::Idle:
		{
		}
		break;
		case AttackState::Warning:
		{
		}
		break;
		case AttackState::Attacking:
		{
		}
		break;
		case AttackState::Returning:
		{
		}
		break;
		}
		break;
	case BossState::Phase3:
	case BossState::Death:
		break;
	}
}

	//* === DRAW LOGIC ===
void Boss::draw(RenderWindow &window, const GameSettings &settings)
{
	if (m_active)
	{
		window.draw(m_boss);

		if (settings.debugMode)
		{
			const auto &handle = getHandleHitboxes();
			CircleShape debugCircle;
			debugCircle.setFillColor(Color::Transparent);
			debugCircle.setOutlineColor(Color::Cyan);
			debugCircle.setOutlineThickness(2.0f);

			for (const auto &circle : handle)
			{
				debugCircle.setRadius(circle.radius);
				debugCircle.setOrigin({circle.radius, circle.radius});
				debugCircle.setPosition(circle.center);
				window.draw(debugCircle);
			}

			FloatRect head = getBrushHitbox();
			RectangleShape debugRect;
			debugRect.setPosition({head.position.x, head.position.y});
			debugRect.setSize(head.size);
			debugRect.setFillColor(Color::Transparent);
			debugRect.setOutlineColor(Color::Magenta);
			debugRect.setOutlineThickness(2.0f);
			window.draw(debugRect);
		}
	}
}

FloatRect Boss::getBounds()
{
	return m_boss.getGlobalBounds();
}

void Boss::loseHealth()
{
	m_currentHP--;
	if (m_currentHP <= 0)
	{
		m_active = false;
		deathSound.play();
	}
}

int Boss::getHealth() const
{
	return m_currentHP;
}

bool Boss::isAlive() const
{
	return m_active;
}

const std::vector<CollisionCircle> &Boss::getHandleHitboxes()
{
	m_handleHitboxes.clear();

	Transform t = m_boss.getTransform();
	FloatRect bounds = m_boss.getLocalBounds();

	Vector2f localStart = {bounds.size.x * 0.03f, bounds.size.y * 0.017f};

	Vector2f localEnd = {bounds.size.x * 0.68f, bounds.size.y * 0.77f};

	int circlesCount = 10;
	float radius = 10.0f;

	for (int i = 0; i <= circlesCount; i++)
	{
		float t_val = static_cast<float>(i) / circlesCount;

		float localX = localStart.x + (localEnd.x - localStart.x) * t_val;
		float localY = localStart.y + (localEnd.y - localStart.y) * t_val;

		Vector2f globalPos = t.transformPoint({localX, localY});

		m_handleHitboxes.push_back({globalPos, radius});
	}

	return m_handleHitboxes;
}

sf::FloatRect Boss::getBrushHitbox()
{
	Transform t = m_boss.getTransform();
	FloatRect bounds = m_boss.getLocalBounds();

	float brushWidth = bounds.size.x * 0.5295f;
	float brushHeight = bounds.size.y * 0.205f;

	float offsetX = bounds.size.x * 0.477f;
	float offsetY = bounds.size.y * 0.795f;

	FloatRect localRect(
			{offsetX, offsetY},
			{brushWidth, brushHeight});

	return t.transformRect(localRect);
}