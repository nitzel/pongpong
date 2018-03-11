#pragma once
#include "IState.hpp"

// The StateManager manages a stack of IStates.
// It calls the topmosts handleInput/update method and goes further down
// the stack calling the next IState's methods - if the upper one returned "true".
// If an IState returns false, the following states won't be allowed to update/draw/etc.
// For drawing it's important to note that the draw methods must be invoked in the reverse order
// but the upper one should still control, whether following states are allowed to draw or not.
class StateManager : public IState {
private:
    std::vector<std::unique_ptr<IState>> stateStack;
    void cleanUpDisposed();

public:
    StateManager();
    bool noStatesLeft();
    void push(std::unique_ptr<IState> state);
    std::unique_ptr<IState> pop();

    // state implementation
    bool handleInput(const sf::Event &input) override;
    bool update(float elapsedSeconds) override;
    void draw(sf::RenderTarget &canvas) override;
};