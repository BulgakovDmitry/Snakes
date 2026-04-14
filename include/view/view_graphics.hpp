#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include "view.hpp"

namespace snakes {
    
class GraphicsView final : public IView {
public:
    GraphicsView(uint32_t screen_width = 1280, uint32_t screen_height = 720);
    //GraphicsView();
    ~GraphicsView();

    void render(GameModel& model) override;
    std::optional<Event> poll_event() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace snakes