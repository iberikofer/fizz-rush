#include <iostream>
#include <cmath>
#include <vector>
#include "Player.hpp"


Player::Player(float winWidth, float winHeight) : m_player(m_canTexture),
																									m_aura(m_auraTexture1),
																									m_arrow(m_arrowTexture),
																									m_HealthSprite(m_fullHeartTexture),
																									m_wallSound(m_wallSoundBuffer),
																									m_hitSound(m_hitSoundBuffer),
																									m_rollSound(m_rollSoundBuffer)
{
	m_waveTotalTime = 0.0f;
	m_playerWidth = 0.045f;
	m_playerHeight = 0.057f;
	m_playerMoved = false;
	m_machineLeftWall = 70.0f;
	m_machineRightWall = 130.0f;
	m_machineUpWall = 19.0f;
	m_HP = 3;
	m_isInvincible = false;
	m_invincibilityTimer = 0.0f;
	m_showAura = false;
	m_wallSound.setVolume(20.0f);
	m_rollSound.setLooping(true);
	m_rollSound.setVolume(45.0f);
	m_wallSoundTimer = 0.5f;
	m_wallSoundInterval = 0.2f;
	m_rollAnimTimer = 0.0f;
	m_isRollTexture = false;
	m_velocity = {0.0f, 0.0f};
	m_acceleration = 2000.0f;
	m_friction = 4.0f;
}

	//* === ASSETS LOADING ===
void Player::loadAssets(float startX, float startY)
{
	if (!m_auraTexture1.loadFromFile("assets/images/aura1.png"))
		std::cerr << "Sprite error!" << std::endl;
	if (!m_auraTexture2.loadFromFile("assets/images/aura2.png"))
		std::cerr << "Sprite error!" << std::endl;
	if (!m_canRollTexture.loadFromFile("assets/images/can_rolled.png"))
		std::cerr << "Sprite error!" << std::endl;
	if (!m_canTexture.loadFromFile("assets/images/can.png"))
		std::cerr << "Sprite error!" << std::endl;
	if (!m_canRollSlowTexture.loadFromFile("assets/images/can_rolled_slowed.png"))
		std::cerr << "Sprite error!" << std::endl;
	if (!m_canSlowtexture.loadFromFile("assets/images/can_slowed.png"))
		std::cerr << "Sprite error!" << std::endl;
	if (!m_fullHeartTexture.loadFromFile("assets/images/heart_full.png"))
		std::cerr << "Full Heart texture error!" << std::endl;
	if (!m_emptyHeartTexture.loadFromFile("assets/images/heart_empty.png"))
		std::cerr << "Empty Heart texture error!" << std::endl;
	if (!m_emptyHeartTexture2.loadFromFile("assets/images/heart_empty2.png"))
		std::cerr << "Empty Heart2 texture error!" << std::endl;
	if (!m_arrowTexture.loadFromFile("assets/images/arrow.png"))
		std::cerr << "Sprite error!" << std::endl;
	if (!m_hitSoundBuffer.loadFromFile("assets/sound/collision.ogg"))
		std::cerr << "Hit sound error!" << std::endl;
	if (!m_wallSoundBuffer.loadFromFile("assets/sound/wall.ogg"))
		std::cerr << "Hit sound error!" << std::endl;
	if (!m_rollSoundBuffer.loadFromFile("assets/sound/roll.ogg"))
		std::cerr << "Roll sound error!" << std::endl;

	m_player.setPosition({startX, startY});

	m_player.setTexture(m_canTexture, true);
	sf::FloatRect m_playerLocalBounds = m_player.getLocalBounds();
	m_player.setOrigin({m_playerLocalBounds.size.x / 2.0f, m_playerLocalBounds.size.y / 2.0f});
	m_player.setScale({m_playerWidth, m_playerHeight});

	m_arrow.setPosition({startX, startY - 25.0f});
	m_arrow.setTexture(m_arrowTexture, true);
	sf::FloatRect arrowBounds = m_arrow.getLocalBounds();
	m_arrow.setOrigin({arrowBounds.size.x / 2.0f, arrowBounds.size.y});
	m_arrow.setScale({0.25f, 0.35f});

	m_aura.setPosition({startX, startY});

	m_aura.setTexture(m_auraTexture1, true);
	sf::FloatRect m_auraLocalBounds = m_aura.getLocalBounds();
	m_aura.setOrigin({m_auraLocalBounds.size.x / 2.0f, m_auraLocalBounds.size.y / 2.0f});
	m_aura.setScale({m_playerWidth * 4.0f, m_playerHeight * 4.0f});

	m_HealthSprite.setScale({0.15f, 0.15f});
}

	//* === UPDATE LOGIC ===
