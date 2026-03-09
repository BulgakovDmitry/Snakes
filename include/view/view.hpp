#pragma once

#include <optional>
#include "game/game_model.hpp"
#include "core/events.hpp"

namespace snakes {

class IView {
public:
    virtual ~IView() = default;
    
    virtual void render(const GameModel& model) = 0;
    virtual std::optional<Event> poll_event() = 0;
};


} // namespace snakes