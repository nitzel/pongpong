#pragma once
#include <SFML/Graphics.hpp>

// interface for sth controlling a paddle
class IPaddleController {
public:
    // weather it should move up or downwards
    enum Action{
        None = 0,
        Up,
        Down
    };
    // Returns the direction the controller wants to move the paddle
    virtual Action Act(sf::Vector2f ball, sf::Vector2f ballSpeed, sf::FloatRect thisPaddle, sf::FloatRect enemyPaddle) = 0;
};

// AI controlling a paddle. 
class PaddleAI : public IPaddleController {
    Action Act(sf::Vector2f ball, sf::Vector2f ballSpeed, sf::FloatRect thisPaddle, sf::FloatRect enemyPaddle) override;
};

// User controlling a paddle with a keyboard - the two private Keys define which key moves the paddle up/down.
class PaddleKeyboard : public IPaddleController {
private:
    sf::Keyboard::Key m_up;
    sf::Keyboard::Key m_down;
public:
    PaddleKeyboard(sf::Keyboard::Key up, sf::Keyboard::Key down);
    Action Act(sf::Vector2f ball, sf::Vector2f ballSpeed, sf::FloatRect thisPaddle, sf::FloatRect enemyPaddle) override;
};