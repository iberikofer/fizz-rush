#include "Boss.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>

//* === INITIAL SETUP ===
Boss::Boss()
    : m_boss(m_bossTexture), m_bossWarning(m_bossWarningTexture),
      m_deathSound(deathSoundBuffer), m_attackSound(attackSoundBuffer),
      m_warningSound(warningSoundBuffer) {
  m_active = false;
  m_maxHP = 50;
  m_currentHP = m_maxHP;
  m_speed = 150.0f;
  m_attackState = AttackState::Idle;
  m_currentIdleDuration = 1.5f;
  m_attackTimer = 0.0f;
  attackSpeed = 0.0f;
  spinSpeed = 0.0f;
  m_warningDuration = 0.0f;
  m_attackSound.setVolume(100.0f);
  m_warningSound.setVolume(30.0f);
  m_deathSound.setVolume(30.0f);
  // !
  m_phase1Duration = 100.0f;
  m_phase2Duration = 100.0f;
  m_phase3Duration = 100.0f;
  // !
  newAlpha = 255.0f;
  m_currentBossScaleX = 1.0f;
}

void Boss::loadAssets() {
  if (!m_bossTexture.loadFromFile("assets/images/boss_1.png"))
    std::cerr << "Boss texture error!" << std::endl;
  if (!m_bossWarningTexture.loadFromFile("assets/images/boss_warning.png"))
    std::cerr << "Boss warning texture error!" << std::endl;
  if (!attackSoundBuffer.loadFromFile("assets/sound/boss_attack1.ogg"))
    std::cerr << "Boss attack sound error!" << std::endl;
  if (!warningSoundBuffer.loadFromFile("assets/sound/boss_warning.ogg"))
    std::cerr << "Boss attack sound error!" << std::endl;
  if (!deathSoundBuffer.loadFromFile("assets/sound/boss_death.ogg"))
    std::cerr << "Boss death sound error!" << std::endl;

  m_boss.setTexture(m_bossTexture, true);
  m_boss.setScale({1.0f, 1.0f});
  m_bossWarning.setTexture(m_bossWarningTexture, true);
  m_warningScaleX = 0.3f;
  m_warningScaleY = 0.3f;
  m_bossWarning.setScale({m_warningScaleX, m_warningScaleY});

  sf::FloatRect bounds = m_boss.getLocalBounds();
  m_boss.setOrigin({bounds.size.x / 2, bounds.size.y / 2});
  bounds = m_bossWarning.getLocalBounds();
  m_bossWarning.setOrigin({bounds.size.x / 2, bounds.size.y / 2});
}

void Boss::spawn(float startX, float startY, const GameSettings &settings) {
  // ! Starting
  m_bossPhase = BossPhase::Phase2;
  // ! Idle
  m_attackState = AttackState::PhaseTransition;
  // !
  m_attackTimer = 0.0f;
  m_phaseTimer = 0.0f;
  m_spawnColorTimer = 0.0f;
  m_active = true;
  isBossCentered = false;
  isDescentFinished = false;
  transitionTimer = 0.0f;
  m_boss.setColor(sf::Color::White);
  m_boss.setPosition({startX, startY});
  m_boss.setRotation(sf::degrees(0));
  m_boss.setScale({1.0f, 1.0f});
  m_targetPos = {startX, 400.0f};
  m_antiCheatTimer = 0.0f;
  m_isSpinning360 = false;
  m_spinProgress = 0.0f;
  m_bounceCount = 0;
  m_currentDashSpeed = 0.0f;

  if (settings.gameDifficulty == GameDifficulty::Easy)
    attackSpeed = 7.0f;
  else if (settings.gameDifficulty == GameDifficulty::Normal)
    attackSpeed = 7.5f;
  else if (settings.gameDifficulty == GameDifficulty::Hard)
    attackSpeed = 6.0f;

  if (settings.gameDifficulty == GameDifficulty::Easy)
    spinSpeed = 400.0f;
  else if (settings.gameDifficulty == GameDifficulty::Normal)
    spinSpeed = 600.0f;
  else if (settings.gameDifficulty == GameDifficulty::Hard)
    spinSpeed = 800.0f;

  if (settings.gameDifficulty == GameDifficulty::Easy) {
    m_currentHP = 50;
    m_speed = 100.0f;
  } else if (settings.gameDifficulty == GameDifficulty::Normal) {
    m_currentHP = 100;
    m_speed = 150.0f;
  } else if (settings.gameDifficulty == GameDifficulty::Hard) {
    m_currentHP = 150;
    m_speed = 200.0f;
  }
}

