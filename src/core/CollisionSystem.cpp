#include "CollisionSystem.h"

#include "Parameter.h"
#include "Game.h"
#include <algorithm>

CollisionSystem::CollisionSystem() {
}

// TODO: modify BallList
bool CollisionSystem::checkPathCollisions(const Ball& shootingBall, QVector<Ball>& pathBalls, const Path& path) {
    CollisionResult result = findCollision(shootingBall, pathBalls);
    
    if (result.hasCollision) {
        handleCollision(shootingBall, pathBalls, path, result.collisionIndex);
    }

    return result.hasCollision;
}

// TODO: modify BallList
CollisionSystem::CollisionResult CollisionSystem::findCollision(
    const Ball& ball, const QVector<Ball>& balls) const {
    CollisionResult result = {false, -1};
    
    for (int i = 0; i < balls.size(); ++i) {
        float distance = ball.distanceTo(balls[i]);
        if (distance < ball.getRadius() + balls[i].getRadius() + COLLISION_THRESHOLD) {
            result.hasCollision = true;
            result.collisionIndex = i;
            break;
        }
    }
    
    return result;
}

// TODO: modify BallList
void CollisionSystem::handleCollision(Ball ball, QVector<Ball>& balls, const Path& path, const int& collisionIndex) {
    // 获取碰撞点前后的球
    int prevIndex = (collisionIndex - 1 >= 0) ? collisionIndex - 1 : collisionIndex;
    int nextIndex = (collisionIndex + 1 < balls.size()) ? collisionIndex + 1 : collisionIndex;

    const Ball& prevBall = balls[prevIndex];
    const Ball& nextBall = balls[nextIndex];

    float prevBallDistance = ball.distanceTo(prevBall);
    float nextBallDistance = ball.distanceTo(nextBall);
    
    // 计算插入位置
    int insertIndex = (prevBallDistance < nextBallDistance) ? collisionIndex : collisionIndex + 1;
    if (insertIndex < balls.size()) {
        ball.setPosition(balls[insertIndex].getPosition());
    }
    else {
        float currentDistance = path.getDistanceAtPoint(balls[collisionIndex].getPosition());
        float newDistance = currentDistance + BALL_RADIUS * 2;
        QPointF newPos = path.getPointAtDistance(newDistance);
        ball.setPosition(newPos);
    }
    for (int i = insertIndex; i < balls.size(); ++i) {
        float currentDistance = path.getDistanceAtPoint(balls[i].getPosition());
        float newDistance = currentDistance + BALL_RADIUS * 2;
        QPointF newPos = path.getPointAtDistance(newDistance);
        balls[i].setPosition(newPos);
        if(i < balls.size() - 1) {
            float distance = balls[i].distanceTo(balls[i+1]);
            if (distance > BALL_RADIUS * 2 + EPS) break;
        }
    }

    // 插入球
    ball.setVelocity(QPointF(0, 0));
    balls.insert(insertIndex, ball);
}
