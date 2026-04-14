#pragma once

namespace snakes {
    struct GameModel;
    class Snake;
    enum class Direction;
    
    Direction choose_bot_direction(const GameModel& model, const Snake& snake);
}