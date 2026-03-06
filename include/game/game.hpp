#pragma once

#include "game_model.hpp"
#include "view.hpp"

namespace snakes {

class Game {
private:
    GameModel& game_model_;
    IView& view_;

public:
    Game(GameModel& game_model, IView& view);
}; 


} // namespace snakes