void Player::update(sf::Time dt, float winWidth, float winHeight, float m_machineLeftWall, float m_machineRightWall, float m_machineUpWall, float m_WallPushBack, bool playWallSound, int currentEpisode)
{
	m_aura.setPosition(m_player.getPosition());
	m_aura.setRotation(m_player.getRotation());

	float inputX = 0.0f;
	float inputY = 0.0f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
		inputX -= 1.0f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
		inputX += 1.0f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
		inputY -= 1.0f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
		inputY += 1.0f;

	if (inputX == 0.0f && inputY == 0.0f && sf::Joystick::isConnected(0))
	{
		float joyX = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X);
		float joyY = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y);

		float deadzone = 20.0f;

		if (abs(joyX) > deadzone)
			inputX = joyX / 100.0f;

		if (abs(joyY) > deadzone)
			inputY = joyY / 100.0f;
	}

	if (currentEpisode == 0)
	{
		float m_targetRotation = 0.0f;

		if (inputX != 0.0f || inputY != 0.0f)
		{
			m_playerMoved = true;

			if (std::abs(inputX) > std::abs(inputY))
			{
				m_targetRotation = 0.0f;
			}
			else
			{
				float angleRad = std::atan2(inputY, inputX);
				float angleDeg = angleRad * 180.0f / 3.14159f;
				m_targetRotation = angleDeg + 90.0f;
			}

			float length = std::sqrt(inputX * inputX + inputY * inputY);
			if (length > 1.0f)
			{
				inputX /= length;
				inputY /= length;
			}

			m_player.move({inputX * m_speed * dt.asSeconds(), inputY * m_speed * dt.asSeconds()});
		}

		float currentAngle = m_player.getRotation().asDegrees();
		float diff = m_targetRotation - currentAngle;

		while (diff < -180.0f)
			diff += 360.0f;
		while (diff > 180.0f)
			diff -= 360.0f;

		float rotationSpeed = 900.0f * dt.asSeconds();

		if (std::abs(diff) < rotationSpeed)
			m_player.setRotation(sf::degrees(m_targetRotation));
		else
			m_player.setRotation(sf::degrees(currentAngle + (diff > 0 ? rotationSpeed : -rotationSpeed)));
	}
	else if (currentEpisode == 1)
	{
		m_playerMoved = (inputX != 0.0f || inputY != 0.0f);
		bool isOnCarpet = (m_carpetMultiplier == 1.0f);

		if (m_playerMoved)
		{
			float length = std::sqrt(inputX * inputX + inputY * inputY);
			if (length > 1.0f)
			{
				inputX /= length;
				inputY /= length;
			}
			m_player.move({inputX * m_speed * dt.asSeconds(), inputY * m_speed * dt.asSeconds()});

			float angleRad = std::atan2(inputY, inputX);
			float angleDeg = angleRad * 180.0f / 3.14159f;

			float targetRotation = angleDeg;

			float currentAngle = m_player.getRotation().asDegrees();
			float diff = targetRotation - currentAngle;

			while (diff < -180.0f)
				diff += 360.0f;
			while (diff > 180.0f)
				diff -= 360.0f;

			float rotationSpeed = 900.0f * dt.asSeconds();

			if (std::abs(diff) < rotationSpeed)
				m_player.setRotation(sf::degrees(targetRotation));
			else
				m_player.setRotation(sf::degrees(currentAngle + (diff > 0 ? rotationSpeed : -rotationSpeed)));

			m_rollAnimTimer += dt.asSeconds();
			if (m_rollAnimTimer > 0.1f)
			{
				m_rollAnimTimer = 0.0f;
				m_isRollTexture = !m_isRollTexture;

				if (isOnCarpet)
				{
					m_isRollTexture ? m_player.setTexture(m_canRollTexture, true) : m_player.setTexture(m_canTexture, true);
				}
				else
				{
					m_isRollTexture ? m_player.setTexture(m_canRollSlowTexture, true) : m_player.setTexture(m_canSlowtexture, true);
				}
			}
		}
		else
		{
			float currentAngle = m_player.getRotation().asDegrees();
			if (std::abs(currentAngle) > 1.0f)
			{
				float diff = 0.0f - currentAngle;
				while (diff < -180.0f)
					diff += 360.0f;
				while (diff > 180.0f)
					diff -= 360.0f;
				m_player.rotate(sf::degrees(diff * 5.0f * dt.asSeconds()));
			}

			m_player.setTexture(isOnCarpet ? m_canTexture : m_canSlowtexture, true);
			m_isRollTexture = false;
		}

		m_velocity = {0.0f, 0.0f};

		if (m_playerMoved)
		{
			if (m_rollSound.getStatus() != sf::Sound::Status::Playing)
				m_rollSound.play();
		}
		else
		{
			m_rollSound.stop();
		}
	}
	else if (currentEpisode == 2)
	{
		bool isMovingInput = (inputX != 0.0f || inputY != 0.0f);
		m_playerMoved = isMovingInput;
		m_player.setTexture(m_canTexture, true);
		m_player.setColor(sf::Color::White);

		if (isMovingInput)
		{
			float length = std::sqrt(inputX * inputX + inputY * inputY);
			if (length > 1.0f)
			{
				inputX /= length;
				inputY /= length;
			}

			m_velocity.x += inputX * m_acceleration * dt.asSeconds();
			m_velocity.y += inputY * m_acceleration * dt.asSeconds();
		}

		m_velocity -= m_velocity * m_friction * dt.asSeconds();

		float currentSpeed = std::sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);
		if (currentSpeed > m_speed)
		{
			float scale = m_speed / currentSpeed;
			m_velocity *= scale;
			currentSpeed = m_speed;
		}

		m_player.move(m_velocity * dt.asSeconds());

		float spinSpeed = 720.0f;
		if (m_velocity.x < 0)
			spinSpeed = -720.0f;
		m_player.rotate(sf::degrees(spinSpeed * dt.asSeconds()));

		if (currentSpeed > 50.0f)
		{
			if (m_rollSound.getStatus() != sf::Sound::Status::Playing)
				m_rollSound.play();
		}
		else
		{
			m_rollSound.stop();
		}
	}

	m_wallSoundTimer += dt.asSeconds();
	checkWorldCollision(winWidth, winHeight, m_machineLeftWall, m_machineRightWall, m_machineUpWall, m_WallPushBack, playWallSound);

	if (m_isInvincible)
	{
		m_showAura = true;
		m_invincibilityTimer += dt.asSeconds();

		if (static_cast<int>(m_invincibilityTimer * 10.0f) % 2 == 0)
			m_player.setColor(sf::Color(255, 255, 255, 150));
		else
			m_player.setColor(sf::Color(255, 255, 255, 255));

		float m_wave = sin(m_invincibilityTimer * 20.0f);
		float m_baseAuraScale = m_playerWidth * 4.0f;
		float m_wobble = m_wave * 0.005f;
		m_aura.setScale({m_baseAuraScale + m_wobble, m_baseAuraScale + m_wobble});

		int auraFrame = static_cast<int>(m_invincibilityTimer * 10.0f);
		if (auraFrame % 2 == 0)
			m_aura.setTexture(m_auraTexture1);
		else
			m_aura.setTexture(m_auraTexture2);

		if (m_invincibilityTimer >= m_maxInvincibilityTime)
		{
			m_isInvincible = false;
			m_showAura = false;
			m_invincibilityTimer = 0.0f;
			m_invincibilityMultiplier = 1.0f;
			m_player.setColor(sf::Color(255, 255, 255, 255));
		}
	}

	if (m_playerMoved == false)
	{
		m_waveTotalTime += dt.asSeconds();
		float bobbing = sin(m_waveTotalTime * 15.0f) * 10.0f;
		m_arrow.setPosition({m_player.getPosition().x, m_player.getPosition().y - 70.0f + bobbing});
	}
	else
	{
		m_arrow.setColor(sf::Color::Transparent);
	}
}

