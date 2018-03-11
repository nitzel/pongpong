#pragma once
#include "IState.hpp"
#include "StateManager.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

class MenuState : public IState {
private:
    int selectedEntry = 0;
    std::string entries[3] = {"Singleplayer (Up/Down)", "2 Player (W/S, Up/Down)", "Exit"};
    std::vector<sf::Text> shTexts;
    float alpha = 0;
    std::shared_ptr<StateManager> stateManager;
public:
    MenuState(std::shared_ptr<StateManager> stateMgr, const sf::Font *font, sf::Vector2f position);
    bool handleInput(const sf::Event &event) override;
    bool update(float elapsedSeconds) override;
    void draw(sf::RenderTarget &canvas) override;
};