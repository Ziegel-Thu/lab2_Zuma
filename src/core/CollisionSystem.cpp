#include "CollisionSystem.h"
#include <iostream>
#include "Parameter.h"
#include "Game.h"
#include <algorithm>


void BallList::append(const Ball& ball) {


        BallNode* newNode = new BallNode(ball, tail, tail->prev);
        tail->prev->next = newNode;
        tail->prev = newNode;
    }



void BallList::clear() {
    BallList::BallNode* current = head;
    while (current != tail)
    {
        BallNode* temp = current;
        current = current->next;
        delete temp;
    }
    current = nullptr;
}
void BallList::insert(BallNode* node, const Ball& ball) {
    
    BallNode* newNode = new BallNode(ball, node, node->prev);
    node->prev->next = newNode;
    node->prev = newNode;
}
void BallList::remove(BallNode* node) {

    if (node->prev) {
        node->prev->next = node->next;
    } else {
        head = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    } else {
        tail = node->prev;
    }
    delete node;
}
CollisionSystem::CollisionSystem() {
}

// TODO:done modify BallList
bool CollisionSystem::checkPathCollisions(const Ball& shootingBall, BallList& pathBallList, const Path& path) {
    CollisionResult result = findCollision(shootingBall, pathBallList);
    if (result.hasCollision) {
        handleCollision(shootingBall, pathBallList, path, result.collisionNode);

    }

    return result.hasCollision;
}

// TODO:done modify BallList
CollisionSystem::CollisionResult CollisionSystem::findCollision(
    const Ball& ball, const BallList& pathBallList) const {
    CollisionResult result = {false, nullptr};

    BallList::BallNode* current = pathBallList.head;
    while (current != pathBallList.tail) {
        current=current->next;
        float distance = ball.distanceTo(current->ball);
        if (distance < ball.getRadius() + current->ball.getRadius() + COLLISION_THRESHOLD) {
            result.hasCollision = true;
            result.collisionNode = current;
            break;
        }
    }
    
    
    return result;}


// TODO: done:modify BallList
void CollisionSystem::handleCollision(Ball ball, BallList& ballList, const Path& path, BallList::BallNode* collisionNode) {
    // 获取碰撞点前后的球
    BallList::BallNode* prevNode = collisionNode->prev==ballList.head?collisionNode:collisionNode->prev;
    BallList::BallNode* nextNode = collisionNode->next==ballList.tail?collisionNode:collisionNode->next;


    const Ball& prevBall = prevNode->ball;
    const Ball& nextBall = nextNode->ball;

    float prevBallDistance = ball.distanceTo(prevBall);
    float nextBallDistance = ball.distanceTo(nextBall);

    
    // 计算插入位置
    BallList::BallNode* insertNode = (prevBallDistance < nextBallDistance) ? collisionNode : collisionNode->next;
    if (insertNode != ballList.tail) {
        ball.setPosition(insertNode->ball.getPosition());
    }
    else {
        float currentDistance = path.getDistanceAtPoint(collisionNode->ball.getPosition());
        float newDistance = currentDistance + BALL_RADIUS * 2;
        QPointF newPos = path.getPointAtDistance(newDistance);
        ball.setPosition(newPos);

    }

    BallList::BallNode* current = insertNode;
    while (current&& current!=ballList.tail) {
        float currentDistance = path.getDistanceAtPoint(current->ball.getPosition());
        float newDistance = currentDistance + BALL_RADIUS * 2;
        QPointF newPos = path.getPointAtDistance(newDistance);
        current->ball.setPosition(newPos);  
        if (current->prev != ballList.head) {
            float distance = current->ball.distanceTo(current->next->ball);
            if (distance > BALL_RADIUS * 2 + COLLISION_THRESHOLD) break;
        }
        current = current->next;

    }



    // 插入球
    ball.setVelocity(QPointF(0, 0));
    ballList.insert(insertNode, ball);
}
