#pragma once

#include "game_model.hpp"

namespace snakes {

class IView {
public:
    virtual ~IView() = default;
    virtual void render(const GameModel& model) = 0;
};


} // namespace snakes