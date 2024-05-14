#ifndef ITP_BALL_H
#define ITP_BALL_H

#include <SFML/Graphics.hpp>

class Ball {
public:
    sf::CircleShape bubble;
    sf::Vector2f velocity;

    Ball();
    Ball(float radius);
    ~Ball();

    void move();
    void stopMoving();
};

#endif //ITP_BALL_H
