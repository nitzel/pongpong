#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>

// normalize a vector, if it's length is null the same vector is returned!
sf::Vector2f normalize(const sf::Vector2f vec){
    auto sqrLen = vec.x*vec.x + vec.y*vec.y;
    if(sqrLen){
        auto len = sqrt(sqrLen);
        return sf::Vector2f(vec.x/len, vec.y/len);
    }
    return sf::Vector2f(vec);
}

// multiply a vector with a scalar
sf::Vector2f multiply(const sf::Vector2f vec, const float scalar){
    return sf::Vector2f(vec.x * scalar, vec.y * scalar);
}

// Reflects the ball at height ballY from the paddle defined by paddleY to paddleY+paddleHeight
// so that the horizontal direction of the ball reverses but also the vertical aspect changes
// depending on where the paddle was hit (further to the border of the paddle means more speed in that direction)
sf::Vector2f reflectBallFromPaddle(sf::Vector2f ballDir, float ballY, float paddleY, float paddleHeight){
    auto offset = ballY - (paddleY + paddleHeight/2);
    auto relativeOffset = offset / (paddleHeight/2);
    auto relativeOffsetSquaredSigned = relativeOffset * fabs(relativeOffset) * fabs(relativeOffset);
    return sf::Vector2f(-ballDir.x, ballDir.y + relativeOffsetSquaredSigned);
}

struct SSCore{
    int left = 0;
    int right = 0;
};

class State {
    private:
        bool disposed = false;
    public:
        /* Handle input, if it should not bubble, return false. */
        virtual bool handleInput(const sf::Event &input) = 0;
        virtual void update(float elapsedSeconds) = 0;
        virtual void draw(sf::RenderWindow *window) = 0;
        const virtual void dispose() {
            disposed = true;
        };
};
class MenuState : State {
    private:
        int selectedEntry = 0;
        std::string entries[2] = {"Play", "Exit"};
        std::vector<sf::Text> shTexts;
    public:
        MenuState(sf::Font &font, sf::Vector2f position){
            for(auto entry : entries){
                sf::Text text;
                text.setFont(font);
                text.setCharacterSize(40.f);
                text.setString(entry);
                text.setFillColor(sf::Color::Black);
                text.setOutlineColor(sf::Color::White);
                text.setOutlineThickness(4.);
                auto bounds = text.getLocalBounds();
                position += sf::Vector2f(0, bounds.height*1.5);
                text.setPosition(position + sf::Vector2f(-bounds.width/2, 0));
                shTexts.push_back(text);
            }
        }
        bool handleInput(const sf::Event &event){
            switch(event.type){
                case sf::Event::KeyPressed: {
                    switch(event.key.code){
                        case sf::Keyboard::Escape:
                            dispose();
                            break;
                        case sf::Keyboard::Down:
                            shTexts[selectedEntry].setFillColor(sf::Color::Black);
                            selectedEntry = (selectedEntry + 1) % shTexts.size();
                            shTexts[selectedEntry].setFillColor(sf::Color::Green);
                            break;
                        case sf::Keyboard::Up:
                            shTexts[selectedEntry].setFillColor(sf::Color::Black);
                            selectedEntry = (selectedEntry - 1) % shTexts.size();
                            shTexts[selectedEntry].setFillColor(sf::Color::Green);
                            break;
                        case sf::Keyboard::Return:
                            std::cout << "Selected " << entries[selectedEntry] << std::endl;
                    }
                break;
                }
            }
        }

        void update(float elapsedSeconds){            
        }
        void draw(sf::RenderWindow *window){
            for(auto entry : shTexts){
                window->draw(entry);
            }
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
    const float MID_X = (MAX_X-MIN_X)/2;
    const float MID_Y = (MAX_Y-MIN_Y)/2;

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
    sf::Vector2f dirBall(1,0);
    sf::Vector2f ballRadius(shBall.getRadius(), shBall.getRadius()); 
    // score
    SSCore score;

    MenuState menuState(font, sf::Vector2f(MID_X, MID_Y));
    
    //clock
    sf::Clock clock;

    while (window.isOpen())
    {        
        sf::Event event;
        while (window.pollEvent(event))
        {
            bool canBubble = true;
            if(canBubble){
                canBubble = menuState.handleInput(event);
            }
            switch(event.type){
                case sf::Event::Closed:
                    window.close();
                    canBubble = false;
                    break;
                case sf::Event::KeyPressed:
                    if(event.key.code == sf::Keyboard::Escape)
                        window.close();
                        canBubble = false;
                    break;
            }
        }

        //
        float elapsedTime = clock.getElapsedTime().asSeconds();
        clock.restart();

        ///////
        // user paddle movements
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)){
            posPaddleRight.y += SPEED_PADDLE * elapsedTime;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)){
            posPaddleRight.y -= SPEED_PADDLE * elapsedTime;
        }
        posPaddleRight.y = fmax(posPaddleRight.y, MIN_Y);
        posPaddleRight.y = fmin(posPaddleRight.y, MAX_Y-shPaddleRight.getLocalBounds().height);
        // AI paddle movements
        if(posBall.y > posPaddleLeft.y + shPaddleLeft.getLocalBounds().height/2)
            posPaddleLeft.y += SPEED_PADDLE * elapsedTime;
        if (posBall.y < posPaddleLeft.y + shPaddleLeft.getLocalBounds().height/2)
            posPaddleLeft.y -= SPEED_PADDLE * elapsedTime;
        
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
        posBall += multiply(normalize(dirBall), SPEED_BALL*elapsedTime);

        menuState.update(elapsedTime);
        

        // update image positions
        shBall.setPosition(posBall-ballRadius);
        shPaddleLeft.setPosition(posPaddleLeft);
        shPaddleRight.setPosition(posPaddleRight);
        shScoreText.setString(std::to_string(score.left) + " :: " + std::to_string(score.right));
        shScoreText.setPosition(MID_X, MIN_Y - shScoreText.getCharacterSize());
        
        // draw screen
        window.clear(sf::Color(0,0,0,10));
        window.draw(fieldBorders);
        window.draw(shBall);
        window.draw(shPaddleLeft);
        window.draw(shPaddleRight);
        window.draw(shScoreText);
        menuState.draw(&window);
        window.display();
    }

    return 0;
}