//* === UPDATE LOGIC ===
void Boss::update(sf::Time dt, sf::Vector2f playerPos, float windowWidth,
                  float windowHeight, const GameSettings &settings) {
  if (!m_active)
    return;

  m_spawnColorTimer += dt.asSeconds();

  if (m_bossPhase != BossPhase::Starting && m_bossPhase != BossPhase::Death) {
    m_phaseTimer += dt.asSeconds();
  }

  switch (m_bossPhase) {
  case BossPhase::Starting: {
    if (static_cast<int>(m_spawnColorTimer * 5.0f) % 2 == 0)
      m_boss.setColor(sf::Color(255, 0, 0));
    else
      m_boss.setColor(sf::Color::White);

    float distance = m_targetPos.y - m_boss.getPosition().y;
    float step = 100 * dt.asSeconds();

    if (std::abs(distance) < step) {
      m_boss.setPosition(m_targetPos);
      m_boss.setColor(sf::Color::White);
      // ! Phase 1
      m_bossPhase = BossPhase::Phase2;
      // !
      m_phaseTimer = 0.0f;
    } else {
      m_boss.move({0.0f, step});
    }
  } break;
  case BossPhase::Phase1: {
    float targetX = 0.0f;
    float targetY = 0.0f;

    if (!m_isSpinning360 && m_attackState != AttackState::Attacking) {
      if (playerPos.y < m_boss.getPosition().y + 100.0f) {
        m_antiCheatTimer += dt.asSeconds();
        if (m_antiCheatTimer >= 2.0f) {
          float scaleX = (m_currentBossScaleX >= 0) ? 1.0f : -1.0f;
          m_boss.setScale({scaleX, 1.0f});
          m_isSpinning360 = true;
          m_spinProgress = 0.0f;
          m_antiCheatTimer = 0.0f;
          m_warningSound.play();
        }
      } else {
        m_antiCheatTimer = 0.0f;
      }
    }

    if (m_isSpinning360) {
      float spinSpeed = 1000.0f;
      float deltaRotation = spinSpeed * dt.asSeconds();
      m_spinProgress += deltaRotation;

      m_boss.rotate(sf::degrees(deltaRotation));
      m_boss.setColor(sf::Color::Red);

      if (m_spinProgress >= 360.0f) {
        m_isSpinning360 = false;
        m_spinProgress = 0.0f;
        m_boss.setRotation(sf::degrees(0));
        m_boss.setColor(sf::Color::White);
      }

      return;
    }

    switch (m_attackState) {
    case AttackState::Idle: {
      if (playerPos.y >= m_boss.getPosition().y + 100.0f) {
        m_attackTimer += dt.asSeconds();

        if (m_attackTimer >= m_currentIdleDuration) {
          m_attackTimer = 0.0f;
          m_attackState = AttackState::Warning;
          m_warningSound.play();
        }
      } else {
        m_attackTimer = 0.0f;
      }

      m_boss.setColor(sf::Color::White);
      targetX = playerPos.x;
      float currentX = m_boss.getPosition().x;
      float diffX = targetX - currentX;
      float smoothFactor = 3.0f;
      float newX =
          currentX + (targetX - currentX) * smoothFactor * dt.asSeconds();
      float baseY = 400.0f;
      float amplitude = 45.0f;
      float frequency = 22.0f;
      float sineY = baseY + std::sin(m_phaseTimer * frequency) * amplitude;
      m_boss.setPosition({newX, sineY});

      float targetScaleX = (m_currentBossScaleX >= 0) ? 1.0f : -1.0f;
      if (std::abs(diffX) > 20.0f) {
        targetScaleX = (diffX > 0) ? 1.0f : -1.0f;
      }
      float turnSpeed = 10.0f;
      m_currentBossScaleX +=
          (targetScaleX - m_currentBossScaleX) * turnSpeed * dt.asSeconds();
      if (std::abs(m_currentBossScaleX - targetScaleX) < 0.01f) {
        m_currentBossScaleX = targetScaleX;
      }
      m_boss.setScale({m_currentBossScaleX, 1.0f});
    } break;
    case AttackState::Warning: {
      m_boss.setColor(sf::Color(255, 140, 0));
      m_attackTimer += dt.asSeconds();

      float finalTargetX = (m_currentBossScaleX >= 0) ? 1.0f : -1.0f;
      float finalTurnSpeed = 15.0f;
      if (std::abs(m_currentBossScaleX - finalTargetX) > 0.001f) {
        m_currentBossScaleX += (finalTargetX - m_currentBossScaleX) *
                               finalTurnSpeed * dt.asSeconds();
        m_boss.setScale({m_currentBossScaleX, 1.0f});
      }

      float shakeFrequency = 100.0f;
      float shakeAmplify = 15.0f;
      float shakeOffset =
          (std::sin(m_attackTimer * shakeFrequency) -
           std::sin((m_attackTimer - dt.asSeconds()) * shakeFrequency)) *
          shakeAmplify;
      m_boss.move({shakeOffset, 0.0f});

      if (m_attackTimer >= 0.5f) {
        m_attackTimer = 0.0f;
        m_boss.setColor(sf::Color(255, 0, 0));
        m_attackState = AttackState::Attacking;
        m_currentIdleDuration =
            1.0f + static_cast<float>(std::rand()) /
                       (static_cast<float>(RAND_MAX / 2.0f));
        m_attackSound.play();
      }
    } break;
    case AttackState::Attacking: {
      m_attackTimer += dt.asSeconds();
      m_boss.setColor(sf::Color::Red);
      float attackSpeed = m_speed * 5.0f;
      float shakeFrequency = 45.0f;
      float shakeAmplify = 50.0f;
      float shakeOffset =
          (std::sin(m_attackTimer * shakeFrequency) -
           std::sin((m_attackTimer - dt.asSeconds()) * shakeFrequency)) *
          shakeAmplify;
      m_boss.move({shakeOffset, attackSpeed * dt.asSeconds()});

      sf::FloatRect bounds = m_boss.getLocalBounds();
      if (m_boss.getPosition().y > windowHeight + bounds.size.y / 2) {
        m_boss.setPosition({m_boss.getPosition().x, -600.0f});
        m_attackState = AttackState::Returning;
      }
    } break;
    case AttackState::Returning: {
      m_boss.setColor(sf::Color::Green);
      m_attackTimer += dt.asSeconds();
      targetX = m_boss.getPosition().x;
      targetY = 400.0f;
      float shakeFrequency = 25.0f;
      float shakeAmplify = 30.0f;
      float shakeOffset =
          (std::sin(m_attackTimer * shakeFrequency) -
           std::sin((m_attackTimer - dt.asSeconds()) * shakeFrequency)) *
          shakeAmplify;

      if (m_boss.getPosition().y < targetY ||
          std::abs(m_boss.getPosition().x - targetX) > 1.0f) {
        m_boss.move({shakeOffset, m_speed * dt.asSeconds() * 4.0f});
      } else {
        m_boss.setPosition({targetX, targetY});
        m_attackState = (m_phaseTimer >= m_phase1Duration)
                            ? AttackState::PhaseTransition
                            : AttackState::Idle;
        m_attackTimer = 0.0f;
        m_currentIdleDuration =
            1.0f + static_cast<float>(std::rand()) /
                       (static_cast<float>(RAND_MAX / 5.5f));
      }

      break;
    }
    case AttackState::PhaseTransition: {
      m_boss.setColor(sf::Color::Magenta);
      float centerX = windowWidth / 2.0f;
      float centerY = windowHeight / 2.0f - 100.0f;
      float dx = centerX - m_boss.getPosition().x;
      float dy = centerY - m_boss.getPosition().y;
      float distance = std::sqrt(dx * dx + dy * dy);

      if (!isBossCentered) {
        if (distance > 5.0f) {
          float angleRad = std::atan2(dy, dx);
          float angleDeg = angleRad * 180.0f / 3.14159f;
          float tiltFactor = std::min(1.0f, distance / 400.0f);

          float targetScaleX = (dx >= 0) ? 1.0f : -1.0f;
          m_currentBossScaleX +=
              (targetScaleX - m_currentBossScaleX) * 10.0f * dt.asSeconds();
          m_boss.setScale({m_currentBossScaleX, 1.0f});

          float finalAngle = angleDeg;
          float tiltOffset = (m_currentBossScaleX >= 0) ? -45.0f : 45.0f;
          if (dx < 0)
            finalAngle -= 180.0f;
          m_boss.setRotation(
              sf::degrees((finalAngle + tiltOffset) * tiltFactor));
          float flySpeed = m_speed * 3.0f;
          m_boss.move({(dx / distance) * flySpeed * dt.asSeconds(),
                       (dy / distance) * flySpeed * dt.asSeconds()});
        } else {
          isBossCentered = true;
          transitionTimer = 0.0f;
          m_boss.setPosition({centerX, centerY});
        }
      } else {
        float turnSpeed = 20.0f;
        float targetScaleX = (dx >= 0) ? 1.0f : -1.0f;
        m_currentBossScaleX +=
            (targetScaleX - m_currentBossScaleX) * turnSpeed * dt.asSeconds();
        if (std::abs(m_currentBossScaleX - targetScaleX) < 0.01f)
          m_currentBossScaleX = targetScaleX;
        m_boss.setScale({m_currentBossScaleX, 1.0f});
        transitionTimer += dt.asSeconds();

        if (transitionTimer >= 2.0f)
          m_boss.rotate(sf::degrees(1000.0f * dt.asSeconds()));
        if (transitionTimer >= 4.0f)
          m_boss.move({0.0f, -m_speed * 3.0f * dt.asSeconds()});

        if (m_boss.getPosition().y < -500.0f) {
          m_boss.setRotation(sf::degrees(0));
          m_boss.setScale({1.0f, 1.0f});
          m_bossPhase = BossPhase::Phase2;
          m_attackState = AttackState::Idle;
          m_phaseTimer = 0.0f;
          transitionTimer = 0.0f;
          isBossCentered = false;
        }
      }
      break;
    }
    }
    break;
  }
  case BossPhase::Phase2: {
    switch (m_attackState) {
    case AttackState::Idle: {
      m_attackTimer += dt.asSeconds();
      m_boss.setColor(sf::Color::White);

      if (m_attackTimer >= m_currentIdleDuration) {
        m_attackTimer = 0.0f;
        m_attackState = AttackState::Warning;

        int side = std::rand() % 4;
        float margin = 100.0f;
        float screenEdgeOffset = 80.0f;
        sf::Vector2f startPos;

        if (side == 0) { // UP
          startPos = {margin + static_cast<float>(
                                   std::rand() %
                                   static_cast<int>(windowWidth - 2 * margin)),
                      -500.0f};
          m_bossWarning.setPosition({startPos.x, screenEdgeOffset});
        } else if (side == 1) { // DOWN
          startPos = {margin + static_cast<float>(
                                   std::rand() %
                                   static_cast<int>(windowWidth - 2 * margin)),
                      windowHeight + 500.0f};
          m_bossWarning.setPosition(
              {startPos.x, windowHeight - screenEdgeOffset});
        } else if (side == 2) { // LEFT
          startPos = {-500.0f,
                      margin + static_cast<float>(
                                   std::rand() % static_cast<int>(windowHeight -
                                                                  2 * margin))};
          m_bossWarning.setPosition({screenEdgeOffset, startPos.y});
        } else { // RIGHT
          startPos = {windowWidth + 500.0f,
                      margin + static_cast<float>(
                                   std::rand() % static_cast<int>(windowHeight -
                                                                  2 * margin))};
          m_bossWarning.setPosition(
              {windowWidth - screenEdgeOffset, startPos.y});
        }

        m_boss.setPosition(startPos);

        sf::Vector2f direction = playerPos - m_boss.getPosition();
        float length =
            std::sqrt(direction.x * direction.x + direction.y * direction.y);
        m_attackDirection = direction / length;
      }
    } break;
    case AttackState::Warning: {
      m_attackTimer += dt.asSeconds();

      float pulseFreq = 19.0f;
      float pulseAmp = 0.1f;
      float currentPulse =
          0.3f + std::sin(m_attackTimer * pulseFreq) * pulseAmp;

      m_bossWarning.setScale({currentPulse, currentPulse});

      if (m_attackTimer == dt.asSeconds()) {
        m_warningSound.play();
      }

      if (m_attackTimer >= 0.4f) {
        m_attackTimer = 0.0f;
        m_bossWarning.setScale({0.3f, 0.3f});

        m_attackState = AttackState::Attacking;
        m_attackSound.play();
      }
    } break;
    case AttackState::Attacking: {
      m_boss.setColor(sf::Color::Red);
      m_boss.move(m_attackDirection * m_speed * attackSpeed * dt.asSeconds());

      float spinFactor = (m_attackDirection.x >= 0) ? 1.0f : -1.0f;
      m_boss.rotate(sf::degrees(spinFactor * spinSpeed * dt.asSeconds()));

      sf::Vector2f pos = m_boss.getPosition();
      if (pos.x < -500 || pos.x > windowWidth + 500 || pos.y < -500 ||
          pos.y > windowHeight + 500)
        m_attackState = AttackState::Returning;
      break;
    }

    case AttackState::Returning: {
      m_boss.setColor(sf::Color::Green);
      m_boss.move(-m_attackDirection * (m_speed * 3.5f) * dt.asSeconds());

      float spinFactor = (m_attackDirection.x >= 0) ? -1.0f : 1.0f;
      m_boss.rotate(sf::degrees(spinFactor * 200.0f * dt.asSeconds()));

      sf::Vector2f pos = m_boss.getPosition();
      bool hasReturned = false;

      if (m_attackDirection.x > 0 && pos.x < -700)
        hasReturned = true;
      else if (m_attackDirection.x < 0 && pos.x > windowWidth + 700)
        hasReturned = true;
      else if (m_attackDirection.y > 0 && pos.y < -700)
        hasReturned = true;
      else if (m_attackDirection.y < 0 && pos.y > windowHeight + 700)
        hasReturned = true;

      if (hasReturned) {
        if (m_phaseTimer >= m_phase2Duration) {
          m_attackState = AttackState::PhaseTransition;
        } else {
          m_attackState = AttackState::Idle;
        }
        m_attackTimer = 0.0f;
        m_boss.setRotation(sf::degrees(0));
      }

      break;
    }
    case AttackState::PhaseTransition: {
      m_boss.setColor(sf::Color::Magenta);
      float centerX = windowWidth / 2.0f;
      float centerY = windowHeight / 2.0f - 100.0f;

      if (!isBossCentered && transitionTimer == 0.0f) {
        float targetX = windowWidth / 2.0f;
        float targetY = windowHeight / 2.0f + 200.0f;
      }

      if (!isBossCentered) {
        m_boss.setPosition({windowWidth / 2.0f, -600.0f});
        isBossCentered = true;
      } else {
        if (m_boss.getPosition().y < centerY && !isDescentFinished) {
          m_boss.rotate(sf::degrees(1000.0f * dt.asSeconds()));
          m_boss.move({0.0f, m_speed * dt.asSeconds()});
        } else {
          if (!isDescentFinished) {
            isDescentFinished = true;
            transitionTimer = 0.0f;
          }
          float totalPhaseTime = 9.0f;
          float waitDuration = 4.0f;

          transitionTimer += dt.asSeconds();

          if (transitionTimer < waitDuration) {
            m_boss.setPosition({centerX, centerY});
            float shake = std::sin(transitionTimer * 10.0f) * 2.0f;
            m_boss.setRotation(sf::degrees(shake));
            return;
          }

          float activeEightTime = transitionTimer - waitDuration;
          float eightDuration = 6.0f;

          float slowdownDuration = 1.5f;
          float slowdownStart = eightDuration - slowdownDuration;
          float entryLerp = std::min(1.0f, activeEightTime / 2.0f);
          float exitLerp = std::max(
              0.0f, std::min(1.0f, (eightDuration - activeEightTime) / 2.0f));
          float smoothFactor = entryLerp * exitLerp;

          float loops = 3.0f;
          float ampX = 700.0f * smoothFactor;
          float ampY = 200.0f * smoothFactor;
          float freq = (loops * 2.0f * 3.14159f) / eightDuration;
          float offsetX = ampX * std::sin(freq * activeEightTime);
          float offsetY = ampY * std::sin(2.0f * freq * activeEightTime);
          m_boss.setPosition({centerX + offsetX, centerY + offsetY});

          if (smoothFactor > 0.05f) {
            float vx = ampX * freq * std::cos(freq * activeEightTime);
            float vy =
                ampY * 2.0f * freq * std::cos(2.0f * freq * activeEightTime);
            float moveAngle = std::atan2(vy, vx) * 180.0f / 3.14159f;
            m_boss.setRotation(sf::degrees(moveAngle - 90.0f));
          } else {
            float currentRot = m_boss.getRotation().asDegrees();
            m_boss.setRotation(sf::degrees(currentRot * 0.95f));
          }

          float vx = ampX * freq * std::cos(freq * activeEightTime);
          float vy =
              ampY * 2.0f * freq * std::cos(2.0f * freq * activeEightTime);
          float moveAngle = std::atan2(vy, vx) * 180.0f / 3.14159f;
          m_boss.setRotation(sf::degrees(moveAngle - 90.0f));

          if (activeEightTime >= eightDuration) {
            m_bossPhase = BossPhase::Phase3;
            m_attackState = AttackState::Idle;
            m_phaseTimer = 0.0f;
            m_attackTimer = 0.0f;
            isBossCentered = false;
            isDescentFinished = false;
          }
        }
      }
    }
    }
    break;
  }
  case BossPhase::Phase3: {
    switch (m_attackState) {
    case AttackState::Idle: {
      m_attackTimer += dt.asSeconds();
      m_boss.setColor(sf::Color::White);

      sf::Vector2f bossPos = m_boss.getPosition();
      sf::Vector2f dir = playerPos - bossPos;
      float distance = std::sqrt(dir.x * dir.x + dir.y * dir.y);

      if (distance > 5.0f) {
        sf::Vector2f unitDir = dir / distance;
        m_boss.move(unitDir * m_speed * 0.90f * dt.asSeconds());
      }

      float currentAngle = m_boss.getRotation().asDegrees();
      float baseAngle = std::atan2(dir.y, dir.x) * 180.0f / 3.14159f;
      float currentOffset = (m_currentBossScaleX >= 0) ? -72.5f : -107.5f;

      float targetAngle = baseAngle + currentOffset;
      float angleDiff = targetAngle - currentAngle;
      while (angleDiff < -180.0f)
        angleDiff += 360.0f;
      while (angleDiff > 180.0f)
        angleDiff -= 360.0f;
      float rotationSmoothing = 2.5f;
      float newAngle =
          currentAngle + (angleDiff * rotationSmoothing * dt.asSeconds());
      m_boss.setRotation(sf::degrees(newAngle));

      float targetScaleX = (dir.x >= 0) ? 1.0f : -1.0f;
      float flipSpeed = 8.0f;
      m_currentBossScaleX +=
          (targetScaleX - m_currentBossScaleX) * flipSpeed * dt.asSeconds();
      m_boss.setScale({m_currentBossScaleX, 1.0f});

      if (m_attackTimer >= 5.0f) {
        currentOffset = (m_currentBossScaleX >= 0) ? -72.5f : -107.5f;
        m_boss.setScale({m_currentBossScaleX, 1.0f});
        m_attackState = AttackState::Warning;
        m_attackTimer = 0.0f;
        m_warningSound.play();
      }
    } break;
    case AttackState::Warning: {
      m_attackTimer += dt.asSeconds();
      m_boss.setColor(sf::Color(255, 140, 0));
      sf::Vector2f dirToPlayer = playerPos - m_boss.getPosition();

      if (m_attackTimer < 1.2f) {
        float baseAngle =
            std::atan2(dirToPlayer.y, dirToPlayer.x) * 180.0f / 3.14159f;
        float currentOffset = (m_currentBossScaleX >= 0) ? -72.5f : -107.5f;

        m_boss.setRotation(sf::degrees(baseAngle + currentOffset));

        float targetScaleX = (dirToPlayer.x >= 0) ? 1.0f : -1.0f;
        m_currentBossScaleX +=
            (targetScaleX - m_currentBossScaleX) * 8.0f * dt.asSeconds();
        m_boss.setScale({m_currentBossScaleX, 1.0f});

        float dist = std::sqrt(dirToPlayer.x * dirToPlayer.x +
                               dirToPlayer.y * dirToPlayer.y);
        sf::Vector2f unitDir =
            (dist > 0.1f) ? dirToPlayer / dist : sf::Vector2f(0, 0);
        m_boss.move(-unitDir * 200.0f * dt.asSeconds());

        float shake = std::sin(m_attackTimer * 150.0f) * 3.0f;
        m_boss.move({shake, shake});

        m_targetPos = playerPos;
      } else {
        m_boss.setScale({m_currentBossScaleX, 1.0f});
        m_boss.setColor(sf::Color(255, 75, 0));
        float targetScaleX = (dirToPlayer.x >= 0) ? 1.0f : -1.0f;
        m_boss.setScale({targetScaleX, 1.0f});

        float fixShakeIntensity = 14.0f;
        float fixShakeSpeed = 180.0f;
        float shakeX =
            std::sin(m_attackTimer * fixShakeSpeed) * fixShakeIntensity;
        float shakeY =
            std::cos(m_attackTimer * fixShakeSpeed * 1.2f) * fixShakeIntensity;
        m_boss.move({shakeX, shakeY});
      }

      if (m_attackTimer >= 2.0f) {
        m_attackTimer = 0.0f;
        m_attackState = AttackState::Attacking;
        m_attackSound.play();
        m_bounceCount = 0;
        m_currentDashSpeed = 0.0f;
      }
    } break;
    case AttackState::Attacking: {
      m_attackTimer += dt.asSeconds();
      m_boss.setColor(sf::Color::Red);

      m_boss.setScale({m_currentBossScaleX, 1.0f});

      float currentOffset = (m_currentBossScaleX >= 0) ? -72.5f : -107.5f;

      if (m_currentDashSpeed == 0.0f) {
        m_currentDashSpeed = m_speed * 7.0f;
        sf::Vector2f dir = m_targetPos - m_boss.getPosition();
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);

        if (len > 0.1f)
          m_dashVelocity = (dir / len) * m_currentDashSpeed;
        else
          m_dashVelocity = {m_currentDashSpeed, 0.0f};

        float targetAngle =
            std::atan2(m_dashVelocity.y, m_dashVelocity.x) * 180.0f / 3.14159f;
        m_boss.setRotation(sf::degrees(targetAngle + currentOffset));
      }

      sf::Vector2f dirToPlayer = playerPos - m_boss.getPosition();
      float dist = std::sqrt(dirToPlayer.x * dirToPlayer.x +
                             dirToPlayer.y * dirToPlayer.y);

      if (dist > 0.1f) {
        sf::Vector2f unitDirToPlayer = dirToPlayer / dist;

        float homingStrength = 1.15f;

        m_dashVelocity += unitDirToPlayer * homingStrength;

        float currentLen = std::sqrt(m_dashVelocity.x * m_dashVelocity.x +
                                     m_dashVelocity.y * m_dashVelocity.y);
        m_dashVelocity = (m_dashVelocity / currentLen) * m_currentDashSpeed;
      }

      m_boss.move(m_dashVelocity * dt.asSeconds());

      sf::Vector2f pos = m_boss.getPosition();
      bool bounced = false;

      if (pos.x < 50.0f || pos.x > windowWidth - 50.0f) {
        m_dashVelocity.x = -m_dashVelocity.x;
        bounced = true;
      }
      if (pos.y < 50.0f || pos.y > windowHeight - 50.0f) {
        m_dashVelocity.y = -m_dashVelocity.y;
        bounced = true;
      }

      if (bounced) {
        m_bounceCount++;
        m_currentDashSpeed *= 1.10f;

        float len = std::sqrt(m_dashVelocity.x * m_dashVelocity.x +
                              m_dashVelocity.y * m_dashVelocity.y);
        m_dashVelocity = (m_dashVelocity / len) * m_currentDashSpeed;

        m_attackSound.play();

        m_boss.setPosition({std::clamp(pos.x, 51.0f, windowWidth - 51.0f),
                            std::clamp(pos.y, 51.0f, windowHeight - 51.0f)});

        if (m_bounceCount >= 5) {
          m_attackState = AttackState::Idle;
          m_attackTimer = 0.0f;
          m_bounceCount = 0;
          m_currentDashSpeed = 0.0f;
          m_boss.setColor(sf::Color::White);
        }
      }

      currentOffset = (m_currentBossScaleX >= 0) ? -72.5f : -107.5f;
      float currentRotation = m_boss.getRotation().asDegrees();
      float targetRotation =
          std::atan2(m_dashVelocity.y, m_dashVelocity.x) * 180.0f / 3.14159f +
          currentOffset;
      float rotationDiff = targetRotation - currentRotation;

      while (rotationDiff < -180.0f)
        rotationDiff += 360.0f;
      while (rotationDiff > 180.0f)
        rotationDiff -= 360.0f;

      float dashTurnSpeed = 25.0f;
      float finalRotation =
          currentRotation + (rotationDiff * dashTurnSpeed * dt.asSeconds());
      m_boss.setRotation(sf::degrees(finalRotation));
    } break;
    case AttackState::Returning: {
      // m_boss.setColor(sf::Color::Green);
      break;
    }
    case AttackState::PhaseTransition: {
      // m_boss.setColor(sf::Color::Magenta);
      break;
    }
    }
    break;
  }
  case BossPhase::Death: {
    sf::Vector2f targetDeathPos = {windowWidth / 2.0f,
                                   windowHeight / 2.0f - 100.0f};
    sf::Vector2f currentPos = m_boss.getPosition();

    sf::Vector2f diff = targetDeathPos - currentPos;
    float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    float lerpFactor = 3.0f * dt.asSeconds();
    m_boss.setPosition(
        {currentPos.x + (targetDeathPos.x - currentPos.x) * lerpFactor,
         currentPos.y + (targetDeathPos.y - currentPos.y) * lerpFactor});

    if (distance < 5.0f) {
      if (m_deathSound.getStatus() != sf::Sound::Status::Playing)
        m_deathSound.play();

      // ALTERNATIVE DEATH ANIMATION (SHAKING)
      // float offsetX = static_cast<float>((std::rand() % 9) - 3);
      // float offsetY = static_cast<float>((std::rand() % 9) - 3);
      // m_boss.setPosition({targetDeathPos.x + offsetX, targetDeathPos.y +
      // offsetY});

      float speed = 10.0f;
      float amplitude = 5.0f;
      float wave = std::sin(m_spawnColorTimer * speed) * amplitude;
      m_boss.setRotation(sf::degrees(wave));

      newAlpha = std::max(0.0f, newAlpha - 30.0f * dt.asSeconds());

      sf::Color currentBossColor;
      if (static_cast<int>(m_spawnColorTimer * 2.25f) % 2 == 0)
        currentBossColor =
            sf::Color(255, 255, 255, static_cast<uint8_t>(newAlpha));
      else
        currentBossColor = sf::Color(255, 0, 0, static_cast<uint8_t>(newAlpha));

      m_boss.setColor(currentBossColor);

      if (newAlpha <= 0.0f)
        m_active = false;
    }
    break;
  }
  }
}

