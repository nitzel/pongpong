#include "IPaddleController.hpp"

IPaddleController::Action PaddleAI::Act(sf::Vector2f ball, sf::Vector2f ballSpeed, sf::FloatRect thisPaddle, sf::FloatRect enemyPaddle) 
{
    // Do nothing if the ball is moving away from the paddle
    if(thisPaddle.left < ball.x) // ball is to the right
    {
        if(ballSpeed.x > 0) // ball moves away from paddle
            return None;
    }
    else // ball is to the left
    {
        if(ballSpeed.x < 0) // ball moves away from paddle
            return None;
    }
    
    // get in position, the ball is flying towards this paddle!
    float thisPaddleYCenter = thisPaddle.top + thisPaddle.height/2;

    if(ball.y > thisPaddleYCenter)
        return Action::Down;
    if (ball.y < thisPaddleYCenter)
        return Action::Up;
    return Action::None;
}