void Player::stopSound()
{
	m_hitSound.stop();
	m_wallSound.stop();
	m_rollSound.stop();
}

void Player::checkWorldCollision(float winWidth, float winHeight, float leftWall, float rightWall, float topWall, float m_WallPushBack, bool playWallSound)
{
	//* === WALLS ===
	sf::FloatRect m_spriteBounds = m_player.getGlobalBounds();
	float m_spriteHalfWidth = m_spriteBounds.size.x / 2.0f;
	float m_spriteHalfHeight = m_spriteBounds.size.y / 2.0f;

	//* LEFT
	if (m_player.getPosition().x < leftWall + m_spriteHalfWidth)
	{
		m_player.setPosition({leftWall + m_spriteHalfWidth + m_WallPushBack, m_player.getPosition().y});
		if (m_wallSoundTimer >= m_wallSoundInterval && playWallSound)
		{
			m_wallSound.play();
			m_wallSoundTimer = 0.0f;
		}
	}
	//* UP
	if (m_player.getPosition().y < topWall + m_spriteHalfHeight)
	{
		m_player.setPosition({m_player.getPosition().x, topWall + m_spriteHalfHeight + m_WallPushBack});
		if (m_wallSoundTimer >= m_wallSoundInterval && playWallSound)
		{
			m_wallSound.play();
			m_wallSoundTimer = 0.0f;
		}
	}
	//* RIGHT
	if (m_player.getPosition().x > winWidth - rightWall - m_spriteHalfWidth)
	{
		m_player.setPosition({winWidth - rightWall - m_spriteHalfWidth - m_WallPushBack, m_player.getPosition().y});
		if (m_wallSoundTimer >= m_wallSoundInterval && playWallSound)
		{
			m_wallSound.play();
			m_wallSoundTimer = 0.0f;
		}
	}
	//* BOTTOM
	if (m_player.getPosition().y + m_spriteHalfHeight > winHeight)
	{
		m_player.setPosition({m_player.getPosition().x, winHeight - m_spriteHalfHeight});
		if (m_wallSoundTimer >= m_wallSoundInterval && playWallSound)
		{
			m_wallSoundTimer = 0.0f;
		}
	}
}

