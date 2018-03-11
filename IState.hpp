#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

// Gamestates to be managed by a StateManager
// they'll keep record of their status, handle events, update and draw themselves.
class IState {
    private:
        bool disposed = false;
        std::string name;
    public:
        /// creates a state that can print it's name
        IState(std::string name)
        : name(name) {};
        ~IState()
        {
            dispose();
        }
        /// methods to be inherited
        virtual void print() const {
            std::cout << "State(" << name << ")" << std::endl;
        }
        void dispose() {
            std::cout << "disposing state(" << name << ")" << std::endl;
            disposed = true;
        }
        bool isDisposed() const{
            return disposed;
        }

        /// IState overridable interface:
        /// return true if you want the updates to bubble through lower states
        virtual bool handleInput(const sf::Event &input) = 0;
        virtual bool update(float elapsedSeconds) = 0;
        virtual void draw(sf::RenderTarget &canvas) = 0;
        virtual bool drawNextState() { return true; }
};