//* === DRAW LOGIC ===
void Boss::draw(sf::RenderWindow &window, const GameSettings &settings) {
  if (m_active) {
    window.draw(m_boss);
    if (m_bossPhase == BossPhase::Phase2 &&
        m_attackState == AttackState::Warning)
      window.draw(m_bossWarning);

    if (settings.showHitbox) {
      const auto &handle = getHandleHitboxes();
      sf::CircleShape hitboxCircle;
      hitboxCircle.setFillColor(sf::Color::Transparent);
      hitboxCircle.setOutlineColor(sf::Color::Cyan);
      hitboxCircle.setOutlineThickness(2.0f);

      for (const auto &circle : handle) {
        hitboxCircle.setRadius(circle.radius);
        hitboxCircle.setOrigin({circle.radius, circle.radius});
        hitboxCircle.setPosition(circle.center);
        window.draw(hitboxCircle);
      }

      const auto &brush = getBrushHitboxes();
      hitboxCircle.setOutlineColor(sf::Color::Cyan);
      for (const auto &circle : brush) {
        hitboxCircle.setRadius(circle.radius);
        hitboxCircle.setOrigin({circle.radius, circle.radius});
        hitboxCircle.setPosition(circle.center);
        window.draw(hitboxCircle);
      }
    }
  }
}

