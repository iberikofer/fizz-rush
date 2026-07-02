#include "Player.hpp"
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;
using namespace sf;

Player::Player(float winWidth, float winHeight)
    : m_player(m_playerTexture), m_playerAura(m_playerAuraTexture1),
      m_arrow(m_arrowTexture), m_wallSound(m_wallSoundBuffer),
      m_hitSound(m_hitSoundBuffer), m_rollSound(m_rollSoundBuffer) {
  m_waveTotalTime = 0.0f;

  m_playerWidth = 0.045f;
  m_playerHeight = 0.057f;
  m_playerMoved = false;
  m_machineLeftWall = 70.0f;
  m_machineUpWall = 19.0f;
  m_machineRightWall = 130.0f;
  m_HP = 3;
  m_isInvincible = false;
  m_invincibilityTimer = 0.0f;
  m_showAura = false;

  m_rollSound.setLooping(true);
  m_wallSoundTimer = 0.5f;
  m_wallSoundInterval = 0.2f;
}

void Player::loadAssets(float startX, float startY) {
  if (!m_playerAuraTexture1.loadFromFile("assets/images/aura1.png"))
    cerr << "Sprite error!" << endl;
  if (!m_playerAuraTexture2.loadFromFile("assets/images/aura2.png"))
    cerr << "Sprite error!" << endl;
  if (!m_playerTexture.loadFromFile("assets/images/can.png"))
    cerr << "Sprite error!" << endl;
  if (!m_arrowTexture.loadFromFile("assets/images/arrow.png"))
    cerr << "Sprite error!" << endl;
  if (!m_hitSoundBuffer.loadFromFile("assets/sound/collision.ogg"))
    cerr << "Hit sound error!" << endl;
  if (!m_wallSoundBuffer.loadFromFile("assets/sound/wall.ogg"))
    cerr << "Hit sound error!" << endl;
  if (!m_rollSoundBuffer.loadFromFile("assets/sound/roll.ogg"))
    cerr << "Roll sound error!" << endl;

  m_player.setPosition({startX, startY});

  m_player.setTexture(m_playerTexture, true);
  FloatRect m_playerLocalBounds = m_player.getLocalBounds();
  m_player.setOrigin(
      {m_playerLocalBounds.size.x / 2.0f, m_playerLocalBounds.size.y / 2.0f});
  m_player.setScale({m_playerWidth, m_playerHeight});

  m_arrow.setPosition({startX, startY - 25.0f});
  m_arrow.setTexture(m_arrowTexture, true);
  FloatRect arrowBounds = m_arrow.getLocalBounds();
  m_arrow.setOrigin({arrowBounds.size.x / 2.0f, arrowBounds.size.y});
  m_arrow.setScale({0.25f, 0.35f});

  m_playerAura.setPosition({startX, startY});

  m_playerAura.setTexture(m_playerAuraTexture1, true);
  FloatRect m_auraLocalBounds = m_playerAura.getLocalBounds();
  m_playerAura.setOrigin(
      {m_auraLocalBounds.size.x / 2.0f, m_auraLocalBounds.size.y / 2.0f});
  m_playerAura.setScale({m_playerWidth * 4.0f, m_playerHeight * 4.0f});
}