bool Player::hasPlayerMoved()
{
	return m_playerMoved;
}

sf::FloatRect Player::getPlayerHitbox()
{
	return m_player.getGlobalBounds();
}

void Player::gainHealth(int amount)
{
	if (m_HP < m_maxHP)
	{
		m_HP += amount;
		if (m_HP > m_maxHP)
			m_HP = m_maxHP;
	}
}

int Player::getHealth()
{
	return m_HP;
}

int Player::loseHealth()
{
	if (!m_isInvincible)
	{
		m_showAura = true;
		--m_HP;
		if (m_HP <= 0)
		{
			m_HP = 0;
			m_hitSound.stop();
			m_wallSound.stop();
		}
		else
		{
			m_hitSound.play();
		}
		if (m_maxInvincibilityTime != 0.0f)
		{
			m_isInvincible = true;
			m_invincibilityTimer = 0.0f;
		}
	}
	return m_HP;
}

void Player::resetGame(float m_startPosX, float m_startPosY, int maxHP, float difficultySpeed, float invincibilityDuration, GameDifficulty difficulty)
{
	m_player.setPosition({m_startPosX, m_startPosY});
	m_maxHP = maxHP;
	m_HP = maxHP;
	m_isInvincible = false;
	m_invincibilityTimer = 0.0f;
	m_showAura = false;
	m_playerMoved = false;
	m_basicSpeed = difficultySpeed;
	m_speed = m_basicSpeed;
	m_carpetMultiplier = 1.0f;
	m_invincibilityMultiplier = 1.0f;
	m_player.setRotation(sf::degrees(0));

	m_maxInvincibilityTime = invincibilityDuration;

	m_difficulty = difficulty;

	m_player.setColor(sf::Color::White);
	m_player.setScale({m_playerWidth, m_playerHeight});
	m_arrow.setColor(sf::Color::White);

	m_acceleration = 1500.0f;
	m_friction = 2.0f;
}

