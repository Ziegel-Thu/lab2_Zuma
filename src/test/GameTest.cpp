#include <QtTest>
#include "../src/core/Game.h"
#include "../src/core/Parameter.h"
#include <cstdio>
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
    game.initialize();
    game.getBallList().clear();    
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
    BallList::BallNode* currentNode = game.getBallList().head->next;
    
    verifyBallSequence(game.getBallList(), colors);
    
    Shooter& shooter = const_cast<Shooter&>(game.getShooter());
    shooter.setNextBall(Ball(shooter.getPosition(), COLORS[3]));
    simulateShot(game, QPointF(50 + BALL_RADIUS * 11, 50));
    
    for (int i = 0; i < 180; ++i) {
        game.update(0.016f); // 60fps

    }
    
    QVector<QColor> expectedColors = {COLORS[1], COLORS[1]};
    verifyBallSequence(game.getBallList(), expectedColors);

    shooter.setNextBall(Ball(shooter.getPosition(), COLORS[1]));
    simulateShot(game, QPointF(50 + BALL_RADIUS * 6, 50));
    
    for (int i = 0; i < 180; ++i) {
        game.update(0.016f); // 60fps
 
    }


    expectedColors.clear();
    verifyBallSequence(game.getBallList(), expectedColors);
}

void GameTest::verifyBallSequence(const BallList& ballList, const QVector<QColor>& expectedColors) {
    BallList::BallNode* currentNode = ballList.head->next;
    int index = 0;
    while (currentNode != ballList.tail) {
        if (index >= expectedColors.size()) {
            QFAIL("Ball sequence is longer than expected");
        }
        QCOMPARE(currentNode->ball.getColor(), expectedColors[index]);
        currentNode = currentNode->next;
        index++;
    }
    if (index < expectedColors.size()) {
        QFAIL("Ball sequence is shorter than expected");
    }
}

void GameTest::simulateShot(Game& game, QPointF target) {
    game.handleClick(target);
}

QTEST_MAIN(GameTest)
#include "GameTest.moc"