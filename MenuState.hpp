#pragma once
#include "IState.hpp"
#include "StateManager.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

class MenuState;

typedef MenuState ActionState;

struct StringAction {
    std::string name;
    std::function<void(ActionState *self)> action;
    StringAction(std::string name, std::function<void(ActionState *self)> action)
    :   name(name), 
        action(action) 
    {
    }
};

// if true the menu will allow 
// states in a lower hierarchy to process 
// the event, too. if false, it will block that.
struct AllowedForwardActions {
    bool handleInput;
    bool update;
    bool draw;
    AllowedForwardActions(bool handleInput, bool update, bool draw)
    :   handleInput(handleInput),
        update(update),
        draw(draw)
    {
    }
};

class MenuState : public IState {
private:
    int selectedEntry = 0;
    std::vector<StringAction> itemActions;
    std::vector<sf::Text> shTexts;
    float alpha = 0;
    AllowedForwardActions forwardActions;
public:
    const sf::Vector2f screenSize;
    sf::Font * const font;
    std::shared_ptr<StateManager> stateManager;
    MenuState(std::string name, std::shared_ptr<StateManager> stateMgr, sf::Font *font, sf::Vector2f const screenSize, std::vector<StringAction> itemsAndActions, AllowedForwardActions forwardActions);
    bool handleInput(const sf::Event &event) override;
    bool update(float elapsedSeconds) override;
    void draw(sf::RenderTarget &canvas) override;
    bool drawNextState() override { return forwardActions.draw; }
};