sf::FloatRect Boss::getBounds() { return m_boss.getGlobalBounds(); }

void Boss::loseHealth() {
  m_currentHP--;
  if (m_currentHP <= 0) {
    m_active = false;
  }
}

int Boss::getHealth() const { return m_currentHP; }

bool Boss::isAlive() const { return m_active; }

const std::vector<CollisionCircle> &Boss::getHandleHitboxes() {
  m_handleHitboxes.clear();

  sf::Transform t = m_boss.getTransform();
  sf::FloatRect bounds = m_boss.getLocalBounds();

  sf::Vector2f localStart = {bounds.size.x * 0.03f, bounds.size.y * 0.017f};

  sf::Vector2f localEnd = {bounds.size.x * 0.68f, bounds.size.y * 0.77f};

  int circlesCount = 10;
  float radius = 10.0f;

  for (int i = 0; i <= circlesCount; i++) {
    float t_val = static_cast<float>(i) / circlesCount;

    float localX = localStart.x + (localEnd.x - localStart.x) * t_val;
    float localY = localStart.y + (localEnd.y - localStart.y) * t_val;

    sf::Vector2f globalPos = t.transformPoint({localX, localY});

    m_handleHitboxes.push_back({globalPos, radius});
  }

  return m_handleHitboxes;
}

const std::vector<CollisionCircle> &Boss::getBrushHitboxes() {
  m_brushHitboxes.clear();
  sf::Transform t = m_boss.getTransform();
  sf::FloatRect b = m_boss.getLocalBounds();
  float circleStartPointX = 0.565f;
  float circlePadding = 0.165f;
  float circleRadius = 43.0f;
  float tiltY = 0.035f;

  for (int x = 0; x < 3; ++x) {
    sf::Vector2f localPos = {b.size.x * (circleStartPointX + x * circlePadding),
                             b.size.y * (0.93f - (x * tiltY))};

    m_brushHitboxes.push_back({t.transformPoint(localPos), circleRadius});
  }

  return m_brushHitboxes;
}

void Boss::stopSound() {
  m_attackSound.stop();
  m_warningSound.stop();
  m_deathSound.stop();
}

BossPhase Boss::getPhase() { return m_bossPhase; }