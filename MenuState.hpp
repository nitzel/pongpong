#pragma once
#include "IState.hpp"
#include "StateManager.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

class MenuState : public IState {
private:
    int selectedEntry = 0;
    std::string entries[4] = {"Singleplayer (Up/Down)", "2 Player (W/S, Up/Down)", "AI vs AI", "Exit"};
    std::vector<sf::Text> shTexts;
    float alpha = 0;
    std::shared_ptr<StateManager> stateManager;
    sf::Font * const font; 
    sf::Vector2f screenSize;
public:
    MenuState(std::shared_ptr<StateManager> stateMgr, sf::Font *font, sf::Vector2f const screenSize);
    bool handleInput(const sf::Event &event) override;
    bool update(float elapsedSeconds) override;
    void draw(sf::RenderTarget &canvas) override;
};