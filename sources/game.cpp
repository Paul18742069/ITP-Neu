#include "../headers/game.h"
#include <array>
#include <iostream>

std::array<sf::Color, 6> colors = {sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan};

std::vector<Ball> balls;

Game::Game() : ball(25.f), secondBall(25.f), window(sf::VideoMode(1080, 960), "SFML works!") {
    //secondBall.bubble.setPosition(25.f, 25.f);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, colors.size() - 1);

    const int numRows = 8; // Anzahl der Reihen
    const int numCols = 21; // Anzahl der Bubbles in jeder Reihe

    for(int row = 0; row < numRows; row++) {
        for(int col = 0; col < numCols; col++) {
            Ball newBall(25.f);
            int colorIndex = distr(gen);
            if(colorIndex >= 0 && colorIndex < colors.size()) {
                newBall.bubble.setFillColor(colors[colorIndex]);
            } else {
                std::cerr << "Error: colorIndex (" << colorIndex << ") is out of range [0, " << colors.size() << ")" << std::endl;
            }
            float offsetX = (row % 2 == 0) ? 0.f : 25.f; // Versatz alle zwei Reihen
            newBall.bubble.setPosition(col * 50.f + offsetX + 25, row * 50.f + 25.f);
            balls.push_back(newBall);
        }
    }
}

Game::~Game() {}

void Game::run() {
    const float speed = 0.5f;

    sf::VertexArray arrow(sf::Lines, 2);
    arrow[0].color = sf::Color::Blue;
    arrow[1].color = sf::Color::Blue;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f direction = sf::Vector2f(mousePosition) - ball.bubble.getPosition();
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        const float arrowLength = 200.f;
        direction /= length;
        sf::Vector2f arrowTip = ball.bubble.getPosition() + direction * arrowLength; // Länge des Pfeils
        arrow[0].position = ball.bubble.getPosition();
        arrow[1].position = arrowTip;

        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f direction = sf::Vector2f(mousePos) - ball.bubble.getPosition();
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            direction.x /= length;
            direction.y /= length;
            ball.velocity = direction * speed;
            newBallSpawned = false;
        }

        ball.move();
        bool isMoving = true;
        checkAndPopBubbles(isMoving);

        if(!isMoving && !newBallSpawned) {
            balls.push_back(ball);
            spawnNewBall();
            newBallSpawned = true;
        }

        if(ball.bubble.getPosition().x - ball.bubble.getRadius() < 0 || ball.bubble.getPosition().x + ball.bubble.getRadius() > window.getSize().x) {
            ball.velocity.x = -ball.velocity.x;
        }
        if(ball.bubble.getPosition().y - ball.bubble.getRadius() < 0 || ball.bubble.getPosition().y + ball.bubble.getRadius() > window.getSize().y) {
            ball.velocity.y = -ball.velocity.y;
        }

        window.clear();
        window.draw(arrow);
        window.draw(ball.bubble);
        for(auto& ball : balls) {
            window.draw(ball.bubble);
        }
        window.display();
    }
}

void Game::checkAndPopBubbles(bool& isMoving) {
    for(int i = balls.size() - 1; i >= 0; i--) {
        if(isTouching(ball,  balls[i])) {
            ball.stopMoving();
            isMoving = false;

            if(ball.bubble.getFillColor() == balls[i].bubble.getFillColor()) {
                balls.erase(balls.begin() + i);
                return;
            }
        }
    }
}

bool Game::isTouching(const Ball &ball1, const Ball &ball2) {
    sf::Vector2f diff = ball1.bubble.getPosition() - ball2.bubble.getPosition();
    float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    return distance <= ball1.bubble.getRadius() + ball2.bubble.getRadius();
}

void Game::spawnNewBall() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, colors.size() - 1);

    Ball newBall(25.f);
    int colorIndex = distr(gen);
    if(colorIndex >= 0 && colorIndex < colors.size()) {
        newBall.bubble.setFillColor(colors[colorIndex]);
        newBall.bubble.setPosition(540.f, 910.f);
        balls.push_back(newBall); // Neu gespawnten Ball zu den anderen Bällen hinzufügen

        // Setze den neu gespawnten Ball als den schießbaren Ball
        ball = newBall;
    } else {
        std::cerr << "Error: colorIndex (" << colorIndex << ") is out of range [0, " << colors.size() << ")" << std::endl;
    }
}