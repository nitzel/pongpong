#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include "IPaddleController.hpp"

// normalize a vector, if it's length is null the same vector is returned!
sf::Vector2f normalize(const sf::Vector2f vec);

// multiply a vector with a scalar
sf::Vector2f multiply(const sf::Vector2f vec, const float scalar);

// keeps min <= obj <= max. if !(min < max) behaviour undefined 
float keepInBounds(float obj, float min, float max);

sf::Vector2f keepInBounds(const sf::Vector2f obj, const sf::FloatRect bounds);

// Reflects the ball at height ballY from the paddle defined by paddleY to paddleY+paddleHeight
// so that the horizontal direction of the ball reverses but also the vertical aspect changes
// depending on where the paddle was hit (further to the border of the paddle means more speed in that direction)
sf::Vector2f reflectBallFromPaddle(sf::Vector2f ballDir, float ballY, float paddleY, float paddleHeight);

// calculates the new position of a paddle depending on the requested movement direction, paddle movement speed and passed time
sf::Vector2f movePaddle(const sf::Vector2f currentPosition, IPaddleController::Action action, const float paddleSpeed, const float elapsedSeconds);