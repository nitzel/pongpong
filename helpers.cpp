#include "helpers.hpp"

sf::Vector2f normalize(const sf::Vector2f vec){
    auto sqrLen = vec.x*vec.x + vec.y*vec.y;
    if(sqrLen){
        auto len = sqrt(sqrLen);
        return sf::Vector2f(vec.x/len, vec.y/len);
    }
    return sf::Vector2f(vec);
}

sf::Vector2f multiply(const sf::Vector2f vec, const float scalar){
    return sf::Vector2f(vec.x * scalar, vec.y * scalar);
}

float keepInBounds(float obj, float min, float max){
    return fmax(fmin(obj, min), max);
}
sf::Vector2f keepInBounds(const sf::Vector2f obj, const sf::FloatRect bounds){
    return sf::Vector2f(
        keepInBounds(obj.x, bounds.left, bounds.left + bounds.width),
        keepInBounds(obj.y, bounds.top, bounds.top + bounds.height)
    );
}

sf::Vector2f reflectBallFromPaddle(sf::Vector2f ballDir, float ballY, float paddleY, float paddleHeight){
    auto offset = ballY - (paddleY + paddleHeight/2);
    auto relativeOffset = offset / (paddleHeight/2);
    auto relativeOffsetSquaredSigned = relativeOffset * fabs(relativeOffset) * fabs(relativeOffset);
    return sf::Vector2f(-ballDir.x, ballDir.y + relativeOffsetSquaredSigned);
}

sf::Vector2f movePaddle(const sf::Vector2f currentPosition, IPaddleController::Action action, const float paddleSpeed, const float elapsedSeconds){
    switch(action){
        case IPaddleController::Action::Up:
            return currentPosition - sf::Vector2f(0, paddleSpeed * elapsedSeconds);
        case IPaddleController::Action::Down:
            return currentPosition + sf::Vector2f(0, paddleSpeed * elapsedSeconds);
        default:
            return currentPosition;
    }
}