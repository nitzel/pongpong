#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <algorithm>
#include "helpers.hpp"
#include "IState.hpp"
#include "StateManager.hpp"
#include "MenuState.hpp"
#include "IPaddleController.hpp"

// keeps track of the score of the left/right player
struct SSCore{
    int left = 0;
    int right = 0;
};

// ptrs to the paddle controllers - exchange depending on game modus
// ai vs ai for background animation while in menu
auto ctrlLeft = std::unique_ptr<IPaddleController>(new PaddleAI());
auto ctrlRight = std::unique_ptr<IPaddleController>(new PaddleAI());

// displays text that rotates around pos in a radius r, starting at angle=relPos
// on spacebar: creates a new TextState element at the same position with different text/radius
// on enter: will delete itself
class TextState : public IState {
private:
    std::shared_ptr<StateManager> stateManager;
    sf::Text text;
    sf::Vector2f position;
    float relativePosition;
    float radius;
    std::string displayText;
public:
    TextState(std::shared_ptr<StateManager> stateManager, std::string sText, const sf::Vector2f pos, const sf::Font *font, float rad, float relPos)
    :   IState("TextState("+sText+")"),
        stateManager(stateManager),
        position(pos),
        radius(rad),
        displayText(sText),
        relativePosition(relPos)
    {                
        text.setFont(*font);
        text.setCharacterSize(40.f);
        text.setString(displayText);
        text.setFillColor(sf::Color::Black);
        text.setOutlineColor(sf::Color::White);
        text.setOutlineThickness(4.);
        text.setPosition(position);
        std::cout << "TextState " << displayText << " initialized" << std::endl;
    }
    bool handleInput(const sf::Event &input){
        std::cout << "TextState::handleInput" << std::endl;
        if(input.type == sf::Event::EventType::KeyReleased && input.key.code == sf::Keyboard::Key::Return)
            dispose();
        else if(input.type == sf::Event::EventType::KeyReleased && input.key.code == sf::Keyboard::Key::Space)
            stateManager->push(std::make_unique<TextState>(stateManager, displayText=="foo"?"bar":"foo", position, text.getFont(), radius+10, relativePosition));
        else if(input.type == sf::Event::EventType::KeyReleased && input.key.code == sf::Keyboard::Key::Tab)
            stateManager->push(std::make_unique<MenuState>(stateManager, text.getFont(), text.getPosition()));
        return false;
    }
    bool update(float elapsedSeconds)
    {
        std::cout << "TextState::update" << std::endl;
        relativePosition += elapsedSeconds * 4;
        auto pos = position + multiply(sf::Vector2f(sin(relativePosition), cos(relativePosition)), radius);
        text.setPosition(pos);
        return false;
    }
    void draw(sf::RenderTarget &canvas){
        std::cout << "TextState::draw" << std::endl;
        std::cout << " -> text is" << text.getString().toAnsiString() << std::endl;
        canvas.draw(text);
    }
};

