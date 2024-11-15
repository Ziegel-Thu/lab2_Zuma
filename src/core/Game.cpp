#include "Game.h"
#include <thread>
#include "Parameter.h"
#include <QDebug>
#include <QApplication>
#include <QMessageBox>
#include "CollisionSystem.h"
#include <iostream>

Game::Game(QObject *parent)
    : QObject(parent)
    , shooter(QPointF(300, 300))  // 发射器位置调整到中间
    , score(0)
    , gameRunning(false)
{
}

void Game::initialize() {
    balls.clear();
    shootingBalls.clear();
    score = 0;
    gameRunning = true;
    
    float ballDiameter = BALL_RADIUS * 2;
    float cornerOffset = ballDiameter * 1.5f;
    
    path.clear();
    
    // 起点（左上角）
    path.addPoint(QPointF(50, 50));
    
    // 第一个转角（右上角）
    path.addPoint(QPointF(550 - cornerOffset, 50));
    path.addPoint(QPointF(550, 50 + cornerOffset));
    
    // 第二个转角（右下角，180度）
    path.addPoint(QPointF(550, 550 - cornerOffset));
    path.addPoint(QPointF(550 - cornerOffset * 1, 550));
    path.addPoint(QPointF(550 - cornerOffset * 2, 550));
    path.addPoint(QPointF(550 - cornerOffset * 3, 550 - cornerOffset));
    
    // 第三个转角（右上角靠里）
    path.addPoint(QPointF(550 - cornerOffset * 3, 50 + cornerOffset * 3));
    path.addPoint(QPointF(550 - cornerOffset * 4, 50 + cornerOffset * 2));
    
    // 第四个转角（左上角靠里）
    path.addPoint(QPointF(50 + cornerOffset, 50 + cornerOffset * 2));
    path.addPoint(QPointF(50, 50 + cornerOffset * 3));
    
    // 第五个转角（左下角靠里）
    path.addPoint(QPointF(50, 550 - cornerOffset));
    path.addPoint(QPointF(50 + cornerOffset, 550));
    
    // 终点（画面底部）
    path.addPoint(QPointF(300, 550));
    
    createInitialBalls();
    shooter.setNextBall(Ball(shooter.getPosition(), generateRandomColor()));
}

void Game::update(float deltaTime) {
    if (!gameRunning) return;
    
    // 更新发射的球
    updateShootingBalls(deltaTime);
    
    float pathLength = path.getTotalLength();
    
    // TODO: done: modify BallList
    // 更新路径上的球，从前向后更新
    BallList::BallNode* current = ballList.head;
    while(current->next != ballList.tail) {
        current = current->next;
        float currentDistance = path.getDistanceAtPoint(current->ball.getPosition()); // 计算球在路径上的当前位置
        float newDistance = currentDistance + BALL_SPEED * deltaTime; // 计算球在路径上的新位置
        QPointF newPos = path.getPointAtDistance(newDistance); // 计算球的新位置
        current->ball.setPosition(newPos); // 设置球的新位置
        
        if(current->next != ballList.tail && current->ball.distanceTo(current->next->ball) > BALL_RADIUS * 2 +EPS) {
            break;} // 如果这个球和下一个球之间有空隙，则后面的所有球都不移动

    }
    checkMatches();
    // 检查游戏是否结束
    checkGameOver();
}

void Game::handleClick(const QPointF& point) {
    if (!gameRunning || !shooter.isReady()) return;
    
    // 计算发射角度
    QPointF shooterPos = shooter.getPosition();
    QPointF direction = point - shooterPos;
    float angle = std::atan2(direction.y(), direction.x());
    
    // 设置发射角度并发射
    shooter.setAngle(angle);
    Ball shotBall = shooter.shoot();
    shootingBalls.append(shotBall);
    
    // 准备下一个球
    shooter.setNextBall(Ball(shooter.getPosition(), generateRandomColor()));
}

void Game::updateShootingBalls(float deltaTime) {
    for (int i = shootingBalls.size() - 1; i >= 0; --i) {
        Ball& ball = shootingBalls[i];
        ball.update(deltaTime);
        
        // 检查球是否飞出屏幕范围
        QPointF pos = ball.getPosition();
        float radius = ball.getRadius();
        
        if (pos.x() + radius < SCREEN_MIN_X || 
            pos.x() - radius > SCREEN_MAX_X ||
            pos.y() + radius < SCREEN_MIN_Y || 
            pos.y() - radius > SCREEN_MAX_Y) 
        {
            shootingBalls.removeAt(i);
            continue;
        }
        
        // TODO:done modify BallList
        // 检查碰撞
        bool hasCollision = collisionSystem.checkPathCollisions(ball, ballList, path);
        if (hasCollision) {
            shootingBalls.removeAt(i);
            while(checkMatches())
            {
            };
        }
    }
    checkGameOver();
}

bool Game::checkMatches() { 
    bool match = false;
    if(ballList.head->next == ballList.tail) {
        return match;
    }
    BallList::BallNode* current = ballList.head->next;
    while(current&&current->next&&current->next->next != ballList.tail) {
        QColor color = current->ball.getColor();
        if(current->next->ball.getColor() == color && current->next->next->ball.getColor() == color) {
                if(current->next->next->next!=ballList.tail&&current->next->next->next->ball.getColor()!=color) {
                    current = current->prev;
                    ballList.remove(current->next);
                    current = current->next;
                }
                current = current->prev;
                ballList.remove(current->next);
                ballList.remove(current->next);
                ballList.remove(current->next);
                match = true;
                score+=100;
                emit scoreChanged(score);
        }
        if(!current||current->next == ballList.tail) {
            return match;
            break;
        }
        else {
            current = current->next;
        }
        
    }
    return match;
}

void Game::createInitialBalls() {
    const float ballSpacing = BALL_RADIUS * 2;
    float distance = 0;
    
    
    while (distance < path.getTotalLength() / 2) { // 只创建一半的路径长度
        QPointF pos = path.getPointAtDistance(distance);
        QPointF nextPos = path.getPointAtDistance(distance + 0.1f); // 获取稍远一点的位置来计算方向
        
        // 计算单位方向向量
        QPointF direction = nextPos - pos;
        float length = std::sqrt(QPointF::dotProduct(direction, direction));
        if (length > 0) {
            direction /= length;
        }
        
        // TODO: modify BallList
        // 创建球并设置飞行速度为0
        Ball ball(pos, generateRandomColor());
        ballList.append(ball);
        
        distance += ballSpacing;
    }

}

QColor Game::generateRandomColor() const {
    return COLORS[QRandomGenerator::global()->bounded(COLORS.size())];
}

void Game::checkGameOver() {
    // TODO:done modify BallList
    if (ballList.head->next != ballList.tail) {
        QPointF ballPos = ballList.tail->prev->ball.getPosition();
        QPointF endPoint = path.getPoints().last();
        
        // 计算两点之间的距离
        QPointF diff = ballPos - endPoint;
        float distance = std::sqrt(QPointF::dotProduct(diff, diff));
        
        if (distance < BALL_RADIUS) {
            gameRunning = false;
            emit gameOver();
        }
    }
    else {
        score += 200;
        emit scoreChanged(score);
        gameRunning = false;
        emit gameOver();
    }
}