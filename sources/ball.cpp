#include "../headers/ball.h"

Ball::Ball() {}

Ball::Ball(float radius) : bubble(radius) {
    bubble.setFillColor(sf::Color::Blue);
    bubble.setPosition(540.f, 910.f);
    bubble.setOrigin(radius, radius);
    velocity = sf::Vector2f(0.f, 0.f);
}

Ball::~Ball() {}

void Ball::move() {
    bubble.move(velocity);
}

void Ball::stopMoving() {
    velocity = sf::Vector2f(0.f, 0.f);
}