sf::Vector2f Player::getPosition()
{
	return m_player.getPosition();
}

void Player::startNextEpisode(float startX, float startY)
{
	m_player.setPosition({startX, startY});
	m_player.setRotation(sf::degrees(0));
	m_carpetMultiplier = 1.0f;
	m_invincibilityMultiplier = 1.0f;

	m_playerMoved = false;

	m_aura.setPosition({startX, startY});
	m_arrow.setPosition({startX, startY - 70.0f});
	m_arrow.setColor(sf::Color::White);
}

	//* === DRAW LOGIC ===
void Player::draw(sf::RenderWindow &window, const GameSettings &gameSettings)
{
	if (m_showAura)
	{
		window.draw(m_aura);
	}

	window.draw(m_player);

	if (!m_playerMoved)
	{
		window.draw(m_arrow);
	}

	for (int i = 0; i < m_maxHP; i++)
	{
		if (i < m_HP)
		{
			m_HealthSprite.setTexture(m_fullHeartTexture, true);
		}
		else
		{
			if (m_isInvincible && sin(m_invincibilityTimer * 20.0f) > 0)
			{
				m_HealthSprite.setTexture(m_emptyHeartTexture2, true);
			}
			else
			{
				m_HealthSprite.setTexture(m_emptyHeartTexture, true);
			}
		}
		m_HealthSprite.setPosition({15.0f, 25.0f + i * 75.0f});
		window.draw(m_HealthSprite);
	}

	if (gameSettings.showHitbox)
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
}

const std::vector<CollisionCircle> &Player::getHitboxes()
{
	m_hitboxes.clear();

	sf::FloatRect localBounds = m_player.getLocalBounds();

	float realWidth = localBounds.size.x * m_player.getScale().x;

	float easyRadius = (realWidth / 2.0f) * 0.75f;
	float normalRadius = (realWidth / 2.0f) * 0.85f;
	float hardRadius = (realWidth / 2.0f) * 0.9f;

	sf::Transform trans = m_player.getTransform();

	float localX = localBounds.size.x / 2.0f;

	if (m_difficulty == GameDifficulty::Easy)
	{
		sf::Vector2f topPoint = trans.transformPoint({localX, localBounds.size.y * 0.3f});
		sf::Vector2f botPoint = trans.transformPoint({localX, localBounds.size.y * 0.7f});

		m_hitboxes.push_back({topPoint, easyRadius});
		m_hitboxes.push_back({botPoint, easyRadius});
	}
	else if (m_difficulty == GameDifficulty::Normal)
	{

		sf::Vector2f topPoint = trans.transformPoint({localX, localBounds.size.y * 0.25f});
		sf::Vector2f botPoint = trans.transformPoint({localX, localBounds.size.y * 0.75f});

		m_hitboxes.push_back({topPoint, normalRadius});
		m_hitboxes.push_back({botPoint, normalRadius});
	}
	else
	{
		sf::Vector2f topPoint = trans.transformPoint({localX, localBounds.size.y * 0.15f});
		sf::Vector2f midPoint = trans.transformPoint({localX, localBounds.size.y * 0.5f});
		sf::Vector2f botPoint = trans.transformPoint({localX, localBounds.size.y * 0.85f});

		m_hitboxes.push_back({topPoint, hardRadius});
		m_hitboxes.push_back({midPoint, hardRadius});
		m_hitboxes.push_back({botPoint, hardRadius});
	}

	return m_hitboxes;
}

void Player::updateCarpetSpeed(bool isOnCarpet)
{
	if (isOnCarpet)
		m_carpetMultiplier = 1.0f;
	else
	{
		m_carpetMultiplier = (m_difficulty == GameDifficulty::Hard) ? 1.0f : 0.75f;
	}
}