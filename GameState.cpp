#include "GameState.hpp"
// contains only the lengthy method-defintions, the shorter ones are in the hpp

GameState::GameState(std::shared_ptr<StateManager> stateManager, sf::Font *font, PaddleModus leftPlayer, PaddleModus rightPlayer, sf::Vector2f screenSize)
:   IState("GameState"),
    stateManager(stateManager),
    font(font),
    screenSize(screenSize)
{
    // calculate field bounds
    field.left = screenSize.x / 16;
    field.right = screenSize.x - field.left;
    field.top = screenSize.y / 12;
    field.bottom = screenSize.y - field.top;

    // set paddle controllers
    if(leftPlayer == PaddleModus::Keyboard)
        ctrlLeft = std::make_unique<PaddleKeyboard>(sf::Keyboard::Key::W,sf::Keyboard::Key::S);
    else 
        ctrlLeft = std::make_unique<PaddleAI>();
    if(rightPlayer == PaddleModus::Keyboard)
        ctrlRight = std::make_unique<PaddleKeyboard>(sf::Keyboard::Key::Up,sf::Keyboard::Key::Down);
    else 
        ctrlRight = std::make_unique<PaddleAI>();
    
    // configure score text
    shScoreText.setFont(*font);
    shScoreText.setCharacterSize(40.f);
    shScoreText.setOutlineColor(sf::Color::White);
    shScoreText.setOutlineThickness(4.f);
    shScoreText.setFillColor(sf::Color::Black);

    // playground
    fieldBorders = sf::RectangleShape(field.getSize());
    fieldBorders.setPosition(sf::Vector2f(field.left, field.top));
    fieldBorders.setOutlineColor(sf::Color(0xfc, 0x9a, 0x04, 0xa0));
    fieldBorders.setOutlineThickness(1.);
    fieldBorders.setFillColor(sf::Color::Black);
        
    // ball
    shBall = sf::CircleShape(20.f); // shape
    shBall.setFillColor(sf::Color::Green);
    posBall = field.getCenter(); // position
    dirBall = sf::Vector2f(1,0); // direction
    ballRadius = sf::Vector2f(shBall.getRadius(), shBall.getRadius()); // radius
    // left & right paddle
    shPaddleLeft = sf::RectangleShape(sf::Vector2f(20.f, 120.f));
    shPaddleLeft.setFillColor(sf::Color::Blue);
    posPaddleLeft = sf::Vector2f(field.left-shPaddleLeft.getLocalBounds().width, field.getCenter().y - shPaddleLeft.getLocalBounds().height/2);
    shPaddleRight = sf::RectangleShape(shPaddleLeft);
    posPaddleRight = sf::Vector2f(field.right, field.getCenter().y - shPaddleRight.getLocalBounds().height/2);
}

bool GameState::update(float elapsedSeconds)
{        
    ///////
    // paddle controller movements
    posPaddleLeft = movePaddle(
        posPaddleLeft,
        ctrlLeft->Act(posBall, dirBall, shPaddleLeft.getGlobalBounds(), shPaddleRight.getGlobalBounds()),
        SPEED_PADDLE,
        elapsedSeconds);
    posPaddleRight = movePaddle(
        posPaddleRight,
        ctrlRight->Act(posBall, dirBall*SPEED_BALL, shPaddleRight.getGlobalBounds(), shPaddleLeft.getGlobalBounds()),
        SPEED_PADDLE,
        elapsedSeconds);
    
    
    ///////
    // ball movement
    // top/bottom border collision. only bounce if the direction of travle is directed further outside the
    // boundaries. otherwise if the ball slips over the boundary (lag?) it will stay there, since it bounces
    // every frame
    if ((posBall.y - ballRadius.y <= field.top && dirBall.y < 0) 
    ||  (posBall.y + ballRadius.y >= field.bottom && dirBall.y > 0)) {
        dirBall.y *= -1;
    }
    // left paddle collision
    if(dirBall.x < 0 && posBall.x - ballRadius.x <= field.left) {
        if(posBall.y+ballRadius.y > posPaddleLeft.y && posBall.y-ballRadius.y < posPaddleLeft.y + shPaddleLeft.getLocalBounds().height){
            dirBall = reflectBallFromPaddle(dirBall, posBall.y, posPaddleLeft.y, shPaddleLeft.getLocalBounds().height);
        }
        else {
            score.right++;
            dirBall = sf::Vector2f(1, 0); // towards the score gainer
            posBall = field.getCenter();
        }
    }
    // right paddle collision
    if(dirBall.x > 0 && posBall.x + ballRadius.x >= field.right){
        if(posBall.y+ballRadius.y > posPaddleRight.y && posBall.y-ballRadius.y < posPaddleRight.y + shPaddleRight.getLocalBounds().height){
            dirBall = reflectBallFromPaddle(dirBall, posBall.y, posPaddleRight.y, shPaddleRight.getLocalBounds().height);
        }
        else {
            score.left++;
            dirBall = sf::Vector2f(-1, 0); // towards score gainer
            posBall = field.getCenter();
        }
    }
    dirBall.y = fmax(fmin(dirBall.y, 0.7), -0.7);
    posBall += multiply(normalize(dirBall), SPEED_BALL*elapsedSeconds);

    // update sprite positions
    shBall.setPosition(posBall-ballRadius);
    shPaddleLeft.setPosition(posPaddleLeft);
    shPaddleRight.setPosition(posPaddleRight);
    shScoreText.setString(std::to_string(score.left) + " : " + std::to_string(score.right));
    sf::Vector2f scorePos(field.getCenter().x - shScoreText.getLocalBounds().width/2, field.top - shScoreText.getCharacterSize());
    shScoreText.setPosition(scorePos);
    return false;
}