#pragma once
#include <QVector>
#include "Ball.h"
#include "Path.h"

class CollisionSystem {
public:
    CollisionSystem();
    
    bool checkPathCollisions(const Ball& shootingBall, QVector<Ball>& pathBalls, const Path& path);// TODO: modify BallList
    
    struct CollisionResult {
        bool hasCollision;
        int collisionIndex;
    };
    
private:
    CollisionResult findCollision(const Ball& ball, const QVector<Ball>& balls) const;// TODO: modify BallList
    void handleCollision(Ball ball, QVector<Ball>& balls, const Path& path, const int& collisionIndex);// TODO: modify BallList
};