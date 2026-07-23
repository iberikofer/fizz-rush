#include "Enemies.hpp"
#include <SFML/Audio.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace sf;

Vector2f normalizeVector(Vector2f source) {
  float length = sqrt((source.x * source.x) + (source.y * source.y));
  if (length != 0)
    return Vector2f(source.x / length, source.y / length);
  else
    return source;
}

Enemy::Enemy(float enemyStartX, float enemyStartY)
    : m_enemySprite(m_enemyTexture), m_type(Type::Can) {
  m_enemySprite.setPosition({enemyStartX, enemyStartY});
  m_enemySprite.setScale({0.053f, 0.053f});
  m_enemySpeed = 1000.0f;
}

//* === ASSETS LOADING ===
void Enemy::loadAssets() {
  if (!m_enemyTexture.loadFromFile("assets/images/enemy_can1.png"))
    cerr << "Enemy sprite error!" << endl;
  else
    m_enemySprite.setTexture(m_enemyTexture, true);
}

//* === INITIAL SETUP ===
void Enemy::spawn(float m_startPosX, float m_startPosY,
                  float m_difficultyEnemySpeed, const Texture &texture,
                  Type type, Vector2f playerPos,
                  const sf::Texture *particleTexture) {
  m_type = type;
  m_enemySpeed = m_difficultyEnemySpeed;

  m_enemySprite.setPosition({m_startPosX, m_startPosY});
  m_enemySprite.setTexture(texture, true);

  if (m_type == Type::Box) {
    m_hasShot = false;
    m_shootHeight = -5.0f + (rand() % 150);

    m_particleTexturePtr = particleTexture;

    m_cookies.clear();
    m_enemySprite.setScale({0.25f, 0.25f});
  } else {
    m_enemySprite.setScale({0.2f, 0.2f});
  }

  FloatRect bounds = m_enemySprite.getLocalBounds();
  m_enemySprite.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});

  m_chaseTimer = 5.0f;

  if (m_type == Type::HandStraight) {
    Vector2f direction = playerPos - Vector2f(m_startPosX, m_startPosY);
    m_velocity = normalizeVector(direction) * m_enemySpeed;

    float angle = atan2(direction.y, direction.x) * 180 / 3.14159f;
    m_enemySprite.setRotation(degrees(angle));
  } else if (m_type == Type::HandChaser) {
    Vector2f direction = playerPos - Vector2f(m_startPosX, m_startPosY);
    m_velocity = normalizeVector(direction) * m_enemySpeed;
  } else {
    m_velocity = {0.0f, m_enemySpeed};
  }
}

//* === UPDATE LOGIC ===
void Enemy::update(Time dt, float winWidth, float winHeight,
                   float m_machineLeftWall, float m_machineRightWall,
                   Vector2f playerPos, Sound &cookieSound) {
  if (m_type == Type::HandStraight) {
    m_enemySprite.move(m_velocity * dt.asSeconds());
  } else if (m_type == Type::HandChaser) {
    m_chaseTimer -= dt.asSeconds();
    if (m_chaseTimer > 0) {
      Vector2f direction = playerPos - m_enemySprite.getPosition();
      m_velocity = normalizeVector(direction) * m_enemySpeed;
      float angle = atan2(direction.y, direction.x) * 180 / 3.14159f;
      m_enemySprite.setRotation(degrees(angle));
    }
    m_enemySprite.move(m_velocity * dt.asSeconds());
  } else {
    m_enemySprite.move({0.0f, m_enemySpeed * dt.asSeconds()});

    if (m_type == Type::Box) {
      float swayAmount =
          std::cos(m_enemySprite.getPosition().y * 0.02f + 3.0f) * 400.0f;
      m_enemySprite.move({swayAmount * dt.asSeconds(), 0.0f});

      bool isLeft = (m_enemySprite.getPosition().x < winWidth / 2.0f);
      float rotSpeed = 405.0f;
      m_enemySprite.rotate(
          sf::degrees((isLeft ? rotSpeed : -rotSpeed) * dt.asSeconds()));
    }
  }

  if (m_type == Type::Box) {
    if (!m_hasShot && m_enemySprite.getPosition().y >= m_shootHeight) {
      m_hasShot = true;

      cookieSound.play();
      for (int i = 0; i < 4; i++) {
        if (m_particleTexturePtr) {
          CookieParticle cookie(*m_particleTexturePtr);
          cookie.sprite.setTexture(*m_particleTexturePtr, true);
          cookie.sprite.setPosition(m_enemySprite.getPosition());
          cookie.sprite.setScale({0.15f, 0.15f});

          FloatRect b = cookie.sprite.getLocalBounds();
          cookie.sprite.setOrigin({b.size.x / 2, b.size.y / 2});

          cookie.active = true;

          bool isLeft = (m_enemySprite.getPosition().x < winWidth / 2);

          float baseVelX = 0.0f;
          if (i == 0) {
            baseVelX =
                50.0f + rand() % 100; //? 50 to 150 (covers the blind spot)
          } else if (i == 1) {
            baseVelX = 250.0f + rand() % 150; //? 250 to 400 (normal range)
          } else if (i == 2) {
            baseVelX = 450.0f + rand() % 200; //? 450 to 650 (far range)
          } else if (i == 3) {
            baseVelX =
                750.0f + rand() % 350; //? 750 to 1100 (reaches opposite wall)
          }

          float velX = isLeft ? baseVelX : -baseVelX;
          float velY = -300.0f - (rand() % 300); //? 300 to 600 upward velocity

          if (i == 3) {
            velY = -600.0f -
                   (rand() %
                    300); //? 600 to 900 upward velocity for the farthest cookie
          }

          cookie.velocity = {velX, velY};
          m_cookies.push_back(cookie);
        }
      }
    }

    float gravity = 1200.0f;
    for (auto &cookie : m_cookies) {
      if (cookie.active) {
        cookie.velocity.y += gravity * dt.asSeconds();
        cookie.sprite.move(cookie.velocity * dt.asSeconds());
        cookie.sprite.rotate(sf::degrees(360.0f * dt.asSeconds()));

        sf::Vector2f pos = cookie.sprite.getPosition();
        float rightWallX = winWidth - m_machineRightWall;

        if (cookie.velocity.x < 0 && pos.x < m_machineLeftWall) {
          pos.x = m_machineLeftWall;
          cookie.velocity.x = -cookie.velocity.x;
        } else if (cookie.velocity.x > 0 && pos.x > rightWallX) {
          pos.x = rightWallX;
          cookie.velocity.x = -cookie.velocity.x;
        }
        cookie.sprite.setPosition(pos);

        if (cookie.sprite.getPosition().y > winHeight + 100) {
          cookie.active = false;
        }
      }
    }
  }
}

