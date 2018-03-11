#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include "StateManager.hpp"
#include "MenuState.hpp"

int main()
{
    // setting up sf window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
    // score
    sf::Font font;
    font.loadFromFile("./Grand_Aventure.otf");


    //MenuState menuState(font, sf::Vector2f(MID_X, MID_Y-MIN_Y));
    auto stateManager = std::make_shared<StateManager>();
    
    stateManager->push(std::make_unique<MenuState>(stateManager, &font, sf::Vector2f(window.getSize())));
    
    //clock
    sf::Clock clock;

    while (window.isOpen() && !stateManager->noStatesLeft())
    {        
        sf::Event event;
        while (window.pollEvent(event))
        {
            stateManager->handleInput(event);
            switch(event.type){
                case sf::Event::Closed:
                    window.close();
                    break;
            }
        }

        float elapsedSeconds = clock.getElapsedTime().asSeconds();
        clock.restart();

        stateManager->update(elapsedSeconds);
        
        // draw
        window.clear(sf::Color(0,0,0,10));
        stateManager->draw(window);
        window.display();
    }

    return 0;
}