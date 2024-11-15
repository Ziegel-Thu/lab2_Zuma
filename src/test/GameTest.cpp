#include <QtTest>
#include "../src/core/Game.h"
#include "../src/core/Parameter.h"
#include <iostream>
class GameTest : public QObject {
    Q_OBJECT
    
private slots:
    void testBallCollisionAndMatch();
    
private:
    void verifyBallSequence(const BallList& ballList, const QVector<QColor>& expectedColors);
    void simulateShot(Game& game, QPointF target);
};

void GameTest::testBallCollisionAndMatch() {
    Game game;
    std::cout << "Initializing game..." << std::endl;
    game.initialize();
    game.getBallList().clear();
    
    std::cout << "Creating initial ball sequence..." << std::endl;
    BallList ballList;
    QVector<QColor> colors = {
        COLORS[1],
        COLORS[0], 
        COLORS[0], 
        COLORS[1], 
        COLORS[1], 
        COLORS[2], 
        COLORS[3], 
        COLORS[3],
        COLORS[2], 
        COLORS[2], 
        COLORS[1], 
        COLORS[1], 
        COLORS[0],
        COLORS[1] 
    };
    float distance = 0;
    for (const QColor& color : colors) {
        Ball ball(QPointF(50 + distance, 50), color);
        game.getBallList().append(ball);
        distance += BALL_RADIUS * 2;
    }
    
    std::cout << "Verifying initial sequence..." << std::endl;
    verifyBallSequence(game.getBallList(), colors);
    
    std::cout << "Simulating first shot..." << std::endl;
    Shooter& shooter = const_cast<Shooter&>(game.getShooter());
    shooter.setNextBall(Ball(shooter.getPosition(), COLORS[3]));
    simulateShot(game, QPointF(50 + BALL_RADIUS * 11, 50));
    
    std::cout << "Updating game state..." << std::endl;
    for (int i = 0; i < 180; ++i) {
        game.update(0.016f); // 60fps
    }
    
    std::cout << "Verifying sequence after first shot..." << std::endl;
    QVector<QColor> expectedColors = {COLORS[1], COLORS[1]};
    verifyBallSequence(game.getBallList(), expectedColors);

    std::cout << "Simulating second shot..." << std::endl;
    shooter.setNextBall(Ball(shooter.getPosition(), COLORS[1]));
    simulateShot(game, QPointF(50 + BALL_RADIUS * 6, 50));
    
    std::cout << "Updating game state..." << std::endl;
    for (int i = 0; i < 180; ++i) {
        game.update(0.016f); // 60fps
    }

    std::cout << "Verifying sequence after second shot..." << std::endl;
    expectedColors.clear();
    verifyBallSequence(game.getBallList(), expectedColors);
}

void GameTest::verifyBallSequence(const BallList& ballList, const QVector<QColor>& expectedColors) {
    BallList::BallNode* currentNode = ballList.head->next;
    for (int i = 0; i < expectedColors.size(); ++i) {
        if (currentNode == nullptr) {
            QFAIL("Ball sequence is shorter than expected");
        }
        QCOMPARE(currentNode->ball.getColor(), expectedColors[i]);
        currentNode = currentNode->next;
    }
    if (currentNode != nullptr) {
        QFAIL("Ball sequence is longer than expected");
    }
}

void GameTest::simulateShot(Game& game, QPointF target) {
    game.handleClick(target);
}

QTEST_MAIN(GameTest)
#include "GameTest.moc"