void Player::update(Time dt, float winWidth, float winHeight,
                    float m_machineLeftWall, float m_machineRightWall) {
  m_playerAura.setPosition(m_player.getPosition());
  m_playerAura.setRotation(m_player.getRotation());
  float m_targetRotation = 0.0f;
  if (Keyboard::isKeyPressed(Keyboard::Key::Right) &&
      Keyboard::isKeyPressed(Keyboard::Key::Up)) {
    m_targetRotation = 45.0f;
    m_player.move(
        {m_playerSpeed * dt.asSeconds(), -m_playerSpeed * dt.asSeconds()});
    m_playerMoved = true;
  } else if (Keyboard::isKeyPressed(Keyboard::Key::Right) &&
             Keyboard::isKeyPressed(Keyboard::Key::Down)) {
    m_targetRotation = 135.0f;
    m_player.move(
        {m_playerSpeed * dt.asSeconds(), m_playerSpeed * dt.asSeconds()});
    m_playerMoved = true;
  } else if (Keyboard::isKeyPressed(Keyboard::Key::Left) &&
             Keyboard::isKeyPressed(Keyboard::Key::Down)) {
    m_targetRotation = 225.0f;
    m_player.move(
        {-m_playerSpeed * dt.asSeconds(), m_playerSpeed * dt.asSeconds()});
    m_playerMoved = true;
  } else if (Keyboard::isKeyPressed(Keyboard::Key::Left) &&
             Keyboard::isKeyPressed(Keyboard::Key::Up)) {
    m_targetRotation = 315.0f;
    m_player.move(
        {-m_playerSpeed * dt.asSeconds(), -m_playerSpeed * dt.asSeconds()});
    m_playerMoved = true;
  } else if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
    m_targetRotation = 0.0f;
    m_player.move({m_playerSpeed * dt.asSeconds(), 0.0f});
    m_playerMoved = true;
  } else if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
    m_targetRotation = 0.0f;
    m_player.move({-m_playerSpeed * dt.asSeconds(), 0.0f});
    m_playerMoved = true;
  } else if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
    m_targetRotation = 0.0f;
    m_player.move({0.0f, -m_playerSpeed * dt.asSeconds()});
    m_playerMoved = true;
  } else if (Keyboard::isKeyPressed(Keyboard::Key::Down)) {
    m_targetRotation = 180.0f;
    m_player.move({0.0f, m_playerSpeed * dt.asSeconds()});
    m_playerMoved = true;
  } else {
    m_targetRotation = 0.0f;
  }

  float m_rotationStep = 900.0f * dt.asSeconds();
  float m_rotationDiff = m_targetRotation - m_player.getRotation().asDegrees();

  if (abs(m_rotationDiff) < m_rotationStep) {
    m_player.setRotation(degrees(m_targetRotation));
  } else {
    if (m_rotationDiff > 180.0f)
      m_rotationDiff -= 360.0f;
    else if (m_rotationDiff < -180.0f)
      m_rotationDiff += 360.0f;

    if (m_rotationDiff > 0)
      m_player.rotate(degrees(m_rotationStep));
    else
      m_player.rotate(degrees(-m_rotationStep));
  }

  m_wallSoundTimer += dt.asSeconds();

  checkWorldCollision(winWidth, winHeight);

  if (m_isInvincible) {
    m_showAura = true;
    m_invincibilityTimer += dt.asSeconds();

    if (static_cast<int>(m_invincibilityTimer * 7.5) % 2 == 0) {
      m_player.setColor(Color(255, 255, 255, 150));
    } else {
      m_player.setColor(Color(255, 255, 255, 255));
    }

    float m_wave = sin(m_invincibilityTimer * 20.0f);
    if (m_wave > 0) {
      m_playerAura.setTexture(m_playerAuraTexture2, true);
      FloatRect m_auraLocalBounds = m_playerAura.getLocalBounds();
      m_playerAura.setOrigin(
          {m_auraLocalBounds.size.x / 2.0f, m_auraLocalBounds.size.y / 2.0f});
    } else {
      m_playerAura.setTexture(m_playerAuraTexture1, true);
      FloatRect m_auraLocalBounds = m_playerAura.getLocalBounds();
      m_playerAura.setOrigin(
          {m_auraLocalBounds.size.x / 2.0f, m_auraLocalBounds.size.y / 2.0f});
    }

    float m_baseAuraScale = m_playerWidth * 4.0f;
    float m_wobble = m_wave * 0.005f;
    m_playerAura.setScale(
        {m_baseAuraScale + m_wobble, m_baseAuraScale + m_wobble});

    if (m_invincibilityTimer >= m_maxInvincibilityTime) {
      m_isInvincible = false;
      m_showAura = false;
      m_invincibilityTimer = 0.0f;
      m_playerSpeed *= 1.5f;
      m_player.setTexture(m_playerTexture, true);
      m_player.setColor(Color(255, 255, 255, 255));
      m_player.setScale({m_playerWidth, m_playerHeight});
    }
  } else if (m_playerMoved == false) {
    m_waveTotalTime += dt.asSeconds();
    float bobbing = sin(m_waveTotalTime * 5.0f) * 10.0f;

    m_arrow.setPosition(
        {m_player.getPosition().x,
         m_player.getPosition().y - m_machineLeftWall + bobbing});
  } else {
    m_arrow.setColor(Color::Transparent);
  }
}

void Player::updateSound() {
  m_hitSound.stop();
  m_wallSound.stop();
  m_rollSound.stop();
}

void Player::setDifficultyParams(int maxHP, float playerSpeed,
                                 float invincibilityDur) {}

void Player::checkWorldCollision(float winWidth, float winHeight) {
  //* === WALLS ===
  FloatRect m_spriteBounds = m_player.getGlobalBounds();
  float m_spriteHalfWidth = m_spriteBounds.size.x / 2.0f;
  float m_spriteHalfHeight = m_spriteBounds.size.y / 2.0f;

  //* LEFT
  if (m_spriteBounds.position.x < m_machineLeftWall) {
    m_player.setPosition({m_spriteHalfWidth + m_machineLeftWall + 30.0f,
                          m_player.getPosition().y});
    if (m_wallSoundTimer >= m_wallSoundInterval) {
      m_wallSound.play();
      m_wallSoundTimer = 0.0f;
    }
  }
  //* UP
  if (m_spriteBounds.position.y < m_machineUpWall) {
    m_player.setPosition(
        {m_player.getPosition().x, m_spriteHalfHeight + 34.0f});
    if (m_wallSoundTimer >= m_wallSoundInterval) {
      m_wallSound.play();
      m_wallSoundTimer = 0.0f;
    }
  }
  //* RIGHT
  if (m_player.getPosition().x + m_spriteHalfWidth >
      winWidth - m_machineRightWall) {
    m_player.setPosition(
        {winWidth - m_spriteHalfWidth - 150.0f, m_player.getPosition().y});
    if (m_wallSoundTimer >= m_wallSoundInterval) {
      m_wallSound.play();
      m_wallSoundTimer = 0.0f;
    }
  }
  //* BOTTOM
  if (m_player.getPosition().y + m_spriteHalfHeight > winHeight) {
    m_player.setPosition(
        {m_player.getPosition().x, winHeight - m_spriteHalfHeight});
    if (m_wallSoundTimer >= m_wallSoundInterval) {
      m_wallSoundTimer = 0.0f;
    }
  }
}

