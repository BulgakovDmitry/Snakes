#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "view.hpp"

namespace snakes {
    
class GraphicsView final : public IView {
public:
    GraphicsView(uint32_t width = 100, uint32_t height = 100);
    ~GraphicsView();

    void render(GameModel& model) override;
    std::optional<Event> poll_event() override;
    bool show_scoreboard(const std::vector<ScoreboardEntry>& entries,
                         std::size_t current_round,
                         std::size_t total_rounds,
                         bool final_screen,
                         std::chrono::milliseconds duration) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace snakes