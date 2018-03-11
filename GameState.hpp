#pragma once
#include "IState.hpp"
#include "StateManager.hpp"
#include "IPaddleController.hpp"
#include "helpers.hpp"
#include <memory>

// Contains the game state, logic and visualization
// Two paddles, a rectangle, a ball. The ball is reflected
// within the rectangle and must not touch the left/right side
// of the rectangle. To avoid that, the two paddles are steered
// by a user/AI and reflect the ball before touching the borders.
// if the ball touches one side the other side's player gains a score.
class GameState : public IState {
private:
    const float SPEED_PADDLE = 200.;
    const float SPEED_BALL = 450.;

    // keeps track of the score of the left/right player
    struct {
        int left = 0;
        int right = 0;
    } score;

    struct {
        float left;
        float right;
        float top;
        float bottom;
        // get the center of the field
        sf::Vector2f getCenter() const {
            return sf::Vector2f((left + right) / 2, (top + bottom) / 2);
        }
        // get the width/height of the field
        sf::Vector2f getSize() const {
            return sf::Vector2f(right - left, bottom - top);
        }
    } field;

    std::shared_ptr<StateManager> stateManager;    
    // ptrs to the paddle controllers - exchange depending on game modus
    // ai vs ai for background animation while in menu
    std::unique_ptr<IPaddleController> ctrlLeft;
    std::unique_ptr<IPaddleController> ctrlRight;

    sf::Text shScoreText;
    // playground
    sf::RectangleShape fieldBorders;

    // ball
    sf::CircleShape shBall;
    // left & right paddle
    sf::RectangleShape shPaddleLeft;
    sf::RectangleShape shPaddleRight;

    // position/dir/radius of ball
    sf::Vector2f posBall;
    sf::Vector2f dirBall;
    sf::Vector2f ballRadius;
    // position of paddles
    sf::Vector2f posPaddleLeft;
    sf::Vector2f posPaddleRight;

public:
    enum PaddleModus {
        Keyboard = 0,
        AI
    };

    GameState(std::shared_ptr<StateManager> stateManager, sf::Font *font, PaddleModus leftPlayer, PaddleModus rightPlayer, sf::Vector2f screenSize);
    // pause on escape
    bool handleInput(const sf::Event &input){
        if(input.type == sf::Event::EventType::KeyReleased && input.key.code == sf::Keyboard::Key::Escape)
            dispose(); // todo push pause state
    }
    // process keyboard input etc. yes, i know.
    bool update(float elapsedSeconds);
    
    void draw(sf::RenderTarget &canvas){
        canvas.draw(fieldBorders);
        canvas.draw(shBall);
        canvas.draw(shPaddleLeft);
        canvas.draw(shPaddleRight);
        canvas.draw(shScoreText);
    }
    // dont allow background states to be drawn
    bool drawNextState() override {
        return false;
    }
};