bool Player::hasPlayerMoved() { return m_playerMoved; }

int Player::getHealth() { return m_HP; }

int Player::loseHealth(Time dt) {
  if (!m_isInvincible) {
    m_showAura = true;
    --m_HP;
    if (m_HP < 0)
      m_HP = 0;
    m_hitSound.play();
    if (m_maxInvincibilityTime != 0.0f) {
      m_playerSpeed = m_playerSpeed / 1.5f;
      m_isInvincible = true;
      m_invincibilityTimer = 0.0f;
      cout << "OUCH! HP:" << m_HP << endl;
    }
  }
  return m_HP;
}

void Player::resetGame(float m_startPosX, float m_startPosY, int maxHP,
                       float difficultySpeed, float invincibilityDuration,
                       GameDifficulty difficulty) {
  m_player.setPosition({m_startPosX, m_startPosY});
  m_HP = maxHP;
  m_isInvincible = false;
  m_invincibilityTimer = 0.0f;
  m_showAura = false;
  m_playerMoved = false;
  m_playerSpeed = difficultySpeed;
  m_player.setRotation(degrees(0));

  m_maxInvincibilityTime = invincibilityDuration;

  m_difficulty = difficulty;

  m_player.setColor(Color::White);
  m_player.setScale({m_playerWidth, m_playerHeight});
  m_arrow.setColor(Color::White);
}

void Player::draw(RenderWindow &window, const GameSettings &gameSettings) {
  if (m_showAura) {
    window.draw(m_playerAura);
  }

  window.draw(m_player);

  if (!m_playerMoved) {
    window.draw(m_arrow);
  }

  if (gameSettings.debugMode) {
    vector<CollisionCircle> debugCircles = getHitboxes();

    for (const auto &circle : debugCircles) {
      sf::CircleShape shape(circle.radius);

      shape.setOrigin({circle.radius, circle.radius});
      shape.setPosition(circle.center);

      shape.setFillColor(sf::Color::Transparent);
      shape.setOutlineColor(sf::Color::Cyan);
      shape.setOutlineThickness(5.0f);

      window.draw(shape);
    }
  }
}

vector<CollisionCircle> Player::getHitboxes() {
  vector<CollisionCircle> circles;

  FloatRect localBounds = m_player.getLocalBounds();

  float realWidth = localBounds.size.x * m_player.getScale().x;
  float realHeight = localBounds.size.y * m_player.getScale().y;

  float easyRadius = (realWidth / 2.0f) * 0.75f;
  float normalRadius = (realWidth / 2.0f) * 0.85f;
  float hardRadius = (realWidth / 2.0f) * 0.9f;

  Transform trans = m_player.getTransform();

  float localX = localBounds.size.x / 2.0f;

  if (m_difficulty == GameDifficulty::Easy) {
    Vector2f topPoint =
        trans.transformPoint({localX, localBounds.size.y * 0.3f});
    Vector2f botPoint =
        trans.transformPoint({localX, localBounds.size.y * 0.7f});

    circles.push_back({topPoint, easyRadius});
    circles.push_back({botPoint, easyRadius});
  } else if (m_difficulty == GameDifficulty::Normal) {

    Vector2f topPoint =
        trans.transformPoint({localX, localBounds.size.y * 0.25f});
    Vector2f botPoint =
        trans.transformPoint({localX, localBounds.size.y * 0.75f});

    circles.push_back({topPoint, normalRadius});
    circles.push_back({botPoint, normalRadius});
  } else {

    Vector2f topPoint =
        trans.transformPoint({localX, localBounds.size.y * 0.15f});
    Vector2f midPoint =
        trans.transformPoint({localX, localBounds.size.y * 0.5f});
    Vector2f botPoint =
        trans.transformPoint({localX, localBounds.size.y * 0.85f});

    circles.push_back({topPoint, hardRadius});
    circles.push_back({midPoint, hardRadius});
    circles.push_back({botPoint, hardRadius});
  }

  return circles;
}