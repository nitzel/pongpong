#include "MenuState.hpp"
#include <memory>
#include "IPaddleController.hpp"
#include "StateManager.hpp"
#include <cmath>

MenuState::MenuState(std::shared_ptr<StateManager> stateMgr, const sf::Font *font, sf::Vector2f position)
:   IState("Menu"),
    stateManager(stateMgr)
{
    for(auto entry : entries){
        sf::Text text;
        text.setFont(*font);
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

bool MenuState::handleInput(const sf::Event &event) {
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
                            //ctrlRight  = std::unique_ptr<IPaddleController>(new PaddleKeyboard(sf::Keyboard::Key::Up, sf::Keyboard::Key::Down));
                            //ctrlLeft  = std::unique_ptr<IPaddleController>(new PaddleAI());
                            break;
                        case 1:
                            //ctrlRight  = std::unique_ptr<IPaddleController>(new PaddleKeyboard(sf::Keyboard::Key::Up, sf::Keyboard::Key::Down));
                            //ctrlLeft = std::unique_ptr<IPaddleController>(new PaddleKeyboard(sf::Keyboard::Key::W, sf::Keyboard::Key::S));
                            break;
                        case 2:
                            break;
                    }
                    dispose();
            }
        break;
        }
    }
    return false;
}

bool MenuState::update(float elapsedSeconds) {   
    alpha = fmod(alpha + elapsedSeconds*512, 512.f);
    shTexts[selectedEntry].setFillColor(sf::Color(0xfc, 0x9a, 0x04, abs(static_cast<int>(256-alpha))));
    return true;
}

void MenuState::draw(sf::RenderTarget &canvas) {
    if(isDisposed())
        return;
    for(auto entry : shTexts){
        canvas.draw(entry);
    }
}
