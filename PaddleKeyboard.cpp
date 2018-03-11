#include "IPaddleController.hpp"

PaddleKeyboard::PaddleKeyboard(sf::Keyboard::Key up, sf::Keyboard::Key down)
:   m_up(up), 
    m_down(down)
{
}

PaddleKeyboard::Action PaddleKeyboard::Act(sf::Vector2f ball, sf::Vector2f ballSpeed, sf::FloatRect thisPaddle, sf::FloatRect enemyPaddle) {
    if(sf::Keyboard::isKeyPressed(m_down)){
        return Action::Down;
    }
    if(sf::Keyboard::isKeyPressed(m_up)){
        return Action::Up;
    }
    return Action::None;
}