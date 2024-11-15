#include <QtTest>
#include "../src/core/Game.h"
#include "../src/core/Parameter.h"
#include <cstdio>

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
    printf("Initializing game...\n");
    game.initialize();
    game.getBallList().clear();
    
    printf("Creating initial ball sequence...\n");
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
        printf("Appending ball with color: %s at distance: %.2f\n", color.name().toStdString().c_str(), distance);
        game.getBallList().append(ball);
        distance += BALL_RADIUS * 2;
    }
    
    printf("Verifying initial sequence...\n");
    verifyBallSequence(game.getBallList(), colors);
    
    printf("Simulating first shot...\n");
    Shooter& shooter = const_cast<Shooter&>(game.getShooter());
    shooter.setNextBall(Ball(shooter.getPosition(), COLORS[3]));
    simulateShot(game, QPointF(50 + BALL_RADIUS * 11, 50));
    
    printf("Updating game state...\n");
    for (int i = 0; i < 180; ++i) {
        game.update(0.016f); // 60fps
        if (i % 30 == 0) {
            printf("Game state updated: %d frames\n", i);
        }
    }
    
    printf("Verifying sequence after first shot...\n");
    QVector<QColor> expectedColors = {COLORS[1], COLORS[1]};
    verifyBallSequence(game.getBallList(), expectedColors);

    printf("Simulating second shot...\n");
    shooter.setNextBall(Ball(shooter.getPosition(), COLORS[1]));
    simulateShot(game, QPointF(50 + BALL_RADIUS * 6, 50));
    
    printf("Updating game state...\n");
    for (int i = 0; i < 180; ++i) {
        game.update(0.016f); // 60fps
        if (i % 30 == 0) {
            printf("Game state updated: %d frames\n", i);
        }
    }


    printf("Verifying sequence after second shot...\n");
    expectedColors.clear();
    verifyBallSequence(game.getBallList(), expectedColors);
}

void GameTest::verifyBallSequence(const BallList& ballList, const QVector<QColor>& expectedColors) {
    BallList::BallNode* currentNode = ballList.head->next;
    int index = 0;
    while (currentNode != nullptr) {
        if (index >= expectedColors.size()) {
            QFAIL("Ball sequence is longer than expected");
        }
        printf("Verifying ball at index %d with color: %s\n", index, currentNode->ball.getColor().name().toStdString().c_str());
        QCOMPARE(currentNode->ball.getColor(), expectedColors[index]);
        currentNode = currentNode->next;
        index++;
    }
    if (index < expectedColors.size()) {
        QFAIL("Ball sequence is shorter than expected");
    }
}

void GameTest::simulateShot(Game& game, QPointF target) {
    printf("Simulating shot at target: (%.2f, %.2f)\n", target.x(), target.y());
    game.handleClick(target);
}

QTEST_MAIN(GameTest)
#include "GameTest.moc"