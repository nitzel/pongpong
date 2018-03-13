#include "StateManager.hpp"

/// ctors
StateManager::StateManager()
: IState("StateManager")
{
    std::cout << "StateManager initialized" << std::endl;
}

/// private methods
void StateManager::cleanUpDisposed(){
    for (auto it = stateStack.begin(); it != stateStack.end(); it++){
        if((*it)->isDisposed()){
            it = stateStack.erase(it);
            it--; 
        }
    }
}

/// public methods
bool StateManager::noStatesLeft(){
    return stateStack.empty();
}

void StateManager::push(std::unique_ptr<IState> state){
    std::cout << "stm push (len: " << stateStack.size() << std::endl;
    stateStack.push_back(std::move(state));
    std::cout << "         (len: " << stateStack.size() << std::endl;
}

/// IState implementation
bool StateManager::handleInput(const sf::Event &input) {
    std::cout << "stm handle input" << std::endl;
    for (auto it = stateStack.rbegin(); it != stateStack.rend(); it++){
        std::cout << "   ...";
        (*it)->print();
        if(!(*it)->handleInput(input))
            break;
    }
    return false;
}

bool StateManager::update(float elapsedSeconds) {
    std::cout << "stm update" << std::endl;
    for (auto it = stateStack.rbegin(); it != stateStack.rend(); it++){
        std::cout << "   ...";
        (*it)->print();
        if(!(*it)->update(elapsedSeconds))
            break;
    }

    return false;
}
void StateManager::draw(sf::RenderTarget &canvas) {
    std::cout << "stm draw" << std::endl;
    
    // figure out the state furthest in the background to draw
    // so that we can draw that one first and then progress
    // up in the stack drawing the more relevant states on top
    int lowestDrawableStateId = 0;
    for (int i = stateStack.size() - 1; i >= 0; i--){
        if(!stateStack[i]->drawNextState()){
            lowestDrawableStateId = i;
            break;
        }
    }
    // draw everything else on top
    for (int i = lowestDrawableStateId; i < stateStack.size(); i++){
        std::cout << "   ...";
        stateStack[i]->print();
        stateStack[i]->draw(canvas);
    }
    
    cleanUpDisposed();
}
