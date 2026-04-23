#pragma once

#include <chrono>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "game/game_model.hpp"
#include "core/events.hpp"

namespace snakes {

struct ScoreboardEntry {
    std::string label;
    int color{fg_white};
    std::size_t score{0};
};

class IView {
public:
    virtual ~IView() = default;
    
    virtual void render(GameModel& model) = 0;
    virtual std::optional<Event> poll_event() = 0;

    virtual bool show_scoreboard(const std::vector<ScoreboardEntry>& /*entries*/,
                                 std::size_t /*current_round*/,
                                 std::size_t /*total_rounds*/,
                                 bool /*final_screen*/,
                                 std::chrono::milliseconds /*duration*/) {
        return true;
    }
};

} // namespace snakes