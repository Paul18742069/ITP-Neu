#ifndef ITP_GAME_H
#define ITP_GAME_H

#include <SFML/Graphics.hpp>
#include "ball.h"
#include <cmath>
#include <random>

class Game {
public:
    sf::RenderWindow window;
    Ball ball;
    Ball secondBall;
    std::mt19937 gen;
    std::uniform_int_distribution<> distr;
    bool newBallSpawned;

    Game();
    ~Game();

    void run();
    void checkAndPopBubbles(bool& isMoving);
    bool isTouching(const Ball& ball1, const Ball& ball2);
    void spawnNewBall();
    void findMatchingBalls(int row, int col, sf::Color color, std::vector<std::pair<int, int>>& toPop); // Add this line
};

#endif //ITP_GAME_H