void Enemy::setDifficultyParams(float enemySpeed, float enemyScale) {}

//* === DRAW LOGIC ===
void Enemy::draw(RenderWindow &window, const GameSettings &gameSettings) {
  window.draw(m_enemySprite);

  if (m_type == Type::Box) {
    for (const auto &cookie : m_cookies) {
      if (cookie.active)
        window.draw(cookie.sprite);
      if (gameSettings.showHitbox) {
        float localWidth = cookie.sprite.getLocalBounds().size.x;
        float currentScale = cookie.sprite.getScale().x;
        float radius = (localWidth * currentScale / 2.0f) * 0.9f;

        CircleShape c;
        c.setRadius(radius);
        c.setOrigin({radius, radius});
        c.setPosition(cookie.sprite.getPosition());
        c.setFillColor(Color::Transparent);
        c.setOutlineColor(Color::Cyan);
        c.setOutlineThickness(2.0f);
        window.draw(c);
      }
    }
  }

  if (gameSettings.showHitbox) {
    if (m_type == Type::Can) {
      const auto &debugCircles = getHitboxes();
      CircleShape debugShape;
      debugShape.setFillColor(Color::Transparent);
      debugShape.setOutlineColor(Color::Cyan);
      debugShape.setOutlineThickness(3.0f);

      for (const auto &circle : debugCircles) {
        debugShape.setRadius(circle.radius);
        debugShape.setOrigin({circle.radius, circle.radius});
        debugShape.setPosition(circle.center);
        window.draw(debugShape);
      }
    } else {
      FloatRect localBounds = m_enemySprite.getLocalBounds();
      RectangleShape debugRect;

      debugRect.setSize({localBounds.size.x, localBounds.size.y});
      debugRect.setOrigin({localBounds.position.x + localBounds.size.x / 2.0f,
                           localBounds.position.y + localBounds.size.y / 2.0f});

      debugRect.setPosition(m_enemySprite.getPosition());
      debugRect.setRotation(m_enemySprite.getRotation());
      debugRect.setScale(m_enemySprite.getScale());

      debugRect.setFillColor(Color::Transparent);
      debugRect.setOutlineColor(Color::Cyan);

      float avgScale = (std::abs(m_enemySprite.getScale().x) +
                        std::abs(m_enemySprite.getScale().y)) /
                       2.0f;
      debugRect.setOutlineThickness(3.0f / avgScale);

      window.draw(debugRect);
    }
  }
}

const vector<CollisionCircle> &Enemy::getHitboxes() {
  m_hitboxes.clear();

  FloatRect bounds = m_enemySprite.getGlobalBounds();

  float radius = (bounds.size.x / 2.0f) * 0.8f;

  float centerX = bounds.position.x + bounds.size.x / 2.0f;

  if (m_type == Type::Can) {
    m_hitboxes.push_back({{centerX, bounds.position.y + radius}, radius});
    m_hitboxes.push_back(
        {{centerX, bounds.position.y + bounds.size.y - radius}, radius});
  } else if (m_type == Type::Bar || m_type == Type::Bar2) {
    float barRadius;
    float offset;

    if (m_type == Type::Bar2) {
      barRadius = (bounds.size.y / 2.0f) * 0.2f;
      offset = (bounds.size.x / 2.0f) * 0.3f;
    } else {
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

FloatRect Enemy::getBounds() { return m_enemySprite.getGlobalBounds(); }

Type Enemy::getType() const { return m_type; }

Vector2f Enemy::getPosition() { return m_enemySprite.getPosition(); }

const std::vector<CookieParticle> &Enemy::getCookies() { return m_cookies; }

bool Enemy::checkCollision(const CollisionCircle &circle) const {
  sf::Transform inv = m_enemySprite.getInverseTransform();
  sf::Vector2f localPoint = inv.transformPoint(circle.center);
  sf::FloatRect localBounds = m_enemySprite.getLocalBounds();

  float scaleX = m_enemySprite.getScale().x;
  float localRadius = circle.radius / scaleX;

  float closeX = std::max(
      localBounds.position.x,
      std::min(localPoint.x, localBounds.position.x + localBounds.size.x));
  float closeY = std::max(
      localBounds.position.y,
      std::min(localPoint.y, localBounds.position.y + localBounds.size.y));

  float dx = localPoint.x - closeX;
  float dy = localPoint.y - closeY;

  return (dx * dx + dy * dy) < (localRadius * localRadius);
}