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


// interface for sth controlling a paddle
class PaddleController {
public:
    enum Action{
        None = 0,
        Up,
        Down
    };
    // Returns the direction the controller wants to move the paddle
    virtual Action Act(sf::Vector2f ball, sf::Vector2f ballSpeed, sf::FloatRect thisPaddle, sf::FloatRect enemyPaddle) = 0;
};
class PaddleAI : public PaddleController {
    Action Act(sf::Vector2f ball, sf::Vector2f ballSpeed, sf::FloatRect thisPaddle, sf::FloatRect enemyPaddle) override {
        std::cout << "thisPaddle" << thisPaddle.left << "/" << thisPaddle.top << " | " << thisPaddle.width << "/" << thisPaddle.height << std::endl;
        float thisPaddleYCenter = thisPaddle.top + thisPaddle.height/2;

        if(ball.y > thisPaddleYCenter)
            return Action::Down;
        if (ball.y < thisPaddleYCenter)
            return Action::Up;
        return Action::None;
    }
};
class PaddleKeyboard : public PaddleController {
private:
    sf::Keyboard::Key m_up;
    sf::Keyboard::Key m_down;
public:
    PaddleKeyboard(sf::Keyboard::Key up, sf::Keyboard::Key down) : 
        m_up(up), 
        m_down(down){
    }
    Action Act(sf::Vector2f ball, sf::Vector2f ballSpeed, sf::FloatRect thisPaddle, sf::FloatRect enemyPaddle) override {
        std::cout << "thisPaddle" << thisPaddle.left << "/" << thisPaddle.top << " | " << thisPaddle.width << "/" << thisPaddle.height << std::endl;
        if(sf::Keyboard::isKeyPressed(m_down)){
            return Action::Down;
        }
        if(sf::Keyboard::isKeyPressed(m_up)){
            return Action::Up;
        }
        return Action::None;
    }
};

sf::Vector2f movePaddle(const sf::Vector2f currentPosition, PaddleController::Action action, const float paddleSpeed, const float elapsedSeconds){
    std::cout << "Action" << action << std::endl;
    switch(action){
        case PaddleController::Action::Up:
            return currentPosition - sf::Vector2f(0, paddleSpeed * elapsedSeconds);
        case PaddleController::Action::Down:
            return currentPosition + sf::Vector2f(0, paddleSpeed * elapsedSeconds);
        default:
            return currentPosition;
    }
}
// keeps min <= obj <= max. if !(min < max) behaviour undefined 
float keepInBounds(float obj, float min, float max){
    return fmax(fmin(obj, min), max);
}
sf::Vector2f keepInBounds(const sf::Vector2f obj, const sf::FloatRect bounds){
    return sf::Vector2f(
        keepInBounds(obj.x, bounds.left, bounds.left + bounds.width),
        keepInBounds(obj.y, bounds.top, bounds.top + bounds.height)
    );
}

auto ctrlLeft = std::unique_ptr<PaddleController>(new PaddleAI());
auto ctrlRight = std::unique_ptr<PaddleController>(new PaddleAI());

class State {
    private:
        bool disposed = false;
    public:
        /* Handle input, if it should not bubble, return false. */
        virtual bool handleInput(const sf::Event &input) = 0;
        virtual void update(float elapsedSeconds) = 0;
        virtual void draw(sf::RenderWindow &window) = 0;
        const void dispose() {
            disposed = true;
        }
        const bool isDisposed() {
            return disposed;
        }
};

class MenuState : State {
    private:
        int selectedEntry = 0;
        std::string entries[3] = {"Singleplayer (Up/Down)", "2 Player (W/S, Up/Down)", "Exit"};
        std::vector<sf::Text> shTexts;
        float alpha = 0;
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
                text.setPosition(position + sf::Vector2f(-bounds.width/2, 0));
                shTexts.push_back(text);
                // next text should be 1.5 lines lower than this one
                position += sf::Vector2f(0, bounds.height*1.5);
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
                            switch(selectedEntry){
                                case 0:
                                    ctrlRight  = std::unique_ptr<PaddleController>(new PaddleKeyboard(sf::Keyboard::Key::Up, sf::Keyboard::Key::Down));
                                    ctrlLeft  = std::unique_ptr<PaddleController>(new PaddleAI());
                                    break;
                                case 1:
                                    ctrlRight  = std::unique_ptr<PaddleController>(new PaddleKeyboard(sf::Keyboard::Key::Up, sf::Keyboard::Key::Down));
                                    ctrlLeft = std::unique_ptr<PaddleController>(new PaddleKeyboard(sf::Keyboard::Key::W, sf::Keyboard::Key::S));
                                    break;
                                case 2:
                                    break;
                            }
                            dispose();
                    }
                break;
                }
            }
        }

        void update(float elapsedSeconds){   
            alpha = fmod(alpha + elapsedSeconds*512, 512.f);
            shTexts[selectedEntry].setFillColor(sf::Color(0xfc, 0x9a, 0x04, abs(static_cast<int>(256-alpha))));
        }
        void draw(sf::RenderWindow &window){
            if(isDisposed())
                return;
            for(auto entry : shTexts){
                window.draw(entry);
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

    MenuState menuState(font, sf::Vector2f(MID_X, MID_Y-MIN_Y));
    
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

        menuState.update(elapsedSeconds);
        

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
        menuState.draw(window);
        window.display();
    }

    return 0;
}