int main()
{
    ////////////////
    // init
    // constants
    const float SPEED_PADDLE = 200.;
    const float SPEED_BALL = 450.;
    const float MIN_X = 50.f;
    const float MIN_Y = 50.f;
    const float MAX_X = 750.f;
    const float MAX_Y = 550.f;
    const float MID_X = (MAX_X+MIN_X)/2;
    const float MID_Y = (MAX_Y+MIN_Y)/2;

    // setting up sf window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
    // ball
    sf::CircleShape shBall(20.f);
    shBall.setFillColor(sf::Color::Green);
    // left & right paddle
    sf::RectangleShape shPaddleLeft(sf::Vector2f(20.f, 120.f));
    shPaddleLeft.setFillColor(sf::Color::Blue);
    sf::RectangleShape shPaddleRight(shPaddleLeft);
    // score
    sf::Font font;
    font.loadFromFile("./Grand_Aventure.otf");
    sf::Text shScoreText;
    shScoreText.setFont(font);
    shScoreText.setCharacterSize(40.f);
    shScoreText.setOutlineColor(sf::Color::White);
    shScoreText.setOutlineThickness(4.f);
    shScoreText.setFillColor(sf::Color::Black);
    // playground
    sf::RectangleShape fieldBorders(sf::Vector2f(MAX_X-MIN_X, MAX_Y-MIN_Y));
    fieldBorders.setOutlineColor(sf::Color(0xfc, 0x9a, 0x04, 0xa0));
    fieldBorders.setOutlineThickness(1.);
    fieldBorders.setFillColor(sf::Color::Black);
    fieldBorders.setPosition(MIN_X, MIN_Y);

    // position/dir of ball and paddles
    sf::Vector2f posPaddleLeft(MIN_X - shPaddleLeft.getLocalBounds().width, MID_Y);
    sf::Vector2f posPaddleRight(MAX_X, MID_Y);
    sf::Vector2f posBall(MID_X, MID_Y);
    sf::Vector2f dirBall(1,1);
    sf::Vector2f ballRadius(shBall.getRadius(), shBall.getRadius()); 
    // score
    SSCore score;

    //MenuState menuState(font, sf::Vector2f(MID_X, MID_Y-MIN_Y));
    auto stateManager = std::make_shared<StateManager>();
    stateManager->push(std::make_unique<TextState>(stateManager, "BabelBa", sf::Vector2f(MID_X, MID_Y), &font, 100, 0));
    
    //clock
    sf::Clock clock;

    while (window.isOpen() && !stateManager->noStatesLeft())
    {        
        sf::Event event;
        while (window.pollEvent(event))
        {
            stateManager->handleInput(event);
            switch(event.type){
                case sf::Event::Closed:
                    window.close();
                    break;
            }
        }

        //
        float elapsedSeconds = clock.getElapsedTime().asSeconds();
        clock.restart();

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
        // top/bottom border collision
        if ((posBall.y - ballRadius.y <= MIN_Y) 
        ||  (posBall.y + ballRadius.y >= MAX_Y)) {
            dirBall.y *= -1;
        }
        // left paddle collision
        if(dirBall.x < 0 && posBall.x - ballRadius.x <= MIN_X) {
            if(posBall.y+ballRadius.y > posPaddleLeft.y && posBall.y-ballRadius.y < posPaddleLeft.y + shPaddleLeft.getLocalBounds().height){
                dirBall = reflectBallFromPaddle(dirBall, posBall.y, posPaddleLeft.y, shPaddleLeft.getLocalBounds().height);
            }
            else {
                score.right++;
                posBall = sf::Vector2f(MID_X, MID_Y);
            }
        }
        // right paddle collision
        if(dirBall.x > 0 && posBall.x + ballRadius.x >= MAX_X){
            if(posBall.y+ballRadius.y > posPaddleRight.y && posBall.y-ballRadius.y < posPaddleRight.y + shPaddleRight.getLocalBounds().height){
                dirBall = reflectBallFromPaddle(dirBall, posBall.y, posPaddleRight.y, shPaddleRight.getLocalBounds().height);
            }
            else {
                score.left++;
                posBall = sf::Vector2f(MID_X, MID_Y);
            }
        }
        dirBall.y = fmax(fmin(dirBall.y, 0.7), -0.7);
        posBall += multiply(normalize(dirBall), SPEED_BALL*elapsedSeconds);

        stateManager->update(elapsedSeconds);
        

        // update image positions
        shBall.setPosition(posBall-ballRadius);
        shPaddleLeft.setPosition(posPaddleLeft);
        shPaddleRight.setPosition(posPaddleRight);
        shScoreText.setString(std::to_string(score.left) + " :: " + std::to_string(score.right));
        shScoreText.setPosition(MID_X - shScoreText.getLocalBounds().width/2, MIN_Y - shScoreText.getCharacterSize());
        
        // draw screen
        window.clear(sf::Color(0,0,0,10));
        window.draw(fieldBorders);
        window.draw(shBall);
        window.draw(shPaddleLeft);
        window.draw(shPaddleRight);
        window.draw(shScoreText);
        stateManager->draw(window);
        window.display();
    }

    return 0;
}