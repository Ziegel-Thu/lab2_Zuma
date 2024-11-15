#pragma once
#include <QVector>
#include "Ball.h"
#include "Path.h"
class BallList{
public:
    struct BallNode{
        Ball ball;
        BallNode* next;
        BallNode* prev;
        BallNode(const Ball& ball, BallNode* next, BallNode* prev): ball(ball), next(next), prev(prev) {}
    };
    BallNode* tail;
    BallNode* head;

    BallList()   {
        head = new BallNode(Ball(), nullptr, nullptr);
        tail = new BallNode(Ball(), nullptr, head);
        head->next = tail;
    }
    ~BallList() {
        clear();
        delete head;
        delete tail;
    }
    void append(const Ball& ball);
    void clear();
    void insert(BallNode* node, const Ball& ball);
    void remove(BallNode* node);
};

class CollisionSystem {
public:
    CollisionSystem();
    
    bool checkPathCollisions(const Ball& shootingBall, BallList& pathBallList, const Path& path);// TODOdone: modify BallList
    
    struct CollisionResult {
        bool hasCollision;
        BallList::BallNode* collisionNode;
    };
    
private:
    CollisionResult findCollision(const Ball& ball,  const BallList& pathBallList) const;// TODOdone: modify BallList
    void handleCollision(Ball ball,  BallList& pathBallList, const Path& path, BallList::BallNode* collisionNode);// TODOdone: modify BallList
};
