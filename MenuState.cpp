#include "MenuState.hpp"
#include "GameState.hpp"
#include <memory>
#include "IPaddleController.hpp"
#include "StateManager.hpp"
#include <cmath>

MenuState::MenuState(std::string name, std::shared_ptr<StateManager> stateMgr, sf::Font *font, sf::Vector2f const screenSize, 
                     std::vector<StringAction> itemsAndActions, AllowedForwardActions forwardActions)
:   IState("Menu<"+name+">"),
    stateManager(stateMgr),
    itemActions(itemsAndActions),
    font(font),
    screenSize(screenSize),
    forwardActions(forwardActions)
{
    sf::Vector2f position(screenSize.x/2, screenSize.y/12);
    for(auto entry : itemsAndActions){
        sf::Text text;
        text.setFont(*font);
        text.setCharacterSize(40.f);
        text.setString(entry.name);
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
        case sf::Event::KeyReleased: {
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
                    std::cout << "Selected " << itemActions[selectedEntry].name << std::endl;
                    itemActions[selectedEntry].action(this);
            }
        break;
        }
    }
    return forwardActions.handleInput;
}

bool MenuState::update(float elapsedSeconds) {   
    alpha = fmod(alpha + elapsedSeconds*512, 512.f);
    shTexts[selectedEntry].setFillColor(sf::Color(0xfc, 0x9a, 0x04, abs(static_cast<int>(256-alpha))));
    return forwardActions.update;
}

void MenuState::draw(sf::RenderTarget &canvas) {
    if(isDisposed())
        return;
    for(auto entry : shTexts){
        canvas.draw(entry);
    }
}
