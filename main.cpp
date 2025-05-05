#include <SFML/Graphics.hpp>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <string>

const int WIDTH = 800;
const int HEIGHT = 600;
const int TILE_SIZE = 20;

struct SnakeSegment {
    int x, y;
    SnakeSegment(int x, int y) : x(x), y(y) {}
};

class Snake {
public:
    std::deque<SnakeSegment> body;
    int dx = 1, dy = 0;
    bool shouldGrow = false;

    Snake() {
        body.emplace_back(5, 5);
    }

    void move() {
        int newX = body.front().x + dx;
        int newY = body.front().y + dy;
        body.push_front(SnakeSegment(newX, newY));
        if (!shouldGrow)
            body.pop_back();
        else
            shouldGrow = false;
    }

    void grow() {
        shouldGrow = true;
    }

    bool checkCollision() {
        auto head = body.front();
        if (head.x < 0 || head.x >= WIDTH / TILE_SIZE || head.y < 0 || head.y >= HEIGHT / TILE_SIZE)
            return true;
        for (size_t i = 1; i < body.size(); ++i)
            if (head.x == body[i].x && head.y == body[i].y)
                return true;
        return false;
    }
};

sf::Vector2i generateFood(const Snake& snake) {
    sf::Vector2i food;
    bool valid;
    do {
        valid = true;
        food.x = std::rand() % (WIDTH / TILE_SIZE);
        food.y = std::rand() % (HEIGHT / TILE_SIZE);
        for (auto& segment : snake.body)
            if (segment.x == food.x && segment.y == food.y)
                valid = false;
    } while (!valid);
    return food;
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML Snake Game");
    window.setFramerateLimit(10);

    Snake snake;
    sf::Vector2i food = generateFood(snake);
    int score = 0;
    bool gameOver = false;

    sf::Font font;
    if (!font.loadFromFile("OpenSans-Regular.ttf")) return -1;

    sf::Text scoreText("Score: 0", font, 20);
    scoreText.setPosition(10, 10);
    scoreText.setFillColor(sf::Color::Black);

    sf::Text playerName("Player: Fresius Jane Avila", font, 20);  // Change name as needed
    playerName.setPosition(500, 10);
    playerName.setFillColor(sf::Color::Black);

    sf::Text gameOverText("GAME OVER", font, 60);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition(220, 250);

    sf::Texture logoTexture;
    sf::Sprite logoSprite;
    if (logoTexture.loadFromFile("fj.png")) {
        logoSprite.setTexture(logoTexture);
        logoSprite.setPosition(0, 0);  // Adjust as needed
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        if (!gameOver) {
            // Handle input
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && snake.dy == 0) { snake.dx = 0; snake.dy = -1; }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && snake.dy == 0) { snake.dx = 0; snake.dy = 1; }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && snake.dx == 0) { snake.dx = -1; snake.dy = 0; }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && snake.dx == 0) { snake.dx = 1; snake.dy = 0; }

            snake.move();

            if (snake.body.front().x == food.x && snake.body.front().y == food.y) {
                snake.grow();
                food = generateFood(snake);
                score += 1;
                scoreText.setString("Score: " + std::to_string(score));
            }

            gameOver = snake.checkCollision();
        }

        window.clear(sf::Color::Black);

        // Draw logo
        if (logoTexture.getSize().x > 0)
            window.draw(logoSprite);

        window.draw(scoreText);
        window.draw(playerName);

        // Draw food
        sf::RectangleShape foodRect(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        foodRect.setFillColor(sf::Color::Red);
        foodRect.setPosition(food.x * TILE_SIZE, food.y * TILE_SIZE);
        window.draw(foodRect);

        // Draw snake
        for (auto& segment : snake.body) {
            sf::RectangleShape rect(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            rect.setFillColor(sf::Color::Green);
            rect.setPosition(segment.x * TILE_SIZE, segment.y * TILE_SIZE);
            window.draw(rect);
        }

        if (gameOver)
            window.draw(gameOverText);

        window.display();
    }

    return 0;
}
