#include "../headers/game.h"
#include <array>
#include <iostream>
#include <set>

std::array<sf::Color, 6> colors = {sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan};

std::vector<std::vector<Ball*>> grid; // Grid-System

Game::Game() : ball(25.f), secondBall(25.f), window(sf::VideoMode(1080, 960), "Bubble Shooter") {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, colors.size() - 1);

    const int numRows = 20; // Anzahl der Reihen
    const int numCols = 21; // Anzahl der Bubbles in jeder Reihe

    grid.resize(numRows, std::vector<Ball*>(numCols, nullptr)); // Grid initialisieren

    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < numCols; col++) {
            Ball* newBall = new Ball(25.f);
            int colorIndex = distr(gen);
            if(colorIndex >= 0 && colorIndex < colors.size()) {
                newBall->bubble.setFillColor(colors[colorIndex]);
            } else {
                std::cerr << "Error: colorIndex (" << colorIndex << ") is out of range [0, " << colors.size() << ")" << std::endl;
            }
            float offsetX = (row % 2 == 0) ? 0.f : 25.f; // Versatz alle zwei Reihen
            newBall->bubble.setPosition(col * 50.f + offsetX + 25, row * 50.f + 25.f);
            grid[row][col] = newBall;
        }
    }
}

Game::~Game() {
    for(auto& row : grid) {
        for(auto& cell : row) {
            delete cell;
        }
    }
}

void Game::run() {
    const float speed = 0.5f;

    sf::VertexArray arrow(sf::Lines, 2);
    arrow[0].color = sf::Color::Blue;
    arrow[1].color = sf::Color::Blue;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f direction = sf::Vector2f(mousePosition) - ball.bubble.getPosition();
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        const float arrowLength = 200.f;
        direction /= length;
        sf::Vector2f arrowTip = ball.bubble.getPosition() + direction * arrowLength;
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

        if (!isMoving && !newBallSpawned) {
            spawnNewBall();
            newBallSpawned = true;
        } else if (!isMoving) {
            int gridY = static_cast<int>(std::round(ball.bubble.getPosition().y / 50.f));
            int gridX = static_cast<int>((ball.bubble.getPosition().x - ((gridY % 2 == 0) ? 25.f : 0.f)) / 50.f);

            if(gridX >= 0 && gridX < grid[0].size() && gridY >= 0 && gridY < grid.size()) {
                if(grid[gridY][gridX] != nullptr) {
                    delete grid[gridY][gridX];
                }
                grid[gridY][gridX] = new Ball(ball);

                float offsetX = (gridY % 2 == 0) ? 0.f : 25.f; // Versatz alle zwei Reihen
                grid[gridY][gridX]->bubble.setPosition(gridX * 50.f + offsetX + 25, gridY * 50.f + 25.f);
            }
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
        for(auto& row : grid) {
            for(auto& cell : row) {
                if(cell != nullptr) {
                    window.draw(cell->bubble);
                }
            }
        }
        window.display();
    }
}

void Game::checkAndPopBubbles(bool& isMoving) {
    const int numRows = grid.size();
    const int numCols = grid[0].size();

    for(int row = 0; row < numRows; row++) {
        for(int col = 0; col < numCols; col++) {
            if(grid[row][col] != nullptr && isTouching(ball, *grid[row][col])) {
                ball.stopMoving();
                isMoving = false;

                std::vector<std::pair<int, int>> toPop;
                findMatchingBalls(row, col, ball.bubble.getFillColor(), toPop);
                if (toPop.size() >= 2) { // +1 für die geschossene Blase
                    toPop.push_back({row, col});
                    for(auto& pos : toPop) {
                        delete grid[pos.first][pos.second];
                        grid[pos.first][pos.second] = nullptr;
                    }
                    spawnNewBall();
                } else {
                    int gridY = static_cast<int>(std::round(ball.bubble.getPosition().y / 50.f));
                    int gridX = static_cast<int>((ball.bubble.getPosition().x - ((gridY % 2 == 0) ? 25.f : 0.f)) / 50.f);

                    if(gridX >= 0 && gridX < numCols && gridY >= 0 && gridY < numRows) {
                        if(grid[gridY][gridX] != nullptr) {
                            delete grid[gridY][gridX];
                        }
                        grid[gridY][gridX] = new Ball(ball);

                        float offsetX = (gridY % 2 == 0) ? 0.f : 25.f; // Versatz alle zwei Reihen
                        // Versatz alle zwei Reihen
                        grid[gridY][gridX]->bubble.setPosition(gridX * 50.f + offsetX + 25, gridY * 50.f + 25.f);
                    }
                }
                return;
            }
        }
    }
}

bool Game::isTouching(const Ball &ball1, const Ball &ball2) {
    sf::Vector2f diff = ball1.bubble.getPosition() - ball2.bubble.getPosition();
    float distanceSquared = diff.x * diff.x + diff.y * diff.y;
    float radiusSumSquared = (ball1.bubble.getRadius() + ball2.bubble.getRadius()) * (ball1.bubble.getRadius() + ball2.bubble.getRadius());

    // Überprüfen, ob die Distanz zwischen den Bällen kleiner oder gleich der Summe ihrer Radien ist
    return distanceSquared <= radiusSumSquared;
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
        ball = newBall;
    } else {
        std::cerr << "Error: colorIndex (" << colorIndex << ") is out of range [0, " << colors.size() << ")" << std::endl;
    }
}

void Game::findMatchingBalls(int row, int col, sf::Color color, std::vector<std::pair<int, int>>& toPop) {
    // Alle acht Richtungen berücksichtigen (horizontal, vertikal, diagonal)
    std::vector<std::pair<int, int>> directions = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };

    std::set<std::pair<int, int>> visited;
    std::vector<std::pair<int, int>> stack = {{row, col}};

    while (!stack.empty()) {
        auto [currentRow, currentCol] = stack.back();
        stack.pop_back();

        if (visited.find({currentRow, currentCol}) != visited.end())
            continue;

        visited.insert({currentRow, currentCol});
        toPop.push_back({currentRow, currentCol});

        for(auto& dir : directions) {
            int newRow = currentRow + dir.first;
            int newCol = currentCol + dir.second;

            if(newRow >= 0 && newRow < grid.size() && newCol >= 0 && newCol < grid[0].size()) {
                if(grid[newRow][newCol] != nullptr && grid[newRow][newCol]->bubble.getFillColor() == color) {
                    stack.push_back({newRow, newCol});
                }
            }
        }
    }
}
